#include <gba.h>
#include <string.h>
#include <stdlib.h>

#include "bricks.h"
#include "main.h"

int main(){
	REG_BG0CNT = CHAR_BASE(0) | SCREEN_BASE(10) | BG_16_COLOR | BG_SIZE_0;
	REG_DISPCNT = MODE_0 | BG0_ON | OBJ_ON | OBJ_1D_MAP;
	
	//allow vblank bios interrupt to save battery
	irqInit();
	irqEnable(IRQ_VBLANK);
	
	//load palette, tiles, map
	memcpy(BG_PALETTE, bgPal, bgPalLen);
	memcpy(CHAR_BASE_BLOCK(0), bgTiles, bgTilesLen);
	memcpy(SCREEN_BASE_BLOCK(10), bgMap, bgMapLen);
	memcpy(SPRITE_PALETTE, bricksPal, bricksPalLen);
	memcpy(SPRITE_GFX, bricksTiles, bricksTilesLen);
	
	init_oam(obj_buffer, 128);
	
	//bricks
	restartGame:
	for (u32 j=0;j<5;j++){
		for (u32 i=0;i<13;i++){
			obj_buffer[i+j*13].attr0 = OBJ_Y(16+j*8) | ATTR0_COLOR_256 | ATTR0_NORMAL | ATTR0_WIDE | OBJ_TRANSLUCENT;
			obj_buffer[i+j*13].attr1 = OBJ_X(16+16*i) | ATTR1_SIZE_8;
			obj_buffer[i+j*13].attr2 = OBJ_CHAR(j*4);	
		}
	}
	
	//paddle
	u32 paddleX = 108;
	u32 key_states = 0;
	obj_buffer[65].attr0 = OBJ_Y(150) | ATTR0_COLOR_256 | ATTR0_NORMAL | ATTR0_WIDE | OBJ_TRANSLUCENT;
	obj_buffer[65].attr1 = OBJ_X(paddleX) | ATTR1_SIZE_8;	
	obj_buffer[65].attr2 = OBJ_CHAR(22);
	obj_buffer[66].attr0 = OBJ_Y(150) | ATTR0_COLOR_256 | ATTR0_NORMAL | ATTR0_WIDE | OBJ_TRANSLUCENT;
	obj_buffer[66].attr1 = OBJ_X(paddleX+16) | ATTR1_SIZE_8;	
	obj_buffer[66].attr2 = OBJ_CHAR(26);
	
	//ball
	u32 ballX = 120;
	u32 ballY = 70;
	s32 xVel = 2;
	s32 yVel = 1;
	obj_buffer[67].attr0 = OBJ_Y(ballY) | ATTR0_COLOR_256 | ATTR0_NORMAL | ATTR0_SQUARE | OBJ_TRANSLUCENT;
	obj_buffer[67].attr1 = OBJ_X(ballX) | ATTR1_SIZE_8;	
	obj_buffer[67].attr2 = OBJ_CHAR(20);		
			
	oam_copy(OAM, obj_buffer, 128);
	
  
	for(;;){
		//paddle
		scanKeys();
		key_states = ~REG_KEYINPUT & KEY_ANY;
		u32 paddleDir = 0;
		if (key_states & KEY_LEFT && paddleX > 16)
		  paddleDir = -1;
		else if (key_states & KEY_RIGHT && paddleX < 192)
		  paddleDir = 1;
		else if (key_states & KEY_L && key_states & KEY_R)
			goto restartGame;
		if (paddleDir != 0)
			paddleX += paddleDir * 2;
		obj_buffer[65].attr1 = OBJ_X(paddleX) | ATTR1_SIZE_8;
		obj_buffer[66].attr1 = OBJ_X(paddleX+16) | ATTR1_SIZE_8;
		
		//collision management
		if (ballX < 17){
			ballX += 3;
			xVel *= -1;
		}
		if (ballX > 215){
			ballX -= 3;
			xVel *= -1;
		}
		if (ballY > 138 && ballY < 152){
			if (checkCollision(ballX,ballY,paddleX,150,32,8)){
				yVel = -1 * abs(yVel);
				ballY -= 3;
				if (ballX-paddleX < 10)  //left third
					xVel = -1 * abs(xVel);
				else if (ballX-paddleX > 20) //right third
					xVel = abs(xVel);
			}
		}
		if (ballY > 159){ //out of bounds
			ballX = 120;
			ballY = 70;
		}		
		if (ballY < 2){
			ballY += 3;
			yVel = abs(yVel);
		}
		
		//let's break some bricks!
		for (u32 j=0;j<5;j++){
			u32 brickY = 16+j*8;
			for (u32 l=0;l<13;l++){
				if (checkCollision(ballX,ballY,(l+1)*16,brickY+1,3,6) && !(obj_buffer[l+j*13].attr0 & OBJ_DISABLE)){ //hits left end of brick 
					xVel = -1 * abs(xVel);
					ballX -= 2;
					obj_buffer[l+j*13].attr0 |= OBJ_DISABLE;
					goto skipCollisionCheck;
				}
				else if (checkCollision(ballX,ballY,((l+1)*16)+15,brickY+1,3,6) && !(obj_buffer[l+j*13].attr0 & OBJ_DISABLE)){ //hits right end of brick 
					xVel = abs(xVel);
					ballX += 2;
					obj_buffer[l+j*13].attr0 |= OBJ_DISABLE;
					goto skipCollisionCheck;
				}
			}
			
			if (checkCollision(ballX,ballY,16,brickY+4,210,5)){  //hits bottom of row
				for (u32 l=0;l<13;l++){
					if (!(obj_buffer[l+j*13].attr0 & OBJ_DISABLE) && checkCollision(ballX,ballY,(l+1)*16,brickY,16,8)){
						yVel = abs(yVel);
						ballY += 3;
						obj_buffer[l+j*13].attr0 |= OBJ_DISABLE;
					}
				}
			}
			else if (checkCollision(ballX,ballY,16,brickY,210,5)){ //hits top of row
				for (u32 l=0;l<13;l++){
					if (!(obj_buffer[l+j*13].attr0 & OBJ_DISABLE) && checkCollision(ballX,ballY,(l+1)*16,16+j*8,16,8)){
						yVel = -1*abs(yVel);
						ballY -= 3;
						obj_buffer[l+j*13].attr0 |= OBJ_DISABLE;
					}	
				}
			}
			skipCollisionCheck: ;
			
		}
		
		//ball movement
		ballX += xVel;
		ballY += yVel;
		obj_buffer[67].attr0 = OBJ_Y(ballY) | ATTR0_COLOR_256 | ATTR0_NORMAL | ATTR0_SQUARE | OBJ_TRANSLUCENT;
		obj_buffer[67].attr1 = OBJ_X(ballX) | ATTR1_SIZE_8;
		
		
		VBlankIntrWait();
		oam_copy(OAM, obj_buffer, 128);
	}
  
	return 0;
}


int checkCollision(u32 ax, u32 ay, u32 bx, u32 by, u32 bw, u32 bh){
  return !(ay+8 <= by || ay >= by+bh || ax >= bx+bw || ax+8 <= bx);
}


// Start tonclib snippet
OBJATTR obj_buffer[128] ;
OBJAFFINE *const  obj_aff_buffer = (OBJAFFINE*)obj_buffer;

void oam_copy(OBJATTR *dst, const OBJATTR *src, u32 count) {

// NOTE: while struct-copying is the Right Thing to do here,
//	 there's a strange bug in DKP that sometimes makes it not work
//	 If you see problems, just use the word-copy version.
#if 1
	while(count--)
		*dst++ = *src++;
#else
	u32 *dstw= (u32*)dst, *srcw= (u32*)src;
	while(count--)
	{
		*dstw++ = *srcw++;
		*dstw++ = *srcw++;
	}
#endif

}

void init_oam(OBJATTR *obj, u32 count) {
	u32 nn = count;
	u32 *dst = (u32*)obj;

	// Hide each object
	while (nn--)
	{
		*dst++ = OBJ_DISABLE;
		*dst++ = 0;
	}

	oam_copy(OAM, obj, count);
}
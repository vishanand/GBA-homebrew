#include <gba_video.h>
#include <gba_interrupt.h>
#include <gba_systemcalls.h>
#include <gba_input.h>
#include <stdlib.h>

#include "main.h"

// extra stuff, also in tonc_video.h
#define M3_WIDTH    SCREEN_WIDTH
// typedef for a whole mode3 line
typedef COLOR       M3LINE[M3_WIDTH];
// m3_mem is a matrix; m3_mem[y][x] is pixel (x,y)
#define m3_mem    ((M3LINE*)MEM_VRAM)
#define KEY_ANY    0x03FF

int main(){
  startGame:

  SetMode(MODE_3 | BG2_ON);

  //allow vblank bios interrupt to save battery
  irqInit();
  irqEnable(IRQ_VBLANK);

  //setup linked list
  tail = NULL;
  tail = malloc(sizeof(node_t));
  tail->x = 120;
  tail->y = 80;
  tail->next = NULL;
  head = tail;

  length = 1;
  u32 key_states = 0;
  dirX = 0;
  dirY = 0;
  srand(1337);
  fruitX = 20 + 4*(rand() % 50);
  fruitY = 20 + 4*(rand() % 30);

	for(;;){
    scanKeys();
    key_states = ~REG_KEYINPUT & KEY_ANY;
    if (key_states & KEY_UP && (dirY!=1 || length==1)){
       dirX=0;
       dirY=-1;
    }
    else if (key_states & KEY_DOWN && (dirY!=-1 || length==1)){
      dirX=0;
      dirY=1;
    }
    if (key_states & KEY_LEFT && (dirX!=1 || length==1)){
      dirY=0;
      dirX=-1;
    }
    else if (key_states & KEY_RIGHT && (dirX!=-1 || length==1)){
      dirY=0;
      dirX=1;
    }

    advanceSnake();
    drawSegment(fruitX,fruitY,CLR_FRUIT);
    drawSnake(CLR_SNAKE);

    VBlankIntrWait();
    if (checkCollision() != 0){
      goto gameOver;
    }

    if (head->x > 236 || head->x < 0 || head->y > 156 || head->y < 0)
      goto gameOver;
    VBlankIntrWait();
    VBlankIntrWait();
	}

  gameOver:
    drawSegment(0,0,CLR_ERROR);
    drawSnake(CLR_BG);
    drawSegment(fruitX, fruitY, CLR_BG);
    freeEmAll();
    while(1){
      scanKeys();
      key_states = ~REG_KEYINPUT & KEY_ANY;
      if (key_states & KEY_L && key_states & KEY_R){
        drawSegment(0,0,CLR_BG);
        goto startGame;
      }
      VBlankIntrWait();
    }
  return 0;
}

static inline void drawSegment(int x, int y, COLOR clr){
  int i;
  int j;
  for (j=0;j<4;j++){
    for (i=0;i<4;i++){
      m3_plot(x+i,y+j,clr);
    }
  }
}

void drawSnake(COLOR clr){
  node_t * current = tail;
  while (current != NULL){
    drawSegment(current->x,current->y,clr);
    current = current->next;
  }
}

void advanceSnake(){
  //add new end node
  node_t * new = NULL;
  node_t * temp = NULL;
  new = malloc(sizeof(node_t));
  new->x = head->x + dirX*4;
  new->y = head->y + dirY*4;
  new->next = NULL;
  head->next = new;
  head = new;
  //remove first node and make second node the new root
  temp = tail->next;
  drawSegment(tail->x,tail->y,CLR_BLACK);
  free(tail);
  tail = temp;
}

void growSnake(){
  //add new end node
  node_t * new = NULL;
  new = malloc(sizeof(node_t));
  new->x = head->x + dirX*4;
  new->y = head->y + dirY*4;
  new->next = NULL;
  head->next = new;
  head = new;
  length++;
}

u32 checkCollision(){
  node_t * current = tail;
  while (current != NULL){
    if (current->x == head->x && current->y == head->y && current->next != NULL){
      return 1; //snake hit itself
    }
    current = current->next;
  }
  if (head->x == fruitX && head->y == fruitY){
    growSnake();
    growSnake();
    growSnake();
    fruitX = 20 + 4*(rand() % 50);
    fruitY = 20 + 4*(rand() % 30);
  }

  return 0;
}

void freeEmAll(){
  node_t * current = tail;
  node_t * temp = NULL;
  while (current != NULL){
    temp = current->next;
    free(current);
    current = temp;
  }
}

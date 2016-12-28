
//{{BLOCK(bg)

//======================================================================
//
//	bg, 256x256@1, 
//	+ palette 256 entries, not compressed
//	+ 2 tiles (t|f|p reduced) not compressed
//	+ regular map (in SBBs), not compressed, 32x32 
//	Total size: 512 + 16 + 2048 = 2576
//
//	Time-stamp: 2016-12-28, 01:34:29
//	Exported by Cearn's GBA Image Transmogrifier, v0.8.3
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_BRICKS_H
#define GRIT_BRICKS_H

#define bricksTilesLen 1024
extern const unsigned short bricksTiles[512];

#define bricksPalLen 512
extern const unsigned short bricksPal[256];

#endif // GRIT_BRICKS_H


#define bgTilesLen 64
extern const unsigned short bgTiles[32];

#define bgMapLen 2048
extern const unsigned short bgMap[1024];

#define bgPalLen 512
extern const unsigned short bgPal[256];

//}}BLOCK(bg)

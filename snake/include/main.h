// main header file


// adapted from tonclib
typedef u16 COLOR;

#define SCREEN_WIDTH   240
#define SCREEN_HEIGHT  160

#define vid_mem     ((u16*)MODE3_FB)

static inline void m3_plot(int x, int y, COLOR clr){
  vid_mem[y*SCREEN_WIDTH+x]= clr;
}

#define CLR_BLACK   0x0000
#define CLR_RED     0x001F
#define CLR_LIME    0x03E0
#define CLR_YELLOW  0x03FF
#define CLR_BLUE    0x7C00
#define CLR_MAG     0x7C1F
#define CLR_CYAN    0x7FE0
#define CLR_WHITE   0x7FFF

#define CLR_FRUIT   CLR_MAG
#define CLR_SNAKE   CLR_CYAN
#define CLR_BG      CLR_BLACK
#define CLR_ERROR   CLR_RED

//const u16 COLOURS[] = {CLR_BLUE,CLR_RED,CLR_MAG,CLR_LIME,CLR_BLACK,CLR_CYAN,CLR_YELLOW,CLR_WHITE};

//draws 4x4 square in specified colour
static inline void drawSegment(int x, int y, COLOR clr);

typedef struct node {
    int x;
    int y;
    struct node * next;
} node_t;

node_t * tail;
node_t * head;
u32 length;
u32 fruitX;
u32 fruitY;
u32 dirX;
u32 dirY;

void drawSnake(COLOR clr);
void advanceSnake();
void growSnake();
u32 checkCollision();
void freeEmAll();

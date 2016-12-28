//main header file

#define KEY_ANY    0x03FF
int checkCollision(u32 ax, u32 ay, u32 bx, u32 by, u32 bw, u32 bh);

//adapted from tonclib
void oam_copy(OBJATTR *dst, const OBJATTR *src, u32 count);
void init_oam(OBJATTR *obj, u32 count);

OBJATTR obj_buffer[128];
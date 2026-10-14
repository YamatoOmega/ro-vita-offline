#ifndef GAME_H
#define GAME_H
#define MAP_W 30
#define MAP_H 17
#define TILE 32
typedef struct { float x,y; int level,xp,kills; float cooldown,flash,respawn; int mob_hp; } Game;
void game_init(Game *g);
int game_blocked(float x,float y);
int game_tile(int x,int y);
int game_need_xp(int level);
void game_step(Game *g,float dx,float dy,int attack,float dt);
int game_save(const Game *g,const char *path);
int game_load(Game *g,const char *path);
#define MOB_X 656.0f
#define MOB_Y 304.0f
#endif

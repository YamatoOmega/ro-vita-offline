#include "../src/game.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>
static void kill(Game *g){g->x=MOB_X-40;g->y=MOB_Y;for(int i=0;i<30;i++)game_step(g,0,0,1,0.05f);}
int main(void){
    Game g,h;game_init(&g);
    for(int i=0;i<200;i++)game_step(&g,-1,0,0,0.05f);
    assert(!game_blocked(g.x,g.y)&&g.x>=44);
    game_init(&g);float x=g.x,y=g.y;game_step(&g,1,1,0,0.05f);
    assert(fabsf(hypotf(g.x-x,g.y-y)-7)<0.001f);
    g.x=240;g.y=240;for(int i=0;i<30;i++)game_step(&g,1,0,0,0.05f);
    assert(g.x<245); /* lake collision */
    game_init(&g);game_step(&g,0,0,1,0.05f);assert(g.mob_hp==30);
    game_init(&g);g.x=MOB_X-40;g.y=MOB_Y;
    game_step(&g,0,0,1,0.05f);int hp=g.mob_hp;game_step(&g,0,0,1,0.05f);assert(g.mob_hp==hp);
    game_init(&g);kill(&g);assert(g.kills==1&&g.xp==15&&g.mob_hp<=0);
    for(int i=0;i<80;i++)game_step(&g,0,0,0,0.05f);
    assert(g.mob_hp==30);kill(&g);assert(g.level==2&&g.xp==0&&g.kills==2);
    game_init(&g);g.level=98;g.xp=game_need_xp(98)-1;kill(&g);assert(g.level==99&&g.xp==0);
    remove("test.save.0");remove("test.save.1");
    assert(!game_load(&h,"test.save"));assert(game_save(&g,"test.save"));
    g.kills=42;assert(game_save(&g,"test.save"));assert(game_load(&h,"test.save"));assert(h.kills==42&&h.level==99);
    FILE *f=fopen("test.save.1","wb");assert(f);fputs("truncated",f);fclose(f);
    assert(game_load(&h,"test.save"));assert(h.kills==1); /* previous valid slot survives */
    assert(game_save(&g,"test.save"));assert(game_load(&h,"test.save")&&h.kills==42);
    assert(!game_save(&g,"nonexistent-directory/test.save"));
    remove("test.save.0");remove("test.save.1");
    puts("PASS: movement, collision, range, cooldown, respawn, XP, cap, save recovery and I/O errors");
    return 0;
}

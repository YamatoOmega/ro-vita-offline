#include "game.h"
#include <vita2d.h>
#include <psp2/ctrl.h>
#include <psp2/kernel/processmgr.h>
#include <psp2/io/fcntl.h>
#include <psp2/io/stat.h>
#include <stdio.h>
#include <math.h>
#define COL(r,g,b) RGBA8(r,g,b,255)
static const char *save_path="ux0:data/rovita/save";
static void label(vita2d_pgf *font,int x,int y,const char *text){if(font)vita2d_pgf_draw_text(font,x,y,COL(238,242,232),1,text);}
static float axis(int raw){float a=(raw-128)/127.0f;return fabsf(a)<0.2f?0:a;}
int main(void){
    Game g;game_init(&g);int loaded=game_load(&g,save_path);
    sceIoMkdir("ux0:data/rovita",0777);
    if(vita2d_init()<0){sceKernelExitProcess(1);return 1;}
    vita2d_pgf *font=vita2d_load_default_pgf();
    sceCtrlSetSamplingMode(SCE_CTRL_MODE_ANALOG);
    unsigned old=0;SceCtrlData pad={0};uint64_t last=sceKernelGetProcessTimeWide();
    float auto_time=0,status_time=4;const char *status=loaded?"Partida cargada":"Nueva partida";
    for(;;){
        uint64_t now=sceKernelGetProcessTimeWide();float dt=(now-last)/1000000.0f;last=now;
        if(dt>0.05f)dt=0.05f;
        sceCtrlPeekBufferPositive(0,&pad,1);unsigned pressed=pad.buttons&~old;old=pad.buttons;
        float dx=axis(pad.lx),dy=axis(pad.ly);
        if(pad.buttons&(SCE_CTRL_LEFT|SCE_CTRL_RIGHT))dx=((pad.buttons&SCE_CTRL_RIGHT)!=0)-((pad.buttons&SCE_CTRL_LEFT)!=0);
        if(pad.buttons&(SCE_CTRL_UP|SCE_CTRL_DOWN))dy=((pad.buttons&SCE_CTRL_DOWN)!=0)-((pad.buttons&SCE_CTRL_UP)!=0);
        int kills=g.kills;
        game_step(&g,dx,dy,(pad.buttons&SCE_CTRL_CROSS)!=0,dt);
        auto_time+=dt;status_time-=dt;
        if((pressed&SCE_CTRL_TRIANGLE)||g.kills!=kills||auto_time>=10||(pressed&SCE_CTRL_START)){
            int ok=game_save(&g,save_path);sceIoSync("ux0:",0);
            status=ok?"Guardado OK":"ERROR al guardar: revisa espacio en ux0";status_time=4;auto_time=0;
            if((pressed&SCE_CTRL_START)&&ok)break;
        }
        vita2d_start_drawing();vita2d_clear_screen();
        for(int y=0;y<MAP_H;y++)for(int x=0;x<MAP_W;x++){
            unsigned c=game_tile(x,y)?COL(39,68,67):((x+y)%2?COL(84,117,77):COL(90,125,82));
            vita2d_draw_rectangle(x*TILE,y*TILE,TILE,TILE,c);
        }
        if(g.mob_hp>0){
            vita2d_draw_fill_circle(MOB_X,MOB_Y+10,20,COL(49,68,54));
            vita2d_draw_fill_circle(MOB_X,MOB_Y,20,COL(239,143,173));
            vita2d_draw_fill_circle(MOB_X-7,MOB_Y-3,2,COL(45,35,48));
            vita2d_draw_fill_circle(MOB_X+7,MOB_Y-3,2,COL(45,35,48));
            vita2d_draw_rectangle(MOB_X-20,MOB_Y-32,40,4,COL(65,43,50));
            vita2d_draw_rectangle(MOB_X-20,MOB_Y-32,40*g.mob_hp/30.0f,4,COL(239,166,85));
            label(font,620,255,"Poring");
        }
        if(g.flash>0)vita2d_draw_fill_circle(g.x,g.y,64,RGBA8(255,226,140,70));
        vita2d_draw_fill_circle(g.x,g.y+12,15,COL(45,64,48));
        vita2d_draw_rectangle(g.x-10,g.y-3,20,23,COL(88,167,215));
        vita2d_draw_fill_circle(g.x,g.y-12,10,COL(242,205,163));
        vita2d_draw_rectangle(0,0,960,78,COL(22,36,43));
        label(font,24,30,"RO VITA / prototipo offline 0.1");
        char hud[160];snprintf(hud,sizeof(hud),"Nivel %d / 99   EXP %d / %d   Porings %d",g.level,g.xp,game_need_xp(g.level),g.kills);
        label(font,24,60,hud);
        vita2d_draw_rectangle(0,480,960,64,COL(22,36,43));
        label(font,24,507,"Stick/cruceta: mover   X: atacar   Triangulo: guardar   START: guardar y salir");
        label(font,24,532,status_time>0?status:"Acercate al Poring. Reaparece en 3 segundos.");
        vita2d_end_drawing();vita2d_swap_buffers();vita2d_wait_rendering_done();
    }
    vita2d_wait_rendering_done();if(font)vita2d_free_pgf(font);vita2d_fini();sceKernelExitProcess(0);return 0;
}

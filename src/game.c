#include "game.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

int game_tile(int x,int y) {
    return x<=0 || y<=2 || x>=MAP_W-1 || y>=MAP_H-2 ||
        (x>=8 && x<=10 && y>=5 && y<=10) || (x>=22 && x<=25 && y>=10 && y<=12);
}
int game_blocked(float x,float y) {
    if (!isfinite(x)||!isfinite(y)||x<44||x>916||y<108||y>468) return 1;
    for(int i=-1;i<=1;i+=2) for(int j=-1;j<=1;j+=2)
        if(game_tile((int)(x+i*11)/TILE,(int)(y+j*11)/TILE)) return 1;
    return 0;
}
int game_need_xp(int level) { return 20+level*10; }
void game_init(Game *g) { memset(g,0,sizeof(*g)); g->x=160;g->y=272;g->level=1;g->mob_hp=30; }
void game_step(Game *g,float dx,float dy,int attack,float dt) {
    if(!isfinite(dt)||dt<=0) return;
    if(dt>0.05f)dt=0.05f;
    float len=sqrtf(dx*dx+dy*dy);
    if(len>1){dx/=len;dy/=len;}
    float x=g->x+dx*140*dt,y=g->y+dy*140*dt;
    if(!game_blocked(x,g->y))g->x=x;
    if(!game_blocked(g->x,y))g->y=y;
    g->cooldown=fmaxf(0,g->cooldown-dt); g->flash=fmaxf(0,g->flash-dt);
    if(g->mob_hp<=0){g->respawn-=dt;if(g->respawn<=0)g->mob_hp=30;}
    if(attack && g->cooldown<=0){
        g->cooldown=0.4f;g->flash=0.14f;
        float mx=g->x-MOB_X,my=g->y-MOB_Y;
        if(g->mob_hp>0 && mx*mx+my*my<=64*64){
            g->mob_hp-=10+g->level*2;
            if(g->mob_hp<=0){
                g->respawn=3; if(g->kills<1000000)g->kills++;
                if(g->level<99)g->xp+=15;
                while(g->level<99 && g->xp>=game_need_xp(g->level)){
                    g->xp-=game_need_xp(g->level);g->level++;
                }
                if(g->level==99)g->xp=0;
            }
        }
    }
}
/* Text payload, version, checksum and two slots: never overwrite the only valid save. */
static uint32_t checksum(const char *s){uint32_t h=2166136261u;for(;*s;s++){h^=(unsigned char)*s;h*=16777619u;}return h;}
static int slot_read(const char *path,Game *g,unsigned *seq){
    char line[192],tail; unsigned hash,s; int version,level,xp,kills; float x,y;
    FILE *f=fopen(path,"rb");if(!f)return 0;
    int ok=fgets(line,sizeof(line),f)!=NULL && fscanf(f,"%x",&hash)==1;
    fclose(f);if(!ok||checksum(line)!=hash)return 0;
    if(sscanf(line,"ROV %d %u %f %f %d %d %d %c",&version,&s,&x,&y,&level,&xp,&kills,&tail)!=7)return 0;
    if(version!=1||level<1||level>99||xp<0||xp>=game_need_xp(level)||
        (level==99&&xp!=0)||kills<0||kills>1000000||game_blocked(x,y))return 0;
    game_init(g);g->x=x;g->y=y;g->level=level;g->xp=xp;g->kills=kills;*seq=s;return 1;
}
static int slot_path(char *out,size_t size,const char *path,int n){int r=snprintf(out,size,"%s.%d",path,n);return r>0&&(size_t)r<size;}
int game_load(Game *g,const char *path){
    char p[512];Game a,b;unsigned sa=0,sb=0;
    if(!slot_path(p,sizeof(p),path,0))return 0;
    int va=slot_read(p,&a,&sa);
    if(!slot_path(p,sizeof(p),path,1))return 0;
    int vb=slot_read(p,&b,&sb);
    if(!va&&!vb)return 0;
    *g=va&&(!vb||sa>=sb)?a:b;return 1;
}
int game_save(const Game *g,const char *path){
    char p[512],line[192];Game tmp;unsigned sa=0,sb=0;
    if(!slot_path(p,sizeof(p),path,0))return 0;
    int va=slot_read(p,&tmp,&sa);
    if(!slot_path(p,sizeof(p),path,1))return 0;
    int vb=slot_read(p,&tmp,&sb);
    unsigned seq=(sa>sb?sa:sb)+1;if(seq==0)return 0;
    int target=!va?0:!vb?1:sa<=sb?0:1;
    if(!slot_path(p,sizeof(p),path,target))return 0;
    snprintf(line,sizeof(line),"ROV 1 %u %.3f %.3f %d %d %d\n",seq,g->x,g->y,g->level,g->xp,g->kills);
    FILE *f=fopen(p,"wb");if(!f)return 0;
    int ok=fprintf(f,"%s%08x\n",line,(unsigned)checksum(line))>0;
    if(fflush(f)!=0)ok=0;
    if(fclose(f)!=0)ok=0;
    return ok;
}

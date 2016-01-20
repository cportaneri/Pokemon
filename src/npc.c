#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "npc.h"


Snpc CreateNpc(int x, int y,int m,int c,int dir,char* dia){

    Snpc npc;

    npc.pos_x=x;
    npc.pos_y=y;
    npc.id_map=m;
    npc.direction=dir;
    strcpy(npc.dialogue,dia);
    npc.image=c;

    return npc;

}

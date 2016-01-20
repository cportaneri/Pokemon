#include <stdlib.h>
#include <stdio.h>
#include "attack.h"
#include <string.h>

Sattack CreateAttack(char name[50], int type, int power,int pp,int cat,int prec, char info[100])
{
    Sattack attack;
    strcpy(attack.name,name);
    strcpy(attack.info,info);
    attack.type=type;
    attack.power=power;
    attack.pp=pp;
    attack.ppTotal = pp;
    attack.cat=cat;
    attack.prec=prec;
    return attack;
}


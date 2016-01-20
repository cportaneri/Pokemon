#include <stdlib.h>
#include <stdio.h>
#include <string.h>



typedef struct
{
    char name[50];
    int type; /* 0 pour eau, 1 pour feu, 2 pour plante 3 pour normal*/
    int power;
    int pp;
    int ppTotal;
    int cat; /* attaque physique 0 et attaque spéciale 1*/
    int prec;
    char info[100];
}Sattack;



/** \brief Creation d'une attaque
 *
 *
 * \return Sattack
 *
 */
Sattack CreateAttack(char name[50], int type, int power,int pp,int cat,int prec, char info[100]);


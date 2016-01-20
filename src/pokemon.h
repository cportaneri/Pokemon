#include <stdlib.h>
#include <stdio.h>
#include "attack.h"
#include <string.h>
#include <time.h>


/* Structure d'un Pokemon (Creature a capturer et a faire combattre) */
typedef struct
{
    char name[20];
    int number;
    int type;/* 0 pour eau, 1 pour feu, 2 pour plante 3 pour normal
*/  int sexe;
    char description[100];
    int pv;
    int pvMax;
    int lvl;
    int xp;
    int next_xp;
    Sattack moves[4];
    int attack;
    int defense;
    int spclatk;
    int spcldef;
    int speed;
    int minisprite;
    int sprite;
    int fightsprite;
    int vu;/* 0 pas vu  ,1 le pokemon a été vu*/
    int capture;/* 0 = pas de tentative de capture, 1 = vien d'être capturer, 2 = pas réussi à capturer, 3= pokemon appartient à un joueur*/
}Spokemon;


/** \brief Creation d'un Pokemon
 *
 *
 * \return Spokemon
 *
 */
Spokemon createPokemon(char name[20] , int number, int type , int sexe , char description[100],int pv, int lvl, int xp, int attack,int defense,int spclatk,int spcldef,int speed,int minisprite,int sprite,int  fightsprite, int vu, int capture);


/** \brief Fonction qui renvoie un nombre aleatoire entre a et b
 *
 *
 * \return int
 *
 */
int rand_ab(int a, int b);


/** \brief  Renvoie 1 si le Pokemon est capturé et 0 sinon
 *
 *
 * \return int
 *
 */
int capture(int typePokeBall, Spokemon pokemon);


/** \brief  Augmente le niveau du Pokemon en parametre si il a suffisament de point d'experience
 *
 *
 * \return void
 *
 */
void levelUp(Spokemon *pokemon);

/** \brief Modifie les points de vie du Pokemon courant
 *
 *
 * \return void
 *
 */
void modifpv(int modif, Spokemon *pokemon);


/** \brief Teste si le Pokemon est KO
 *
 *
 * \return int
 *
 */
int isdead(Spokemon pokemon);


/** \brief Calcul les degats causée par une attaque
 *
 *
 * \return int
 *
 */
int calculdmg(Spokemon pokemonatt,int id_attack,Spokemon pokemondef);


/** \brief Renvoie un coefficiant concernant les degats d'une attaque en fonction des types des Pokemons
 *
 *
 * \return float
 *
 */
float coefftype(Spokemon pokemon,int type);


/** \brief Initialise les stats d'un pokemon en fonction de son niveau
 *
 * \return void
 *
 */
void initPokemonStat(Spokemon *pkmn,int lvl);





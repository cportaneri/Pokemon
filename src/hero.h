#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "pokemon.h"
#include "objet.h"
#define HERO_NAME_MAX 256


/* Structure du héro (Main Characteur) */
typedef struct
{

  int pos_x; /*Position Initail en x*/
  int pos_y; /*Position Initail en y*/
  int id_map; /*Id de la map ou est le perso*/
  char image[HERO_NAME_MAX]; /* String pour recuperer les charsets du MC */
  int sol; /*Id du sol sous le MC */
  int direction;
  int nb_pok;
  int aAttaquer;
  int etatPoke; /*1 si tout les pokemon sont K.O*/
  Spokemon pokemon[6] ;
  Spokemon reservePokemon[200];
  int nb_pokReserve;
  int nbObjet;
  Objet mesObjet[6];
  int argent;
  int enCombat;/* 1 si le hero est entrain de combattre, 0 sinon*/
  int nbPokeVu;


}Shero;

/** \brief Creation d'un Heros
 *
 *
 * \return Shero
 *
 */
Shero CreateHero(int pos_x, int pos_y,int id, char * image,int sol,int direction);



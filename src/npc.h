
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#define NPC_NAME_MAX 256

/* Structure d'un NPC (Non Playable Character) */
typedef struct
{

  int pos_x; /*Position Initail en x*/
  int pos_y; /*Position Initail en y*/
  int id_map;
  int image; /* Tableau des Charset du NPC */
  int direction; /* Direction Initial*/
  char dialogue[800];


}Snpc;

/** \brief Creation d'un NPC
 *
 *
 * \return Snpc
 *
 */
Snpc CreateNpc(int pos_x, int pos_y,int id_map,int image,int direction,char * dialogue);


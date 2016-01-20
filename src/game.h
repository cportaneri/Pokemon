#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <FMOD/fmod.h>
#include "square.h"
#include "npc.h"
#include "hero.h"

#define POSITION_DEPART_HEROS_X 7
#define POSITION_DEPART_HEROS_Y 7
#define DIRECTION_DEPART_HEROS 2

#define MAX_SIZE_MAP_X 40
#define MAX_SIZE_MAP_Y 50

#define VIDE 0
#define SOLIDE 1

#define HAUTEHERBE 50
#define HERBE 49
#define PARQUET 111
#define TAPIS 170
#define TAPIS2 172
#define SOL 141

/* ID des NPCs */
#define NPC_MOTHER 1
#define NPC_MAN 2
#define NPC_BOY 3
#define NPC_GIRL 4
#define NPC_BIGBOY 5
#define NPC_BIGBOY2 6
#define NPC_GIRL2 7
#define NPC_PROF1 8
#define NPC_PROF2 9
#define NPC_OLD1 10
#define NPC_OLD2 11
#define NPC_RIVAL 12
#define NPC_NURSE 13
#define NPC_BOY2 14

#define NB_NPC_MAX 15
#define NB_POKEMON_MAX 9

/* Structure du jeu Pokemon */
typedef struct
{

  /* Terrain de jeu (map) */
  Square field[MAX_SIZE_MAP_Y][MAX_SIZE_MAP_X];

  /* Hero du jeu */
  Shero hero;

  Spokemon wildPokemon;


  /*Tableau des npc (personnages non jouables) dans le jeu*/
  Snpc charset[NB_NPC_MAX];

  /* Elements principaux de FMOD qui regissent le son dans le jeu*/
  FMOD_SYSTEM *system;
  FMOD_SOUND *son;

  /* Boucle globale du jeu*/
  int boucle;

  /* Cheminement de l'histoire */
  int scenario;


  Spokemon pokedex[20];



} Sgame;


/** \brief Creation du jeu
 *
 * \return Sgame*
 *
 */
Sgame CreateGame(int id_map);


/** \brief Ajoute un npc dans le jeu
 *
 * \return void
 *
 */
void addNpc(Sgame *g);

/** \brief Deplace le hero dans le jeu
 *
 * \return void
 *
 */
void moveDir(Sgame *g, Shero *p,int etat,int direction);

/** \brief Permet au hero de ne pas marcher sur certains emplacements
 *
 * \return int
 *
 */
int CheckDir(Sgame * game,int direction);

/** \brief Teleporte le hero d'une map a une autre
 *
 * \return void
 *
 */
void teleportationHeros(Sgame *game,int id_map,int pos_x,int pos_y,int direction,int sol);




/** \brief Change la musique actuelle
 *
 * \return void
 *
 */
void changeSound(Sgame *g,int new_song);


/** \brief Sauvegarde du jeu dans un fichier
 *
 * \return void
 *
 */
void saveGame(Sgame *game);

/** \brief Chargement des données du jeu a partir d'un fichier
 *
 * \return void
 *
 */
void loadGame(Sgame *game);

/** \brief Retourne un Pokemon aleatoirement dans la base de donnée
 *
 * \return void
 *
 */
Spokemon randPokemon(Sgame game);



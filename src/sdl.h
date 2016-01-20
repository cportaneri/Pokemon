#define _Sdl_H

#include <SDL/SDL.h>
#include "game.h"
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>

#define NB_CHIPSET 183
#define NB_HERO_CHARSET 24
#define NB_NPC_CHARSET 50
#define NB_DIALOG_CHIPSET 76
#define NB_MENU_SPRITE 51
#define NB_POKEMON_SPRITE 30
#define NB_FIGHT_ANIMATION 16
#define NB_INTRO_ANIMATION 30

/* Structure de l'affichage du jeu*/
typedef struct
{
    Sgame jeu;
    SDL_Surface* surface_ecran;
    SDL_Surface* surface_Chipset[NB_CHIPSET];
    SDL_Surface* surface_Hero_Charset[NB_HERO_CHARSET];
    SDL_Surface* surface_Npc_Charset[NB_NPC_CHARSET];
    SDL_Surface* surface_Dialog_Chipset[NB_DIALOG_CHIPSET];
    SDL_Surface* surface_Menu_Sprite[NB_MENU_SPRITE];
    SDL_Surface* surface_Pokemon_Sprite[NB_POKEMON_SPRITE];
    SDL_Surface* surface_Fight_Animation[NB_FIGHT_ANIMATION];
    SDL_Surface* surface_Intro_Animation[NB_INTRO_ANIMATION];
    SDL_Rect offset;

} sdlJeu;


/** \brief Charge toute les images d'un repertoire et le met dans un tableau de SDL Surface
 *
 * \param SDL_Surface**
 * \param char*
 * \return SDL_Surface*
 *
 */
void sdlLoadSurface(SDL_Surface** surfaceArray,char* path);

/** \brief Charge toutes les images de tous les repertoires et les stocks dans les differents tableau de SDL Surface
 *
 * \param SDL_Surface**
 * \param char*
 * \return SDL_Surface*
 *
 */
void sdlInitSurface(sdlJeu *pSdlJeu);

/** \brief Charge une image
 *
 * \param filename const char*
 * \return SDL_Surface*
 *
 */
SDL_Surface *SDL_load_image(const char* filename );


/** \brief Colle une surface sur l'écran
 *
 * \param source SDL_Surface*
 * \param destination SDL_Surface*
 * \param x int
 * \param y int
 * \return void
 *
 */
void SDL_apply_surface( sdlJeu *pSdljeu,SDL_Surface* source, SDL_Surface* destination, int x, int y );


/** \brief Affiche le terrain autour du heros
 *
 * \return void
 *
 */
void sdljeuAff(sdlJeu *pSdlJeu,int decalage_x,int decalage_y);

/** \brief Affiche le Heros
 *
 * \param  sdlJeu*
 * \return void
 *
 */
void sdlAffHeros(sdlJeu *pSdlJeu,int indice);

/** \brief Boucle evenementielle principale du jeu
 *
 * \param  sdlJeu*
 * \return void
 *
 */
void sdljeuBoucle(sdlJeu *pSdlJeu );

/** \brief Efface les surfaces declarees en SDL et efface le terrain du jeu
 *
 * \param  sdlJeu*
 * \return void
 *
 */
void sdljeuLibere( sdlJeu* pSdlJeu );

/** \brief Affiche un texte sur l'ecran avec une vitesse d'affiche (aucune si delay=0)
 *
 * \return void
 *
 */
void sdlShowTexte(sdlJeu pSdlJeu, char * message,int pos_x,int pos_y,int delay);

/** \brief Affiche une ou plusieurs boite de dialogue et son contenu en fonction de la taille du message avec vitesse d'affichage
 *
 * \return void
 *
 */
void sdlShowDialog(sdlJeu *pSdlJeu, char * message);

/** \brief Ajoute un evenement dialogue sur la map
 *
 * \return void
 *
 */
void sdlAddDialog(sdlJeu *pSdlJeu, char * message,int id_map, int pos_x, int pos_y, int direction,int npc,int event);

/** \brief Ajoute les evenements dialogues autour des NPC
 *
 * \return void
 *
 */
void addNpcDialog(sdlJeu *pSdlJeu);

/** \brief Affiche un evenement speciale dans le jeu
 *
 * \return void
 *
 */
void showEvent(sdlJeu *pSdlJeu,int event);

/** \brief Affiche les animations dans le jeu (fondu..)
 *
 * \return void
 *
 */
void animationInGame( sdlJeu *pSdlJeu,int animation);

/** \brief Affiche l'introduction du jeu
 *
 * \return void
 *
 */
void animeIntro(sdlJeu *pSdlJeu);

/** \brief Ajoute un point de teleportation dans le jeu
 *
 * \return void
 *
 */
void addTeleport(sdlJeu *pSdlJeu,int from_id_map,int from_pos_x,int from_pos_y,int from_direction,int to_id_map,int to_pos_x, int to_pos_y,int to_direction,int to_sol,int fadeOut);

/** \brief Affichage Principal d'un combat avec un Pokemon Sauvage
 *
 * \return void
 *
 */
void sdlfightAff(sdlJeu *pSdlJeu);



/******* Menus du Jeu ********/

/** \brief Affiche le menu de demarrage du Jeu
 *
 * \return void
 *
 */
void Menu( sdlJeu *pSdlJeu);

/** \brief Affiche le menu au cours du Jeu
 *
 * \return void
 *
 */
void MenuJeu( sdlJeu *pSdlJeu);

/** \brief Affiche le sous-menu des attaques du pokemon en combat
 *
 * \return void
 *
 */
void attaque(sdlJeu *pSdlJeu, int enCombat);

/** \brief Affichage du Pokedex (Liste des pokemons vus )
 *
 * \return void
 *
 */
void pokedex(sdlJeu pSdlJeu);

/** \brief Affichage de la liste des Pokemons de notre equipe
 *
 * \return void
 *
 */
void pokemon(sdlJeu *pSdlJeu);

/** \brief Affichage de la liste des Pokemons de notre equipe en combat
 *
 * \return void
 *
 */
void pokemonCombat(sdlJeu *pSdlJeu, int *enCombat);

/** \brief Affichage de la liste des Pokemons pour les soigner
 *
 * \return void
 *
 */
void pokemonSoin(sdlJeu* pSdlJeu, int indObjet);

/** \brief Affichage de la liste des objets en notre possession
 *
 * \return void
 *
 */
void objet(sdlJeu *pSdlJeu);


/** \brief Affichage du statut du heros
 *
 * \return void
 *
 */
void moi(sdlJeu pSdlJeu);

/** \brief Affichage du sous-menu pour sauvegarder
 *
 * \return void
 *
 */
void sauvegarde(sdlJeu pSdlJeu);

/** \brief Affichage du sous-menu d'information dans le Pokedex
 *
 * \return void
 *
 */
void info(sdlJeu *pSdlJeu, int ind2);
void descrip(sdlJeu pSdlJeu, int ind);

/** \brief Affichage du sous-menu des stats d'un Pokemon
 *
 * \return void
 *
 */
void stat(sdlJeu pSdlJeu, int ind);/*affiche les stat d'un pokemon*/

/** \brief Affichage du sous-menu du Magasin
 *
 * \return void
 *
 */
void affMarket(sdlJeu *pSdlJeu);
void buy(sdlJeu *pSdlJeu);

/** \brief Verifie si le Hero a assez d'argent pour acheter un objet
 *
 * \return int
 *
 */
int paye(sdlJeu *pSdlJeu,int montant);

/** \brief Affiche un nouveau objet dans le sac
 *
 * \return int
 *
 */
void ajout(sdlJeu *pSdlJeu, int type);


/** \brief Affiche les points de vie actuel du pokemon du hero ou de du pokemon sauvage
 *
 * \return void
 *
 */
void affPv(sdlJeu *pSdlJeu, int type);


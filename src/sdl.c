#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "sdl.h"
#include <SDL/SDL.h>
#include <unistd.h>
#include <windows.h>
#include <FMOD/fmod.h>

/* Pad */
#define A 'x'
#define B 'c'
#define START ' '
#define UP SDLK_UP
#define LEFT SDLK_LEFT
#define RIGHT SDLK_RIGHT
#define DOWN SDLK_DOWN

const int TAILLE_SPRITE= 16;

void sdlLoadSurface(SDL_Surface** surfaceArray,char* path){

    char total_path[50];
    int i=-1;
    /*
    struct dirent **namelist;
    int n;
    n = scandir(path, &namelist, 0, alphasort );
    if (n < 0)
       perror("scandir");
    else {
       while (i<n) {
            if(strcmp(namelist[i]->d_name,".") != 0 && strcmp(namelist[i]->d_name,"..") != 0){
               sprintf(total_path,"%s/%s",path, namelist[i]->d_name);
               surfaceArray[i-2]=SDL_load_image(total_path);

            }
            free(namelist[i]);
            i++;


       }
       free(namelist);
   }*/

    WIN32_FIND_DATA FindFileData;
    HANDLE hFind = INVALID_HANDLE_VALUE;
    LPSTR dirpath;

    /* Setup directory */
    dirpath = malloc(MAX_PATH+1);
    snprintf(dirpath, MAX_PATH-3, "%s/*", path);

    /* Find the first file in the directory. */
    hFind = FindFirstFile(dirpath, &FindFileData);

    if( hFind == INVALID_HANDLE_VALUE )
    {
         printf("Invalid file handle. Error is %u.\n", GetLastError());
         free(dirpath);
    }
    else
    {
        /* List all the other files in the directory.*/
        while( FindNextFile(hFind, &FindFileData) != 0 )
        {
            if( strcmp(FindFileData.cFileName,".") != 0 && strcmp(FindFileData.cFileName,"..") != 0){
               sprintf(total_path,"%s/%s",path, FindFileData.cFileName);
               surfaceArray[i]=SDL_load_image(total_path);
            }
            i++;
        }
        FindClose(hFind);
        free(dirpath);
    }


}

void sdlInitSurface(sdlJeu *pSdlJeu){

    /* On charge toutes les images qu'une seule fois */

    /* Chargement des Chipset du Jeu */
    sdlLoadSurface(pSdlJeu->surface_Chipset,"data/map_chipset");

    /* Chargement des Chipset des Dialogues */
    sdlLoadSurface(pSdlJeu->surface_Dialog_Chipset,"data/dialog_chipset");

    /* Chargement des Animations de Combats */
    sdlLoadSurface(pSdlJeu->surface_Fight_Animation,"data/fight_animation");

     /* Chargement des Animations de l'intro */
    sdlLoadSurface(pSdlJeu->surface_Intro_Animation,"data/intro_animation");

    /* Chargement des Charset du Hero */
    sdlLoadSurface(pSdlJeu->surface_Hero_Charset,"data/hero_charset");

    /* Chargement des Sprites du Menu */
    sdlLoadSurface(pSdlJeu->surface_Menu_Sprite,"data/menu_sprite");

    /* Chargement des Charset des Npc */
    sdlLoadSurface(pSdlJeu->surface_Npc_Charset,"data/npc_charset");

    /* Chargement des Sprite des Pokemon */
    sdlLoadSurface(pSdlJeu->surface_Pokemon_Sprite,"data/pokemon_sprite");

}

SDL_Surface *SDL_load_image(const char* filename ){

	SDL_Surface* loadedImage = NULL;

	SDL_Surface* optimizedImage = NULL;

	/* Charge une image BMP */
	loadedImage = SDL_LoadBMP( filename );


	if ( loadedImage != NULL )
	{
		optimizedImage = SDL_DisplayFormat( loadedImage );

		SDL_FreeSurface( loadedImage );
	}

	if(optimizedImage !=NULL){

        Uint32 colorkey = SDL_MapRGB(optimizedImage->format,0xFF,0xFF,0xFF);

        SDL_SetColorKey(optimizedImage, SDL_RLEACCEL | SDL_SRCCOLORKEY, colorkey);
	}

	return optimizedImage;
}

void SDL_apply_surface(sdlJeu *pSdljeu, SDL_Surface* source, SDL_Surface* destination, int x, int y ){

	/* Donne les coordonnés des rectangles */
	pSdljeu->offset.x = x;
	pSdljeu->offset.y = y;

	SDL_BlitSurface( source, 0, destination, &pSdljeu->offset );

}

void sdljeuAff(sdlJeu* pSdlJeu,int decalage_x,int decalage_y){

    int i,j,x,y;

    y=-1;
    x=-1;

	for (i=pSdlJeu->jeu.hero.pos_y-5;i<=pSdlJeu->jeu.hero.pos_y+6;i++){
        for (j=pSdlJeu->jeu.hero.pos_x-5;j<=pSdlJeu->jeu.hero.pos_x+6;j++){

            if (pSdlJeu->jeu.field[i][j].chipset < 182) {
                    /* Pour chaque square entourant le héros on applique la surface correspondante à l'id du chipset*/
                    SDL_apply_surface(pSdlJeu,pSdlJeu->surface_Chipset[pSdlJeu->jeu.field[i][j].chipset], pSdlJeu->surface_ecran, x*TAILLE_SPRITE-decalage_x, y*TAILLE_SPRITE-decalage_y);

            }

             /* Si c'est un personnage */
             else {
                /* On affiche son sol puis son charset*/
                SDL_apply_surface(pSdlJeu,pSdlJeu->surface_Chipset[pSdlJeu->jeu.field[i][j].type], pSdlJeu->surface_ecran, x*TAILLE_SPRITE-decalage_x, y*TAILLE_SPRITE-decalage_y);
                SDL_apply_surface(pSdlJeu,pSdlJeu->surface_Npc_Charset[pSdlJeu->jeu.field[i][j].chipset - 182], pSdlJeu->surface_ecran, x*TAILLE_SPRITE-decalage_x, y*TAILLE_SPRITE-decalage_y);

             }
            x++;
        }
        x=-1;
        y++;
    }

}

void sdlAffHeros(sdlJeu *pSdlJeu,int etat){



    /* On affiche le hero en fonction de sa position (sur l'herbe ou non) */
    switch (pSdlJeu->jeu.hero.sol) {

        case HAUTEHERBE :

            /* On affiche le hero en fonction de sa direction */
            switch (pSdlJeu->jeu.hero.direction) {

                /* Vers le haut */
                case 1:

                    /* On affiche le hero en fonction de son etat (en deplacement ou non)*/
                    switch (etat) {

                        /* En deplacement 1er partie */
                        case 1: SDL_apply_surface(pSdlJeu,pSdlJeu->surface_Hero_Charset[4], pSdlJeu->surface_ecran, 4*TAILLE_SPRITE, 4*TAILLE_SPRITE);
                                break;

                        /* En deplacement 2eme partie */
                        case 2: SDL_apply_surface(pSdlJeu,pSdlJeu->surface_Hero_Charset[3], pSdlJeu->surface_ecran, 4*TAILLE_SPRITE, 4*TAILLE_SPRITE);
                                       break;

                        /* En deplacement 3eme partie */
                        case 3: SDL_apply_surface(pSdlJeu,pSdlJeu->surface_Hero_Charset[5], pSdlJeu->surface_ecran, 4*TAILLE_SPRITE, 4*TAILLE_SPRITE);
                                break;

                        /* Ne bouge pas */
                        case 4:  /* On affiche tout dabord le sol ou est positionne le heros quand il ne bouge pas*/
                                SDL_apply_surface(pSdlJeu,pSdlJeu->surface_Chipset[pSdlJeu->jeu.hero.sol], pSdlJeu->surface_ecran, 4*TAILLE_SPRITE, 4*TAILLE_SPRITE);
                                SDL_apply_surface(pSdlJeu,pSdlJeu->surface_Hero_Charset[3], pSdlJeu->surface_ecran, 4*TAILLE_SPRITE, 4*TAILLE_SPRITE);
                                break;

                        default : break;

                    }
                    break;

                /* Vers le bas */
                case 2:

                    switch (etat) {

                        case 1: SDL_apply_surface(pSdlJeu,pSdlJeu->surface_Hero_Charset[16], pSdlJeu->surface_ecran, 4*TAILLE_SPRITE, 4*TAILLE_SPRITE);
                                break;

                        case 2: SDL_apply_surface(pSdlJeu,pSdlJeu->surface_Hero_Charset[15], pSdlJeu->surface_ecran, 4*TAILLE_SPRITE, 4*TAILLE_SPRITE);
                                       break;

                        case 3: SDL_apply_surface(pSdlJeu,pSdlJeu->surface_Hero_Charset[17], pSdlJeu->surface_ecran, 4*TAILLE_SPRITE, 4*TAILLE_SPRITE);
                                break;

                        case 4:  /* On affiche tout dabord le sol ou est positionne le heros quand il ne bouge pas*/
                                SDL_apply_surface(pSdlJeu,pSdlJeu->surface_Chipset[pSdlJeu->jeu.hero.sol], pSdlJeu->surface_ecran, 4*TAILLE_SPRITE, 4*TAILLE_SPRITE);
                                SDL_apply_surface(pSdlJeu,pSdlJeu->surface_Hero_Charset[15], pSdlJeu->surface_ecran, 4*TAILLE_SPRITE, 4*TAILLE_SPRITE);
                                break;

                        default : break;

                    }
                    break;

                /* Vers la gauche */
                case 3:

                    switch (etat) {

                        case 1: SDL_apply_surface(pSdlJeu,pSdlJeu->surface_Hero_Charset[22], pSdlJeu->surface_ecran, 4*TAILLE_SPRITE, 4*TAILLE_SPRITE);
                                break;

                        case 2: SDL_apply_surface(pSdlJeu,pSdlJeu->surface_Hero_Charset[21], pSdlJeu->surface_ecran, 4*TAILLE_SPRITE, 4*TAILLE_SPRITE);
                                       break;

                        case 3: SDL_apply_surface(pSdlJeu,pSdlJeu->surface_Hero_Charset[23], pSdlJeu->surface_ecran, 4*TAILLE_SPRITE, 4*TAILLE_SPRITE);
                                break;

                        case 4:  /* On affiche tout dabord le sol ou est positionne le heros quand il ne bouge pas*/
                                SDL_apply_surface(pSdlJeu,pSdlJeu->surface_Chipset[pSdlJeu->jeu.hero.sol], pSdlJeu->surface_ecran, 4*TAILLE_SPRITE, 4*TAILLE_SPRITE);
                                SDL_apply_surface(pSdlJeu,pSdlJeu->surface_Hero_Charset[21], pSdlJeu->surface_ecran, 4*TAILLE_SPRITE, 4*TAILLE_SPRITE);
                                break;

                        default : break;

                    }
                    break;

                /* Vers la droite */
                case 4:

                    switch (etat) {

                        case 1: SDL_apply_surface(pSdlJeu,pSdlJeu->surface_Hero_Charset[10], pSdlJeu->surface_ecran, 4*TAILLE_SPRITE, 4*TAILLE_SPRITE);
                                break;

                        case 2: SDL_apply_surface(pSdlJeu,pSdlJeu->surface_Hero_Charset[9], pSdlJeu->surface_ecran, 4*TAILLE_SPRITE, 4*TAILLE_SPRITE);
                                       break;

                        case 3: SDL_apply_surface(pSdlJeu,pSdlJeu->surface_Hero_Charset[11], pSdlJeu->surface_ecran, 4*TAILLE_SPRITE, 4*TAILLE_SPRITE);
                                break;

                        case 4:  /* On affiche tout dabord le sol ou est positionne le heros quand il ne bouge pas*/
                                SDL_apply_surface(pSdlJeu,pSdlJeu->surface_Chipset[pSdlJeu->jeu.hero.sol], pSdlJeu->surface_ecran, 4*TAILLE_SPRITE, 4*TAILLE_SPRITE);
                                SDL_apply_surface(pSdlJeu,pSdlJeu->surface_Hero_Charset[9], pSdlJeu->surface_ecran, 4*TAILLE_SPRITE, 4*TAILLE_SPRITE);
                                break;

                        default : break;

                    }
                    break;

                default: break;

            }
            break;




        default :
            /* On affiche le hero en fonction de sa direction */
            switch (pSdlJeu->jeu.hero.direction) {

                /* Vers le haut */
                case 1:

                    /* On affiche le hero en fonction de son etat (en deplacement ou non)*/
                    switch (etat) {

                        /* En deplacement 1er partie */
                        case 1: SDL_apply_surface(pSdlJeu,pSdlJeu->surface_Hero_Charset[1], pSdlJeu->surface_ecran, 4*TAILLE_SPRITE, 4*TAILLE_SPRITE);
                                break;

                        /* En deplacement 2eme partie ou ne bouge pas*/
                        case 2: SDL_apply_surface(pSdlJeu,pSdlJeu->surface_Hero_Charset[0], pSdlJeu->surface_ecran, 4*TAILLE_SPRITE, 4*TAILLE_SPRITE);
                                       break;

                        /* En deplacement 3eme partie */
                        case 3: SDL_apply_surface(pSdlJeu,pSdlJeu->surface_Hero_Charset[2], pSdlJeu->surface_ecran, 4*TAILLE_SPRITE, 4*TAILLE_SPRITE);
                                break;

                        case 4:  /* On affiche tout dabord le sol ou est positionne le heros quand il ne bouge pas*/
                                SDL_apply_surface(pSdlJeu,pSdlJeu->surface_Chipset[pSdlJeu->jeu.hero.sol], pSdlJeu->surface_ecran, 4*TAILLE_SPRITE, 4*TAILLE_SPRITE);
                                SDL_apply_surface(pSdlJeu,pSdlJeu->surface_Hero_Charset[0], pSdlJeu->surface_ecran, 4*TAILLE_SPRITE, 4*TAILLE_SPRITE);
                                break;

                        default : break;

                    }
                    break;

                /* Vers le bas */
                case 2:

                    switch (etat) {

                        case 1: SDL_apply_surface(pSdlJeu,pSdlJeu->surface_Hero_Charset[13], pSdlJeu->surface_ecran, 4*TAILLE_SPRITE, 4*TAILLE_SPRITE);
                                break;

                        case 2: SDL_apply_surface(pSdlJeu,pSdlJeu->surface_Hero_Charset[12], pSdlJeu->surface_ecran, 4*TAILLE_SPRITE, 4*TAILLE_SPRITE);
                                       break;

                        case 3: SDL_apply_surface(pSdlJeu,pSdlJeu->surface_Hero_Charset[14], pSdlJeu->surface_ecran, 4*TAILLE_SPRITE, 4*TAILLE_SPRITE);
                                break;

                        case 4:  /* On affiche tout dabord le sol ou est positionne le heros quand il ne bouge pas*/
                                SDL_apply_surface(pSdlJeu,pSdlJeu->surface_Chipset[pSdlJeu->jeu.hero.sol], pSdlJeu->surface_ecran, 4*TAILLE_SPRITE, 4*TAILLE_SPRITE);
                                SDL_apply_surface(pSdlJeu,pSdlJeu->surface_Hero_Charset[12], pSdlJeu->surface_ecran, 4*TAILLE_SPRITE, 4*TAILLE_SPRITE);
                                break;

                        default : break;

                    }
                    break;

                /* Vers la gauche */
                case 3:

                    switch (etat) {

                        case 1: SDL_apply_surface(pSdlJeu,pSdlJeu->surface_Hero_Charset[19], pSdlJeu->surface_ecran, 4*TAILLE_SPRITE, 4*TAILLE_SPRITE);
                                break;

                        case 2: SDL_apply_surface(pSdlJeu,pSdlJeu->surface_Hero_Charset[18], pSdlJeu->surface_ecran, 4*TAILLE_SPRITE, 4*TAILLE_SPRITE);
                                       break;

                        case 3: SDL_apply_surface(pSdlJeu,pSdlJeu->surface_Hero_Charset[20], pSdlJeu->surface_ecran, 4*TAILLE_SPRITE, 4*TAILLE_SPRITE);
                                break;

                        case 4:  /* On affiche tout dabord le sol ou est positionne le heros quand il ne bouge pas*/
                                SDL_apply_surface(pSdlJeu,pSdlJeu->surface_Chipset[pSdlJeu->jeu.hero.sol], pSdlJeu->surface_ecran, 4*TAILLE_SPRITE, 4*TAILLE_SPRITE);
                                SDL_apply_surface(pSdlJeu,pSdlJeu->surface_Hero_Charset[18], pSdlJeu->surface_ecran, 4*TAILLE_SPRITE, 4*TAILLE_SPRITE);
                                break;

                        default : break;

                    }
                    break;

                /* Vers la droite */
                case 4:

                    switch (etat) {

                        case 1: SDL_apply_surface(pSdlJeu,pSdlJeu->surface_Hero_Charset[7], pSdlJeu->surface_ecran, 4*TAILLE_SPRITE, 4*TAILLE_SPRITE);
                                break;

                        case 2: SDL_apply_surface(pSdlJeu,pSdlJeu->surface_Hero_Charset[6], pSdlJeu->surface_ecran, 4*TAILLE_SPRITE, 4*TAILLE_SPRITE);
                                       break;

                        case 3: SDL_apply_surface(pSdlJeu,pSdlJeu->surface_Hero_Charset[8], pSdlJeu->surface_ecran, 4*TAILLE_SPRITE, 4*TAILLE_SPRITE);
                                break;

                        case 4:  /* On affiche tout dabord le sol ou est positionne le heros quand il ne bouge pas*/
                                SDL_apply_surface(pSdlJeu,pSdlJeu->surface_Chipset[pSdlJeu->jeu.hero.sol], pSdlJeu->surface_ecran, 4*TAILLE_SPRITE, 4*TAILLE_SPRITE);
                                SDL_apply_surface(pSdlJeu,pSdlJeu->surface_Hero_Charset[6], pSdlJeu->surface_ecran, 4*TAILLE_SPRITE, 4*TAILLE_SPRITE);
                                break;

                        default : break;

                    }
                    break;

                default: break;

            }
            break;
    }

}

void sdljeuBoucle(sdlJeu *pSdlJeu ){


    int toucheD=0;
    int toucheE=0, toucheS=0, toucheF=0;
    SDL_Event event;

     /* Horloges (en secondes) */
    float horloge_courante, horloge_precedente;

    int pressedDownD=0;
    int pressedDownS=0;
    int pressedDownF=0;
    int pressedDownE=0;

    int rand;

    float intervalle_horloge = 0.1f;
    horloge_precedente = (float)clock()/(float)CLOCKS_PER_SEC;

    pSdlJeu->jeu.boucle= 1;
    SDL_WM_SetCaption( "Pokemon", NULL );

    /*La camera*/
    pSdlJeu->surface_ecran = SDL_SetVideoMode(160,160, 32, SDL_HWSURFACE);

    /* Affiche de la map courante*/

    SDL_FillRect(pSdlJeu->surface_ecran, NULL, 0xFFFFFF);
    sdljeuAff(pSdlJeu,0,0);
    sdlAffHeros(pSdlJeu,4);


	/* Boucle evenementielle du jeu */
	while ( pSdlJeu->jeu.boucle == 1 )
	{



        horloge_courante = (float)clock()/(float)CLOCKS_PER_SEC;

	    if (horloge_courante-horloge_precedente>=intervalle_horloge){


             /* Point de teleportation */
            addTeleport(pSdlJeu,1,22,20,1,3,10,12,1,TAPIS,1);
            addTeleport(pSdlJeu,2,11,5,1,3,13,6,2,PARQUET,1);
            addTeleport(pSdlJeu,3,13,5,1,2,11,6,2,PARQUET,1);
            addTeleport(pSdlJeu,3,10,13,2,1,22,21,2,SOL,1);
            addTeleport(pSdlJeu,3,11,13,2,1,22,21,2,SOL,1);

            addTeleport(pSdlJeu,4,6,11,2,1,10,28,2,SOL,1);
            addTeleport(pSdlJeu,4,7,11,2,1,10,28,2,SOL,1);
            addTeleport(pSdlJeu,1,10,27,1,4,7,11,1,TAPIS2,1);

            addTeleport(pSdlJeu,1,5,27,1,5,7,11,1,TAPIS2,1);
            addTeleport(pSdlJeu,5,6,11,2,1,5,28,2,SOL,1);
            addTeleport(pSdlJeu,5,7,11,2,1,5,28,2,SOL,1);

            addTeleport(pSdlJeu,1,14,7,1,6,8,21,1,TAPIS2,1);
            addTeleport(pSdlJeu,6,8,21,2,1,14,8,2,SOL,1);
            addTeleport(pSdlJeu,6,9,21,2,1,14,8,2,SOL,1);

            addTeleport(pSdlJeu,1,10,20,1,7,8,15,1,TAPIS2,1);
            addTeleport(pSdlJeu,7,8,15,2,1,10,21,2,SOL,1);
            addTeleport(pSdlJeu,7,9,15,2,1,10,21,2,SOL,1);

            addTeleport(pSdlJeu,1,18,27,1,8,6,12,1,TAPIS2,1);
            addTeleport(pSdlJeu,8,6,12,2,1,18,28,2,SOL,1);
            addTeleport(pSdlJeu,8,7,12,2,1,18,28,2,SOL,1);

            addTeleport(pSdlJeu,1,14,34,2,9,12,6,2,SOL,0);
            addTeleport(pSdlJeu,1,15,34,2,9,13,6,2,SOL,0);

            addTeleport(pSdlJeu,9,12,5,1,1,14,33,1,SOL,0);
            addTeleport(pSdlJeu,9,13,5,1,1,15,33,1,SOL,0);

            addTeleport(pSdlJeu,9,25,31,4,10,5,12,4,HERBE,0);
            addTeleport(pSdlJeu,9,25,32,4,10,5,13,4,HERBE,0);

            addTeleport(pSdlJeu,10,4,12,3,9,24,31,3,HAUTEHERBE,0);
            addTeleport(pSdlJeu,10,4,13,3,9,24,32,3,HAUTEHERBE,0);


            if (pressedDownD == 0 && pressedDownF == 0 && pressedDownS == 0 && pressedDownE == 0){

                sdlAddDialog(pSdlJeu,"",3,13,6,2,NB_NPC_CHARSET,2);
                sdlAddDialog(pSdlJeu,"",7,8,15,1,NB_NPC_CHARSET,3);


                /* tant qu'il y a des evenements à traiter : cette boucle n'est pas bloquante */
                while ( SDL_PollEvent( &event ) ){

                    /* Si l'utilisateur a cliqué sur la croix de fermeture */
			        if ( event.type == SDL_QUIT ) {
                        pSdlJeu->jeu.boucle = 0;
                    }

                    /* Si l'utilisateur a appuyé sur une touche*/
			        if ( event.type == SDL_KEYDOWN){

                        switch ( event.key.keysym.sym ){



                            case UP:   if(CheckDir(&pSdlJeu->jeu,1) == 0) {
                                                toucheE=1;
                                                pressedDownE =1;
                                            }

                                            else {
                                                pSdlJeu->jeu.hero.direction=1;
                                                sdlAffHeros(pSdlJeu,4);
                                            }
                                            break;

                            case LEFT: if(CheckDir(&pSdlJeu->jeu,3) == 0) {
                                                toucheS=1;
                                                pressedDownS =1;
                                            }
                                            else {
                                                pSdlJeu->jeu.hero.direction=3;
                                                sdlAffHeros(pSdlJeu,4);
                                            }
                                            break;

                            case DOWN: if(CheckDir(&pSdlJeu->jeu,2) == 0) {
                                                toucheD=1;
                                                pressedDownD =1;
                                            }
                                            else {
                                                pSdlJeu->jeu.hero.direction=2;
                                                sdlAffHeros(pSdlJeu,4);
                                            }
                                            break;

                            case RIGHT:   if(CheckDir(&pSdlJeu->jeu,4) == 0) {
                                                    toucheF=1;
                                                    pressedDownF =1;
                                                }
                                                else {
                                                    pSdlJeu->jeu.hero.direction=4;
                                                    sdlAffHeros(pSdlJeu,4);
                                                }
                                                break;




                            case 32:/*NOUVEAU ****************************************/
                                        MenuJeu(pSdlJeu);

                                        sdljeuAff(pSdlJeu,0,0);sdlAffHeros(pSdlJeu,4);/*affiche le menu du jeu, 'v' = select du gameBoy , on réaffiche le terrain sinon le menu reste sur le terrain. */
                                        break;
                            /*FIN NOUVEAU ****************************************/


                            case 'x': sdlAddDialog(pSdlJeu,"Maison de Sacha",1,20,21,1,NB_NPC_CHARSET,0);
                                      sdlAddDialog(pSdlJeu,"Labo du Professeur",1,13,21,1,NB_NPC_CHARSET,0);
                                      sdlAddDialog(pSdlJeu,"Bienvenue a New Bark Town!",1,16,28,1,NB_NPC_CHARSET,0);
                                      sdlAddDialog(pSdlJeu,"Direction arene Pokemon",1,16,18,1,NB_NPC_CHARSET,0);
                                      sdlAddDialog(pSdlJeu,"Centre Pokemon",1,11,28,1,NB_NPC_CHARSET,0);
                                      sdlAddDialog(pSdlJeu,"Magasin",1,6,28,1,NB_NPC_CHARSET,0);
                                      sdlAddDialog(pSdlJeu,"Foret de Jade",9,19,31,1,NB_NPC_CHARSET,0);
                                      sdlAddDialog(pSdlJeu,"Attention Danger! Ne pas continuer au Sud",10,6,19,1,NB_NPC_CHARSET,0);
                                      sdlAddDialog(pSdlJeu,"",4,7,7,1,NB_NPC_CHARSET,1);
                                      sdlAddDialog(pSdlJeu,"",5,7,6,3,NB_NPC_CHARSET,4);
                                      addNpcDialog(pSdlJeu);
                                      sdljeuAff(pSdlJeu,0,0);
                                      sdlAffHeros(pSdlJeu,4);



                            default: break;
				        }
                    }

                }

            }

            else if (pressedDownD==1 ) {

                switch (toucheD){

                    case 1: pSdlJeu->jeu.hero.direction=2;
                            moveDir(&pSdlJeu->jeu,&pSdlJeu->jeu.hero,0,2);

                            sdljeuAff(pSdlJeu,0,4);
                            sdlAffHeros(pSdlJeu,toucheD);
                            toucheD++;
                            break;

                    case 2:
                            sdljeuAff(pSdlJeu,0,8);
                            sdlAffHeros(pSdlJeu,toucheD);
                            toucheD++;
                            break;

                    case 3:
                            sdljeuAff(pSdlJeu,0,12);
                            sdlAffHeros(pSdlJeu,toucheD);
                            toucheD++;
                            break;

                    case 4:
                            moveDir(&pSdlJeu->jeu,&pSdlJeu->jeu.hero,1,2);
                            sdljeuAff(pSdlJeu,0,0) ;
                            sdlAffHeros(pSdlJeu,toucheD);
                            if (pSdlJeu->jeu.hero.sol==HAUTEHERBE) {
                                rand=rand_ab(0,9);
                                if (rand==0) {
                                    animationInGame(pSdlJeu,1);
                                    pressedDownD = 0;
                                }
                            }
                            toucheD=1;
                            break;

                }
            }

            else if (pressedDownE==1 ) {

                switch (toucheE){

                    case 1: pSdlJeu->jeu.hero.direction=1;
                            moveDir(&pSdlJeu->jeu,&pSdlJeu->jeu.hero,0,1);
                            sdljeuAff(pSdlJeu,0,-4);
                            sdlAffHeros(pSdlJeu,toucheE);
                            toucheE++;
                            break;

                    case 2: sdljeuAff(pSdlJeu,0,-8);
                            sdlAffHeros(pSdlJeu,toucheE);
                            toucheE++;
                            break;

                    case 3: sdljeuAff(pSdlJeu,0,-12);
                            sdlAffHeros(pSdlJeu,toucheE);
                            toucheE++;
                            break;

                    case 4: moveDir(&pSdlJeu->jeu,&pSdlJeu->jeu.hero,1,1);
                            sdljeuAff(pSdlJeu,0,0) ;
                            sdlAffHeros(pSdlJeu,toucheE);
                            if (pSdlJeu->jeu.hero.sol==HAUTEHERBE) {
                                rand=rand_ab(0,9);
                                printf("rand in sdl: %d\n",rand);
                                if (rand==0) {
                                    animationInGame(pSdlJeu,1);
                                    pressedDownE = 0;
                                }
                            }
                            toucheE=1;
                            break;
                }
            }

            else if (pressedDownS==1 ) {

                switch (toucheS){

                    case 1: pSdlJeu->jeu.hero.direction=3;
                            moveDir(&pSdlJeu->jeu,&pSdlJeu->jeu.hero,0,3);
                            sdljeuAff(pSdlJeu,-4,0);
                            sdlAffHeros(pSdlJeu,toucheS);
                            toucheS++;
                            break;

                    case 2: sdljeuAff(pSdlJeu,-8,0);
                            sdlAffHeros(pSdlJeu,toucheS);
                            toucheS++;
                            break;

                    case 3: sdljeuAff(pSdlJeu,-12,0);
                            sdlAffHeros(pSdlJeu,toucheS);
                            toucheS++;
                            break;

                    case 4: moveDir(&pSdlJeu->jeu,&pSdlJeu->jeu.hero,1,3);
                            sdljeuAff(pSdlJeu,0,0) ;
                            sdlAffHeros(pSdlJeu,toucheS);
                            if (pSdlJeu->jeu.hero.sol==HAUTEHERBE) {
                                rand=rand_ab(0,9);
                                printf("rand in sdl: %d\n",rand);
                                if (rand==0) {
                                    animationInGame(pSdlJeu,1);
                                    pressedDownS = 0;
                                }
                            }
                            toucheS=1;
                            break;
                }
            }

            else if (pressedDownF==1 ) {

                switch (toucheF){

                    case 1: pSdlJeu->jeu.hero.direction=4;
                            moveDir(&pSdlJeu->jeu,&pSdlJeu->jeu.hero,0,4);
                            sdljeuAff(pSdlJeu,4,0);
                            sdlAffHeros(pSdlJeu,toucheF);
                            toucheF++;break;

                    case 2: sdljeuAff(pSdlJeu,8,0);
                            sdlAffHeros(pSdlJeu,toucheF);
                            toucheF++;
                            break;

                    case 3: sdljeuAff(pSdlJeu,12,0);
                            sdlAffHeros(pSdlJeu,toucheF);
                            toucheF++;
                            break;

                    case 4: moveDir(&pSdlJeu->jeu,&pSdlJeu->jeu.hero,1,4);
                            sdljeuAff(pSdlJeu,0,0) ;
                            sdlAffHeros(pSdlJeu,toucheF);
                            if (pSdlJeu->jeu.hero.sol==HAUTEHERBE) {
                                rand=rand_ab(0,9);
                                printf("rand in sdl: %d\n",rand);
                                if (rand==0) {
                                    animationInGame(pSdlJeu,1);
                                    pressedDownF = 0;
                                }
                            }


                            toucheF=1;
                            break;

                }
            }


            if ((pressedDownD ==1 && toucheD ==1) || (pressedDownE ==1 && toucheE ==1) || (pressedDownS ==1 && toucheS ==1) || (pressedDownF ==1 && toucheF ==1) ){

                if(CheckDir(&pSdlJeu->jeu,2) == 1) {
                    pressedDownD=0;
                }

                if(CheckDir(&pSdlJeu->jeu,1) == 1) {
                    pressedDownE=0;
                }

                if(CheckDir(&pSdlJeu->jeu,3) == 1) {
                    pressedDownS=0;
                }

                if(CheckDir(&pSdlJeu->jeu,4) == 1) {
                    pressedDownF=0;
                }

                while ( SDL_PollEvent( &event )  ){

                    if ( event.type == SDL_KEYUP){

                        switch ( event.key.keysym.sym ){

                            case UP: if(CheckDir(&pSdlJeu->jeu,1) == 0) { pressedDownE=0;}
                                      break;
                            case LEFT: if(CheckDir(&pSdlJeu->jeu,3) == 0) { pressedDownS=0;}
                                      break;
                            case DOWN: if(CheckDir(&pSdlJeu->jeu,2) == 0) { pressedDownD=0;}
                                      break;
                            case RIGHT: if(CheckDir(&pSdlJeu->jeu,4) == 0) { pressedDownF=0;}
                                      break;
                            default:  break;
                        }
                    }

                }

            }
            }


            SDL_Delay(100);
            SDL_Flip( pSdlJeu->surface_ecran );
            /* on permute les deux buffers (cette fonction ne doit se faire qu'une seule fois dans a boucle)*/
            SDL_FreeSurface( pSdlJeu->surface_ecran );


    }
}

void animationInGame( sdlJeu *pSdlJeu,int animation) {

        int i,j,color;
        SDL_Event event;

        switch (animation) {

            /* Transition Debut Combat*/
            case 1 :    changeSound(&pSdlJeu->jeu,4);
                        for(i=0;i<5;i++){
                            for (j=10-i;j>=0;j--){
                                SDL_apply_surface(pSdlJeu,pSdlJeu->surface_Chipset[101], pSdlJeu->surface_ecran, i*TAILLE_SPRITE,j*TAILLE_SPRITE);
                                SDL_Flip( pSdlJeu->surface_ecran);
                                SDL_Delay(17);

                            }
                            for (j=i;j<11;j++){
                                SDL_apply_surface(pSdlJeu,pSdlJeu->surface_Chipset[101], pSdlJeu->surface_ecran, j*TAILLE_SPRITE,i*TAILLE_SPRITE);
                                SDL_Flip( pSdlJeu->surface_ecran);
                                SDL_Delay(17);

                            }
                            for (j=i;j<11;j++){
                                SDL_apply_surface(pSdlJeu,pSdlJeu->surface_Chipset[101], pSdlJeu->surface_ecran, (9-i)*TAILLE_SPRITE,j*TAILLE_SPRITE);
                                SDL_Flip( pSdlJeu->surface_ecran);
                                SDL_Delay(17);

                            }
                            for (j=9-i;j>=0;j--){
                                SDL_apply_surface(pSdlJeu,pSdlJeu->surface_Chipset[101], pSdlJeu->surface_ecran, j*TAILLE_SPRITE,(9-i)*TAILLE_SPRITE);
                                SDL_Flip( pSdlJeu->surface_ecran);
                                SDL_Delay(17);

                            }
                        }
                        pSdlJeu->jeu.wildPokemon=randPokemon(pSdlJeu->jeu);
                        sdlfightAff(pSdlJeu); sdljeuAff(pSdlJeu,0,0);sdlAffHeros(pSdlJeu,4);
                        break;

                 /* Entrée dans les Maisons*/

                case 2 :
                    for (color=0; color<60; color++){
                                 while ( SDL_PollEvent( &event ) ){

			        if ( event.type == SDL_KEYDOWN || event.type == SDL_KEYUP ){

                        switch ( event.key.keysym.sym ){
                            default: break;
                        }

                    }


                    }
                     SDL_SetAlpha(pSdlJeu->surface_Chipset[101],SDL_SRCALPHA,color);
                                for(i=0;i<11;i++){
                                    for (j=0;j<11;j++){
                                        SDL_apply_surface(pSdlJeu,pSdlJeu->surface_Chipset[101], pSdlJeu->surface_ecran, j*TAILLE_SPRITE,i*TAILLE_SPRITE);
                                    }
                                }
                                SDL_Flip(pSdlJeu->surface_ecran);
                                SDL_Delay(10);
                }
                SDL_SetAlpha(pSdlJeu->surface_Chipset[101],SDL_SRCALPHA,255);


                         break;





        }




}

void sdljeuLibere( sdlJeu *pSdlJeu){

    int i;
	SDL_FreeSurface( pSdlJeu->surface_ecran);
	for(i=0;i<NB_CHIPSET-1;i++){
        SDL_FreeSurface(pSdlJeu->surface_Chipset[i]);
	}
	for(i=0;i<NB_HERO_CHARSET;i++){
        SDL_FreeSurface(pSdlJeu->surface_Hero_Charset[i]);
	}
	for(i=0;i<NB_NPC_CHARSET;i++){
        SDL_FreeSurface(pSdlJeu->surface_Npc_Charset[i]);
	}
	for(i=0;i<NB_DIALOG_CHIPSET;i++){
        SDL_FreeSurface(pSdlJeu->surface_Dialog_Chipset[i]);
	}
	for(i=0;i<NB_MENU_SPRITE;i++){
        SDL_FreeSurface(pSdlJeu->surface_Menu_Sprite[i]);
	}
	for(i=0;i<NB_POKEMON_SPRITE;i++){
        SDL_FreeSurface(pSdlJeu->surface_Pokemon_Sprite[i]);
	}
	for(i=0;i<NB_FIGHT_ANIMATION;i++){
        SDL_FreeSurface(pSdlJeu->surface_Fight_Animation[i]);
	}
	for(i=0;i<NB_INTRO_ANIMATION;i++){
        SDL_FreeSurface(pSdlJeu->surface_Intro_Animation[i]);
	}


}

void addTeleport(sdlJeu *pSdlJeu,int from_id_map,int from_pos_x,int from_pos_y,int from_direction,int to_id_map,int to_pos_x, int to_pos_y,int to_direction,int to_sol,int fadeOut) {

    if ( pSdlJeu->jeu.hero.id_map==from_id_map && pSdlJeu->jeu.hero.pos_x == from_pos_x && pSdlJeu->jeu.hero.pos_y == from_pos_y &&  pSdlJeu->jeu.hero.direction==from_direction) {

        teleportationHeros(&pSdlJeu->jeu,to_id_map,to_pos_x,to_pos_y,to_direction,to_sol);
        /*Transition Entrée/Sortie Maison*/
        if(fadeOut ==1  ){

            animationInGame(pSdlJeu,2);
        }

        if (from_id_map==1 && to_id_map==9){
            changeSound(&pSdlJeu->jeu,1);
        }
        else if ((from_id_map==9 || from_id_map==4)&& to_id_map==1){
            changeSound(&pSdlJeu->jeu,2);
        }
        else if(to_id_map==4){
            changeSound(&pSdlJeu->jeu,3);
        }

        sdljeuAff(pSdlJeu,0,0);
        sdlAffHeros(pSdlJeu,4);

    }

}

void sdlShowTexte(sdlJeu pSdlJeu, char * message,int pos_x,int pos_y,int delay){

	int i;

	int lettre_space_x= 4;

	int id_lettre;

	SDL_Init( SDL_INIT_EVERYTHING);

	 for(i=0;i<strlen(message);i++) {

		if(message[i] != ' '){

           		 switch (message[i]) {

                        case '0': id_lettre = 0;
                                  break;

                        case '1': id_lettre = 1;
                                  break;

                        case '2': id_lettre = 2;
                                  break;

                        case '3': id_lettre = 3;
                                  break;

                        case '4': id_lettre = 4;
                                  break;

                        case '5': id_lettre = 5;
                                  break;

                        case '6': id_lettre = 6;
                                  break;

                        case '7': id_lettre = 7;
                                  break;

                        case '8': id_lettre = 8;
                                  break;

                        case '9': id_lettre = 9;
                                  break;

                        case 'A': id_lettre = 10;
                                  break;

                        case 'B': id_lettre = 11;
                                  break;

                        case 'C': id_lettre = 12;
                                  break;

                        case 'D': id_lettre = 13;
                                  break;

                        case 'E': id_lettre = 14;
                                  break;

                        case 'F': id_lettre = 15;
                                  break;

                        case 'G': id_lettre = 16;
                                  break;

                        case 'H': id_lettre = 17;
                                  break;

                        case 'I': id_lettre = 18;
                                  break;

                        case 'J': id_lettre = 19;
                                  break;

                        case 'K': id_lettre = 20;
                                  break;

                        case 'L': id_lettre = 21;
                                  break;

                        case 'M': id_lettre = 22;
                                  break;

                        case 'N': id_lettre = 23;
                                  break;

                        case 'O': id_lettre = 24;
                                  break;

                        case 'P': id_lettre = 25;
                                  break;

                        case 'Q': id_lettre = 26;
                                  break;

                        case 'R': id_lettre = 27;
                                  break;

                        case 'S': id_lettre = 28;
                                  break;

                        case 'T': id_lettre = 29;
                                  break;

                        case 'U': id_lettre = 30;
                                  break;

                        case 'V': id_lettre = 31;
                                  break;

                        case 'W': id_lettre = 32;
                                  break;

                        case 'X': id_lettre = 33;
                                  break;

                        case 'Y': id_lettre = 34;
                                  break;

                        case 'Z': id_lettre = 35;
                                  break;

                        case 'a': id_lettre = 36;
                                  break;

                        case 'b': id_lettre = 37;
                                  break;

                        case 'c': id_lettre = 38;
                                  break;

                        case 'd': id_lettre = 41;
                                  break;

                        case 'e': id_lettre = 44;
                                  break;

                        case 'f': id_lettre = 45;
                                  break;

                        case 'g': id_lettre = 46;
                                  break;

                        case 'h': id_lettre = 47;
                                  break;

                        case 'i': id_lettre = 48;
                                  break;

                        case 'j': id_lettre = 49;
                                  break;

                        case 'k': id_lettre = 50;
                                  break;

                        case 'l': id_lettre = 51;
                                  break;

                        case 'm': id_lettre = 52;
                                  break;

                        case 'n': id_lettre = 53;
                                  break;

                        case 'o': id_lettre = 54;
                                  break;

                        case 'p': id_lettre = 55;
                                  break;

                        case 'q': id_lettre = 62;
                                  break;

                        case 'r': id_lettre = 63;
                                  break;

                        case 's': id_lettre = 64;
                                  break;

                        case 't': id_lettre = 67;
                                  break;

                        case 'u': id_lettre = 69;
                                  break;

                        case 'v': id_lettre = 70;
                                  break;

                        case 'w': id_lettre = 71;
                                  break;

                        case 'x': id_lettre = 72;
                                  break;

                        case 'y': id_lettre = 74;
                                  break;

                        case 'z': id_lettre = 75;
                                  break;

		                case '(': id_lettre = 56;
		                         break;

		                case ')': id_lettre = 57;
		                         break;

		                case '[': id_lettre = 39;
		                         break;

		                case ']': id_lettre = 40;
		                         break;

		                case '.': id_lettre = 58;
		                         break;

		                case ',': id_lettre = 61;
		                         break;

		              /*apostrpohe*/
		                case 39: id_lettre = 61;
		                         break;

		                case ':': id_lettre = 42;
		                         break;

		                case '!': id_lettre = 59;
		                         break;

		                case '?': id_lettre = 60;
		                         break;

		                case '-': id_lettre = 68;
		                         break;

		                case '/': id_lettre = 65;
		                         break;

                        default : break;

            		}

           		 /*Ajustement Majuscule/Minuscule*/
            if (message[i]==39){
                		SDL_apply_surface(&pSdlJeu, pSdlJeu.surface_Dialog_Chipset[id_lettre] , pSdlJeu.surface_ecran, pos_x, pos_y-8);
            }
            else if (message[i]<97){
				SDL_apply_surface( &pSdlJeu, pSdlJeu.surface_Dialog_Chipset[id_lettre] , pSdlJeu.surface_ecran, pos_x, pos_y-1);
            }
            else if (message[i]>114 && message[i]<123){
				SDL_apply_surface( &pSdlJeu,    pSdlJeu.surface_Dialog_Chipset[id_lettre] , pSdlJeu.surface_ecran, pos_x, pos_y+1);
			 }
            else {
				SDL_apply_surface(&pSdlJeu,  pSdlJeu.surface_Dialog_Chipset[id_lettre] , pSdlJeu.surface_ecran, pos_x, pos_y);
            }

			pos_x=pos_x+lettre_space_x;


            SDL_Delay(delay);

            if(delay!=0){
                SDL_Flip( pSdlJeu.surface_ecran );
            }

        }

        pos_x=pos_x+lettre_space_x;

	}

}

void sdlShowDialog(sdlJeu *pSdlJeu, char * message){

    int i,j;
	int boucle = 0;

    char ** texte = (char**)calloc(80,sizeof(char*));
    for(i=0;i<80;i++) {
        texte[i] = (char*)calloc(50,sizeof(char));
    }


    int pos_rel=0;
    int nb_mot=0;

    int box_x = 0;
    int box_y= 106;
    int box_size_x=155;
    int box_size_y=40;

    int lettre_space_x= 8;
    int lettre_space_y= 16;

    int lettre_x = box_x+6;
    int lettre_y = box_y+13;


    SDL_Event event;

    SDL_Init( SDL_INIT_EVERYTHING);

    SDL_apply_surface( pSdlJeu,pSdlJeu->surface_Dialog_Chipset[43], pSdlJeu->surface_ecran, box_x, box_y);



   /* On decoupe le message en plusieurs mots */

    j=0;
    for(i=0;i<strlen(message);i++) {
        if (message[i] != ' ') {

            texte[j][pos_rel]=message[i];
            pos_rel++;

        }
        else {
            pos_rel=0;
            j++;
            nb_mot++;
        }

    }

    nb_mot++;

    /* On affiche mot par mot */
    for (j=0;j<nb_mot;j++){

		/* Separe le message en plusieurs boite de dialogue*/
		if ( lettre_y+lettre_space_y >= box_y+box_size_y-10 && lettre_x + (strlen(texte[j])*lettre_space_x)>= box_size_x-6){

		        lettre_x=box_x+6;
		        lettre_y=box_y+13;
		        SDL_apply_surface(pSdlJeu, pSdlJeu->surface_Dialog_Chipset[66] , pSdlJeu->surface_ecran, 147, 143);
		         SDL_Flip( pSdlJeu->surface_ecran );
		        while (boucle==0){
		            while ( SDL_PollEvent( &event ) ) {

		                if ( event.type == SDL_KEYDOWN){

		                    switch ( event.key.keysym.sym ){

		                            case 'x' : /* Affichage de la prochaine boite */
		                                boucle=1;
		                                SDL_apply_surface(pSdlJeu, pSdlJeu->surface_Dialog_Chipset[43], pSdlJeu->surface_ecran, box_x, box_y);

		                                break;
		                            default: break;

		                    }
		                }

		            }
                    SDL_Delay(200);
		          }
		        boucle=0;
		}

		/* Gere le retour a la ligne*/
		else if (  lettre_x + (strlen(texte[j])*lettre_space_x)>= box_size_x-6) {
		        lettre_x=box_x+6;
		        lettre_y=lettre_y+lettre_space_y;
		}

		/* Affichage des mots */
		sdlShowTexte(*pSdlJeu, texte[j],lettre_x,lettre_y,50);
		lettre_x=lettre_x+(strlen(texte[j])+1)*lettre_space_x;


	}

	/* Boucle a la fin du dialogue*/
        while (boucle==0){
                    while ( SDL_PollEvent( &event ) ) {

                        if ( event.type == SDL_KEYDOWN){

                            switch ( event.key.keysym.sym ){

                                    case 'x' : /* Si c'est un événement de type "Quitter"*/
                                        boucle=1;


                                        break;
                                    default: break;

                            }
                        }

                    }
                 SDL_Delay(200);
          }


	for (i=0;i<nb_mot;i++) {

		free(texte[i]);

	    }
	free(texte);


}

void sdlAddDialog(sdlJeu *pSdlJeu, char * message,int id_map, int pos_x, int pos_y, int direction,int charset,int event){


	if ( pSdlJeu->jeu.hero.id_map==id_map && pSdlJeu->jeu.hero.pos_x == pos_x && pSdlJeu->jeu.hero.pos_y == pos_y &&  pSdlJeu->jeu.hero.direction==direction) {

		/* Si il on parle a un personnage, il se tourne vers nous */
		if (charset < NB_NPC_CHARSET && event ==0) {
			switch (direction) {

				case 1:  pSdlJeu->jeu.field[pos_y-1][pos_x].chipset=charset+1+182;
					break;
				case 2: pSdlJeu->jeu.field[pos_y+1][pos_x].chipset=charset+182;
					break;
				case 3: pSdlJeu->jeu.field[pos_y][pos_x-1].chipset=charset+3+182;
					break;
				case 4: pSdlJeu->jeu.field[pos_y][pos_x+1].chipset=charset+2+182;
					break;
				default: break;
			}


		}

		sdljeuAff(pSdlJeu,0,0) ;
		sdlAffHeros(pSdlJeu,4);
		SDL_Flip( pSdlJeu->surface_ecran );
		if (event ==0) {
            sdlShowDialog(pSdlJeu,message);
        }
        if (event ==1 || (event ==2 && pSdlJeu->jeu.scenario==0) || (event == 3 && pSdlJeu->jeu.scenario==1) || event==4){
            showEvent(pSdlJeu,event);

        }

   	 }
}

void addNpcDialog(sdlJeu *pSdlJeu){

    int i;

    for(i=0;i<NB_NPC_MAX;i++){

        sdlAddDialog(pSdlJeu,pSdlJeu->jeu.charset[i].dialogue,pSdlJeu->jeu.charset[i].id_map,pSdlJeu->jeu.charset[i].pos_x,pSdlJeu->jeu.charset[i].pos_y-1,2,pSdlJeu->jeu.charset[i].image,0);
        sdlAddDialog(pSdlJeu,pSdlJeu->jeu.charset[i].dialogue,pSdlJeu->jeu.charset[i].id_map,pSdlJeu->jeu.charset[i].pos_x,pSdlJeu->jeu.charset[i].pos_y+1,1,pSdlJeu->jeu.charset[i].image,0);
        sdlAddDialog(pSdlJeu,pSdlJeu->jeu.charset[i].dialogue,pSdlJeu->jeu.charset[i].id_map,pSdlJeu->jeu.charset[i].pos_x-1,pSdlJeu->jeu.charset[i].pos_y,4,pSdlJeu->jeu.charset[i].image,0);
        sdlAddDialog(pSdlJeu,pSdlJeu->jeu.charset[i].dialogue,pSdlJeu->jeu.charset[i].id_map,pSdlJeu->jeu.charset[i].pos_x+1,pSdlJeu->jeu.charset[i].pos_y,3,pSdlJeu->jeu.charset[i].image,0);


    }






}

void showEvent(sdlJeu *pSdlJeu,int id_event){
    int boucle = 1;
    int hauteur= 70;
    int ind =1;
    int i,j;
    SDL_Event event;


    switch (id_event){
        /* Centre Pokemon */
        case 1 :
                sdlShowDialog(pSdlJeu,"Bonjour! Bienvenue au CENTRE POKEMON. Nous pouvons soigner vos POKEMON. Voulez vous les soigner?");
                while (boucle){
                    SDL_apply_surface(pSdlJeu,pSdlJeu->surface_Menu_Sprite[44],pSdlJeu->surface_ecran,90,60);
                    SDL_apply_surface(pSdlJeu,pSdlJeu->surface_Menu_Sprite[38],pSdlJeu->surface_ecran,95,hauteur);
                    SDL_Flip( pSdlJeu->surface_ecran );
                      while ( SDL_PollEvent( &event ) ){



                SDL_Delay(60);


                 if ( event.type == SDL_KEYDOWN)
                      {
                        switch ( event.key.keysym.sym )
                        {
                            case DOWN: if(ind==1)
                                    {
                                        ind++;hauteur=hauteur+20;
                                    }
                                    break;

                            case UP: if(ind==2){
                                        ind--;hauteur=hauteur-20;}
                                    break;

                            case A: if(ind==1){
                                        sdljeuAff(pSdlJeu,0,0);
                                        sdlAffHeros(pSdlJeu,4);
                                         SDL_Flip( pSdlJeu->surface_ecran );
                                        sdlShowDialog(pSdlJeu,"Okay puis-je voir vos POKEMON?");
                                        pSdlJeu->jeu.field[5][7].chipset++;
                                        /*Bruitage*/
                                        sdljeuAff(pSdlJeu,0,0);
                                        sdlAffHeros(pSdlJeu,4);
                                        SDL_Flip(pSdlJeu->surface_ecran);
                                        SDL_Delay(1600);
                                        pSdlJeu->jeu.field[5][7].chipset--;
                                        sdljeuAff(pSdlJeu,0,0);
                                        sdlAffHeros(pSdlJeu,4);

                                        /*Bruitage*/
                                        for(i=0;i<pSdlJeu->jeu.hero.nb_pok;i++){
                                            pSdlJeu->jeu.hero.pokemon[i].pv=pSdlJeu->jeu.hero.pokemon[i].pvMax;
                                            for(j=0;j<4;j++){
                                                 pSdlJeu->jeu.hero.pokemon[i].moves[j].pp= pSdlJeu->jeu.hero.pokemon[i].moves[j].ppTotal;

                                            }
                                        }

                                        SDL_Flip(pSdlJeu->surface_ecran);
                                        sdlShowDialog(pSdlJeu,"Merci d'avoir attendu, vos pokemon sont en super-forme!");
                                    }
                                    sdljeuAff(pSdlJeu,0,0);
                                    sdlAffHeros(pSdlJeu,4);
                                    sdlShowDialog(pSdlJeu,"Encore merci et a bientot!");
                                    boucle=0;
                                    break;

                            case B:
                                sdljeuAff(pSdlJeu,0,0);
                                        sdlAffHeros(pSdlJeu,4);
                                        sdlShowDialog(pSdlJeu,"Encore merci et a bientot!");
                                    boucle=0;

                            default:break;
                        }
                    }
                    SDL_Flip( pSdlJeu->surface_ecran );


            }


        }
       break;

        /* Dialogue de la mere */

        case 2:SDL_Delay(300);
            pSdlJeu->jeu.field[9][11].chipset-=2;
            sdljeuAff(pSdlJeu,0,0);
            sdlAffHeros(pSdlJeu,4);
            SDL_Flip(pSdlJeu->surface_ecran);
            SDL_Delay(1000);
            pSdlJeu->jeu.field[9][11].chipset+=3;
            sdljeuAff(pSdlJeu,0,0);
            sdlAffHeros(pSdlJeu,4);
            SDL_Flip(pSdlJeu->surface_ecran);
            for(i=0;i<3;i++){

                moveDir(&pSdlJeu->jeu,&pSdlJeu->jeu.hero,0,2);
                sdljeuAff(pSdlJeu,0,4);
                sdlAffHeros(pSdlJeu,1);
                SDL_Flip(pSdlJeu->surface_ecran);
                SDL_Delay(100);

                sdljeuAff(pSdlJeu,0,8);
                sdlAffHeros(pSdlJeu,2);
                SDL_Flip(pSdlJeu->surface_ecran);
                SDL_Delay(100);

                sdljeuAff(pSdlJeu,0,12);
                sdlAffHeros(pSdlJeu,3);
                SDL_Flip(pSdlJeu->surface_ecran);
                SDL_Delay(100);

                moveDir(&pSdlJeu->jeu,&pSdlJeu->jeu.hero,1,2);
                sdljeuAff(pSdlJeu,0,0) ;
                sdlAffHeros(pSdlJeu,4);
                SDL_Flip(pSdlJeu->surface_ecran);
                SDL_Delay(100);

            }


                moveDir(&pSdlJeu->jeu,&pSdlJeu->jeu.hero,0,3);
                sdljeuAff(pSdlJeu,-4,0);
                sdlAffHeros(pSdlJeu,1);
                SDL_Flip(pSdlJeu->surface_ecran);
                SDL_Delay(100);

                sdljeuAff(pSdlJeu,-8,0);
                sdlAffHeros(pSdlJeu,2);
                SDL_Flip(pSdlJeu->surface_ecran);
                SDL_Delay(100);

                sdljeuAff(pSdlJeu,-12,0);
                sdlAffHeros(pSdlJeu,3);
                SDL_Flip(pSdlJeu->surface_ecran);
                SDL_Delay(100);

                moveDir(&pSdlJeu->jeu,&pSdlJeu->jeu.hero,1,3);
                sdljeuAff(pSdlJeu,0,0) ;
                sdlAffHeros(pSdlJeu,4);
                SDL_Flip(pSdlJeu->surface_ecran);
                SDL_Delay(100);


            sdlShowDialog(pSdlJeu,"MAMAN: Notre voisin, PROF. CHEN, te cherche. Il a dit qu'il voulait te voir c'est tres important! Au fait, avant de partir prends un peu d'argent.");
            sdlShowDialog(pSdlJeu,"Vous recevez 50E de la part de votre MAMAN!");
            sdljeuAff(pSdlJeu,0,0) ;
            sdlAffHeros(pSdlJeu,4);
            SDL_Flip(pSdlJeu->surface_ecran);
            pSdlJeu->jeu.hero.argent+=50;
            pSdlJeu->jeu.scenario=1;
            boucle=0;
            break;

        /* Dialogue du prof Chen*/
        case 3:
            for(i=0;i<9;i++){

                moveDir(&pSdlJeu->jeu,&pSdlJeu->jeu.hero,0,1);
                sdljeuAff(pSdlJeu,0,-4);
                sdlAffHeros(pSdlJeu,1);
                SDL_Flip(pSdlJeu->surface_ecran);
                SDL_Delay(100);

                sdljeuAff(pSdlJeu,0,-8);
                sdlAffHeros(pSdlJeu,2);
                SDL_Flip(pSdlJeu->surface_ecran);
                SDL_Delay(100);

                sdljeuAff(pSdlJeu,0,-12);
                sdlAffHeros(pSdlJeu,3);
                SDL_Flip(pSdlJeu->surface_ecran);
                SDL_Delay(100);

                moveDir(&pSdlJeu->jeu,&pSdlJeu->jeu.hero,1,1);
                sdljeuAff(pSdlJeu,0,0) ;
                sdlAffHeros(pSdlJeu,4);
                SDL_Flip(pSdlJeu->surface_ecran);
                SDL_Delay(100);


            }
            pSdlJeu->jeu.hero.direction=4;
            pSdlJeu->jeu.field[6][9].chipset++;
            sdljeuAff(pSdlJeu,0,0) ;
            sdlAffHeros(pSdlJeu,4);
            SDL_Flip(pSdlJeu->surface_ecran);


            sdlShowDialog(pSdlJeu,"CHEN: SASHA! Tu es la! Comme je t'avais promis, je te donnes un de mes POKEMON comme coequiper. On vient de les decouvrir ! Vas-y Choisis en un! ");

            sdljeuAff(pSdlJeu,0,0);
            sdlAffHeros(pSdlJeu,4);

            while(boucle){

            SDL_apply_surface(pSdlJeu,pSdlJeu->surface_Menu_Sprite[46],pSdlJeu->surface_ecran,40,40);
                switch(ind){


                    case 1: SDL_apply_surface(pSdlJeu,pSdlJeu->surface_Menu_Sprite[38],pSdlJeu->surface_ecran,110,70);
                            SDL_apply_surface(pSdlJeu,pSdlJeu->surface_Pokemon_Sprite[1],pSdlJeu->surface_ecran,57,52);
                            sdlShowTexte(*pSdlJeu,"BULBIZARRE",47,105,0);
                            break;
                    case 2: SDL_apply_surface(pSdlJeu,pSdlJeu->surface_Menu_Sprite[38],pSdlJeu->surface_ecran,110,70);
                            SDL_apply_surface(pSdlJeu,pSdlJeu->surface_Menu_Sprite[47],pSdlJeu->surface_ecran,50,70);
                            SDL_apply_surface(pSdlJeu,pSdlJeu->surface_Pokemon_Sprite[4],pSdlJeu->surface_ecran,54,52);
                            sdlShowTexte(*pSdlJeu,"SALAMECHE",47,105,0);
                            break;
                    case 3:
                            SDL_apply_surface(pSdlJeu,pSdlJeu->surface_Menu_Sprite[47],pSdlJeu->surface_ecran,50,70);
                            SDL_apply_surface(pSdlJeu,pSdlJeu->surface_Pokemon_Sprite[7],pSdlJeu->surface_ecran,54,52);
                            sdlShowTexte(*pSdlJeu,"CARAPUCE",47,105,0);
                            break;

                }
                SDL_Flip(pSdlJeu->surface_ecran);

                while ( SDL_PollEvent( &event )  ){





                SDL_Delay(60);


                    if ( event.type == SDL_KEYDOWN){

                        switch ( event.key.keysym.sym ){

                            case LEFT: if(ind>1){
                                            ind--;
                                        }
                                        break;

                            case RIGHT: if(ind<3){
                                            ind++;
                                        }break;
                            case A: switch (ind) {
                                        case 1: sdlShowDialog(pSdlJeu,"CHEN: Tu as choisi BULBIZARRE, C'est un POKEMON type PLANTE.");
                                                pSdlJeu->jeu.hero.pokemon[1] = createPokemon("BULBIZARRE" ,1, 2 , 1 ,"Votre premier pokemon",100, 2, 10, 4,5,2,5,2,2,6, 2,1,3);
                                                pSdlJeu->jeu.hero.nb_pok++;
                                                pSdlJeu->jeu.hero.pokemon[1].moves[3]= CreateAttack("CHARGE",3,35,35, 0,100,"Charge l'ennemi avec un violent plaquage.");
                                                pSdlJeu->jeu.hero.pokemon[1].moves[2]= CreateAttack("FOUET-LIANES",2,35,10,0,100,"lance des plante");
                                                pSdlJeu->jeu.hero.pokemon[1].moves[0]= CreateAttack("LANCE-SOLEIL",2,120,10,1,100,"Envoie un puissant jet d'eau pour frapper l'ennemi.");
                                                pSdlJeu->jeu.hero.pokemon[1].moves[1]= CreateAttack("TRANCHE",3,70,20,0,100,"Une puissante attaque de feu pouvant brûler l'ennemi.");
                                                pSdlJeu->jeu.hero.pokemon[1].vu =1;
                                                pSdlJeu->jeu.hero.pokemon[1].capture=3;
                                                boucle=0;
                                                break;
                                        case 2: sdlShowDialog(pSdlJeu,"CHEN: Tu as choisi SALAMECHE, C'est un POKEMON type FEU.");
                                                pSdlJeu->jeu.hero.pokemon[1] =  createPokemon("SALAMECHE" ,2, 1 , 1 ,"Votre premier pokemon",100, 2, 10, 4,5,2,5,2,2,6, 2,1,3);
                                                pSdlJeu->jeu.hero.nb_pok++;
                                                pSdlJeu->jeu.hero.pokemon[1].moves[3]= CreateAttack("GRIFFE",3,40,35, 0,100,"Charge l'ennemi avec un violent plaquage.");
                                                pSdlJeu->jeu.hero.pokemon[1].moves[2]= CreateAttack("FLAMMECHE",1,40,25,1,100,"lance des plante");
                                                pSdlJeu->jeu.hero.pokemon[1].moves[0]= CreateAttack("LANCE-FLAMME",1,40,25,1,100,"Envoie un puissant jet d'eau pour frapper l'ennemi.");
                                                pSdlJeu->jeu.hero.pokemon[1].moves[1]= CreateAttack("TRANCHE",3,80,15,0,100,"Une puissante attaque de feu pouvant brûler l'ennemi.");
                                                pSdlJeu->jeu.hero.pokemon[1].vu =1;
                                                pSdlJeu->jeu.hero.pokemon[1].capture=3;
                                                boucle=0;
                                                break;
                                        case 3: sdlShowDialog(pSdlJeu,"CHEN: Tu as choisi CARAPUCE, C'est un POKEMON type EAU.");
                                                pSdlJeu->jeu.hero.pokemon[1] = createPokemon("CARAPUCE" ,3, 0 , 1 ,"Votre premier pokemon",100, 2, 10, 4,5,2,5,2,2,6, 2,1,3);
                                                pSdlJeu->jeu.hero.nb_pok++;
                                                pSdlJeu->jeu.hero.pokemon[1].moves[3]= CreateAttack("CHARGE",3,35,35, 0,100,"Charge l'ennemi avec un violent plaquage.");
                                                pSdlJeu->jeu.hero.pokemon[1].moves[2]= CreateAttack("HYDROCANON",0,110,5,1,80,"lance des plante");
                                                pSdlJeu->jeu.hero.pokemon[1].moves[0]= CreateAttack("PISTOLET-A-O",0,40,25,1,100,"Envoie un puissant jet d'eau pour frapper l'ennemi.");
                                                pSdlJeu->jeu.hero.pokemon[1].moves[1]= CreateAttack("FORCE",3,80,15,0,100,"Une puissante attaque de feu pouvant brûler l'ennemi.");
                                                pSdlJeu->jeu.hero.pokemon[1].vu =1;
                                                pSdlJeu->jeu.hero.pokemon[1].capture=3;
                                                boucle=0;
                                                break;
                                    }
                                    pSdlJeu->jeu.pokedex[pSdlJeu->jeu.hero.pokemon[1].number] = pSdlJeu->jeu.hero.pokemon[1];
                                    pSdlJeu->jeu.pokedex[pSdlJeu->jeu.hero.pokemon[1].number].vu=1;
                                    pSdlJeu->jeu.hero.nbPokeVu++;


                                    break;
                            default : break;

                        }
                    }
                    SDL_Flip(pSdlJeu->surface_ecran);
                }
            }
            sdljeuAff(pSdlJeu,0,0);
            sdlAffHeros(pSdlJeu,4);
            SDL_Flip(pSdlJeu->surface_ecran);
            sdlShowDialog(pSdlJeu,"CHEN: C'est un tres bon choix! Je sens que tu deviendras un grand dresseur SASHA! Commence par te diriger vers la route au sud pour capturer d'autres POKEMON. Utilises des POKEBALL quand les POKEMON sauvages sont affaiblis pour les capturer!");
            sdlShowDialog(pSdlJeu,"Vous recevez : SuperPokeBall!");
            ajout(pSdlJeu, 6);
            pSdlJeu->jeu.scenario=2;
        break;

        case 4: affMarket(pSdlJeu);
            break;

}

}

void sdlfightAff(sdlJeu *pSdlJeu){



    char nom[100];
    int boucle = 0;
    int ind = 1;/*pour connaitre la position de la souri*/
    int ind2 = 1;
    int enCombat = 0;
    int hauteur = 120;/*pour déplacer la souri*/
    int largeur = 55;
    int i;
    int j=0;
    int pv =0;
    char c[100];
    int perdu =0;
    srand(time(NULL));
    int indice;
    int gainXp = 0;
    int deja =0;
    int vu =0;

    for(i=0;i<20;i++){
        if(pSdlJeu->jeu.pokedex[i].number ==pSdlJeu->jeu.wildPokemon.number )
            {vu = 1;}

    }
    if(vu==0){
        pSdlJeu->jeu.pokedex[pSdlJeu->jeu.wildPokemon.number-1]=pSdlJeu->jeu.wildPokemon;
         pSdlJeu->jeu.pokedex[pSdlJeu->jeu.wildPokemon.number-1].vu=1;
          pSdlJeu->jeu.hero.nbPokeVu ++;
    }

    pSdlJeu->jeu.hero.enCombat = 1;/*on précise qu'on est en combat*/


    SDL_Event event;

    SDL_Init( SDL_INIT_EVERYTHING);




    SDL_apply_surface( pSdlJeu, pSdlJeu->surface_Menu_Sprite[42], pSdlJeu->surface_ecran, 0, 0);

    SDL_apply_surface( pSdlJeu,pSdlJeu->surface_Pokemon_Sprite[3*pSdlJeu->jeu.wildPokemon.number-2], pSdlJeu->surface_ecran,90,11);
    strcpy(nom,"");



    for(i=0;i<pSdlJeu->jeu.hero.nb_pok;i++){

	    SDL_apply_surface(pSdlJeu, pSdlJeu->surface_Menu_Sprite[26], pSdlJeu->surface_ecran,105+i*10,90);
    }


    SDL_Flip( pSdlJeu->surface_ecran );

    sdlShowDialog( pSdlJeu, "Un pokemon sauvage nous attaque !");

    sprintf(c, "Go %s !!!", pSdlJeu->jeu.hero.pokemon[enCombat].name);

    sdlShowDialog( pSdlJeu, c);

    affPv(pSdlJeu, 1);

    while(boucle == 0)
    {




/*-------------------------partie gèrant k.o de notre pokemon-------------------------------------------------------------------*/

		  if(isdead( pSdlJeu->jeu.hero.pokemon[enCombat] )==1) /*si notre pokemon est K.O*/
		{

			sprintf(nom,"%s est mort.",pSdlJeu->jeu.hero.pokemon[enCombat].name);
			sdlShowDialog(pSdlJeu, nom);
			strcpy(nom,"");

			for(i=0;i<pSdlJeu->jeu.hero.nb_pok;i++){

				if(pSdlJeu->jeu.hero.pokemon[i].pv<1){j++;}


			}

			if(j==pSdlJeu->jeu.hero.nb_pok)
			{
			 pSdlJeu->jeu.hero.etatPoke = 1;
			sprintf(nom,"%s","Vous avez perdu.");
			sdlShowDialog(pSdlJeu, nom);
			strcpy(nom,"");
			teleportationHeros(&pSdlJeu->jeu,2,5,8,4,PARQUET);
			for(i=0;i<pSdlJeu->jeu.hero.nb_pok;i++){
                    pSdlJeu->jeu.hero.pokemon[i].pv=1;
			}
			boucle = 1;
			perdu=1;

			}
			else{


			pokemonCombat( pSdlJeu, &enCombat);/*on choisi un autre pokemon en reserve*/
			}
		}

/*------------------------- FIN partie gèrant k.o de notre pokemon-------------------------------------------------------------------*/




/*-------------------------------------------------------------------------------------------------------------*/





/*-------------------------partie animation attaque de notre pokemon------------------------------------------------------*/



		if(perdu==0){
		/*on affiche les annimation selon le type d'attaque*/
		if(pSdlJeu->jeu.hero.aAttaquer != 5)
			{
           /*SDL_apply_surface( pSdlJeu, pSdlJeu->surface_Menu_Sprite[4], pSdlJeu->surface_ecran, 0, 0);

             SDL_apply_surface( pSdlJeu,pSdlJeu->surface_Pokemon_Sprite[3*pSdlJeu->jeu.hero.pokemon[enCombat].number-3], pSdlJeu->surface_ecran,22,54);
             SDL_apply_surface(pSdlJeu,pSdlJeu->surface_Pokemon_Sprite[3*pSdlJeu->jeu.wildPokemon.number-2], pSdlJeu->surface_ecran,90,11);
             sdlShowTexte(*pSdlJeu, pSdlJeu->jeu.hero.pokemon[enCombat].name,79,72,0);
			sdlShowTexte(*pSdlJeu, pSdlJeu->jeu.wildPokemon.name,9,15,0);*/

            indice = rand_ab(0,3);
			SDL_apply_surface(pSdlJeu,  pSdlJeu->surface_Fight_Animation[pSdlJeu->jeu.hero.pokemon[enCombat].moves[pSdlJeu->jeu.hero.aAttaquer].type*4], pSdlJeu->surface_ecran,96,18);
			SDL_Flip( pSdlJeu->surface_ecran );
			SDL_Delay(200);
			strcpy(nom,"");


			SDL_apply_surface(pSdlJeu,   pSdlJeu->surface_Fight_Animation[pSdlJeu->jeu.hero.pokemon[enCombat].moves[pSdlJeu->jeu.hero.aAttaquer].type*4+1], pSdlJeu->surface_ecran,96,18);
			SDL_Flip( pSdlJeu->surface_ecran );
			SDL_Delay(200);
			strcpy(nom,"");

			SDL_apply_surface( pSdlJeu,  pSdlJeu->surface_Fight_Animation[pSdlJeu->jeu.hero.pokemon[enCombat].moves[pSdlJeu->jeu.hero.aAttaquer].type*4+2], pSdlJeu->surface_ecran,96,18);
			SDL_Flip( pSdlJeu->surface_ecran );
			SDL_Delay(200);
			strcpy(nom,"");


			SDL_apply_surface( pSdlJeu,  pSdlJeu->surface_Fight_Animation[pSdlJeu->jeu.hero.pokemon[enCombat].moves[pSdlJeu->jeu.hero.aAttaquer].type*4+3], pSdlJeu->surface_ecran,96,18);
			SDL_Flip( pSdlJeu->surface_ecran );
			SDL_Delay(400);
			strcpy(nom,"");


			pSdlJeu->jeu.hero.aAttaquer = 5; /*je le remet a 0*/

            affPv(pSdlJeu, 1);affPv(pSdlJeu, 2);

            /*on réaffiche l'écran de combat*/
			sdlShowTexte(*pSdlJeu, pSdlJeu->jeu.hero.pokemon[enCombat].name,79,72,0);
			sdlShowTexte(*pSdlJeu, pSdlJeu->jeu.wildPokemon.name,9,15,0);
			SDL_apply_surface( pSdlJeu,pSdlJeu->surface_Pokemon_Sprite[3*pSdlJeu->jeu.hero.pokemon[enCombat].number-3], pSdlJeu->surface_ecran,22,54);
			strcpy(nom,"");

            SDL_apply_surface( pSdlJeu, pSdlJeu->surface_Menu_Sprite[9], pSdlJeu->surface_ecran, 0, 0);
			SDL_apply_surface( pSdlJeu,pSdlJeu->surface_Menu_Sprite[38], pSdlJeu->surface_ecran, largeur, hauteur);
			affPv(pSdlJeu, 1);affPv(pSdlJeu, 2);



/*-------------------------FIN partie animation attaque de notre pokemon------------------------------------------------------*/





/*--------------------------partie animation attaque du pokemon ennemi---------------------------------------------------------*/


			 if (isdead(pSdlJeu->jeu.wildPokemon)==1){


				SDL_apply_surface(pSdlJeu,pSdlJeu->surface_Pokemon_Sprite[3*pSdlJeu->jeu.hero.pokemon[enCombat].number-3], pSdlJeu->surface_ecran,22,54);
				strcpy(nom,"");
				boucle =1;
				sdlShowTexte(*pSdlJeu, pSdlJeu->jeu.hero.pokemon[enCombat].name,79,72,0);
				sdlShowTexte(*pSdlJeu, pSdlJeu->jeu.wildPokemon.name,9,15,0);
				sprintf(nom,"%s est KO.",pSdlJeu->jeu.wildPokemon.name);
				sdlShowDialog(pSdlJeu, nom);
				gainXp = (pSdlJeu->jeu.wildPokemon.lvl )*( pSdlJeu->jeu.hero.pokemon[enCombat].xp)/7;/*on calcule le xp que notre pokemon gagne*/
						sprintf(nom,"%s est KO, vous avez gagnez. %s a gagne %d xp et 10 E.",pSdlJeu->jeu.wildPokemon.name,pSdlJeu->jeu.hero.pokemon[enCombat].name,gainXp);
						 pSdlJeu->jeu.hero.pokemon[enCombat].xp=pSdlJeu->jeu.hero.pokemon[enCombat].xp+gainXp;/*on remettre le xp de notre pokemon à jours*/
						 levelUp(&pSdlJeu->jeu.hero.pokemon[enCombat]);
						sdlShowDialog(pSdlJeu, nom);
						pSdlJeu->jeu.hero.argent=pSdlJeu->jeu.hero.argent+50;
						 affPv(pSdlJeu, 2);


				}

			else{/*si le pokemon ennemi n'est pas mort alors il nous attaque*/


				 pv= calculdmg( pSdlJeu->jeu.wildPokemon,indice,pSdlJeu->jeu.hero.pokemon[enCombat]);
				modifpv(pv, &pSdlJeu->jeu.hero.pokemon[enCombat]);


				sprintf(c,"%s a attaque avec %s.",pSdlJeu->jeu.wildPokemon.name, pSdlJeu->jeu.wildPokemon.moves[indice].name);



				/*on affiche l'image du pokemon ennemi*/
				SDL_apply_surface(pSdlJeu,pSdlJeu->surface_Pokemon_Sprite[3*pSdlJeu->jeu.wildPokemon.number-2], pSdlJeu->surface_ecran,90,11);


				/*on affiche les noms des 2 pokemons en combat*/
				sdlShowTexte(*pSdlJeu, pSdlJeu->jeu.hero.pokemon[enCombat].name,79,72,0);
				sdlShowTexte(*pSdlJeu, pSdlJeu->jeu.wildPokemon.name,9,15,0);


				/*on affiche l'image de notre pokemon*/

				SDL_apply_surface(pSdlJeu, pSdlJeu->surface_Pokemon_Sprite[3*pSdlJeu->jeu.hero.pokemon[enCombat].number-3], pSdlJeu->surface_ecran,22,54);
				strcpy(nom,"");

				SDL_Delay(400);
				sdlShowDialog(pSdlJeu, c);
				strcpy(c,"");

				SDL_apply_surface( pSdlJeu,  pSdlJeu->surface_Fight_Animation[pSdlJeu->jeu.wildPokemon.moves[indice].type*4], pSdlJeu->surface_ecran,15,55);
				SDL_Flip( pSdlJeu->surface_ecran );
				SDL_Delay(200);
				strcpy(nom,"");


				SDL_apply_surface( pSdlJeu, pSdlJeu->surface_Fight_Animation[pSdlJeu->jeu.wildPokemon.moves[indice].type*4+1], pSdlJeu->surface_ecran,15,55);
				SDL_Flip( pSdlJeu->surface_ecran );
				SDL_Delay(200);
				strcpy(nom,"");


				SDL_apply_surface( pSdlJeu, pSdlJeu->surface_Fight_Animation[pSdlJeu->jeu.wildPokemon.moves[indice].type*4+2], pSdlJeu->surface_ecran,15,55);
				SDL_Flip( pSdlJeu->surface_ecran );
				SDL_Delay(200);
				strcpy(nom,"");

				SDL_apply_surface( pSdlJeu,pSdlJeu->surface_Fight_Animation[pSdlJeu->jeu.wildPokemon.moves[indice].type*4+3], pSdlJeu->surface_ecran,15,55);
				SDL_Flip( pSdlJeu->surface_ecran );
				SDL_Delay(400);
				strcpy(nom,"");


                /****fin animation de combat*****/

                affPv(pSdlJeu, 2);  affPv(pSdlJeu, 1);/*on met à jours le pv de l'ennemi*/
				SDL_apply_surface( pSdlJeu, pSdlJeu->surface_Menu_Sprite[9], pSdlJeu->surface_ecran, 0, 0);
				SDL_apply_surface( pSdlJeu,pSdlJeu->surface_Menu_Sprite[38], pSdlJeu->surface_ecran, largeur, hauteur);





				/*on affiche l'image de notre pokemon*/
				SDL_apply_surface(pSdlJeu, pSdlJeu->surface_Pokemon_Sprite[3*pSdlJeu->jeu.hero.pokemon[enCombat].number-3], pSdlJeu->surface_ecran,22,54);
				strcpy(nom,"");




				/*on affiche l'image du pokemon ennemi*/

				SDL_apply_surface( pSdlJeu,pSdlJeu->surface_Pokemon_Sprite[3*pSdlJeu->jeu.wildPokemon.number-2], pSdlJeu->surface_ecran,90,11);


				/*on affiche les noms des 2 pokemons en combat*/
				sdlShowTexte(*pSdlJeu, pSdlJeu->jeu.hero.pokemon[enCombat].name,79,72,0);
				sdlShowTexte(*pSdlJeu, pSdlJeu->jeu.wildPokemon.name,9,15,0);
                affPv(pSdlJeu, 1);/*On réaffiche les pv*/
                affPv(pSdlJeu, 2);

			}


			SDL_Flip( pSdlJeu->surface_ecran );



			}
/*--------------------------FIN   partie animation attaque du pokemon ennemi---------------------------------------------------------*/


/*-------------------------------Réaffichage écran combat si il y a pas d'ATTAQUE---------------------------------------------*/


			else{

				SDL_apply_surface( pSdlJeu, pSdlJeu->surface_Menu_Sprite[9], pSdlJeu->surface_ecran, 0, 0);
				SDL_apply_surface(pSdlJeu, pSdlJeu->surface_Menu_Sprite[38], pSdlJeu->surface_ecran, largeur, hauteur);

				/*on affiche l'image de notre pokemon*/

				SDL_apply_surface(pSdlJeu, pSdlJeu->surface_Pokemon_Sprite[3*pSdlJeu->jeu.hero.pokemon[enCombat].number-3], pSdlJeu->surface_ecran,22,54);
				strcpy(nom,"");

				/*on affiche l'image du pokemon ennemi*/
                SDL_apply_surface( pSdlJeu,pSdlJeu->surface_Pokemon_Sprite[3*pSdlJeu->jeu.wildPokemon.number-2], pSdlJeu->surface_ecran,90,11);


				/*on affiche les noms des 2 pokemons en combat*/
				sdlShowTexte(*pSdlJeu, pSdlJeu->jeu.hero.pokemon[enCombat].name,79,72,0);
				sdlShowTexte(*pSdlJeu, pSdlJeu->jeu.wildPokemon.name,9,15,0);
				affPv(pSdlJeu, 1);
                affPv(pSdlJeu, 2);


			if(pSdlJeu->jeu.wildPokemon.capture == 1)
			{
				sprintf(nom,"Vous avez capture %s .",pSdlJeu->jeu.wildPokemon.name);
				sdlShowDialog(pSdlJeu, nom);
				strcpy(nom,"");

				if(pSdlJeu->jeu.hero.nb_pok<5){
					pSdlJeu->jeu.hero.pokemon[pSdlJeu->jeu.hero.nb_pok]=pSdlJeu->jeu.wildPokemon;
					pSdlJeu->jeu.hero.nb_pok++;
				 }
				else{
					if(pSdlJeu->jeu.hero.nb_pokReserve<200){
						pSdlJeu->jeu.hero.pokemon[pSdlJeu->jeu.hero.nb_pokReserve]=pSdlJeu->jeu.wildPokemon;
						pSdlJeu->jeu.hero.nb_pokReserve++;
				        }

				}
				boucle = 1;
				perdu=1;
  			}





  			if(deja==2){
			sprintf(nom,"Vous avez pas reussit a capturer %s .",pSdlJeu->jeu.wildPokemon.name);
			sdlShowDialog(pSdlJeu, nom);
			strcpy(nom,"");
			 pSdlJeu->jeu.wildPokemon.capture = 0;
			deja=0;
			 }

				SDL_Flip( pSdlJeu->surface_ecran );





				}

				/*---------------------------FIN Réaffichage écran combat si il y a pas d'ATTAQUE---------------------------------------------*/









        while ( SDL_PollEvent( &event ) )
            {

		 if ( event.type == SDL_KEYDOWN)
		  {
			switch ( event.key.keysym.sym )
			{
				case B:
					    boucle =1;
				break;

				case UP:
					 if(ind>1){ind--; hauteur =hauteur -16; }
				break;

				case DOWN:
					 if(ind<2)
					 {
						ind++;
						hauteur = hauteur +16;
					 }
				break;

				case LEFT :
					if(ind2>1){ind2--; largeur =largeur -50; }
				break;

				case RIGHT  :
					if(ind2<2)
					 {
						ind2++;
						largeur = largeur +50;
					 }
				break;

			       case  A:

					switch (ind)
					{
						    case 1:
							 switch (ind2)
							{

							case 1 :

							attaque(pSdlJeu, enCombat);

							break;

							case 2 :

							 pokemonCombat( pSdlJeu, &enCombat);

							break;

							}
				       		  break;

					       case 2:/*scene de combat*/

							 switch (ind2)
							{
							case 1 :/*sac à dos*/
								objet(pSdlJeu);
								if(pSdlJeu->jeu.wildPokemon.capture==2){ deja=2;}

							break;

							case 2 : /*fuite*/
							/*à faire : affiche boite dialogue*/
							boucle = 1;

							break;

							}

					    break;

					default: break;

		      			}

				break;


			default: break;
		        }
		 }

		 }

		 if ( event.type == SDL_QUIT )
		        {	pSdlJeu->jeu.boucle = 0;
		            boucle = 1;

		            SDL_Quit();
		        }
        }

    }







    pSdlJeu->jeu.hero.etatPoke = 0;

    changeSound(&pSdlJeu->jeu,1);


     pSdlJeu->jeu.hero.enCombat = 0;/*on est plus en combat*/
}

void Menu( sdlJeu *pSdlJeu){

    FILE* fichier=NULL;

    fichier= fopen("save/save.txt","r");

    int i=1;
    int j=-115;
    int continuer = 1;
    int ind = 2;
    int hauteur = 20;
    SDL_Event event;

    putenv("SDL_VIDEO_WINDOW_POS=center"); /*pour centrer la fenêtre*/

    SDL_Init( SDL_INIT_EVERYTHING);
    /* make sure SDL cleans up before exit*/
    atexit(SDL_Quit);
    SDL_WM_SetCaption( "Pokemon", NULL );

    pSdlJeu->surface_ecran = SDL_SetVideoMode(160, 160, 32, SDL_HWSURFACE);
    sdlInitSurface(pSdlJeu);
    SDL_FillRect(pSdlJeu->surface_ecran, 0, SDL_MapRGB(pSdlJeu->surface_ecran->format, 0, 0, 0));

    SDL_apply_surface(pSdlJeu,pSdlJeu->surface_Menu_Sprite[16], pSdlJeu->surface_ecran, 0, 0);
    SDL_Flip(pSdlJeu->surface_ecran);
    while (continuer){
        SDL_Flip( pSdlJeu->surface_ecran );
        SDL_Delay(80);
        SDL_PollEvent(&event);
        switch(event.type){
            case SDL_QUIT:
                SDL_FreeSurface(pSdlJeu->surface_ecran);
                FMOD_Sound_Release( pSdlJeu->jeu.son );
                FMOD_System_Close( pSdlJeu->jeu.system );
                FMOD_System_Release( pSdlJeu->jeu.system );
                exit(0);

                break;

            default: break;
        }

        if ( event.type == SDL_KEYUP){

            switch ( event.key.keysym.sym ){

				case A:


                while (continuer){
                    if (fichier != NULL){
                    SDL_apply_surface( pSdlJeu,pSdlJeu->surface_Menu_Sprite[15], pSdlJeu->surface_ecran, 0, 0);
                    SDL_apply_surface(pSdlJeu, pSdlJeu->surface_Menu_Sprite[38], pSdlJeu->surface_ecran, 17, hauteur);

                    SDL_Flip( pSdlJeu->surface_ecran );
                    SDL_WaitEvent(&event);

                    switch(event.type){

                        case SDL_QUIT:
                            FMOD_Sound_Release( pSdlJeu->jeu.son );
                            FMOD_System_Close( pSdlJeu->jeu.system );
                            FMOD_System_Release( pSdlJeu->jeu.system );
                            exit(0);
                            break;

                        default: break;
                    }

                    if ( event.type == SDL_KEYUP){
                        switch ( event.key.keysym.sym ){

                            case UP:
                                    if(ind<2){ind++; hauteur =hauteur -30; }
                            break;

                            case DOWN:
                                    if(ind>1)
                                    {
                                        ind--;
                                        hauteur = hauteur +30;
                                    }
                            break;

                            case A:/*appuye sur entré*/

                                if(ind==2){
                                changeSound(&pSdlJeu->jeu,2);
                                teleportationHeros(&pSdlJeu->jeu,pSdlJeu->jeu.hero.id_map,pSdlJeu->jeu.hero.pos_x,pSdlJeu->jeu.hero.pos_y,pSdlJeu->jeu.hero.direction,pSdlJeu->jeu.hero.sol);
                                animeIntro(pSdlJeu);
                                sdljeuBoucle( pSdlJeu );
                                continuer = 0;


                                }
                                else{
                                    loadGame(&pSdlJeu->jeu);
                                    if((pSdlJeu->jeu.hero.id_map==9)||(pSdlJeu->jeu.hero.id_map==10))
                                    {
                                        changeSound(&pSdlJeu->jeu,1);
                                    }
                                    else{changeSound(&pSdlJeu->jeu,2);}
                                    sdljeuBoucle( pSdlJeu );
                                    continuer = 0;

                                }
                                break;

                            default: break;
                        }
                    }
                    }
                    else {
                    SDL_apply_surface( pSdlJeu,pSdlJeu->surface_Menu_Sprite[45], pSdlJeu->surface_ecran, 0, 0);
                    SDL_apply_surface(pSdlJeu, pSdlJeu->surface_Menu_Sprite[38], pSdlJeu->surface_ecran, 17, hauteur);

                    SDL_Flip( pSdlJeu->surface_ecran );
                    SDL_WaitEvent(&event);

                    switch(event.type){

                        case SDL_QUIT:
                            FMOD_Sound_Release( pSdlJeu->jeu.son );
                            FMOD_System_Close( pSdlJeu->jeu.system );
                            FMOD_System_Release( pSdlJeu->jeu.system );
                            exit(0);
                            break;

                        default: break;
                    }

                    if ( event.type == SDL_KEYUP){
                        switch ( event.key.keysym.sym ){



                            case A:

                                changeSound(&pSdlJeu->jeu,2);
                                teleportationHeros(&pSdlJeu->jeu,pSdlJeu->jeu.hero.id_map,pSdlJeu->jeu.hero.pos_x,pSdlJeu->jeu.hero.pos_y,pSdlJeu->jeu.hero.direction,pSdlJeu->jeu.hero.sol);
                                animeIntro(pSdlJeu);
                                sdljeuBoucle( pSdlJeu );
                                continuer = 0;


                                break;
                             default: break;

                }



                }
            }
            }
            default : break;
            }

        }

         SDL_FillRect(pSdlJeu->surface_ecran, NULL, 0xFFFFFF);
         SDL_apply_surface(pSdlJeu,pSdlJeu->surface_Menu_Sprite[16], pSdlJeu->surface_ecran, 0, 0);
         SDL_apply_surface(pSdlJeu,pSdlJeu->surface_Intro_Animation[8], pSdlJeu->surface_ecran, j, 77);
         j=j+1;
         if(j>=0){j=-125;}
            SDL_apply_surface(pSdlJeu,pSdlJeu->surface_Intro_Animation[8+i], pSdlJeu->surface_ecran, 38, 57);
	     i++;
	     if(i==7){i=1;}

	     SDL_Flip( pSdlJeu->surface_ecran );

	     SDL_Delay(150);


    }
}

void animeIntro(sdlJeu *pSdlJeu){
    int i;
    int j;

    int boucle = 0;

    SDL_Event event;

    SDL_Init( SDL_INIT_EVERYTHING);



  while(boucle == 0)
    {



                SDL_FillRect(pSdlJeu->surface_ecran, NULL, 0xFFFFFF);


                SDL_Flip( pSdlJeu->surface_ecran );

                for(i=1;i<9;i++){

                     SDL_apply_surface(pSdlJeu,pSdlJeu->surface_Intro_Animation[i-1], pSdlJeu->surface_ecran, 60, 30);
                     SDL_Flip( pSdlJeu->surface_ecran );
                     SDL_Delay(220);
                     if(i==5){
                         sdlShowDialog(pSdlJeu,"Bienvenue dans le monde des Pokemon, Mon nom est CHEN. Mais on m'appelle le prof. POKEMON");
                         sdlShowDialog(pSdlJeu,"Ce monde est peuple de creatures appelees POKEMON.");

                          for(j=1;j<4;j++){
                             SDL_FillRect(pSdlJeu->surface_ecran, NULL, 0xFFFFFF);
                             SDL_apply_surface(pSdlJeu,pSdlJeu->surface_Intro_Animation[14+j], pSdlJeu->surface_ecran, 60, 40);
                             SDL_Flip( pSdlJeu->surface_ecran );
                             SDL_Delay(230);

                         }
                          sdlShowDialog(pSdlJeu,"Humains et pokemon vivent en parfaite harmonie, certains jouent avec les pokemon, d'autres font des combats avec eux.");

                         SDL_FillRect(pSdlJeu->surface_ecran, NULL, 0xFFFFFF);
                        SDL_apply_surface(pSdlJeu,pSdlJeu->surface_Intro_Animation[5], pSdlJeu->surface_ecran, 60, 30);
                        SDL_Flip( pSdlJeu->surface_ecran );
                        SDL_Delay(40);
                        sdlShowDialog(pSdlJeu,"Mais il reste beaucoup a apprendre sur nos amis les pokemon. De nombreux mystere plent sur le sujet. Et c'est pourquoi j'etudie les pokemon tous les jours");

                     }

                }

                for(i=1;i<7;i++){
                    SDL_FillRect(pSdlJeu->surface_ecran, NULL, 0xFFFFFF);
                     SDL_apply_surface(pSdlJeu,pSdlJeu->surface_Intro_Animation[17+i], pSdlJeu->surface_ecran, 60, 30);
                     SDL_Flip( pSdlJeu->surface_ecran );
                     SDL_Delay(220);
                }
                 sdlShowDialog(pSdlJeu,"Sasha est-tu pret ? Ta quete pokemon est sur le point de commencer.");
                  sdlShowDialog(pSdlJeu,"Joies et perils paveront ta route.");
                 sdlShowDialog(pSdlJeu,"Un monde de reve, de dangers et de pokemon t'attend ! En avant ! a plus tard !");
                SDL_FillRect(pSdlJeu->surface_ecran, NULL, 0xFFFFFF);


                for(i=6;i<13;i++){
                    SDL_FillRect(pSdlJeu->surface_ecran, NULL, 0xFFFFFF);
                    SDL_apply_surface(pSdlJeu,pSdlJeu->surface_Intro_Animation[17+i], pSdlJeu->surface_ecran, 60, 30);
                     SDL_Flip( pSdlJeu->surface_ecran );
                     if(i==9){SDL_Delay(330);}
                     else{SDL_Delay(270);}
                }
                SDL_Delay(230);
                boucle = 1;

                  if ( event.type == SDL_QUIT )
                        {
                            boucle = 1;


                        }


    }

}

void MenuJeu( sdlJeu *pSdlJeu){


    int boucle = 0;
    int hauteur = 13;/*pour déplacer la souri*/
    int ind = 1;/*pour connaitre la position de la souri*/
    SDL_Event event;


    SDL_Init( SDL_INIT_EVERYTHING);



  while(boucle == 0)
    {
        SDL_Delay(60);

        while ( SDL_PollEvent( &event ) )
            {

                SDL_apply_surface( pSdlJeu, pSdlJeu->surface_Menu_Sprite[17], pSdlJeu->surface_ecran, 76, 0);
                SDL_apply_surface( pSdlJeu, pSdlJeu->surface_Menu_Sprite[38], pSdlJeu->surface_ecran, 82, hauteur);

                SDL_Flip( pSdlJeu->surface_ecran );


                 if ( event.type == SDL_KEYDOWN)
                      {
                        switch ( event.key.keysym.sym )
                        {
                        case B:
                            boucle =1;
                        break;

                        case  START:
                            boucle=1;
                            break;

                        case UP:
                             if(ind>1){ind--; hauteur =hauteur -17; }
                        break;

                        case DOWN:
                             if(ind<6)
                             {
                                ind++;
                                hauteur = hauteur +17;


                             }
                        break;

                        case  A:/*appuye sur entré*/

                                switch (ind)
                                {
                                    case 1:/*pokédex*/
                                            pokedex(*pSdlJeu);sdljeuAff(pSdlJeu,0,0);sdlAffHeros(pSdlJeu,4);

                                    break;

                                    case 2: /*pokemon*/
                                             pokemon(pSdlJeu);sdljeuAff(pSdlJeu,0,0);sdlAffHeros(pSdlJeu,4);
                                    break;

                                    case 3:/*objet*/

                                             objet(pSdlJeu);sdljeuAff(pSdlJeu,0,0);sdlAffHeros(pSdlJeu,4);
                                    break;

                                    case 4:/*moi*/

                                             moi(*pSdlJeu);sdljeuAff(pSdlJeu,0,0);sdlAffHeros(pSdlJeu,4);
                                    break;

                                    case 5: /*sauvegarde*/
                                            sauvegarde( * pSdlJeu);sdljeuAff(pSdlJeu,0,0);sdlAffHeros(pSdlJeu,4);
                                    break;



                                    case 6:  boucle = 1; pSdlJeu->jeu.boucle = 0;

                                    break;

                                    default: break;

                                }

                        break;


                        default: break;
                        }
                       }


                  if ( event.type == SDL_QUIT )
                        {
                            boucle = 1;


                        }
                }

    }

}

void pokedex(sdlJeu pSdlJeu){

    int boucle = 0;
    int hauteur =17;
    int ind =1;
    int i;
    int k =0;

    char nbVu[3];
    char nbpok[10];

    SDL_Event event;
    SDL_Init( SDL_INIT_EVERYTHING);


    sprintf(nbVu,"%d",pSdlJeu.jeu.hero.nbPokeVu);


  while(boucle == 0)
    {

        SDL_Delay(60);
        while ( SDL_PollEvent( &event ) )
        {
		SDL_apply_surface( &pSdlJeu,  pSdlJeu.surface_Menu_Sprite[27], pSdlJeu.surface_ecran, 0, 0);/*on affiche l'écran de pokedex*/
        SDL_apply_surface( &pSdlJeu, pSdlJeu.surface_Menu_Sprite[37], pSdlJeu.surface_ecran, 61, hauteur);
		sdlShowTexte(pSdlJeu,nbVu,37,95,0);
		sprintf(nbpok,"%d",pSdlJeu.jeu.hero.nb_pok);/*on affiche le nombre de pokemon que le hero possede*/
		sdlShowTexte(pSdlJeu,nbpok,37,115,0);

		for(i=0;i<7;i++){
			if(pSdlJeu.jeu.pokedex[i+k].vu == 1){
				sdlShowTexte(pSdlJeu,pSdlJeu.jeu.pokedex[i+k].name,74,24+i*16,0);/*on affiche les pokemon qui ont été vu*/
			}
			else{
				SDL_apply_surface( &pSdlJeu,  pSdlJeu.surface_Menu_Sprite[41], pSdlJeu.surface_ecran, 74, 24+i*16);

			}
		}

		if(pSdlJeu.jeu.pokedex[ind+k-1].vu == 1)
		{
			SDL_apply_surface( &pSdlJeu,  pSdlJeu.surface_Pokemon_Sprite[3*pSdlJeu.jeu.pokedex[ind+k-1].number-2], pSdlJeu.surface_ecran, 4, 17);
		}
		else{
			SDL_apply_surface( &pSdlJeu, pSdlJeu.surface_Menu_Sprite[7], pSdlJeu.surface_ecran, 1, 13);
		}

        SDL_Flip( pSdlJeu.surface_ecran );
        if ( event.type == SDL_KEYDOWN){

            switch ( event.key.keysym.sym ){

				case B: boucle =1;
					    break;

				case UP: if(ind>1){ind--; hauteur =hauteur -16;/*on déplace le curseur*/}
                         if((ind==1)&&(k!=0)){k--;}/*on déplace les pokemons lorsqu'on est à l'extrêmité*/
                         break;

				case DOWN: if(ind<7){
                                ind++;
                                hauteur = hauteur +16;/*on déplace le curseur*/

                           }
                           if((ind>=7)&&(k<13)){/*on déplace les pokemons lorsqu'on est à l'extrêmité*/
                                k++;

                            }

                        break;

				case A:

					if(pSdlJeu.jeu.pokedex[ind+k-1].vu == 1)
					{

  						descrip( pSdlJeu,  ind+k-1);/*lorsqu'on selectionne un pokemon, on obtien une page de description*/
					}
                    break;


				default: break;
            }
        }


          if ( event.type == SDL_QUIT )
                {
                    boucle = 1;
                    SDL_Quit();
                }
        }

    }

}

void descrip(sdlJeu pSdlJeu, int ind){


    char nom[20];
    int boucle = 0;

    SDL_Event event;

    SDL_Init( SDL_INIT_EVERYTHING);


  while(boucle == 0)
    {

    SDL_Delay(60);
	SDL_apply_surface( &pSdlJeu,  pSdlJeu.surface_Menu_Sprite[14], pSdlJeu.surface_ecran, 0, 0);

    /*on affiche le numero du pokemon*/
 	sprintf(nom,"%d",pSdlJeu.jeu.pokedex[ind].number);
	sdlShowTexte(pSdlJeu,nom,40,75,0);
	strcpy(nom,"");
	sdlShowTexte(pSdlJeu,"TYPE  : " ,65,60,0);
	switch(pSdlJeu.jeu.pokedex[ind].type)/*on affiche le type*/
	{


		case 0:
			sdlShowTexte(pSdlJeu,"EAU" ,80,75,0);
		break;

		case 1:
			sdlShowTexte(pSdlJeu,"FEU" ,80,75,0);
		break;


		case 2:
			sdlShowTexte(pSdlJeu,"PLANTE" ,80,75,0);
		break;

		case 3:
			sdlShowTexte(pSdlJeu,"NORMAL" ,80,75,0);
		break;


		default: break;



		}
        /* on affiche le nom du pokemon*/

        sdlShowTexte(pSdlJeu,"NOM  :  ",65,30,0);
     	sdlShowTexte(pSdlJeu,pSdlJeu.jeu.pokedex[ind].name,80,45,0);
        SDL_Flip( pSdlJeu.surface_ecran );


        while ( SDL_PollEvent( &event ) )
            {




         if ( event.type == SDL_KEYDOWN)
			  {
				switch ( event.key.keysym.sym )
				{
					case B:
						    boucle =1;
					break;



					default: break;
					 }
					 }


          if ( event.type == SDL_QUIT )
                {
                    boucle = 1;
                    SDL_Quit();
                }
        }

    }

}

void pokemon(sdlJeu *pSdlJeu){
    char c[4];
    int nb;

    int larg = 16;
    int i;
    int boucle = 0;
    char nom[60];
    int ind = 1;/*pour connaitre la position de la souri*/
    int hauteur = 6;/*pour déplacer la souri*/
    int nb_pok;
    nb_pok = pSdlJeu->jeu.hero.nb_pok;
    SDL_Event event;

    SDL_Init( SDL_INIT_EVERYTHING);


  while(boucle == 0)
    {
                SDL_Delay(60);

				while ( SDL_PollEvent( &event ) )
				    {

						  SDL_apply_surface( pSdlJeu,  pSdlJeu->surface_Menu_Sprite[28], pSdlJeu->surface_ecran, 0, 0);
						  for(i=1;i<=pSdlJeu->jeu.hero.nb_pok;i++)
						    {

						       if(i==ind){
						            larg = 16;
						        }

						        else{larg = 10;}
                                /*on affiche les pokemons*/
						        SDL_apply_surface(pSdlJeu,  pSdlJeu->surface_Pokemon_Sprite[3*pSdlJeu->jeu.hero.pokemon[i-1].number-1], pSdlJeu->surface_ecran,larg,(i-1)*20);
                                sdlShowTexte(*pSdlJeu, pSdlJeu->jeu.hero.pokemon[i-1].name,40,(i-1)*20+1,0);/*le nom*/
                                nb = pSdlJeu->jeu.hero.pokemon[i-1].lvl;/*le level du pokemon*/
                                sdlShowTexte(*pSdlJeu, "Choisisez un pokemon.",3,137,0);



							if(nb==100)
							{

							sprintf(c,"%d",1);
							sdlShowTexte(*pSdlJeu, c,65,(i-1)*20+7,0);
						        strcpy(c,"");
							sprintf(c,"%d",0);
							sdlShowTexte(*pSdlJeu, c,65,(i-1)*20+7,0);
						        strcpy(c,"");
							sprintf(c,"%d",0);
							sdlShowTexte(*pSdlJeu, c,65,(i-1)*20+7,0);


						         }



							sprintf(c,"%d",nb);
							sdlShowTexte(*pSdlJeu, c,65,(i-1)*20+7,0);


						        strcpy(nom,"");
							strcpy(c,"");

						    }


						for(i=0;i<(6-pSdlJeu->jeu.hero.nb_pok);i++){


						  SDL_apply_surface(pSdlJeu, pSdlJeu->surface_Menu_Sprite[5], pSdlJeu->surface_ecran, 5, 105-i*19);/*on cache les pv par default*/

						}
							affPv(pSdlJeu, 3);/*on réaffiche les pv*/
							 SDL_apply_surface(pSdlJeu,  pSdlJeu->surface_Menu_Sprite[38], pSdlJeu->surface_ecran, 9, hauteur);
							SDL_Flip( pSdlJeu->surface_ecran );

						 if ( event.type == SDL_KEYDOWN)
								{

										switch ( event.key.keysym.sym )
										{
											case B:
												 boucle =1;
											break;

											case UP:
												 if(ind>1){ind--; hauteur =hauteur -21; }/*on désincrément ind pour faire descendre le curseur*/


											break;

											case DOWN:
												 if(ind<nb_pok)
												{
												ind++;
												hauteur = hauteur +21;/*on incrément le curseur*/

												}
											break;


											case A:
												info(pSdlJeu, ind);/*on affiche une page de question info sur le pokemon*/

											break;


											default: break;
										}
								}


							  if ( event.type == SDL_QUIT )
								{
								    boucle = 1;
								    SDL_Quit();
								}

				}

    }





}

/*même principe d'affichage que pokemon */
void pokemonCombat(sdlJeu* pSdlJeu, int *enCombat){
	char c[4];
	int nb;
    int tmp[3]={0};
    int larg = 16;
    int i;

    int boucle = 0;
    char nom[60];
    int ind = 1;/*pour connaitre la position de la souri*/
    int hauteur = 6;/*pour déplacer la souri*/
    int nb_pok;
    Spokemon tmpP;
    nb_pok = pSdlJeu->jeu.hero.nb_pok;

    SDL_Event event;

    SDL_Init( SDL_INIT_EVERYTHING);


  while(boucle == 0)
    {
                SDL_Delay(60);

				while ( SDL_PollEvent( &event ) )
				    {

						SDL_apply_surface( pSdlJeu,  pSdlJeu->surface_Menu_Sprite[28], pSdlJeu->surface_ecran, 0, 0);
						  for(i=1;i<=pSdlJeu->jeu.hero.nb_pok;i++)
						    {


						       if(i==ind){
						            larg = 16;
						        }

						        else{larg = 10;}
                                /*on affiche les pokemons*/
						        SDL_apply_surface(pSdlJeu,  pSdlJeu->surface_Pokemon_Sprite[3*pSdlJeu->jeu.hero.pokemon[i-1].number-1], pSdlJeu->surface_ecran,larg,(i-1)*20);
                                sdlShowTexte(*pSdlJeu, pSdlJeu->jeu.hero.pokemon[i-1].name,40,(i-1)*20+1,0);/*le nom*/
                                nb = pSdlJeu->jeu.hero.pokemon[i-1].lvl;/*le level du pokemon*/


							if(pSdlJeu->jeu.hero.pokemon[ind-1].pv<1)
							{
							sdlShowTexte(*pSdlJeu, "Il est K.O.",3,137,0);
							}
							else{sdlShowTexte(*pSdlJeu, "Choisisez un pokemon.",3,137,0);}


							if(nb==100)
							{

							sprintf(c,"%d",1);
							sdlShowTexte(*pSdlJeu, c,65,(i-1)*20+7,0);
						        strcpy(c,"");
							sprintf(c,"%d",0);
							sdlShowTexte(*pSdlJeu, c,65,(i-1)*20+7,0);
						        strcpy(c,"");
							sprintf(c,"%d",0);
							sdlShowTexte(*pSdlJeu, c,65,(i-1)*20+7,0);


						         }


							else if(nb<10){
							sprintf(c,"%d",nb);
							sdlShowTexte(*pSdlJeu, c,65,(i-1)*20+7,0);

							}


							else{

							tmp[0]=nb/10;
							tmp[1]=nb%10;

							 sprintf(c,"%d",tmp[0]);
							sdlShowTexte(*pSdlJeu, c,65,(i-1)*20+7,0);
						           strcpy(c,"");
							 sprintf(c,"%d",tmp[1]);
							sdlShowTexte(*pSdlJeu, c,75,(i-1)*20+7,0);


							}



						        strcpy(nom,"");
							strcpy(c,"");

						    }


						for(i=0;i<(6-pSdlJeu->jeu.hero.nb_pok);i++){


						SDL_apply_surface(pSdlJeu,  pSdlJeu->surface_Menu_Sprite[5], pSdlJeu->surface_ecran, 5, 105-i*19);


						}

							affPv(pSdlJeu, 3);
							  SDL_apply_surface(pSdlJeu,  pSdlJeu->surface_Menu_Sprite[38], pSdlJeu->surface_ecran, 9, hauteur);
							SDL_Flip( pSdlJeu->surface_ecran );

							 if ( event.type == SDL_KEYDOWN)
									{

									switch ( event.key.keysym.sym )
									{
										case B:
											 boucle =1;
										break;

										case UP:
											 if(ind>1){ind--; hauteur =hauteur -21; }


										break;

										case DOWN:
											 if(ind<nb_pok)
											{
											ind++;
											hauteur = hauteur +21;

											}
										break;


										case A:
											if(pSdlJeu->jeu.hero.pokemon[ind-1].pv>1){
											tmpP = pSdlJeu->jeu.hero.pokemon[0];/*on change de pokemon lorsque le notre est k.o*/
											pSdlJeu->jeu.hero.pokemon[0] = pSdlJeu->jeu.hero.pokemon[ind-1];
											pSdlJeu->jeu.hero.pokemon[ind-1]=tmpP;
											boucle = 1;
											}

										break;


										default: break;
									}
									}


							  if ( event.type == SDL_QUIT )
								{
								    boucle = 1;
								    SDL_Quit();
								}

				}

    }

}

void attaque(sdlJeu *pSdlJeu, int enCombat){
    int pp;
    int ppTotal;
    int pv =0;
    int i;
    int boucle = 0;
    char c[50];
    int ind = 1;/*pour connaitre la position de la souri*/
    int hauteur = 115;/*pour déplacer la souri*/
    char s[100];

    SDL_Event event;

    SDL_Init( SDL_INIT_EVERYTHING);



  while(boucle == 0)
    {

        SDL_Delay(60);
        while ( SDL_PollEvent( &event ) )
            {


                SDL_apply_surface( pSdlJeu, pSdlJeu->surface_Menu_Sprite[0], pSdlJeu->surface_ecran, 0, 80);
                SDL_apply_surface(pSdlJeu,  pSdlJeu->surface_Menu_Sprite[38], pSdlJeu->surface_ecran, 40, hauteur);
                affPv(pSdlJeu, 1);
		for(i=0;i<4;i++)/*on affiche les noms des 4 attaques (maxi 4)*/
		{
			sdlShowTexte(*pSdlJeu, pSdlJeu->jeu.hero.pokemon[enCombat].moves[i].name,52,116+i*8,0);
		}



		pp = pSdlJeu->jeu.hero.pokemon[enCombat].moves[ind-1].pp;
		ppTotal = pSdlJeu->jeu.hero.pokemon[enCombat].moves[ind-1].ppTotal;

		switch (pSdlJeu->jeu.hero.pokemon[enCombat].moves[ind-1].type)/*on affiche les types des attaque dans la petite case*/
		{

			case 0:
				sdlShowTexte(*pSdlJeu, "EAU",7,93,0);
				sprintf(c,"%d",pp);
				sdlShowTexte(*pSdlJeu, c,37,100,0);
				strcpy ( c, "");
				sprintf(c,"%d",ppTotal);
				sdlShowTexte(*pSdlJeu, c,62,100,0);
				strcpy ( c, "");
			break;

			case 1:
				sdlShowTexte(*pSdlJeu, "FEU",7,93,0);
				sprintf(c,"%d",pp);
				sdlShowTexte(*pSdlJeu, c,37,100,0);
				strcpy ( c, "");
				sprintf(c,"%d",ppTotal);
				sdlShowTexte(*pSdlJeu, c,62,100,0);
				strcpy ( c, "");
			break;

			case 2:
				sdlShowTexte(*pSdlJeu, "PLANTE",7,93,0);
				sprintf(c,"%d",pp);
				sdlShowTexte(*pSdlJeu, c,37,100,0);
				strcpy ( c, "");
				sprintf(c,"%d",ppTotal);
				sdlShowTexte(*pSdlJeu, c,62,100,0);
				strcpy ( c, "");
			break;

			case 3:
				sdlShowTexte(*pSdlJeu, "NORMAL",7,93,0);
				sprintf(c,"%d",pp);
				sdlShowTexte(*pSdlJeu, c,37,100,0);
				strcpy ( c, "");
				sprintf(c,"%d",ppTotal);
				sdlShowTexte(*pSdlJeu, c,62,100,0);
				strcpy ( c, "");
			break;

			 default: break;

		}/*fin affichage des types d'attaques*/





		SDL_Flip( pSdlJeu->surface_ecran );



         if ( event.type == SDL_KEYDOWN)
	  {
		switch ( event.key.keysym.sym )
		{




			case B :

			    boucle =1;



			break;



			case UP :
				 if(ind>1){ind--; hauteur =hauteur -8;/*on bouge la flèche vers le haut*/

				}
			break;

			case DOWN  :
				 if(ind<4)
				 {
					ind++;
					hauteur = hauteur +8;/*on bouge la flèche vers le  bas*/
				 }
			break;


			case  A:



				if(pSdlJeu->jeu.hero.pokemon[enCombat].moves[ind-1].pp > 0){/*on attaque si il reste des attaque*/


					pv= calculdmg(pSdlJeu->jeu.hero.pokemon[enCombat], ind, pSdlJeu->jeu.wildPokemon);
					modifpv(pv, &pSdlJeu->jeu.wildPokemon);/*on modife le pv du pokemon en défense*/
					pSdlJeu->jeu.hero.pokemon[enCombat].moves[ind-1].pp=pp-1;/*on enlève un pp de l'attaque*/

					/*on cache le menu de combat pour pas qu'il se retrouve derrière la boite de dialoque */
					SDL_apply_surface( pSdlJeu,pSdlJeu->surface_Menu_Sprite[4], pSdlJeu->surface_ecran, 0, 73);

					SDL_apply_surface( pSdlJeu,pSdlJeu->surface_Pokemon_Sprite[3*pSdlJeu->jeu.hero.pokemon[enCombat].number-3], pSdlJeu->surface_ecran,22,54);/*on réaffiche notre pokemon*/
					affPv(pSdlJeu,1);

					sprintf(s,"%s a lance %s .",pSdlJeu->jeu.hero.pokemon[enCombat].name, pSdlJeu->jeu.hero.pokemon[enCombat].moves[ind-1].name);/*on réaffiche le nom de notre pokemon*/
					SDL_Flip( pSdlJeu->surface_ecran );
					sdlShowDialog(pSdlJeu, s);/*on affiche la boitede dialoque qui contient la chaine s*/
					pSdlJeu->jeu.hero.aAttaquer = ind-1;/*on indique l'attauqe que le joeur a utilisé*/


					boucle = 1;
				}
			break;


			default: break;
		}/*on quitte le switch*/
	 }



          if ( event.type == SDL_QUIT )/*si on quitte en appuyant sur la croix*/
		{
		    boucle = 1;

		    SDL_Quit();
		}
        }

    }



}

void info(sdlJeu *pSdlJeu, int ind2){

    int ind = 1;/*pour connaitre la position de la souri*/
    int hauteur = 71;/*pour déplacer la souri*/
    int boucle = 0;


    SDL_Event event;

    SDL_Init( SDL_INIT_EVERYTHING);


  while(boucle == 0)
    {
        SDL_Delay(60);

        while ( SDL_PollEvent( &event ) )
            {

                SDL_apply_surface( pSdlJeu,  pSdlJeu->surface_Menu_Sprite[18], pSdlJeu->surface_ecran, 45,60);
                SDL_apply_surface( pSdlJeu, pSdlJeu->surface_Menu_Sprite[38], pSdlJeu->surface_ecran, 60, hauteur);
                SDL_Flip( pSdlJeu->surface_ecran );


         if ( event.type == SDL_KEYDOWN)
			  {
				switch ( event.key.keysym.sym )
				{


						case UP:
						 if(ind>1){ind--; hauteur =hauteur -17; }


						break;

						case DOWN:
						 if(ind<3)
							{
						ind++;
						hauteur = hauteur +17;
						}
					        break;

						case B:
						    boucle =1;
						break;

						case A:
						 switch (ind)
							{
							    case 1:
									stat(*pSdlJeu,ind2);
									boucle = 1;/*on quitte l'écran info*/

							    break;


							    case 2: objet(pSdlJeu);

									 boucle = 1;
							    break;



							    case 3:  boucle = 1; /*annule*/
							    break;



							    default: break;

							}

						break;

						default: break;
					 }
					 }


          if ( event.type == SDL_QUIT )
                {
                    boucle = 1;
                    SDL_Quit();
                }
        }

    }

}

void stat(sdlJeu pSdlJeu, int ind2){
    int i;
    int ind = 1;
    int boucle = 0;

     char nom[40];

    SDL_Event event;
    sprintf(nom,"data/pokemon/%sface.bmp",pSdlJeu.jeu.hero.pokemon[ind2-1].name);


    SDL_Init( SDL_INIT_EVERYTHING);
    strcpy(nom,"");
    SDL_apply_surface( &pSdlJeu, pSdlJeu.surface_Menu_Sprite[40], pSdlJeu.surface_ecran, 0, 0);


  while(boucle == 0)
    {
        SDL_Delay(60);

        while ( SDL_PollEvent( &event ) )
            {

		SDL_apply_surface(&pSdlJeu, pSdlJeu.surface_Pokemon_Sprite[3*pSdlJeu.jeu.hero.pokemon[ind2-1].number-2], pSdlJeu.surface_ecran, 20, 20);
		sdlShowTexte( pSdlJeu, pSdlJeu.jeu.hero.pokemon[ind2-1].name,80,40,0);

		sprintf(nom,"%d",pSdlJeu.jeu.hero.pokemon[ind2-1].number);
		sdlShowTexte( pSdlJeu, nom,80,8,0);
		strcpy(nom,"");
		sprintf(nom,"%d",pSdlJeu.jeu.hero.pokemon[ind2-1].lvl);
		sdlShowTexte( pSdlJeu,nom,125,8,0);
		strcpy(nom,"");

		switch(ind)
		{
			case 1 :

				sprintf(nom,"%d",pSdlJeu.jeu.hero.pokemon[ind2-1].xp);
					sdlShowTexte(pSdlJeu,nom ,123,78,0);
					strcpy(nom,"");
				sprintf(nom,"%d",0);
					sdlShowTexte(pSdlJeu,nom ,141,119,0);
					strcpy(nom,"");


				sprintf(nom,"%d",pSdlJeu.jeu.hero.pokemon[ind2-1].pv);
					sdlShowTexte(pSdlJeu,nom ,18,88,0);
					strcpy(nom,"");
				sprintf(nom,"%d",pSdlJeu.jeu.hero.pokemon[ind2-1].pvMax);
					sdlShowTexte(pSdlJeu,nom ,49,88,0);
					strcpy(nom,"");

				if(pSdlJeu.jeu.hero.pokemon[ind2-1].pv == 0)
				{
					sdlShowTexte(pSdlJeu,"K.O" ,50,113,0);
				}
				else{
					sdlShowTexte(pSdlJeu,"OK" ,50,113,0);

				}



				switch(pSdlJeu.jeu.hero.pokemon[ind2-1].type)
				{
					case 0:
						sdlShowTexte(pSdlJeu,"EAU" ,10,135,0);
					break;

					case 1:
						sdlShowTexte(pSdlJeu,"FEU" ,10,135,0);
					break;


					case 2:
						sdlShowTexte(pSdlJeu,"PLANTE" ,10,135,0);
					break;

					case 3:
						sdlShowTexte(pSdlJeu,"NORMAL" ,10,135,0);
					break;


					default: break;

				}


			break;

			case 2 :


				for(i=0;i<4;i++)/*on affiche les noms des 4 attaques (maxi 4)*/
				{
					sdlShowTexte(pSdlJeu, pSdlJeu.jeu.hero.pokemon[ind2-1].moves[i].name,52,90+i*16,0);
				}

				for(i=0;i<4;i++)/*on affiche les noms des 4 attaques (maxi 4)*/
				{
					sprintf(nom,"%d",pSdlJeu.jeu.hero.pokemon[ind2-1].moves[i].pp);
					sdlShowTexte(pSdlJeu,nom ,123,96+i*16,0);
					strcpy(nom,"");
				}
				for(i=0;i<4;i++)/*on affiche les noms des 4 attaques (maxi 4)*/
				{
					sprintf(nom,"%d",pSdlJeu.jeu.hero.pokemon[ind2-1].moves[i].ppTotal);
					sdlShowTexte(pSdlJeu,nom ,142,96+i*16,0);
					strcpy(nom,"");
				}



			break;

			case 3 :

				sprintf(nom,"%d",pSdlJeu.jeu.hero.pokemon[ind2-1].attack);
				sdlShowTexte(pSdlJeu,nom ,140,77,0);
				strcpy(nom,"");

				sprintf(nom,"%d",pSdlJeu.jeu.hero.pokemon[ind2-1].defense);
				sdlShowTexte(pSdlJeu,nom ,140,94,0);
				strcpy(nom,"");

				sprintf(nom,"%d",pSdlJeu.jeu.hero.pokemon[ind2-1].spclatk);
				sdlShowTexte(pSdlJeu,nom ,140,111,0);
				strcpy(nom,"");

				sprintf(nom,"%d",pSdlJeu.jeu.hero.pokemon[ind2-1].spcldef);
				sdlShowTexte(pSdlJeu,nom ,140,128,0);
				strcpy(nom,"");

				sprintf(nom,"%d",pSdlJeu.jeu.hero.pokemon[ind2-1].speed);
				sdlShowTexte(pSdlJeu,nom ,140,144,0);
				strcpy(nom,"");


				sdlShowTexte( pSdlJeu,"Sasha ",5,80,0);
			break;







		default: break;

		}









                SDL_Flip( pSdlJeu.surface_ecran );


			 if ( event.type == SDL_KEYDOWN)
				 {
						switch ( event.key.keysym.sym )
						{



							case B:
							    boucle =1;
							break;

							case LEFT:
								if(ind==2){ind--;
                                    SDL_apply_surface( &pSdlJeu, pSdlJeu.surface_Menu_Sprite[40], pSdlJeu.surface_ecran, 0, 0);







								}
								else if(ind == 3){ind--;
                                    SDL_apply_surface( &pSdlJeu, pSdlJeu.surface_Menu_Sprite[24], pSdlJeu.surface_ecran, 0, 0);







								}
							break;

							case RIGHT:
								if(ind==1){ind++;
                                    SDL_apply_surface( &pSdlJeu, pSdlJeu.surface_Menu_Sprite[24], pSdlJeu.surface_ecran, 0, 0);







								}
								else if(ind == 2){ind++;
                                    SDL_apply_surface( &pSdlJeu, pSdlJeu.surface_Menu_Sprite[13], pSdlJeu.surface_ecran, 0, 0);








								}
							break;










							default: break;
						 }
				}


			  if ( event.type == SDL_QUIT )
				{
				    boucle = 1;
				    SDL_Quit();
				}
      	  }

    }




}

void objet(sdlJeu* pSdlJeu){

    int boucle = 0;
    int indHorizontal =1;/*position du curseur horizontal*/
    int indVerticalPotion = 0;/* position du curseur dans la page potion*/
    int indVerticalPokeball =0;/* position du curseur dans la page pokeBalle*/
    int hauteurPotion = 21;/*la hauteur d'affiche du curseur pour la page potion*/
    int hauteurPokeball = 21;/*la hauteur d'affiche du curseur pour la page pokeBall*/
    char nb[50];
    int indPoke =0;/* sert à sauter des lignes pour afficher les pokeBalles*/
    int indPotion =0;/* sert à sauter des lignes pour afficher les potions*/
    int tabPokeBall[50]={0};/*sert à sauvegarder la position d'une pokeBalle dans le tableau des objet*/
    int tabPotion[50]={0};/*sert à sauvegarder la position d'une potion dans le tableau des objet*/
    int k=0;
    int j=0;
    int i;

    SDL_Event event;

    SDL_Init( SDL_INIT_EVERYTHING);


    while(boucle == 0)
    {
        SDL_Delay(60);

        while ( SDL_PollEvent( &event ) )
            {

		if(indHorizontal==1){
                    SDL_apply_surface(pSdlJeu, pSdlJeu->surface_Menu_Sprite[22], pSdlJeu->surface_ecran, 0, 0);
              		SDL_apply_surface(pSdlJeu, pSdlJeu->surface_Menu_Sprite[39], pSdlJeu->surface_ecran, 45, hauteurPotion);
			indPoke =0;/*on remette à zéro pour l'affichage des objets reste fixe*/
   			indPotion =0;
		 }


		else if(indHorizontal==2){
         SDL_apply_surface(pSdlJeu, pSdlJeu->surface_Menu_Sprite[23], pSdlJeu->surface_ecran, 0, 0);
			SDL_apply_surface( pSdlJeu,  pSdlJeu->surface_Menu_Sprite[39],pSdlJeu->surface_ecran, 45, hauteurPokeball);
			indPoke =0;
   			indPotion =0;

		}




/*************************on affiche les potion*********************************/

		for(i=0;i<pSdlJeu->jeu.hero.nbObjet;i++){ /*on parcours la boucle des objet*/
			if(pSdlJeu->jeu.hero.mesObjet[i].effet<5)/*on affiche les potions*/
			{	if((indHorizontal==1)&& (pSdlJeu->jeu.hero.mesObjet[i].nbObjetCourant>0)){/*on affiche  sur la page des potions*/
				sdlShowTexte(*pSdlJeu,pSdlJeu->jeu.hero.mesObjet[i].nom,53,21+indPotion*23,0);/*on affiche les objets*/
				sdlShowTexte(*pSdlJeu," x ",115,31+indPotion*23,0);
				sprintf(nb,"%d",pSdlJeu->jeu.hero.mesObjet[i].nbObjetCourant);
				sdlShowTexte(*pSdlJeu,nb,122,31+indPotion*23,0);
				indPotion++;
				tabPotion[j]=i;/*on stocke la position du potion "i"ème dans tabPotion*/
				j++;
				}


			}





/*************************on affiche les pokeballes*********************************/

/****new*****/	else if((pSdlJeu->jeu.hero.mesObjet[i].effet>4)&&(pSdlJeu->jeu.hero.mesObjet[i].effet<7)) {/*on affiche les pokeball, même instruction que pour les potions*/
				if((indHorizontal==2)&& (pSdlJeu->jeu.hero.mesObjet[i].nbObjetCourant>0)){
				sdlShowTexte(*pSdlJeu,pSdlJeu->jeu.hero.mesObjet[i].nom,53,21+indPoke*23,0);
				sdlShowTexte(*pSdlJeu," x ",123,31+indPoke*23,0);
				sprintf(nb,"%d",pSdlJeu->jeu.hero.mesObjet[i].nbObjetCourant);
				sdlShowTexte(*pSdlJeu,nb,133,31+indPoke*23,0);
				indPoke++;
				tabPokeBall[k]=i;
				k++;
				}
			}
		}


                SDL_Flip( pSdlJeu->surface_ecran );








      		if ( event.type == SDL_KEYDOWN)
		{
			switch ( event.key.keysym.sym )
			{


				case B:
				    boucle =1;
				break;





				case LEFT:/*si on appuye sur la flèche de gauche*/
					if(indHorizontal==2){indHorizontal--;/*on désincrément ind = 1 et on affiche la page objet */
              		SDL_apply_surface(pSdlJeu, pSdlJeu->surface_Menu_Sprite[39], pSdlJeu->surface_ecran, 45, hauteurPotion);

					}

				break;

				case RIGHT:/*même instruction de LEFT , juste inversément */
					if(indHorizontal==1){indHorizontal++;
                        SDL_apply_surface(pSdlJeu, pSdlJeu->surface_Menu_Sprite[39], pSdlJeu->surface_ecran, 45, hauteurPokeball);

					}

				break;






				case UP:
					if((indVerticalPotion>0)&&(indHorizontal==1))/*le nombre de déplacement du curseur est limité selon le nombre de  différente  potion.*/
						{
						indVerticalPotion--;/*on désincrémente ind2 pour remonter*/
						hauteurPotion = hauteurPotion -23;/*on désincrément la hauteur de curseur*/
					}
					else if((indVerticalPokeball>0)&&(indHorizontal==2))/*le nombre de déplacement du curseur est limité selon le nombre de  différente  pokeballe.*/
						{
						indVerticalPokeball--;
						hauteurPokeball = hauteurPokeball -23;
					}

				break;








				case DOWN:
					 if((indVerticalPotion<indPotion-1)&&(indHorizontal==1))/*le nombre de déplacement du curseur est limité selon le nombre de  différente potion.*/
						{
						indVerticalPotion++;
						hauteurPotion = hauteurPotion +23;/*on augmente la hauteur de curseur de la page potion*/
					}
					else if((indVerticalPokeball<indPoke-1)&&(indHorizontal==2))/*le nombre de déplacement du curseur est limité selon le nombre de  differente pokeball .*/
						{
						indVerticalPokeball++;
						hauteurPokeball = hauteurPokeball +23;
					}



				break;


				case A:



					if((indHorizontal== 1)&&(indPotion>0)){
/*******new*****/
						pokemonSoin(pSdlJeu,tabPotion[indVerticalPotion]);
						boucle=1;

					}











					else if((indHorizontal==2)&&(pSdlJeu->jeu.hero.enCombat == 1)&&(indPoke>0))
					{	pSdlJeu->jeu.hero.mesObjet[tabPokeBall[indVerticalPokeball]].nbObjetCourant--;

						if( capture(pSdlJeu->jeu.hero.mesObjet[tabPokeBall[indVerticalPokeball]].effet, pSdlJeu->jeu.wildPokemon)==1)
						{
							pSdlJeu->jeu.wildPokemon.capture=1;/*on a réussit à capturer*/
							boucle = 1;



						}
						 else if( capture(pSdlJeu->jeu.hero.mesObjet[tabPokeBall[indVerticalPokeball]].effet, pSdlJeu->jeu.wildPokemon)==0){pSdlJeu->jeu.wildPokemon.capture=2; /*on a pas réussit à capturer*/
						 	boucle = 1;


							}





















/*******fin new*****/
					}
				break;

				default: break;
			 }
		}






          if ( event.type == SDL_QUIT )
                {
                    boucle = 1;
                    SDL_Quit();
                }
        }

    }


}

void pokemonSoin(sdlJeu* pSdlJeu, int indObjet){
    char c[20];
    int nb;
    char chaine[30];
    int larg = 16;
    int i;
    int pv;
    int pvMax;
    int dejaDonne=0;
    int boucle = 0;
    int ind = 1;/*pour connaitre la position de la souri*/
    int hauteur = 6;/*pour déplacer la souri*/
    int nb_pok;

    nb_pok = pSdlJeu->jeu.hero.nb_pok;


    SDL_Event event;


    SDL_Init( SDL_INIT_EVERYTHING);


    while(boucle == 0)
    {   SDL_Delay(60);
        SDL_apply_surface( pSdlJeu, pSdlJeu->surface_Menu_Sprite[29], pSdlJeu->surface_ecran, 0, 0);

	while ( SDL_PollEvent( &event ) )
	{


	  for(i=1;i<=pSdlJeu->jeu.hero.nb_pok;i++)
	    {

	       if(i==ind){
	            larg = 16;
	        }

	        else{larg = 10;}


        SDL_apply_surface(pSdlJeu, pSdlJeu->surface_Pokemon_Sprite[3*pSdlJeu->jeu.hero.pokemon[i-1].number-1], pSdlJeu->surface_ecran,larg,(i-1)*20);
		sdlShowTexte(*pSdlJeu, pSdlJeu->jeu.hero.pokemon[i-1].name,40,(i-1)*20+1,0);
		nb = pSdlJeu->jeu.hero.pokemon[i-1].lvl;

		if(pSdlJeu->jeu.hero.pokemon[ind-1].pv<1)/*on affiche l'état des pokemons */
		{
			sdlShowTexte(*pSdlJeu, "Il est K.O.",3,137,0);
		}
		else{

        sdlShowTexte(*pSdlJeu, "Soigner un pokemon.",3,137,0);}
        sprintf(c,"%d",nb);
        sdlShowTexte(*pSdlJeu, c,65,(i-1)*20+7,0);

		strcpy(c,"");

	    }


        for(i=0;i<(6-pSdlJeu->jeu.hero.nb_pok);i++){
        SDL_apply_surface( pSdlJeu, pSdlJeu->surface_Menu_Sprite[5], pSdlJeu->surface_ecran, 5, 105-i*19);
        }

        affPv(pSdlJeu, 3);
        SDL_apply_surface(pSdlJeu,  pSdlJeu->surface_Menu_Sprite[38], pSdlJeu->surface_ecran, 9, hauteur);






        if(dejaDonne ==4){/*beugue possible*/

            SDL_apply_surface( pSdlJeu, pSdlJeu->surface_Menu_Sprite[10], pSdlJeu->surface_ecran,0,125);
            sdlShowDialog(pSdlJeu, chaine);
            SDL_Flip( pSdlJeu->surface_ecran );
            boucle=1;
        }



        else{
            SDL_Flip( pSdlJeu->surface_ecran );

            if ( event.type == SDL_KEYDOWN)
            {
                pv=pSdlJeu->jeu.hero.pokemon[ind-1].pv;
                pvMax=pSdlJeu->jeu.hero.pokemon[ind-1].pvMax;

                switch ( event.key.keysym.sym )
                {
                    case B:
                         boucle =1;
                    break;

                    case UP:
                         if(ind>1){
                            ind--;
                            hauteur =hauteur -21;
                         }


                    break;

                    case DOWN:
                         if(ind<nb_pok)
                        {
                            ind++;
                            hauteur = hauteur +21;
                        }
                    break;


                    case A:

                        switch( pSdlJeu->jeu.hero.mesObjet[indObjet].effet)
                        {
                             case 1:
                                if((pv+50 <pvMax)&&(pv>0)){/*on test si le total des pv ne dépasse pas pvMax, sinon on met pv = pvMax, et  il faut que les pokemon soit en vie*/
                                    pSdlJeu->jeu.hero.pokemon[ind-1].pv = pv+50;
                                    pSdlJeu->jeu.hero.mesObjet[indObjet].nbObjetCourant--;
                                }

                                else if((pv+50 >=pvMax)&&(pv>0)){
                                    pSdlJeu->jeu.hero.pokemon[ind-1].pv = pvMax;
                                    pSdlJeu->jeu.hero.mesObjet[indObjet].nbObjetCourant--;
                                }
                            break;

                            case 2:
                                if((pv+100 <pvMax)&&(pv>0)){
                                    pSdlJeu->jeu.hero.pokemon[ind-1].pv =pv+100;
                                    pSdlJeu->jeu.hero.mesObjet[indObjet].nbObjetCourant--;
                                }
                                else if((pv+100 >pvMax)&&(pv>0))
                                {	pSdlJeu->jeu.hero.pokemon[ind-1].pv = pvMax;
                                    pSdlJeu->jeu.hero.mesObjet[indObjet].nbObjetCourant--;
                                }
                            break;

                            case 3:
                                if(pv>0){
                                    pSdlJeu->jeu.hero.pokemon[ind-1].pv = pvMax;
                                    pSdlJeu->jeu.hero.mesObjet[indObjet].nbObjetCourant--;
                                }
                            break;

                            case 4:if(pv==0){
                                    pSdlJeu->jeu.hero.pokemon[ind-1].pv = pvMax;/*on rapelle le pokemon à la vie*/
                                    pSdlJeu->jeu.hero.mesObjet[indObjet].nbObjetCourant--;
                                }
                            break;


                            default: break;
                        }



                         if(pv < pSdlJeu->jeu.hero.pokemon[ind-1].pv){/*si le pv a été modifier on affiche : */

                          /*  sprintf(chaine , "%s recoit %s ." ,pSdlJeu->jeu.hero.pokemon[ind-1].name, pSdlJeu->jeu.hero.mesObjet[indObjet].nom);*//*on affiche le pokemon qui a reçu la potion , et type de potion*/
                          SDL_apply_surface( pSdlJeu,  pSdlJeu->surface_Menu_Sprite[29], pSdlJeu->surface_ecran, 0, 0);
/*  beugue */			    /*dejaDonne = 4; *//*sert à sortir de la boucle mais il faut d'abord réafficher tout les pokémons et leurs pv donc on besoin de repasser par l'affiche qui est en haut de la fonction*/
                            boucle =1;
					     }


					    else if(pv==pSdlJeu->jeu.hero.pokemon[ind-1].pv){
					        /*on affiche le pokemon avec l'objet qu'il a reçu*/
                            sprintf(chaine , "%s ne peut pas recevoir %s." ,pSdlJeu->jeu.hero.pokemon[ind-1].name, pSdlJeu->jeu.hero.mesObjet[indObjet].nom);
                            SDL_apply_surface( pSdlJeu,   pSdlJeu->surface_Menu_Sprite[10], pSdlJeu->surface_ecran, 0, 125);

                            sdlShowDialog(pSdlJeu, chaine);
                            SDL_apply_surface( pSdlJeu, pSdlJeu->surface_Menu_Sprite[29], pSdlJeu->surface_ecran,0,0);

                            pSdlJeu->jeu.hero.mesObjet[indObjet].nbObjetCourant++;/*on réajoute car pardefault on l'a enlevé*/
                        }

				break;


				default: break;
			}
		}

		 if ( event.type == SDL_QUIT )
            {
		    boucle = 1;
		    SDL_Quit();
		}

	}








	}

    }





}

void moi(sdlJeu pSdlJeu){

    int boucle = 0;
    char argent[20];

    SDL_Event event;


    SDL_Init( SDL_INIT_EVERYTHING);
    sprintf(argent,"%d",pSdlJeu.jeu.hero.argent);


  while(boucle == 0)
    {
        SDL_Delay(60);

        while ( SDL_PollEvent( &event ) )
            {

                SDL_apply_surface( &pSdlJeu,  pSdlJeu.surface_Menu_Sprite[19], pSdlJeu.surface_ecran, 0, 0);
		sdlShowTexte(pSdlJeu,"Nom  : SASHA",62,30,0);
		sdlShowTexte(pSdlJeu,"Age  : 18ans",62,45,0);
		sdlShowTexte(pSdlJeu,"Taille  : 1m75",62,60,0);
		sdlShowTexte(pSdlJeu,"Argent  :" ,62,75,0);
		sdlShowTexte(pSdlJeu,argent ,124,75,0);

                SDL_Flip( pSdlJeu.surface_ecran );


         if ( event.type == SDL_KEYDOWN)
			  {
				switch ( event.key.keysym.sym )
				{



                case B :
                    boucle =1;
                break;



                default: break;
					 }
					 }


          if ( event.type == SDL_QUIT )
                {
                    boucle = 1;
                    SDL_Quit();
                }
        }

    }







}


void sauvegarde(sdlJeu pSdlJeu){

    int boucle = 0;
    int ind = 1;/*pour connaitre la position de la souri*/
    int largeur = 23;/*pour déplacer la souri*/

    SDL_Event event;


    SDL_Init( SDL_INIT_EVERYTHING);


  while(boucle == 0)
    {


        while ( SDL_PollEvent( &event ) )
            {

                SDL_apply_surface( &pSdlJeu,  pSdlJeu.surface_Menu_Sprite[35], pSdlJeu.surface_ecran, 0, 85);
                SDL_apply_surface(&pSdlJeu,  pSdlJeu.surface_Menu_Sprite[38], pSdlJeu.surface_ecran, largeur,120);
                SDL_Flip( pSdlJeu.surface_ecran );


         if ( event.type == SDL_KEYDOWN)
			  {
				switch ( event.key.keysym.sym )
				{




                case B :
                    boucle =1;
                break;



                case LEFT :
					 if(ind>1){ind--; largeur =largeur -50; }
                break;

                case RIGHT  :
					 if(ind<2)
					 {
                        ind++;
                        largeur = largeur +50;


					 }
                break;

              case  A:/*appuye sur entré*/

                        switch (ind)
                        {
                            case 1:/*oui*/
                                saveGame(&pSdlJeu.jeu);
                                sdlShowDialog(&pSdlJeu,"Votre partie a ete sauvgarde!");
                                boucle =1;
                            break;

                            case 2:/*non*/

                                    boucle =1;
                            break;


                            default: break;

                        }

                break;


                default: break;
					 }
					 }


          if ( event.type == SDL_QUIT )
                {
                    boucle = 1;
                    SDL_Quit();
                }
        }

    }

}

void affPv(sdlJeu *pSdlJeu, int type){

	int i;
	int k;
	int j=0;
	int nb;
	int restePv = 0;
	char c[5];


	if(type == 1){/*1 = on affiche le pv de notre pokemon en combat*/

		restePv = ((pSdlJeu->jeu.hero.pokemon[0].pv*100)/pSdlJeu->jeu.hero.pokemon[0].pvMax);/*on calcule de pourcentage du pv restant*/
		if(restePv>60){/*si le pv restant est supérieur à 60% alors la bar reste verte et on a juste à cacher la partie que l'on veut cacher , ceci permettre de faire varier la barre de pv*/


				for(i=0;i<100-restePv;i=i+4){
					SDL_apply_surface( pSdlJeu,  pSdlJeu->surface_Menu_Sprite[8], pSdlJeu->surface_ecran, 145-j*2, 83);/*on applique autant de fois que le nombre de pv manquant, en décalant de 2 pixel pour chaque incrémetation de j , ceci correspond a la largeur de cachePv.bmp*/
					j++;

				}j=0;

		}
		else if((restePv<=60)&&(restePv>=30)){/*de même que en haut, mais on change juste la couleur de la barre en orange*/


				SDL_apply_surface( pSdlJeu,  pSdlJeu->surface_Menu_Sprite[31], pSdlJeu->surface_ecran, 96, 83);/*on change la couleur de la barre en orange*/
				for(i=0;i<100-restePv;i=i+4){
					SDL_apply_surface( pSdlJeu,  pSdlJeu->surface_Menu_Sprite[8], pSdlJeu->surface_ecran, 145-j*2, 83);
					j++;
				}j=0;


		}
		else if((restePv<=29)&&(restePv>0)){/*de même que en haut, mais on change juste la couleur de la barre en rouge*/

				SDL_apply_surface( pSdlJeu, pSdlJeu->surface_Menu_Sprite[33], pSdlJeu->surface_ecran, 96, 83);/*on change la couleur de la barre en rouge*/
				for(i=0;i<100-restePv;i=i+4){
					SDL_apply_surface( pSdlJeu,  pSdlJeu->surface_Menu_Sprite[8], pSdlJeu->surface_ecran, 145-j*2, 83);
					j++;
				}j=0;


		}

		else
		{

			for(i=0;i<100;i=i+4){
					SDL_apply_surface( pSdlJeu, pSdlJeu->surface_Menu_Sprite[8], pSdlJeu->surface_ecran, 145-j*2, 83);
					j++;
				}j=0;
		}



		 nb = pSdlJeu->jeu.hero.pokemon[0].pv;


		 sprintf(c,"%d",nb);
		sdlShowTexte(*pSdlJeu, c,90,91,0);


		strcpy(c,"");

		nb = pSdlJeu->jeu.hero.pokemon[0].pvMax;


		 sprintf(c,"%d",nb);
		sdlShowTexte(*pSdlJeu, c,123,91,0);
		strcpy(c,"");

	}

	else if(type ==2){/*1 = on affiche le pv du pokemon ennemi, même principe que type == 1, il y a juste les cordonnées qui changent */
		restePv = ((pSdlJeu->jeu.wildPokemon.pv*100)/pSdlJeu->jeu.wildPokemon.pvMax);

		if(restePv>60){
				for(i=1;i<100-restePv;i=i+4){
					SDL_apply_surface( pSdlJeu, pSdlJeu->surface_Menu_Sprite[8], pSdlJeu->surface_ecran, 81-j*2, 25);
					j++;
				}
		}

		else if((restePv<=60)&&(restePv>=30)){

				SDL_apply_surface( pSdlJeu,  pSdlJeu->surface_Menu_Sprite[31], pSdlJeu->surface_ecran, 32, 25);
				for(i=0;i<100-restePv;i=i+4){
					SDL_apply_surface( pSdlJeu, pSdlJeu->surface_Menu_Sprite[8], pSdlJeu->surface_ecran, 81-j*2, 25);
					j++;
				}j=0;

		}

		else if((restePv<=29)&&(restePv>0)){

				SDL_apply_surface( pSdlJeu, pSdlJeu->surface_Menu_Sprite[33], pSdlJeu->surface_ecran, 32, 25);
				for(i=0;i<100-restePv;i=i+4){
					SDL_apply_surface( pSdlJeu, pSdlJeu->surface_Menu_Sprite[8], pSdlJeu->surface_ecran, 81-j*2, 25);
					j++;
				}j=0;

		}





		else
		{

			for(i=0;i<100;i=i+4){
					SDL_apply_surface( pSdlJeu, pSdlJeu->surface_Menu_Sprite[8], pSdlJeu->surface_ecran, 81-j*2,25);
					j++;
				}j=0;
		}


	}





	else if(type ==3){/*on affiche la barre pv de tous les pokemons du joueur dans le menu pokemon*/

		for(k=0;k<pSdlJeu->jeu.hero.nb_pok;k++){

			restePv = ((pSdlJeu->jeu.hero.pokemon[k].pv*100)/pSdlJeu->jeu.hero.pokemon[k].pvMax);
			 if(restePv>60){


					for(i=0;i<100-restePv;i=i+4){
						SDL_apply_surface( pSdlJeu, pSdlJeu->surface_Menu_Sprite[8], pSdlJeu->surface_ecran, 153-j*2, 11+k*20);
						j++;
					}	j=0;

			}
			else if((restePv<=60)&&(restePv>=30)){

					SDL_apply_surface( pSdlJeu,  pSdlJeu->surface_Menu_Sprite[32], pSdlJeu->surface_ecran, 105, 11+k*20);
					for(i=0;i<100-restePv;i=i+4){
						SDL_apply_surface( pSdlJeu, pSdlJeu->surface_Menu_Sprite[8], pSdlJeu->surface_ecran, 155-j*2, 11+k*20);
						j++;
					}j=0;

			}
			else if((restePv<=29)&&(restePv>0)){

					SDL_apply_surface( pSdlJeu,  pSdlJeu->surface_Menu_Sprite[34], pSdlJeu->surface_ecran, 105, 11+k*20);
					for(i=0;i<100-restePv;i=i+4){
						SDL_apply_surface( pSdlJeu, pSdlJeu->surface_Menu_Sprite[8], pSdlJeu->surface_ecran, 155-j*2, 11+k*20);
						j++;
					}j=0;

				}


			else if (pSdlJeu->jeu.hero.pokemon[k].pv==0)
			{

				for(i=0;i<100;i=i+4){
						SDL_apply_surface( pSdlJeu, pSdlJeu->surface_Menu_Sprite[8], pSdlJeu->surface_ecran, 155-j*2, 11+k*20);
						j++;
					}j=0;
			}




		}
	}



}

void affMarket(sdlJeu *pSdlJeu){

  int boucle = 0;
    int ind = 1;/*pour connaitre la position de la souri*/
    int hauteur = 10;/*pour déplacer la souri*/

    SDL_Event event;
    SDL_Init( SDL_INIT_EVERYTHING);
     sdlShowDialog(pSdlJeu, "Bonjours ! Que puis-je faire pour vous ?");

  while(boucle == 0)
    {


        while ( SDL_PollEvent( &event ) )
            {


                SDL_apply_surface(pSdlJeu, pSdlJeu->surface_Menu_Sprite[50], pSdlJeu->surface_ecran, 0, 0);
                SDL_apply_surface(pSdlJeu, pSdlJeu->surface_Menu_Sprite[38], pSdlJeu->surface_ecran, 10, hauteur);
                sdlShowTexte(*pSdlJeu, "Acheter", 20,10,0);
                sdlShowTexte(*pSdlJeu, "Annuler ", 20,30,0);

                SDL_Flip( pSdlJeu->surface_ecran );


         if ( event.type == SDL_KEYDOWN)
			  {
				switch ( event.key.keysym.sym )
				{
					case B:
						    boucle =1;
					break;

					case UP:
								 if(ind>1){ind--; hauteur =hauteur -20; }
					break;

					case DOWN:
								 if(ind<2)
								 {
						ind++;
						hauteur = hauteur +20;


								 }
					break;

				        case  A:/*appuye sur entré*/

						switch (ind)
						{
						    case 1: buy(pSdlJeu);boucle =1;


						    break;

						    case 2:boucle =1;
						     sdlShowDialog(pSdlJeu, "Aurevoir ! A bientot !");

						    default: break;

						}

					break;


					default: break;
					 }
					 }


          if ( event.type == SDL_QUIT )
                {
                    boucle = 1;
                    SDL_Quit();
                }
        }

    }




}

int pay(sdlJeu *pSdlJeu,int montant){


    if (pSdlJeu->jeu.hero.argent-montant>=0)/*si on a assez d'argent alors on achete*/
    {
       pSdlJeu->jeu.hero.argent= pSdlJeu->jeu.hero.argent-montant;/*on enlève le montant à payer*/
       return 0;/*on retourne 0 , sinon on retourne 1 */

    }
    return 1;
}
void ajout(sdlJeu *pSdlJeu, int type){

    int i;
    int existe =0;/* 0  si on a pas encore cet objet, et 1 sinon*/

    for(i=0;i<pSdlJeu->jeu.hero.nbObjet;i++)
    {

        if(pSdlJeu->jeu.hero.mesObjet[i].effet==type)/*si le hero possède déjà cet objet alors on  le rajouter */
        {
            pSdlJeu->jeu.hero.mesObjet[i].nbObjetCourant++;
            existe=1;
        }

    }
    if(existe==0){/*si le hero ne possède pas encore cet objet alors on le crée */
     pSdlJeu->jeu.hero.mesObjet[pSdlJeu->jeu.hero.nbObjet].nbObjetCourant++;
     pSdlJeu->jeu.hero.mesObjet[pSdlJeu->jeu.hero.nbObjet].effet=type;
     pSdlJeu->jeu.hero.nbObjet++;
     switch(type)
     {
         case 1:strcpy(pSdlJeu->jeu.hero.mesObjet[i].nom,"Potion");
         break;

         case 2:strcpy(pSdlJeu->jeu.hero.mesObjet[i].nom,"SuperPotion");
         break;

         case 3:strcpy(pSdlJeu->jeu.hero.mesObjet[i].nom,"PotionMax");
         break;


         case 4:strcpy(pSdlJeu->jeu.hero.mesObjet[i].nom,"Rappel");
         break;

         case 5:strcpy(pSdlJeu->jeu.hero.mesObjet[i].nom,"PokeBall");
         break;


         case 6:strcpy(pSdlJeu->jeu.hero.mesObjet[i].nom,"SuperPokeBall");
         break;

        default : break;


     }

    }


}

void buy(sdlJeu *pSdlJeu){

    char arg[20];
    int boucle = 0;
    int ind = 1;/*pour connaitre la position de la pSdlJeu, pSdlJeu->surface_Menu_Sprite[38]*/
    int hauteur = 20;/*pour déplacer la pSdlJeu, pSdlJeu->surface_Menu_Sprite[38]*/


    SDL_Event event;

    SDL_Init( SDL_INIT_EVERYTHING);

    while(boucle == 0)
    {


        while ( SDL_PollEvent( &event ) )
            {


                SDL_apply_surface( pSdlJeu, pSdlJeu->surface_Menu_Sprite[48], pSdlJeu->surface_ecran, 0, 0);
                SDL_apply_surface( pSdlJeu, pSdlJeu->surface_Menu_Sprite[38], pSdlJeu->surface_ecran, 5, hauteur);
                sdlShowTexte(*pSdlJeu, "PokeBall : 50euro", 12,20,0);
                sdlShowTexte(*pSdlJeu, "SuperPokeBall : 75euro", 12,40,0);
                sdlShowTexte(*pSdlJeu, "Potion : 20 euro", 12,60,0);
                sdlShowTexte(*pSdlJeu, "SuperPotion: 40euro", 12,80,0);
                sdlShowTexte(*pSdlJeu, "PotionMax : 70euro", 12,100,0);
                sdlShowTexte(*pSdlJeu, "Rappel : 110euro", 12,120,0);
                sdlShowTexte(*pSdlJeu, "Annuler ", 12,140,0);
                sprintf(arg,"%d",pSdlJeu->jeu.hero.argent);
                sdlShowTexte(*pSdlJeu, arg, 115,4,0);
                SDL_Flip( pSdlJeu->surface_ecran );


         if ( event.type == SDL_KEYDOWN)
			  {
				switch ( event.key.keysym.sym )
				{
					case B:
						    boucle =1;
					break;

					case UP:
								 if(ind>1){ind--; hauteur =hauteur -20; }
					break;

					case DOWN:
								 if(ind<7)
								 {
						ind++;
						hauteur = hauteur +20;


								 }
					break;

				        case  A:/*appuye sur entré*/

						switch (ind)
						{
						    case 1:
						        if(pay(pSdlJeu,50)==0){
						        ajout(pSdlJeu, 5);
						        sdlShowDialog( pSdlJeu, "Vous avez achetez un PokeBall");
						        }
						        else{ sdlShowDialog( pSdlJeu, "Vous ne pouvez pas acheter ce produit.");}
						    break;

						    case 2:
						         if(pay(pSdlJeu,75)==0){
						        ajout(pSdlJeu, 6);
						        sdlShowDialog( pSdlJeu, "Vous avez achetez un SuperPokeBall");
						        }
						        else{ sdlShowDialog( pSdlJeu, "Vous ne pouvez pas acheter ce produit.");}

						    break;



                            case 3:
                                 if(pay(pSdlJeu,20)==0){
						        ajout(pSdlJeu, 1);
						        sdlShowDialog( pSdlJeu, "Vous avez achetez un potion");
						        }
						        else{ sdlShowDialog( pSdlJeu, "Vous ne pouvez pas acheter ce produit.");}

						    break;



                             case 4:
                                     if(pay(pSdlJeu,40)==0){
						        ajout(pSdlJeu, 2);
						        sdlShowDialog( pSdlJeu, "Vous avez achetez un Super potion");
						        }
						        else{ sdlShowDialog( pSdlJeu, "Vous ne pouvez pas acheter ce produit.");}
						    break;


                             case 5:
                                    if(pay(pSdlJeu,70)==0){
						        ajout(pSdlJeu, 3);
						        sdlShowDialog( pSdlJeu, "Vous avez achetez un potionMax");
						        }
						        else{ sdlShowDialog( pSdlJeu, "Vous ne pouvez pas acheter ce produit.");}
						    break;



						     case 6:
                                    if(pay(pSdlJeu,110)==0){
						        ajout(pSdlJeu, 4);

						        sdlShowDialog( pSdlJeu, "Vous avez achetez un Rappel");
						        }
						        else{ sdlShowDialog( pSdlJeu, "Vous ne pouvez pas acheter ce produit.");}

						    break;




						     case 7:  sdlShowDialog(pSdlJeu, "Aurevoir ! A bientot !");
						     boucle=1;



						    break;

                            default: break;

						}
						 /*on sort du magasin*/

					break;


					default: break;
					 }
					 }


          if ( event.type == SDL_QUIT )
                {
                    boucle = 1;
                    SDL_Quit();
                }
        }

    }



}

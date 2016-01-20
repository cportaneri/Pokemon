#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL/SDL.h>
#include "sdl.h"


int main(int argc, char *argv[])
{
    srand(time(NULL));
    /* Initialisation du jeu */
    sdlJeu pSdlJeu;


    Sgame j=CreateGame(2);
    pSdlJeu.jeu = j;

    /*Affichage du menu principal*/
    Menu(&pSdlJeu );
    sdljeuLibere( &pSdlJeu);

    return 0;
}

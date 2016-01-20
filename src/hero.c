#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "hero.h"

/* "Constructeur" de la structure hero */
Shero CreateHero(int x, int y,int z, char * c,int s,int d)
{


    Shero hero;
    hero.pos_x=x;
    hero.pos_y=y;
    hero.id_map=z;
    hero.aAttaquer = 5;
    hero.etatPoke = 0;
    hero.sol=s;
    hero.argent = 0;
    hero.nbObjet = 0;
    hero.enCombat=0;
    hero.direction=d;
    strcpy(hero.image,c);
    hero.nb_pok=1;
    hero.nb_pokReserve=0;
    hero.pokemon[0] = createPokemon("PIKACHU" ,10, 3 , 1 ,"Pika pika",100, 5, 10, 4,5,2,5,2,2,6, 2,1,3);

    hero.pokemon[0].moves[3]= CreateAttack("CHARGE",3,3,3, 0,3,"Charge l'ennemi avec un violent plaquage.");
    hero.pokemon[0].moves[2]= CreateAttack("VIVE-ATTAQUE",3,3,3, 0,3,"lance des plante");
	hero.pokemon[0].moves[0]= CreateAttack("MIMI-QUEUE",3,6,4, 0,3,"Envoie un puissant jet d'eau pour frapper l'ennemi.");
	hero.pokemon[0].moves[1]= CreateAttack("ECRAS'FACE",3,1,8, 1,3,"Une puissante attaque de feu pouvant brûler l'ennemi.");
    hero.nbPokeVu = 1;

    return hero;

}


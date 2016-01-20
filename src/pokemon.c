
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "pokemon.h"


int rand_ab(int a, int b){
    return (rand()/((float)RAND_MAX + 1)*(b-a) +a);
}


Spokemon createPokemon(char name[20] , int number, int type , int sexe , char description[100],int pv, int lvl, int xp, int attack,int defense,int spclatk,int spcldef,int speed,int minisprite,int sprite,int fightsprite, int vu, int capture)
{
        Spokemon pokemon;
        strcpy(pokemon.name,name);
        pokemon.number=number;
        pokemon.type=type;
        pokemon.sexe=sexe;
        strcpy(pokemon.description,description);
        pokemon.pv=pv;
        pokemon.lvl=lvl;
        pokemon.xp=xp;
        pokemon.next_xp=pokemon.lvl*pokemon.lvl*pokemon.lvl*pokemon.lvl; /* Courbe de progression "moyenne"*/
        pokemon.attack=attack;
        pokemon.defense=defense;
        pokemon.spclatk=spclatk;
        pokemon.spcldef=spcldef;
        pokemon.speed=speed;
        pokemon.minisprite=minisprite;
        pokemon.sprite=sprite;
        pokemon.fightsprite=fightsprite;
        pokemon.pvMax = pv;
        pokemon.vu = vu;
        pokemon.capture=capture;

        return pokemon;
}


int capture(int typePokeBall, Spokemon pokemon){

	srand(time(NULL));
	int tmp;
	int chance=0;

	tmp = (pokemon.pv*100)/pokemon.pvMax;
	switch(typePokeBall)
		{
			case 5:
				if(tmp<30){/*si le pv du pokemon sauvage est <30% => on a 25% de chance de le capturer ou de tier sur un 1 */
				 chance =rand_ab(1,4);
				}
				else if((tmp<50)&&(tmp>30)){chance=rand_ab(1,6);}/*30% < pv < 50%   => 1/6 de chance */
				else {chance=rand_ab(1,12);}/*sinon 1/12 de chance */
			break;

			case 6:
				if(tmp<30){
				chance=rand_ab(1,2);
				}
				else if((tmp<50)&&(tmp>30)){chance=rand_ab(1,4);}/*30% < pv < 50%   => 25% de chance */
				else {chance=rand_ab(1,10);}/*sinon 10% de chance */
			break;

			default:
			break;

		}
	if(chance==1)
	{return 1;}

	return 0;


}



void levelUp(Spokemon *pokemon)
{
    if(pokemon->next_xp<=pokemon->xp) /* Si le pokemon a atteint le niveau d'expérience requis par pokemon.next_xp*/
    {
        pokemon->lvl+=1;
	pokemon->xp=pokemon->xp-pokemon->next_xp;
        pokemon->next_xp=pokemon->lvl*pokemon->lvl*pokemon->lvl*pokemon->lvl;
    }

}





void modifpv(int modif, Spokemon *pokemon)
{
    if(pokemon->pv-modif<0){pokemon->pv=0;}
   else{ pokemon->pv = pokemon->pv-modif;}

}



int isdead(Spokemon pokemon)
{
    if(pokemon.pv<=0)
        return 1;


    return 0;
}



int calculdmg(Spokemon pokemonatt,int id_attack,Spokemon pokemondef)
{
    int dmg=0;
    float coeff;
    coeff=coefftype(pokemondef,pokemonatt.moves[id_attack].type);
      if(pokemonatt.moves[id_attack].cat==0)
    {
           dmg= (((pokemondef.lvl*(0.4)+2)*pokemonatt.attack*pokemonatt.moves[id_attack].power)/(pokemondef.defense*50)+2)*coeff;
            return dmg;
    }
    else
    {
          dmg= (((pokemondef.lvl*(0.4)+2)*pokemonatt.spclatk*pokemonatt.moves[id_attack].power)/(pokemondef.spcldef*50)+2)*coeff;
            return dmg;
    }

}







float coefftype(Spokemon pokemon,int type)
{
    switch(type){
	    case 0 :  /*si attaque eau
*/		    if(pokemon.type==2)
		    {
			return (0.5);
		    }
		    else if(pokemon.type==1)
		    {
			return 2;
		    }

		     else if(pokemon.type==0)
		    {
			return (0.5);
		    }


		    else{
			return 1;
			}


		    break;





	    case 1 :   /*si attaque feu*/
		    if(pokemon.type==2)
		    {
			return 2;

		    }
		    else if(pokemon.type==0)
		    {
			return (0.5);
		    }

		     else if(pokemon.type==1)
		    {
			return (0.5);
		    }


		    else{
			return 1;
			}


		    break;







	    case 2 :  /* si attaque plante*/
		    if(pokemon.type==0)
		    {
			return 2;

		    }
		    else if(pokemon.type==1)
		    {
			return (0.5);
		    }

		    else if(pokemon.type==2)
		    {
			return (0.5);
		    }


		    else{
			return 1;
			}
		    break;

	    case 3 : /* si attaque normale*/
		    return 1;
		    break;


    }

	return 0;

}

void initPokemonStat(Spokemon *pkmn,int lvl){

    pkmn->pvMax=(((pkmn->pvMax+50)*lvl)/50)+10;
    pkmn->attack=((pkmn->attack+50*lvl)/50)+5;
    pkmn->spclatk=((pkmn->spclatk+50*lvl)/50)+5;
    pkmn->defense=((pkmn->defense+50*lvl)/50)+5;
    pkmn->spcldef=((pkmn->spcldef+50*lvl)/50)+5;
    pkmn->speed=((pkmn->speed+50*lvl)/50)+5;


}

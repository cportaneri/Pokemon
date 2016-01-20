
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


typedef struct
{


	char nom[20];
	int nbObjetCourant;
	int effet;/*1 +50pv , 2 = +100pv, 3 potion max, 4 reanime le pokemon , 5 pokeball, 6 superball */


}Objet;



/** \brief Creation d'un Objet
 *
 *
 * \return Objet
 *
 */
Objet CreateObjet(char *nom, int nbObjetCourant, int effet);


#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "objet.h"




Objet CreateObjet(char *nom, int nbObjetCourant, int effet){

	Objet newObjet;
	strcpy(newObjet.nom,nom); 
	newObjet.nbObjetCourant = nbObjetCourant;
	newObjet.effet = effet;
	return newObjet;


}


#include "game.h"

Sgame CreateGame(int id_map) {

    Sgame game;

    /* Creation du hero */
    Shero Heros = CreateHero(POSITION_DEPART_HEROS_X, POSITION_DEPART_HEROS_Y,id_map,"sasha",PARQUET,DIRECTION_DEPART_HEROS);
    game.hero= Heros;

    /* Demarrage du son */
    FMOD_SYSTEM *system;
    FMOD_SOUND *son;
    FMOD_System_Create(&system);
    FMOD_System_Init(system, 7, FMOD_INIT_NORMAL, NULL);
    FMOD_System_CreateSound(system, "data/music/Menutheme.mp3",  FMOD_2D | FMOD_CREATESTREAM | FMOD_LOOP_NORMAL, 0, &son);
    FMOD_Sound_SetLoopCount(son, -1);
    FMOD_System_PlaySound(system, son, NULL, 0, NULL);
    game.pokedex[9]=game.hero.pokemon[0];
    game.pokedex[9].vu=1;
    game.son = son;
    game.system = system;
    game.scenario=0;

    /* Ajout des personnages non jouables du jeu */
    addNpc(&game);

	return game;
}

void teleportationHeros(Sgame *game,int id_map,int pos_x,int pos_y,int direction,int sol){

    int i,j;

    /* Mise a jour de la position et direction du heros */
    game->hero.pos_x=pos_x;
    game->hero.pos_y=pos_y;
    game->hero.direction=direction;

    /* Ouverture du fichier texte de la map */
    FILE* fichier=NULL;
    char map_path[50];
    sprintf(map_path,"data/map_file/%d.txt",id_map);

    fichier= fopen(map_path,"r");

    if (fichier != NULL){
        for(i=0;i<MAX_SIZE_MAP_Y;i++){
            for(j=0;j<MAX_SIZE_MAP_X;j++){

                    /* Recuperation des données dans le game.field */
                    fscanf(fichier,"%d %d ",&game->field[i][j].chipset,&game->field[i][j].type);



            }
        }
    }

    /* Mise a jour du sol et de la map du heros */
    game->field[game->hero.pos_y][game->hero.pos_x].chipset=-1;
    game->hero.sol=sol;
    game->hero.id_map=id_map;

}

void changeSound(Sgame *g,int new_song){

    /* Fermeture du son  actuel */
    FMOD_System_Close( g->system );
    FMOD_System_Release( g->system );

    FMOD_SYSTEM *system;
    FMOD_SOUND *son;

    FMOD_System_Create(&system);
    FMOD_System_Init(system, 7, FMOD_INIT_NORMAL, NULL);
    /*on selection un nouveau son*/
    switch(new_song) {

        case 1: FMOD_System_CreateSound(system, "data/music/route .mp3",FMOD_2D | FMOD_CREATESTREAM | FMOD_LOOP_NORMAL, 0, &son);
                break;

        case 2: FMOD_System_CreateSound(system, "data/music/Town.mp3", FMOD_2D | FMOD_CREATESTREAM | FMOD_LOOP_NORMAL, 0, &son);
                break;

        case 3: FMOD_System_CreateSound(system, "data/music/Pokemon_Center.mp3",  FMOD_2D | FMOD_CREATESTREAM | FMOD_LOOP_NORMAL, 0, &son);
                break;
        case 4 : FMOD_System_CreateSound(system, "data/music/combat.mp3",  FMOD_2D | FMOD_CREATESTREAM | FMOD_LOOP_NORMAL, 0, &son);
                break;
        case 5: FMOD_System_CreateSound(system, "data/music/Gym.mp3",  FMOD_2D | FMOD_CREATESTREAM | FMOD_LOOP_NORMAL, 0, &son);
                break;

    }

    FMOD_Sound_SetLoopCount(son, -1);/*on joue le son en boucle*/
    FMOD_System_PlaySound(system, son, NULL , 0, NULL);

    g->son = son;
    g->system = system;

}

void addNpc(Sgame *g){

    g->charset[NPC_BIGBOY]=CreateNpc(8,9,4,4,3,"Ici c'est le centre de soin Pokemon. Tu peux venir soigner tes Pokemons quand tu veux!");
    g->charset[NPC_BIGBOY2]=CreateNpc(17,23,1,0,2,"Salut Sacha ! Tu cherches quelque chose? Le laboratoire du professeur est a l'Ouest, la route au Sud et l'arene Pokemon au Nord!");
    g->charset[NPC_BOY]=CreateNpc(5,6,5,12,4,"");
    g->charset[NPC_BOY2]=CreateNpc(14,32,9,12,2,"Tu viens de New Bark Town? Ici marque la fin de la route, a l'Est se trouve la foret de Jade. Ne t'y aventures pas trop, les Pokemons y sont dangeureux..");
    g->charset[NPC_GIRL]=CreateNpc(6,7,8,24,2,"I am error");
    g->charset[NPC_GIRL2]=CreateNpc(9,30,1,20,4,"Si tu veux soigner tes Pokemons tu peux aller au centre Pokemon juste ici. Tu peux egalement acheter des potions et pokeballs dans le magasin juste a cote.");
    g->charset[NPC_MAN]=CreateNpc(12,6,5,28,1,"Il faut toujours avoir des potions et des pokeball sur soi! On ne sait jamais!");
    g->charset[NPC_MOTHER]=CreateNpc(11,9,3,32,2,"Profites de cette journee ensoleille!");
    g->charset[NPC_NURSE]=CreateNpc(7,5,4,36,2,"");
    g->charset[NPC_OLD1]=CreateNpc(23,28,1,38,2,"New Bark Town est un village paisible, mais j'aime me promener sur la route au Sud. Tu devrais y jeter un coup d'oeil surtout si tu es un dresseur..");
    g->charset[NPC_OLD2]=CreateNpc(14,8,1,8,2,"Desole petit mais il faut que tu attrapes au moins six Pokemons avant de pouvoir entrer dans l'arene. Comment? Tu ne sais pas attraper de pokemon? Il faut utiliser des pokeball quand ton adversaire est faible pour le capturer, tout le monde sait ca..");
    g->charset[NPC_PROF1]=CreateNpc(9,6,7,42,2,"Entraine toi et deviens un bon dresseur.");
    g->charset[NPC_PROF2]=CreateNpc(6,13,7,16,4,"Le savais-tu? Les Pokemons type feu craignent l'eau, les types eau les plantes et les plantes le feu! Choisis bien tes pokemons en fonction de ton adversaire!");
    g->charset[NPC_RIVAL]=CreateNpc(9,17,1,46,2,"C'est donc ici le laboratoire du professeur.....");

}

void moveDir(Sgame *g, Shero *p,int etat,int direction){

    /* Etat = 0->debut du deplacement 1->Fin du deplacement*/
    int i,j,k,m,dir;

    if (etat==0) {

         for (i=0;i<MAX_SIZE_MAP_Y;i++){
            for(j=0;j<MAX_SIZE_MAP_Y;j++){

                 if (etat==0) {

                    if (g->field[i][j].chipset== -1){

                         switch(direction) {
                            case 1:
                                k= i-1;
                                m= j;
                                dir=1;
                                break;

                            case 2:
                                k= i+1;
                                m= j;
                                dir=2;
                                break;

                            case 3:
                                k= i;
                                m= j-1;
                                dir=3;
                                break;

                             case 4:
                                k= i;
                                m= j+1;
                                dir=4;
                                break;
                        }

                        p->direction=dir;


                            g->field[i][j].chipset = p->sol;



                            p->sol=g->field[k][m].chipset;

                    }
                }
            }
        }
    }

    else if (etat==1) {
        switch(direction) {
            case 1:
                g->field[p->pos_y-1][p->pos_x].chipset= -1;
                p->pos_y--;
                break;

            case 2:

                g->field[p->pos_y+1][p->pos_x].chipset = -1;
                p->pos_y++;
                break;

            case 3:
                g->field[p->pos_y][p->pos_x-1].chipset = -1;
                p->pos_x--;
                break;

            case 4:
                g->field[p->pos_y][p->pos_x+1].chipset = -1;
                p->pos_x++;
                break;
        }

    }

}

int CheckDir(Sgame * game,int direction){

	int x = game->hero.pos_x;
	int y = game->hero.pos_y;

	int m,n;

	switch(direction){/*selon le direction du hero, on test si l'endroit devant lui n'est pas occupé */
        case 1:
            m=y-1;
            n=x;
            break;
        case 2:
            m=y+1;
            n=x;
            break;
        case 3:
            m=y;
            n=x-1;
            break;
        case 4:
            m=y;
            n=x+1;
            break;

	}

	if(game->field[m][n].type == VIDE){
            return (0);
    }

    else{
        return(1);
    }

}

void saveGame(Sgame *game){

    FILE* fichier = NULL;/*on crée un fichier text*/
    int i,j;

    fichier= fopen("save/save.txt","w+");



    if(fichier!=NULL)
    {
     /* traitement de la sauvegarde */
     fprintf(fichier,"%d %d %d %d %d %d %d %d %d %d ",game->scenario,game->hero.pos_x,game->hero.pos_y,game->hero.id_map,game->hero.sol,game->hero.direction,game->hero.nb_pok, game->hero.argent, game->hero.nbObjet, game->hero.nbPokeVu);
     fclose(fichier);
     fichier= fopen("save/save.txt","a");



     for(i=0;i<game->hero.nb_pok;i++)
     {
        fprintf(fichier,"%d %d %d %d %d %d %d %d %d %d %d %d %s ",game->hero.pokemon[i].number,game->hero.pokemon[i].pvMax,game->hero.pokemon[i].pv,game->hero.pokemon[i].lvl,game->hero.pokemon[i].xp,game->hero.pokemon[i].attack,
        game->hero.pokemon[i].defense,game->hero.pokemon[i].spcldef,game->hero.pokemon[i].spclatk,game->hero.pokemon[i].speed,game->hero.pokemon[i].vu,game->hero.pokemon[i].capture,game->hero.pokemon[i].name
        );
        for(j=0;j<4;j++)
        {
             fprintf(fichier,"%d %d %s %d ",game->hero.pokemon[i].moves[j].pp,game->hero.pokemon[i].moves[j].ppTotal, game->hero.pokemon[i].moves[j].name, game->hero.pokemon[i].moves[j].type);
        }
     }








/*
     fclose(fichier);
     fichier= fopen("save/save.txt","a");
*/
       i=0;
        j=0;/*****************sauvegarde pokedex*******************************************************/
      while(j<game->hero.nbPokeVu){
            if(game->pokedex[i].number!=0){
           fprintf(fichier,"%d %s %d %d ",game->pokedex[i].number,game->pokedex[i].name,game->pokedex[i].type,game->pokedex[i].vu);
            j++;
            i++;
            }
           else{ i++;}

        }
     fclose(fichier);
     fichier= fopen("save/save.txt","a");






    for(i=0;i<game->hero.nbObjet;i++){
        fprintf(fichier,"%d %d %s ", game->hero.mesObjet[i].effet, game->hero.mesObjet[i].nbObjetCourant,game->hero.mesObjet[i].nom);

    }

    fclose(fichier);
    }
    else
    {
        /* erreur de la sauvegarde */
    }
}

void loadGame(Sgame *game){
    FILE* fichier=NULL;
    int i,j;
    int tmp;

    fichier= fopen("save/save.txt","r");

    if(fichier!=NULL)
    {
        /* traitement du chargement */
        fscanf(fichier,"%d %d %d %d %d %d %d %d %d %d",&game->scenario,&game->hero.pos_x,&game->hero.pos_y,&game->hero.id_map,&game->hero.sol,&game->hero.direction,&game->hero.nb_pok,&game->hero.argent,&game->hero.nbObjet, &game->hero.nbPokeVu);



        for(i=0;i<game->hero.nb_pok;i++)
        {
            fscanf(fichier," %d %d %d %d %d %d %d %d %d %d %d %d %s" ,&game->hero.pokemon[i].number,&game->hero.pokemon[i].pvMax,&game->hero.pokemon[i].pv,&game->hero.pokemon[i].lvl,&game->hero.pokemon[i].xp,&game->hero.pokemon[i].attack,
            &game->hero.pokemon[i].defense,&game->hero.pokemon[i].spcldef,&game->hero.pokemon[i].spclatk,&game->hero.pokemon[i].speed,&game->hero.pokemon[i].vu,&game->hero.pokemon[i].capture,game->hero.pokemon[i].name);
            for(j=0;j<4;j++)
            {
                fscanf(fichier," %d %d %s %d",&game->hero.pokemon[i].moves[j].pp,&game->hero.pokemon[i].moves[j].ppTotal,game->hero.pokemon[i].moves[j].name,&game->hero.pokemon[i].moves[j].type);
            }

        }





          for(i=0;i<game->hero.nbPokeVu;i++){
            fscanf(fichier," %d", &tmp);
            printf(" %d", tmp);
            game->pokedex[tmp-1].number = tmp;
            fscanf(fichier," %s %d %d",game->pokedex[tmp-1].name,&game->pokedex[tmp-1].type,&game->pokedex[tmp-1].vu);

        }












        for(i=0;i<game->hero.nbObjet;i++){
        fscanf(fichier," %d %d %s", &game->hero.mesObjet[i].effet, &game->hero.mesObjet[i].nbObjetCourant, game->hero.mesObjet[i].nom);

    }

        teleportationHeros(game,game->hero.id_map,game->hero.pos_x,game->hero.pos_y,game->hero.direction,game->hero.sol);
    }
    fclose(fichier);

}

Spokemon randPokemon(Sgame game) {

    int i,rand_lvl,j;
    int average_lvl=0;
    Spokemon randpkmn;
    int rand = rand_ab(1,10);
    FILE* fichier=NULL;
    fichier= fopen("data/other_file/BDPokemon.txt","r");
    /* Recuperation des caracteristiques initiales d'un pokemon aleatoire dans la base de donne*/
    for(i=0;i<rand;i++){

        fscanf(fichier,"%s %d %d %d %d %d %d %d %d %s %d %d %d %d %d %s %d %d %d %d %d %s %d %d %d %d %d %s %d %d %d %d %d ",randpkmn.name,&randpkmn.number,&randpkmn.type,&randpkmn.pvMax,&randpkmn.attack,&randpkmn.defense,&randpkmn.spclatk,&randpkmn.spcldef,&randpkmn.speed,randpkmn.moves[0].name,&randpkmn.moves[0].type,&randpkmn.moves[0].power,&randpkmn.moves[0].ppTotal,&randpkmn.moves[0].cat,&randpkmn.moves[0].prec,randpkmn.moves[1].name,&randpkmn.moves[1].type,&randpkmn.moves[1].power,&randpkmn.moves[1].ppTotal,&randpkmn.moves[1].cat,&randpkmn.moves[1].prec,randpkmn.moves[2].name,&randpkmn.moves[2].type,&randpkmn.moves[2].power,&randpkmn.moves[2].ppTotal,&randpkmn.moves[2].cat,&randpkmn.moves[2].prec,randpkmn.moves[3].name,&randpkmn.moves[3].type,&randpkmn.moves[3].power,&randpkmn.moves[3].ppTotal,&randpkmn.moves[3].cat,&randpkmn.moves[3].prec);

    }

    /* calcul du niveau moyen des pokemons du hero */
    for(i=0;i<game.hero.nb_pok;i++){
            average_lvl=average_lvl+game.hero.pokemon[i].lvl;
            for(j=0;j<4;j++){
                game.hero.pokemon[i].moves[j].pp= game.hero.pokemon[i].moves[j].ppTotal;

            }
    }

    average_lvl=average_lvl/game.hero.nb_pok;

    /*calcul de niveau de pokemon aleatoire*/

    rand_lvl=rand_ab(average_lvl-2,average_lvl+2);

    initPokemonStat(&randpkmn,rand_lvl);

    randpkmn.pv=randpkmn.pvMax;
    randpkmn.xp=0;
    randpkmn.capture=0;
    randpkmn.lvl=rand_lvl;


    return randpkmn;


}



/* *********************************************************************

projet réalisé dans le cadre du master PISE / Université Paris Diderot 
par Lyes KESSAL & Michael ARBIB
année universitaire 2015/2016

objectif :

réaliser un programme qui indique le plus court chemin d'un point A à un point B, en langage C

********************************************************************* */

#define _GNU_SOURCE /*ajout car strcasestr n'est pas une fonction standard du C*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#define NB_JONCTIONS 520 		// à modifier en fonction du nombre de jonctions défini
#define TAILLE_NOM_FICHIER 100 	// taille max d'un nom de fichier 
#define TAILLE_NOM_JONCTION 70 	// taille max d'un nom de jonction --> 62+4 caractères max 
#define NON_TROUVE -1
#define INFINI 9999

/* définition d'une jonction */
typedef struct jonction Jonction ;
struct jonction 
{
	char nom[TAILLE_NOM_JONCTION]	;
	int antecedent					;
	int longueur					;
	int passage						;
};

/* appel des procédures et fonctions principales */
int init_jonction() 			 														;
int init_rues_distances(int choix_mode)													;
void reinit_jonctions() 																;
int recherche_nom_rue()			 														;
int mode_transport()																	;
int plus_courte_jonction()																;
void maj_longueur_jonctions(int antecedent)												;
void dijkstra(int point_arrivee)														;
void affiche_chemin(int num_jonction_depart, int num_jonction_arrivee, int choix_mode)	;

/* appel des procédures et fonctions annexes */
void purge()					 														;
void conv_char_speciaux(char saisie[])													;
char RemplaceLettre(char c)																;
void verif_saisie_numerique(char saisie[], int *saisie_ok)								;
void recup_saisie_user(char *ch)														;
void save_trajet(int num_jonction_depart, int num_jonction_arrivee, int choix_mode) 	;

/* variables globales */

// tableau listant toutes les jonctions 
Jonction tab_jonctions[NB_JONCTIONS]; 

// tableau représentant la matrice des noms des rues selon les jonctions, sert à récupérer le contenu du fichier de noms des rues
char tab_noms_rues[NB_JONCTIONS][NB_JONCTIONS][TAILLE_NOM_JONCTION]; // 3e dimension est pour la taille des string stockés (possibilité de l'enlever et de mettre un pointeur mais attention à faire les malloc lors d'insertion)

// équivalent à tab_noms_rues mais pour les distances
int tab_longueur[NB_JONCTIONS][NB_JONCTIONS]; // 

// variable de parcours, compte le nombre total de jonctions
int nbjonction = 0;


/* programme principal */

int main(int argc, char const *argv[])
{
	int point_depart = NON_TROUVE 					; 
	int point_arrivee = NON_TROUVE 					;
	int choix_mode = 0								;
	int choix_menu = NON_TROUVE 					;
	int itineraire_de_base_calcule = NON_TROUVE		;
	int retour_checked = NON_TROUVE					;
	int alternatif_checked = NON_TROUVE				;

	if (init_jonction() != NON_TROUVE) {
		printf("\nBienvenue dans le programme de calcul du chemin le plus court.\nCe programme vous donnera le trajet le plus court entre 2 intersections de rues du 11e arrondissement de Paris.\n");
		
		while (choix_menu != 0) {
			choix_menu = NON_TROUVE 														; // on réinitialise la variable pour le cas où on a bouclé
			printf("\n************** Menu ************** \n")								;
			printf("1 - Calculer un itinéraire principal\n")								;
			if (itineraire_de_base_calcule != NON_TROUVE){
				printf("2 - Calculer le trajet de retour\n")										;
				printf("3 - Recalculer l'itinéraire principal avec l'autre mode de transport\n")	;
			}
			printf("\n0 - Quitter le programme\n")											;
			printf("\nVotre choix : ")														;
			scanf("%d", &choix_menu)														;
			printf("\n")																	;

			switch (choix_menu) 
			{
				case 0 : // quitter
					printf("\nMerci d'avoir utilisé ce programme.\nAu revoir et à bientôt.\n");
					break ;
				case 1 : // calcul itinéraire
					choix_mode = mode_transport()	;
					if (init_rues_distances(choix_mode) != NON_TROUVE) {
						if (itineraire_de_base_calcule == 1) 
						{
							reinit_jonctions() 									;
						}
						point_depart = recherche_nom_rue("de départ")			; // numéro du point de départ
						point_arrivee = recherche_nom_rue("d'arrivée")			; // numéro du point d'arrivée
						tab_jonctions[point_depart].longueur = 0				; // initialisation de la longueur de la rue du point de départ à 0
						dijkstra(point_arrivee)									; // fonction qui utilise l'algo de Dijkstra
						affiche_chemin(point_depart, point_arrivee, choix_mode) ; // affichage 
						itineraire_de_base_calcule = 1 							;
						choix_menu = NON_TROUVE									; // remise à état initial de la variable choix_menu
						retour_checked = NON_TROUVE								; // ré-init de la variable pour faire le retour sur nouvel itinéraire
						alternatif_checked = NON_TROUVE 						; // ré-init de la variable pour faire alternatif sur nouvel itinéraire
					}

					break ;
				case 2 : // trajet retour suite au choix 1
					if (itineraire_de_base_calcule == NON_TROUVE)
					{
						printf("Merci de saisir un choix valide\n")				;
					} else if (retour_checked == 1) {
						printf("Vous avez déjà calculé l'itinéraire de retour pour ce trajet.\n");
					} else { // on inverse les points de départ et arrivée pour recalculer l'itinéraire de retour
						reinit_jonctions() 										; // ré-initialisation du tableau des jonctions pour repartir du point d'arrivée du calcul d'itinéraire précédent
						tab_jonctions[point_arrivee].longueur = 0				; // initialisation de la longueur de la rue du point d'arrivée à 0
						dijkstra(point_depart) 									; // fonction qui utilise l'algo de Dijkstra
						affiche_chemin(point_arrivee, point_depart, choix_mode) ; // affichage  
						retour_checked = 1										;
						choix_menu = NON_TROUVE									; // remise à état initial de la variable choix_menu
					}
					break ;
				case 3 : // mode de transport alternatif avec itinéraire du choix 1
					if (itineraire_de_base_calcule == NON_TROUVE)
					{
						printf("Merci de saisir un choix valide\n")	;
					} else if (alternatif_checked == 1) {
							printf("Vous avez déjà visualisé l'itinéraire alternatif pour ce trajet.\n");
					} else {
						if (choix_mode == 1)
						{
							choix_mode = 2 ;
						}  else {
							choix_mode = 1 ; 
						}
						//printf("mode --> %d\n", choix_mode);
						if (init_rues_distances(choix_mode) != NON_TROUVE) 
						{
							reinit_jonctions()										; // ré-initialisation du tableau des jonctions pour recalculer l'itinéraire précédent
							tab_jonctions[point_depart].longueur = 0				; // initialisation de la longueur de la rue du point de départ à 0
							dijkstra(point_arrivee)									; // fonction qui utilise l'algo de Dijkstra
							affiche_chemin(point_depart, point_arrivee, choix_mode) ; // affichage 
							alternatif_checked = 1 									;
							choix_menu = NON_TROUVE 								; // remise à état initial de la variable choix_menu
						}
					}
					break ;
				default : 
					printf("Merci de saisir un choix valide\n")	;
					purge()				 						;
			}
		}
	}
}

/* code des procédures et fonctions */

/* initialisation des jonctions dans tab_jonctions */

int init_jonction()
{
	int test_init = 0 ;
	FILE *fichier_jontions = fopen("jeu_donnees/noms_jonctions.txt","r");
	char nom_jonction[TAILLE_NOM_JONCTION];

	// test ouverture fichier
	if (fichier_jontions == NULL) {
		// si ouverture fichier ko 
		test_init = NON_TROUVE;
		printf("Erreur de chargement du fichier des noms de jonction.\n");
	} else {
		// si ouverture fichier ok 
		while (!feof(fichier_jontions)){ // possibilité de faire un tableau dynamique avec un malloc et de supprimer #DEFINE NB_JONCTIONS ?
			if (fscanf(fichier_jontions, "%s", nom_jonction) != EOF){
				strcpy(tab_jonctions[nbjonction].nom, nom_jonction)	; // insertion de la ligne du fichier dans le nom de chaque structure jonction
				tab_jonctions[nbjonction].antecedent = NON_TROUVE	;
				tab_jonctions[nbjonction].longueur = INFINI			;
				tab_jonctions[nbjonction].passage = 0				;
				nbjonction++										;
			}
		}
	}
	fclose(fichier_jontions);
	return test_init ;
}

/* initialisation des noms des rues et de leur longueur */

int init_rues_distances(int choix_mode)
{
	int test_init = 0						;
	char nom_rue[TAILLE_NOM_JONCTION]		;
	FILE *fichier_noms, *fichier_longueur 	;

	// définition des fichiers selon mode piéton ou voiture
	if(choix_mode == 1){
		fichier_noms = fopen("jeu_donnees/pieton_noms_rues.txt","r")			;
		fichier_longueur = fopen("jeu_donnees/pieton_longueurs_rues.txt","r")	;
	} else {
		fichier_noms = fopen("jeu_donnees/voiture_noms_rues.txt","r")			;
		fichier_longueur = fopen("jeu_donnees/voiture_longueurs_rues.txt","r")	;
	}

	// test ouverture fichiers
	if (fichier_noms == NULL || fichier_longueur == NULL)
	{	
		// si ouverture fichier ko 
		test_init = NON_TROUVE;
		if (fichier_noms == NULL){
			printf("Erreur de chargement du fichier des noms de rues.\n")		;	
		} else {
			printf("Erreur de chargement du fichier des longueurs de rues.\n")	;
		}
	} else {
		// si ouverture fichier ok
		// double boucle de parcours pour les affecter les données des fichiers 
		for(int i = 0; i < nbjonction; i++){
			for(int j = 0; j < nbjonction; j++){
				fscanf(fichier_noms,"%s", nom_rue)						; // dans matrice des noms
				strcpy(tab_noms_rues[i][j], nom_rue)					;
				fscanf(fichier_longueur, "%d", &tab_longueur[i][j])		; // dans matrice des longueurs
			}
		}
	}
	fclose(fichier_noms);
	fclose(fichier_longueur);
	return test_init;
}

/* procédure de ré-initialisation des jonctions dans tab_jonctions */
void reinit_jonctions() {
	for (int i = 0; i < nbjonction; i++) {
		tab_jonctions[i].antecedent = NON_TROUVE	;
		tab_jonctions[i].longueur = INFINI			;
		tab_jonctions[i].passage = 0				;
	}
}

/* recherche nom rue suite à saisie utilisateur */

int recherche_nom_rue(char contexte[20]) 
{
	int choix_ok = 0, nb_result = 0	, saisie_ok=0				;
	int choix = NON_TROUVE, test_saisie_char = 0 				;
	char nom_rue[TAILLE_NOM_JONCTION]							;
	char *test, tab_result[NB_JONCTIONS][TAILLE_NOM_JONCTION]	;


	purge(); /*On vide le buffer avant de demander la saisie à l'utilisateur*/
	while(nb_result == 0){
		while (saisie_ok == 0){
			saisie_ok=1;
  			printf("\nEntrez le nom de la voie, sans son type (rue, avenue, boulevard, ...)\nExemple : pour la rue de la Roquette, tapez roquette \nNom du point %s : ", contexte);
  			recup_saisie_user(nom_rue);
  			if (nom_rue[0] == '\0'){
  				saisie_ok=0;
  				printf ("Attention! Vous avez validé en n'ayant fait aucune saisie!\n");
  			}
  			else{
				verif_saisie_numerique(nom_rue, &saisie_ok);
				if (saisie_ok==0) printf("Votre saisie ne doit pas contenir de caractères de type numérique\n");
  			}
		}
		saisie_ok=0;	/*on remet saisie_ok à 0 sinon on part en boucle infinie dans le cas où la recherche ne donne rien*/
		conv_char_speciaux(nom_rue);

		for(int i = 0 ; i < nbjonction ; i++) // boucle de recherche du nom saisie dans liste des rues
		{
			test = strcasestr(tab_jonctions[i].nom, nom_rue) ;
			if(test != NULL)
			{
				nb_result++;
				printf("%-s\n", tab_jonctions[i].nom)		;
				strcpy(tab_result[i], tab_jonctions[i].nom)	;
			}
		}
		if(nb_result == 0) {
				//purge();
				printf("Le nom que vous avez saisi ne correspond à aucune données en mémoire.\nMerci de ressaisir le nom ou de choisir une autre rue.\n\n");
		}
	}
	
	printf("\nEntrez le numéro de jonction correspondant à votre point %s : ", contexte) ;
	while(!choix_ok) // boucle de vérification de la saisie
	{
		test_saisie_char = scanf("%d",&choix) ;
		if(test_saisie_char)
		{
			for(int i = 0 ; i < NB_JONCTIONS ; i++)
			{
				if (choix == atoi(tab_result[i]) && choix != 0) 
				{
					choix_ok = 1 ;
					printf("Vous avez sélectionné : %s\n", tab_result[i]); // affichage du choix
				}
			}
		} else {
			purge();
		}

		if (!choix_ok) 
		{
			printf("Choix erroné. Merci de renseigner un numéro valide : ");
			//printf("%d\n", choix);
		}
	}
	return choix-1;

}

/* choix du mode de transport */

int mode_transport(){
	int choix_mode = 0 ;
	while(choix_mode != 1 && choix_mode != 2){	
		printf("\nChoisissez votre mode de transport :\n\n")	;
		printf("1 - piéton\n2 - voiture\n\n")					;
		printf("Choix : ")										;
		scanf("%d", &choix_mode)								;
		if(choix_mode != 1 && choix_mode != 2){
			purge();
			printf("Merci de sélectionner le choix 1 ou 2.\n\n");
			}
		}
	return choix_mode;
}

/* recherche de la jonction avec la longueur la plus faible */
int plus_courte_jonction() {
	int courte = NON_TROUVE, longueur = INFINI ;
	for (int i = 0; i < nbjonction; i++)
	{
		if (tab_jonctions[i].passage == 0 && tab_jonctions[i].longueur < longueur)
		{
			longueur = tab_jonctions[i].longueur 	;
			courte = i 								;
		}
	}
	return courte ;
}

/* mise à jour de la longueur des jonctions adjacentes à celle de longueur minimum */
void maj_longueur_jonctions(int antecedent) {
	for (int i = 0; i < nbjonction; i++){ // on parcourt l'intégralité du tableau des jonctions pour la m-à-j
		if ((tab_jonctions[i].longueur > (tab_jonctions[antecedent].longueur + tab_longueur[antecedent][i])) && tab_jonctions[i].passage == 0) 
		{ // elle est faite seulement si la longueur de la jonction est supérieure à celle de son antécédant + la longueur entre les deux (dans le tableau des longueurs) et si on n'est pas déjà passé par cette jonction (cf : pathfinding avec dijkstra - openclassroom)
			tab_jonctions[i].longueur = tab_jonctions[antecedent].longueur + tab_longueur[antecedent][i] 	; // m-à-j longueur 
			tab_jonctions[i].antecedent = antecedent 														; // m-à-j antécédent 
		}
	}
}

/* algo de dijkstra */

void dijkstra(int point_arrivee) {
	int jonction_tmp = NON_TROUVE 	;
	
	while ((jonction_tmp = plus_courte_jonction()) != point_arrivee) { // tant que la plus courte jonction traitée n'est pas la jonction d'arrivée
		tab_jonctions[jonction_tmp].passage = 1 ;  // on marque la jonction pour dire qu'on y est passé et qu'elle ne nous intéresse plus
		maj_longueur_jonctions(jonction_tmp)	; // on calcule la longueur qui la sépare des prochaines jonctions et on reboucle avec la plus courte longueur trouvée
		}
		//printf("\nnom --> %s | longueur --> %d\n", tab_jonctions[point_arrivee].nom, tab_jonctions[point_arrivee].longueur);		// test de longueur finale
}


/* procédure d'affichage de l'itinéraire détaillé */
void affiche_chemin(int num_jonction_depart, int num_jonction_arrivee, int choix_mode) {

  	/*Si la longueur de la jonction d'arrivée est à 9999, il n'y a pas de chemin*/
  	if (tab_jonctions[num_jonction_arrivee].longueur == INFINI)
    	printf("Aucun chemin n'existe !\n");
    /*Sinon, si la longueur de la jonction d'arrivée est différent de 9999*/
  	else
  	{ 	
  		/*Création des variables qui seront utilisées*/
  		int etapes[NB_JONCTIONS]    	; /*Tableau pour stocker le chemin qui mène du point de départ au point d'arrivée (dans le sens inverse)*/
		int jonction_temp				; /*Jouera le rôle de mémoire temporaire*/
  		int jonction_j1, jonction_j2	; /*seront utilisées pour les calculs de l'affichage du chemin inverse (récupérer les infos depuis les matrices)*/
  		int nb_jonctions_afficher=1		; 
  		int save_test = NON_TROUVE		;
  		
  		etapes[0]=num_jonction_arrivee		; /*on met la jonction d'arrivée dans la 1ère case du tableau*/
  		jonction_temp=num_jonction_arrivee	; /*Initailisation de la variable avec la valeur de num_jonction_arrivee*/
  		
    	while (jonction_temp != NON_TROUVE) 
    	{ 
    		etapes[nb_jonctions_afficher++]=jonction_temp			; /*on commence par mettre le num de la jonction d'arrivée dans la case avec l'indice 1 et on boucle tant que la condition est respectée*/
      		jonction_temp = tab_jonctions[jonction_temp].antecedent ; /*on affecte le prédecesseur à jonction_temp pour remonter jusqu'au point de départ */
    	}
    	
    	if (choix_mode == 1) 
	    {
	    	printf("À pied, ");
	    } else {
	    	printf("En voiture, ");
	    }
	    printf("voici le plus court chemin pour aller de : \n %s \n\t à %s \n\n", tab_jonctions[num_jonction_depart].nom, tab_jonctions[num_jonction_arrivee].nom)					;
	    
	    for(int i=nb_jonctions_afficher-1; i>0; i--) /*On commence une case */
	    { 
	    	jonction_j1 = etapes[i]	;	 /*On affecte à jonction_j1 le numéro de la jonction qui se trouve dans la case i*/	
	      	if(jonction_j1 == num_jonction_arrivee)	 /*On teste si jonction_j1 est la jonction d'arrivée. Si oui, on arrête l'affichage et on lui affiche le récapitulatif du trajet*/
	      	{
	       		printf("\nVous êtes arrivé(e) à %s.\nDistance totale parcourue (en mètres) : %d\n",tab_jonctions[jonction_j1].nom, tab_jonctions[num_jonction_arrivee].longueur)		;
	      	} else { 
	      		jonction_j2 = etapes[i-1]			;	/*jonction_j2 se voit affecter le numéro de jonction qui se trouve en i-1*/
	        	printf("de %-70s  ",tab_jonctions[jonction_j1].nom);
	        
	       		/*test si mode 1 ou mode 2 : peut avoir un impact sur l'affichage*/
	        	if (choix_mode==1)
	        	{	/*Mode piéton: n'a pas d'impact sur l'affichage car les matrices sont symétriques*/
	        		printf("suivre %-30s  ",tab_noms_rues[jonction_j1][jonction_j2])				;
		        	printf("vers %-70s  ",tab_jonctions[jonction_j2].nom)        					;
		        	printf("[Distance : %3d mètres]\n",tab_longueur[jonction_j1][jonction_j2])   	;
		        } else { 		/*si le mode de transport est voiture, alors on teste si c'est un sens interdit pour récupérer les bonnes informations. Sinon, on risque de se retrouver avec des 9999 ou des INFINI comme valeurs*/
		        	if (tab_longueur[jonction_j1][jonction_j2] != INFINI)
		        	{							/*si le sens de circulation va de J1 à J2*/
			        	printf("suivre %-30s  ",tab_noms_rues[jonction_j1][jonction_j2])			;
		        		printf("vers %-70s  ",tab_jonctions[jonction_j2].nom)        				;
		        		printf("[Distance : %3d mètres]\n",tab_longueur[jonction_j1][jonction_j2])  ;
		        	} else { 	/*si le sens de circulation va de J2 à J1*/
			        	printf("suivre %-30s  ",tab_noms_rues[jonction_j2][jonction_j1])			;
		        		printf("vers %-70s  ",tab_jonctions[jonction_j2].nom)        				;
		        		printf("[Distance : %3d mètres]\n",tab_longueur[jonction_j2][jonction_j1])  ;
	        		}
        		} 
      		}
    	}
    	while (save_test != 0 && save_test != 1) {
    		printf("Voulez-vous sauvegarder le trajet dans un fichier ? (tapez 0 pour Non, 1 pour Oui) : ")		;
    		scanf("%d", &save_test)																				;
    		
    		switch(save_test){
    			case 0 : 
    				printf("Trajet non sauvegardé.\n")	;
    				break 								;
    			case 1 : 
    				save_trajet(num_jonction_depart, num_jonction_arrivee, choix_mode) 	;
    				break 																;
    			default :
    				printf("Merci de choisir 0 ou 1.\n") 	;
    				purge() 								;
    		}
    	}
  	}
}

/* sauvegarde du trajet dans un fichier */
void save_trajet(int num_jonction_depart, int num_jonction_arrivee, int choix_mode) {

	int etapes[NB_JONCTIONS]    			; /*Tableau pour stocker le chemin qui mène du point de départ au point d'arrivée (dans le sens inverse)*/
	int jonction_temp						; /*Jouera le rôle de mémoire temporaire*/
	int jonction_j1, jonction_j2			; /*seront utilisées pour les calculs de l'affichage du chemin inverse (récupérer les infos depuis les matrices)*/
	int nb_jonctions_afficher=1				; 
	FILE *fichier_trajet					;
	char nom_fichier[TAILLE_NOM_FICHIER] = "sauvegarde_trajet/"	;
	char tmp[TAILLE_NOM_FICHIER] 								;

	printf("Entrez le nom du fichier (avec l'extension .txt à la fin) où vous souhaitez sauvegarder le trajet. Exemple : mon_trajet.txt\nNom du fichier : ");
	purge() ;
	recup_saisie_user(tmp)		;
	strcat(nom_fichier, tmp)	;

	fichier_trajet = fopen(nom_fichier, "w") 		; // création fichier en écriture

	etapes[0]=num_jonction_arrivee					; /*on met la jonction d'arrivée dans la 1ère case du tableau*/
	jonction_temp=num_jonction_arrivee				; /*Initailisation de la variable avec la valeur de num_jonction_arrivee*/
		
	while (jonction_temp != NON_TROUVE) 
	{ 
		etapes[nb_jonctions_afficher++]=jonction_temp			; /*on commence par mettre le num de la jonction d'arrivée dans la case avec l'indice 1 et on boucle tant que la condition est respectée*/
  		jonction_temp = tab_jonctions[jonction_temp].antecedent ; /*on affecte le prédecesseur à jonction_temp pour remonter jusqu'au point de départ */
	}

	if (choix_mode == 1) 
	    {
	    	fprintf(fichier_trajet, "À pied, ");
	    } else {
	    	fprintf(fichier_trajet, "En voiture, ");
	    }
    fprintf(fichier_trajet, "voici le plus court chemin pour aller de : \n %s \n\t à %s \n\n", tab_jonctions[num_jonction_depart].nom, tab_jonctions[num_jonction_arrivee].nom)					;

    for(int i=nb_jonctions_afficher-1; i>0; i--) /*On commence une case */
    { 
    	jonction_j1 = etapes[i]	;	 /*On affecte à jonction_j1 le numéro de la jonction qui se trouve dans la case i*/	
      	if(jonction_j1 == num_jonction_arrivee)	 /*On teste si jonction_j1 est la jonction d'arrivée. Si oui, on arrête l'affichage et on lui affiche le récapitulatif du trajet*/
      	{
       		fprintf(fichier_trajet, "\nVous êtes arrivé(e) à %s.\nDistance totale parcourue (en mètres) : %d\n",tab_jonctions[jonction_j1].nom, tab_jonctions[num_jonction_arrivee].longueur)		;
      	} else { 
      		jonction_j2 = etapes[i-1]			;	/*jonction_j2 se voit affecter le numéro de jonction qui se trouve en i-1*/
        	fprintf(fichier_trajet, "de %-70s  ",tab_jonctions[jonction_j1].nom);
        
       		/*test si mode 1 ou mode 2 : peut avoir un impact sur l'affichage*/
        	if (choix_mode==1)
        	{	/*Mode piéton: n'a pas d'impact sur l'affichage car les matrices sont symétriques*/
        		fprintf(fichier_trajet, "suivre %-30s  ",tab_noms_rues[jonction_j1][jonction_j2])				;
	        	fprintf(fichier_trajet, "vers %-70s  ",tab_jonctions[jonction_j2].nom)        					;
	        	fprintf(fichier_trajet, "[Distance : %3d mètres]\n",tab_longueur[jonction_j1][jonction_j2])   	;
	        } else { 		/*si le mode de transport est voiture, alors on teste si c'est un sens interdit pour récupérer les bonnes informations. Sinon, on risque de se retrouver avec des 9999 ou des INFINI comme valeurs*/
	        	if (tab_longueur[jonction_j1][jonction_j2] != INFINI)
	        	{							/*si le sens de circulation va de J1 à J2*/
		        	fprintf(fichier_trajet, "suivre %-30s  ",tab_noms_rues[jonction_j1][jonction_j2])				;
	        		fprintf(fichier_trajet, "vers %-70s  ",tab_jonctions[jonction_j2].nom)        					;
	        		fprintf(fichier_trajet, "[Distance : %3d mètres]\n",tab_longueur[jonction_j1][jonction_j2])  	;
	        	} else { 	/*si le sens de circulation va de J2 à J1*/
		        	fprintf(fichier_trajet, "suivre %-30s  ",tab_noms_rues[jonction_j2][jonction_j1])				;
	        		fprintf(fichier_trajet, "vers %-70s  ",tab_jonctions[jonction_j2].nom)        					;
	        		fprintf(fichier_trajet, "[Distance : %3d mètres]\n",tab_longueur[jonction_j2][jonction_j1])  	;
        		}
    		} 
  		}
	}
	fclose(fichier_trajet) ;
	printf("Votre fichier à été sauvegardé sous --> %s\n", nom_fichier);
}

/* purge de saisie */

void purge() {
	int purge;
	while((purge = fgetc(stdin)) != '\n' && purge != EOF) {}
}

/*fonction pour corriger les caractères spéciaux*/
void conv_char_speciaux(char saisie[]){
	int taille, i, nb_rempl=0;

	taille = strlen(saisie);
	for (i=0; i<taille; i++){
		if 	(saisie[i]<'A' || saisie[i]>'z'){																	/*on ne considère que les caractères qui ne sont pas des lettres, on inclut les caractères avec le code ASCII 91, 92, 93, 94, 95, 96*/
			if (RemplaceLettre(saisie[i])!='!'){ 																/*si la fonction RemplaceLettre renvoie autre chose que '!'*/
				if (RemplaceLettre(saisie[i])=='_'){ 															/*si elle renvoie '_'*/ 
					saisie[i]=RemplaceLettre(saisie[i]); }														/*on remplace ' ou - par _*/
				else{																							/*sinon*/
					int Index_A_Supp = i; 																		/*on retient l'index du caractère à modifier*/
					memmove(&saisie[Index_A_Supp], &saisie[Index_A_Supp + 1], strlen(saisie) - Index_A_Supp);	 /*on décale les n caractères après le i(exclus) dans la case[i] */
					saisie[i]=RemplaceLettre(saisie[i]);														/*on remplace le caractère accentués par son équivalent*/
					nb_rempl++; 																				/*on compte le nombre de changements faits sur les caractères codés sur 2 octets. A noter que ce nombre avaance par pas de 2*/
					}
				}
			}
		}
	saisie[taille-(nb_rempl/2)]='\0'; 	/*on supprime l'autre octet des caractères codés sur 2 octets. Comme le nbre de remplacement est compté en double, on le divise/2 pour ne pas supprimer plus de caractères qu'il ne faut*/
}	

/*fonction pour remplacer les caractères accentués*/
char RemplaceLettre(char c)
{
    char lettre;
    char* liste_equiv = "' -àâäéêèëîïôöûüç";

    const char *lettre_equiv = strchr(liste_equiv, c); 		/*pointe sur la première occurence de c rencontrée dans liste_equiv*/
    if (lettre_equiv != NULL){ 								/*si le pointeur n'est pas NULL on fait*/
    	int index = lettre_equiv - liste_equiv; 			/*on soustrait le premier pointeur du second pour avoir l'indice du pointeur *lettre_equiv. En fonction de l'index, on renvoie la bonne lettre*/
    	if (index<3) lettre = 95; //_ 						//on met 2 car ' et - sont codés sur 1 octet'

    	else if (index>2 && index<9) lettre =  97	; //a 		//on avance par pas de 2 car les lettres accentuées sont codés sur 2 octets : à est référencé par les index 2 et 3 par exemple. Même logique pour les autres lettres
    	else if (index>8 && index<17) lettre =  101	; //e
		else if (index>16 && index<21) lettre =  105; //i
		else if (index>20 && index<25) lettre =  111; //o
		else if (index>24 && index<29) lettre =  117; //u
		else if (index>28) lettre =  99;

    	return lettre; 										/*on renvoie la lettre désirée: _ a e i o u*/
    }
    else return '!'; 										//s'il n'y a pas d'équivalence pour la lettre cherchée, on renvoie !
}

void verif_saisie_numerique(char saisie[], int *saisie_ok){
	for(int i = 0; i < strlen(saisie); i++){
		if(isdigit(saisie[i]))
		{
		*saisie_ok = 0 ;
		}
	}
}

void recup_saisie_user(char saisie[])
{
 	char tmp=' ' ;
 	int i=0 ;
 	while ((tmp=getchar()) != '\n')
 	{
		saisie[i++]=tmp;
 	}
 	saisie[i++]='\0';
}
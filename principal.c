#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define NB_JONCTIONS 700 // à modifier une fois que nombre de jonctions défini
#define TAILLE_NOM_FICHIER 100 // vérifier taille max d'un nom de fichier 
#define TAILLE_NOM_JONCTION 70 // vérifier taille max d'un nom de jonction --> 62+4 caractères max 
#define NON_TROUVE -1
#define INCONNU -1 // à garder ?
#define INFINI 9999

/* définition du passage par un sommet */
enum verif_passage 
{ 
	non, // correspond à 0
	oui  // correspond à 1
};

/* définition d'une jonction */
typedef struct jonction Jonction ;
struct jonction 
{
	char nom[TAILLE_NOM_JONCTION]	;
	int antecedent					;
	int poids						;
	enum verif_passage passage		;
};

/* appel des procédures et fonctions */
int init_jonction() 			 			;
int init_rues_distances(int choix_mode)		;
int recherche_nom_rue()			 			;
int mode_trajet()							;
void purge()					 			;

/* variables globales */

// tableau listant toutes les jonctions 
Jonction tab_jonctions[NB_JONCTIONS]; 

// tableau représentant la matrice des noms des rues selon les jonctions, sert à récupérer le contenu du fichier de noms des rues
char tab_noms_rues[NB_JONCTIONS][NB_JONCTIONS][TAILLE_NOM_JONCTION]; // 3e dimension est pour la taille des string stockés (possibilité de l'enlever et de mettre un pointeur mais attention à faire les malloc lors d'insertion)

// équivalent à tab_noms_rues mais pour les distances
int tab_longueur[NB_JONCTIONS][NB_JONCTIONS]; // 

// variable de parcours 
int nbjonction = 0;


/* programme principal */

int main(int argc, char const *argv[])
{
	int point_depart, point_arrivee	;
	int choix_mode = 0 				;
	if (init_jonction() != NON_TROUVE) {
		choix_mode = mode_trajet();
		if (init_rues_distances(choix_mode) != NON_TROUVE) {
			point_depart = recherche_nom_rue("de départ"); 	// numéro du point de départ
			point_arrivee = recherche_nom_rue("d'arrivée");	// numéro du point d'arrivée
			tab_jonctions[point_depart].poids = 0; // initialisation du poids du point de départ à 0
			printf("\nD --> poids dans tab_jonctions --> %d\n", tab_jonctions[point_depart].poids);
			printf("A --> poids dans tab_jonctions --> %d\n", tab_jonctions[point_arrivee].poids);
			//printf("\nD --> nom : %s\nantecedent : %d\npoids : %d\npassage : %d\n", tab_jonctions[point_depart].nom, tab_jonctions[point_depart].antecedent, tab_jonctions[point_depart].poids, tab_jonctions[point_depart].passage);		//test
			//printf("\nA --> nom : %s\nantecedent : %d\npoids : %d\npassage : %d\n", tab_jonctions[point_arrivee].nom, tab_jonctions[point_arrivee].antecedent, tab_jonctions[point_arrivee].poids, tab_jonctions[point_arrivee].passage);	//test
			//printf("\njonction dans tab_noms_rues --> %s\n", tab_noms_rues[point_depart][point_depart]);
			printf("\nD --> poids dans tab_longueur     --> %d\n", tab_longueur[point_depart][point_depart]);
			printf("A --> poids dans tab_longueur     --> %d\n", tab_longueur[point_arrivee][point_arrivee]);
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
				strcpy(tab_jonctions[nbjonction].nom, nom_jonction); // insertion de la ligne du fichier dans le nom de chaque structure jonction
				tab_jonctions[nbjonction].antecedent = NON_TROUVE;
				tab_jonctions[nbjonction].poids = INFINI;
				tab_jonctions[nbjonction].passage = non;
				nbjonction++;
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
			printf("Erreur de chargement du fichier des noms de rues.\n");	
		} else {
			printf("Erreur de chargement du fichier des longueurs de rues.\n");
		}
	} else {
		// si ouverture fichier ok
		// double boucle de parcours pour les affecter les données des fichiers 
		for(int i = 0; i < nbjonction; i++){
			for(int j = 0; j < nbjonction; j++){
				fscanf(fichier_noms,"%s", nom_rue); // dans matrice des noms
				strcpy(tab_noms_rues[i][j], nom_rue);
				fscanf(fichier_longueur, "%d", &tab_longueur[i][j]); // dans matrice des longueurs
			}
		}
	}
	fclose(fichier_noms);
	fclose(fichier_longueur);
	return test_init;
}

/* recherche nom rue suite à saisie utilisateur */

int recherche_nom_rue(char contexte[20]) 
{
	int choix_ok = 0, nb_result = 0								;
	int choix = NON_TROUVE, test_saisie_char = 0 				;
	char nom_rue[TAILLE_NOM_JONCTION]							;
	char *test, tab_result[NB_JONCTIONS][TAILLE_NOM_JONCTION]	;

	while(nb_result == 0){
		printf("\nEntrez le nom de la voie, sans son type (rue, avenue, boulevard, ...)\nExemple : pour la rue de la Roquette, tapez roquette \nNom du point %s : ", contexte);
		scanf("%s", nom_rue);

		for(int i = 0 ; i < nbjonction ; i++) // boucle de recherche du nom saisie dans liste des rues
		{
			test = strcasestr(tab_jonctions[i].nom, nom_rue) ;
			if(test != NULL)
			{
				nb_result++;
				printf("%-s\n", tab_jonctions[i].nom);
				strcpy(tab_result[i], tab_jonctions[i].nom);
			}
		}
		if(nb_result == 0) {
			printf("Le nom que vous avez saisi ne correspond à aucune données en mémoire.\nMerci de ressaisir le nom ou de choisir une autre rue.\n\n");
		}
	}
	
	printf("\nEntrez le numéro de votre point %s : ", contexte) ;
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
					printf("%s\n", tab_result[i]); // test
				}
			}
		} else {
			purge();
		}

		if (!choix_ok) 
		{
			printf("Choix erroné. Merci de renseigner un numéro valide : ");
		}
	}
	return choix-1;

}

/* choix du mode de trajet */

int mode_trajet(){
	int choix_mode = 0 ;
	while(choix_mode != 1 && choix_mode != 2){	
		printf("Choisissez votre mode de trajet :\n\n");
		printf("1 - piéton\n2 - voiture\n\n");
		printf("Choix : ");
		scanf("%d", &choix_mode);
		if(choix_mode != 1 && choix_mode != 2){
			purge();
			printf("Merci de sélectionner le choix 1 ou 2.\n\n");
			}
		}
	return choix_mode;
}

/* purge de saisie */

void purge() {
	int purge;
	while((purge = fgetc(stdin)) != '\n' && purge != EOF) {}
}
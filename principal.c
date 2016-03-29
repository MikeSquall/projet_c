#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define NB_JONCTIONS 700 // à modifier une fois que nombre de jonctions défini
#define TAILLE_NOM_FICHIER 100 // vérifier taille max d'un nom de fichier 
#define TAILLE_NOM_JONCTION 70 // vérifier taille max d'un nom de jonction
#define NON_TROUVE -1
#define INCONNU -1 // à garder ?
#define INFINI 99999

/* définition du passage par un sommet */
enum verif_passage 
{
	oui, 
	non 
};

/* définition d'une jonction */
typedef struct jonction Jonction ;
struct jonction 
{
	char nom[TAILLE_NOM_JONCTION]	;
	int predecesseur				;
	int poids 						;
	enum verif_passage passage 		;
};

/* appel des procédures et fonctions */
int init_jonction() 										 ;
int init_rues_distances()									 ;
int recherche_nom_rue()									 ;

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
	int point_depart, point_arrivee;
	if (init_jonction() != NON_TROUVE) {
		if (init_rues_distances() != NON_TROUVE) {
			point_depart = recherche_nom_rue("de départ");
			point_arrivee = recherche_nom_rue("d'arrivée");
			printf("départ %d\n", point_depart);
			printf("arrivée %d\n", point_arrivee);
		}
	}
	

}

/* code des procédures et fonctions */

/* initialisation des jonctions dans tab_jonctions */

int init_jonction()
{
	int test_init = 0 ;
	FILE *fichier_jontions = fopen("jeu_test/test_jonctions.txt","r");
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
				nbjonction++;
			}
		}
	}
	fclose(fichier_jontions);
	return test_init ;
}

/* initialisation des noms des rues et de leur longueur */

int init_rues_distances()
{
	int test_init = 0;
	char nom_rue[TAILLE_NOM_JONCTION];
	FILE *fichier_noms = fopen("jeu_test/test_noms_rues.txt","r");
	FILE *fichier_longueur = fopen("jeu_test/test_longueurs.txt","r");

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
	int choix_ok = 0, purge, nb_result = 0			;
	int choix = NON_TROUVE, test_saisie_char = 0 	;
	char nom_rue[TAILLE_NOM_JONCTION]				;
	char *test, tab_result[NB_JONCTIONS][TAILLE_NOM_JONCTION]	;

	printf("Rue %s : ", contexte);
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
	
	printf("\nEntrez le numéro de votre point %s : ", contexte) ;
	while(!choix_ok) // boucle de vérification de la saisie
	{
		test_saisie_char = scanf("%d",&choix) ;
		if(test_saisie_char)
		{
			for(int i = 0 ; i < NB_JONCTIONS ; i++)
			{
				if (choix == atoi(tab_result[i])) 
				{
					choix_ok = 1 ;
					printf("%s\n", tab_result[i]);
				}
			}
		}
		else 
		{
			while((purge = fgetc(stdin)) != '\n' && purge != EOF) {}
		}

		if (!choix_ok) 
		{
			printf("Choix erroné. Merci de renseigner un numéro valide : ");
		}
	}
	return choix;

}
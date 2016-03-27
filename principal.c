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
void recherche_nom_rue()									 ;

/* variables globales */
Jonction tab_jonctions[NB_JONCTIONS]; // tableau listant toutes le jonctions
int nbjonction = 0;
/* programme principal */

int main(int argc, char const *argv[])
{
	printf("%d\n", init_jonction());
	for(int i = 0; i < 11; i++){
		printf("%s\n", tab_jonctions[i].nom);
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

/* recherche nom rue suite à saisie utilisateur */

void recherche_nom_rue(char contexte[20]) 
{
	int j = 0 								;
	char nom_rue[50]						;
	char tab_affichage[100][NB_JONCTIONS]	;
	char *test ;

	printf("Rue %s : ", contexte);
	scanf("%s", nom_rue);

	for(int i = 0 ; i < nbjonction ; i++)
	{
		test = strcasestr(tab_jonctions[i].nom, nom_rue) ;
		if(test != NULL)
		{
			printf("%2d - %-s\n", j+1, tab_jonctions[i].nom);
			j++ ;
		}
	}
}
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define NBSOMMETS 1000
#define TAILLE_NOM_FICHIER 100
#define TAILLE_NOM 50
#define NON_TROUVE -1
#define INCONNU -1
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
	char nom[TAILLE_NOM]		;
	int predecesseur			;
	int poids 					;
	enum verif_passage passage 	;
};

/* appel des procédures et fonctions */
void recherche_nom_rue()									 ;

/* programme principal */

int int main(int argc, char const *argv[])
{
	
}

/* code des procédures et fonctions */

/* recherche nom rue suite à saisie utilisateur */

void recherche_nom_rue(char contexte[20]) 
{
	int j = 0 							;
	char nom_rue[50]					;
	char tab_affichage[100][NBSOMMETS]	;
	char *test ;

	printf("Rue %s : ", contexte);
	scanf("%s", nom_rue);

	for(int i = 0 ; i < nbsommets ; i++)
	{
		test = strcasestr(tab_sommets[i].nom, nom_rue) ;
		if(test != NULL)
		{
			printf("%2d - %-s\n", j+1, tab_sommets[i].nom);
			j++ ;
		}
	}
}
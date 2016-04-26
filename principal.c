#define _GNU_SOURCE /*ajout car strcasestr n'est pas une fonction standard du C*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define NB_JONCTIONS 700 // à modifier en fonction du nombre de jonctions défini
#define TAILLE_NOM_FICHIER 100 // taille max d'un nom de fichier 
#define TAILLE_NOM_JONCTION 70 // taille max d'un nom de jonction --> 62+4 caractères max 
#define NON_TROUVE -1
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
	int longueur					;
	enum verif_passage passage		;
};

/* appel des procédures et fonctions */
int init_jonction() 			 			;
int init_rues_distances(int choix_mode)		;
int recherche_nom_rue()			 			;
int mode_transport()						;
void purge()					 			;
int plus_courte_jonction()					;
void check_passage(int jonction_validee)	;
void maj_longueur_jonctions(int antecedent)	;
char RemplaceLettre(char c)					;
void conv_char_speciaux(char saisie[])		;
void dijkstra(int point_arrivee)			;


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
	int point_depart = NON_TROUVE 					; 
	int point_arrivee = NON_TROUVE 					;
	int choix_mode = 0								;
	int choix_menu = NON_TROUVE 					;
	int itineraire_de_base_calcule = NON_TROUVE		;

	if (init_jonction() != NON_TROUVE) {
		printf("\nBienvenue dans le programme de calcul du chemin le plus court.\nCe programme vous donnera le trajet le plus court entre 2 intersections de rues du 11e arrondissement de Paris.\n");
		
		while (choix_menu != 0) {
			printf("\n************** Menu ************** \n")								;
			printf("1 - Calculer un itinéraire\n")											;
			if (itineraire_de_base_calcule != NON_TROUVE){
				printf("2 - Calculer le trajet de retour\n")								;
				printf("3 - Calculer le même itinéraire avec l'autre mode de transport\n")	;
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
					
					itineraire_de_base_calcule = 1 ;
					
					choix_mode = mode_transport()	;
					if (init_rues_distances(choix_mode) != NON_TROUVE) {
						point_depart = recherche_nom_rue("de départ")	; // numéro du point de départ
						point_arrivee = recherche_nom_rue("d'arrivée")	; // numéro du point d'arrivée
						tab_jonctions[point_depart].longueur = 0		; // initialisation de la longueur de la rue du point de départ à 0
						dijkstra(point_arrivee)							; // fonction qui utilise l'algo de Dijkstra
					}

					break ;
				case 2 : // trajet retour suite au choix 1
					if (itineraire_de_base_calcule == NON_TROUVE)
					{
						printf("Merci de saisir un choix valide\n")	;
					} else {
						if (init_jonction() != NON_TROUVE) // ré-initialisation du tableau des jonctions pour repartir du point d'arrivée du calcul d'itinéraire précédent
						{ // on inverse les points de départ et arrivée pour recalculer l'itinéraire de retour
							tab_jonctions[point_arrivee].longueur = 0	; // initialisation de la longueur de la rue du point d'arrivée à 0
							dijkstra(point_depart) 						; // fonction qui utilise l'algo de Dijkstra
						}
					}
					break ;
				case 3 : // mode de transport alternatif avec itinéraire du choix 1
					if (itineraire_de_base_calcule == NON_TROUVE)
					{
						printf("Merci de saisir un choix valide\n")	;
					} else {
						if (choix_mode == 1)
						{
							choix_mode = 2 ;
						} else {
							choix_mode = 1 ; 
						}
						printf("mode --> %d\n", choix_mode);
						if (init_rues_distances(choix_mode) != NON_TROUVE) 
						{
							if (init_jonction() != NON_TROUVE) // ré-initialisation du tableau des jonctions pour recalculer l'itinéraire précédent
							{
								tab_jonctions[point_depart].longueur = 0		; // initialisation de la longueur de la rue du point de départ à 0
								dijkstra(point_arrivee)							; // fonction qui utilise l'algo de Dijkstra
							}
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
				strcpy(tab_jonctions[nbjonction].nom, nom_jonction); // insertion de la ligne du fichier dans le nom de chaque structure jonction
				tab_jonctions[nbjonction].antecedent = NON_TROUVE;
				tab_jonctions[nbjonction].longueur = INFINI;
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
		conv_char_speciaux(nom_rue);
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

/* choix du mode de transport */

int mode_transport(){
	int choix_mode = 0 ;
	while(choix_mode != 1 && choix_mode != 2){	
		printf("\nChoisissez votre mode de transport :\n\n");
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

/* recherche de la jonction avec la longueur la plus faible */
int plus_courte_jonction() {
	int courte = NON_TROUVE, longueur = INFINI ;
	for (int i = 0; i < nbjonction; i++)
	{
		if (tab_jonctions[i].passage == non && tab_jonctions[i].longueur < longueur)
		{
			longueur = tab_jonctions[i].longueur 	;
			courte = i 								;
		}
	}
	return courte ;
}

/* marqueur de passage par une jonction */ 
void check_passage(int jonction_validee) {
	tab_jonctions[jonction_validee].passage = oui ;
}

/* mise à jour de la longueur des jonctions adjacentes à celle de longueur minimum */
void maj_longueur_jonctions(int antecedent) {
	for (int i = 0; i < nbjonction; i++){ // on parcourt l'intégralité du tableau des jonctions pour la m-à-j
		if ((tab_jonctions[i].longueur > (tab_jonctions[antecedent].longueur + tab_longueur[antecedent][i])) && tab_jonctions[i].passage == non) 
		{ // elle est faite seulement si la longueur de la jonction est supérieure à celle de son antécédant + la longueur entre les deux (dans le tableau des longueurs) et si on n'est pas déjà passé par cette jonction (cf : pathfinding avec dijkstra - openclassroom)
			tab_jonctions[i].longueur = tab_jonctions[antecedent].longueur + tab_longueur[antecedent][i] 	; // m-à-j longueur 
			tab_jonctions[i].antecedent = antecedent 											; // m-à-j antécédent 
		}
	}
}

/*fonction pour corriger les caractères spéciaux*/
void conv_char_speciaux(char saisie[]){
	int taille, i, nb_rempl=0;
	char temp;

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
    int i;
    char lettre;
    char* liste_equiv = "' -àâäéêèëîïôöûüç";

    const char *lettre_equiv = strchr(liste_equiv, c); 		/*pointe sur la première occurence de c rencontrée dans liste_equiv*/
    if (lettre_equiv != NULL){ 								/*si le pointeur n'est pas NULL on fait*/
    	int index = lettre_equiv - liste_equiv; 			/*on soustrait le premier pointeur du second pour avoir l'indice du pointeur *lettre_equiv. En fonction de l'index, on renvoie la bonne lettre*/
    	if (index<3) lettre = 95; //_ 						//on met 2 car ' et - sont codés sur 1 octet'
    	else if (index>3 && index<9) lettre =  97; //a 		//on avance par pas de 2 car les lettres accentuées sont codés sur 2 octets : à est référencé par les index 2 et 3 par exemple. Même logique pour les autres lettres
    	else if (index>9 && index<17) lettre =  101; //e
		else if (index>17 && index<21) lettre =  105; //i
		else if (index>21 && index<25) lettre =  111; //o
		else if (index>25 && index<29) lettre =  117; //u
		else if (index==30) lettre =  99;
    	
    	return lettre; 										/*on renvoie la lettre désirée: _ a e i o u*/
    }
    else return '!'; 										//s'il n'y a pas d'équivalence pour la lettre cherchée, on renvoie !
}

/* dijkstra */

void dijkstra(int point_arrivee) {
	int jonction_tmp = NON_TROUVE 	;
	
	while ((jonction_tmp = plus_courte_jonction()) != point_arrivee) { // tant que la plus courte jonction traitée n'est pas la jonction d'arrivée
		check_passage(jonction_tmp) 		; // on marque la jonction pour dire qu'on y est passé et qu'elle ne nous intéresse plus
		maj_longueur_jonctions(jonction_tmp); // on calcule la longueur qui la sépare des prochaines jonctions et on reboucle avec la plus courte longueur trouvée
		// test affichage des jonctions traitées + longueur
		//printf("\n nom --> %s | longueur --> %d\n", tab_jonctions[jonction_tmp].nom, tab_jonctions[jonction_tmp].longueur);
		}
		printf("\nnom --> %s | longueur --> %d\n", tab_jonctions[point_arrivee].nom, tab_jonctions[point_arrivee].longueur);
		// fonction d'affichage à insérer ci-dessous
		
}


/* *********************************************************************

projet réalisé dans le cadre du master PISE / Université Paris Diderot 
par Lyes KESSAL & Michael ARBIB
année universitaire 2015/2016

objectif :

réaliser un programme qui indique le plus court chemin d'un point A à un point B, en langage C

********************************************************************* */
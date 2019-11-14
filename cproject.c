#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <stdbool.h>

#define NOM_FICH1 "mots_courants.txt"
#define NOM_FICH2 "graphe.txt"
#define DICO "dictionnaire.txt"

//Structures utilisées

struct successeur;
typedef struct _graphe
{
	char lettre;
	int freq;
	int fin;
	struct _successeur *fils_tete;
} graphe;

typedef struct _successeur
{
	graphe *fils;
	struct _successeur *suiv;
} successeur;

typedef struct _occurrence_mot
{
	char m[100];
	int occ;
	struct _occurrence_mot *suivant;
} occurrence_mot;

//Variables globales

	char mot1[50];
	char mot2[50];
	char mot3[50];
		
	GtkWidget* Bouton1 = NULL;
	
	GtkWidget* Bouton2 = NULL;
	
	GtkWidget* Bouton3 = NULL;
	
	GtkWidget* suppr = NULL;
	
	GtkWidget* toggle = NULL;
	
	gint positionBeforeWord = 0;
	
	occurrence_mot *tete = NULL;

//Prototypes

void creation_graphe(graphe *G);
graphe *test_lettre_arbre(graphe* G, char a_comparer);
void ajouter_mot(graphe *G, char *mot, float f);
void initialisation(void);
void init_premiere_fois(void);
void init_random(void);
void sortie(void);
void visualisation_liste_chainee(void);
bool ajout_volee(char *a_ajouter, graphe *G);
void maj_occurrence(char *mot);
void suppression_volee(char *a_supprimer);
void parcours_profondeur(graphe *g);
graphe *graphe_to_look(graphe *G, char *debut);
static gboolean OnType(GtkWidget *widget, GdkEventKey *event, graphe* G);
gint OnPress (GtkButton *button, GtkEntry* entry);
gint OnDel (GtkButton *button, graphe *G);
int BestMatch(char* wordToLook, graphe *G, char mot1[], char mot2[], char mot3[]);
void OnQuit (void);

void initialisation(void) //cette fonction va initialiser le graphe au lancement du programme. Son principe est le suivant : si n'existe que le fichier avec les mots donné par le professeur, il va utiliser ce fichier pour créer la liste chainée. Sinon, la liste chainée sera crée à partir du fichier texte contenant les mots et leur nombre d'occurence
{
	FILE *fpp = fopen(NOM_FICH2,"r");

	if(fpp!=NULL)
	{
		fclose(fpp);
		init_random();
	}

	else
	{
		init_premiere_fois();
	}
}

void visualisation_liste_chainee(void)
{
	occurrence_mot *visual = tete;
	
	while(visual!=NULL)
	{
		printf("mot : %s freq : %d\n", visual->m, visual->occ);
		visual = visual->suivant;
	}
}

void init_premiere_fois(void) //Initialisation dans le cas où on ne dispose que de mots_courants.txt, on crée une liste chainée qui compte le nombre d'occurrence des mots présents dans le fichier
{
	char mot_courant[100];
	occurrence_mot *courant;
	int trouve=0;

	FILE *fp = fopen(NOM_FICH1, "r");
	while(fscanf(fp, " %s", mot_courant)==1) //On parcourt le fichier des mots, et on fait une liste chainée avec leur nombre d'occurrence
	{
		courant = tete;
		trouve=0;
		while((trouve==0) && (courant!=NULL))
		{
			if(strcmp(courant->m,mot_courant)==0)
			{
				trouve=1;
				courant->occ = courant->occ +1;
			}
	
			courant=courant->suivant;
		}
		
		if(trouve==0)	
		{
			occurrence_mot *new = malloc(sizeof(occurrence_mot));
			strcpy(new->m,mot_courant);
			new->occ = 1;
			new->suivant=tete;	
			tete = new;
		}
	}

	fclose(fp);
}

void init_random(void) //cas où le fichier de la bonne configuration existe, on l'ouvre alors pour créer la liste chainée
{
	FILE *fm = fopen(NOM_FICH2, "r");

	char mot_courant[100];
	int occu;
	
	while(fscanf(fm, "%s%d", mot_courant, &occu)==2)
	{
		occurrence_mot *new = malloc(sizeof(occurrence_mot));
		strcpy(new->m,mot_courant);
		new->occ = occu;
		new->suivant=tete;	
		tete = new;
	}

	fclose(fm);
}

void sortie(void) //Fonction de sortie, inscrit le contenu de la liste chainée dans graphe.txt
{	
	occurrence_mot *courant = tete;
	FILE *fg = fopen(NOM_FICH2, "w");
	
	while(courant!=NULL) //On parcourt la liste chainée en réécrivant dans le nouveau fichier texte
	{
		fprintf(fg, "%s %d\n", courant->m, courant->occ);
		courant=courant->suivant;
	}

	fclose(fg);
}

void creation_graphe(graphe *G) //Fonction qui créée le graphe à partir de la liste chainée
{
	G->fils_tete = NULL;

	occurrence_mot *courant = tete;

	while(courant!=NULL)
	{
		ajouter_mot(G, courant->m, courant->occ);
		courant = courant->suivant;
	}
}

graphe *test_lettre_arbre(graphe* G, char a_comparer) //Renvoie le pointeur sur le maillon de type graphe si existe (avec la bonne lettre), sinon NULL
{
	successeur *courant = G->fils_tete;

	while(courant!=NULL)
	{
		if(courant->fils->lettre==a_comparer)
		{
			return(courant->fils);
		}
		
		courant = courant->suiv;
	}

	return NULL;
}

void test(graphe *G) //Fonction test qui permet de visualiser tous les fils (direct) d'un graphe
{
	successeur *courant = G->fils_tete;

	while(courant!=NULL)
	{
		printf("Lettre : %c\n", courant->fils->lettre);
	
		courant=courant->suiv;
	}
}

void ajouter_mot(graphe *G, char *mot, float f) //Ajoute un mot dans l'arbre, ainsi que sa fréquence
{
	if(mot[0]!='\0')
	{
		char courant = mot[0];
		graphe *existe = test_lettre_arbre(G, courant); 

		if(existe != NULL) //ie il existe une banche avec la lettre
		{
			int len = strlen(mot);
			char *nouveau_mot = mot+1;
			if(f>existe->freq)
			{
				existe->freq = f;
			}

			if(strlen(mot)==1)
			{
				existe->fin = 1;
			}

			ajouter_mot(existe, nouveau_mot, f);
		}

		else //Si la branche avec la lettre cherchée n'existe pas 
		{
			graphe *new_graph = malloc(sizeof(graphe));
			new_graph->lettre = mot[0]; //On initialise le nouveau sous graphe
			new_graph->fils_tete = NULL;
			new_graph->freq = f;
			
			successeur *new_successeur = malloc(sizeof(successeur)); //On crée un nouveau maillon dans la liste chainée des successeurs du noeud courant, et on l'ajoute
			new_successeur->fils = new_graph;
			new_successeur->suiv = G->fils_tete;
			G->fils_tete = new_successeur;

			if(strlen(mot)==1)
			{
				new_graph->fin = 1;
			}

			int len = strlen(mot);
			char *nouveau_mot = mot+1;
			ajouter_mot(new_graph, nouveau_mot, f);
		}
	}
}

bool ajout_volee(char *a_ajouter, graphe *G) //Ajoute un mot à la liste chainée et à l'arbre, utilisé quand on veut ajouter un mot à la volée (soit avec une fréquence de 1)
{
	int interdit=0;
	occurrence_mot *courant = tete;

	while(courant!=NULL)
	{
		if(strcmp(a_ajouter, courant->m)==0)
		{
			printf("Le mot que vous voulez ajouter existe déjà\n");
			interdit=1;
		}

		courant = courant->suivant;
	}

	if(interdit==0)
	{
		ajouter_mot(G, a_ajouter, 1);

		occurrence_mot *new = malloc(sizeof(occurrence_mot));
		strcpy(new->m, a_ajouter);
		new->occ = 1;

		new->suivant = tete;
		tete = new;
	}
	
	if(interdit ==1)
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

void suppression_volee(char *a_supprimer) //Supprime le mot cherchée de la liste chainée si il existe
{
	occurrence_mot *courant = tete;
	occurrence_mot *courant_devant = tete->suivant;
	int trouve=0;

	if(strcmp(a_supprimer, courant->m)==0)
	{
		tete = courant_devant;
		free(courant);
		trouve +=1;
	}

	while(courant_devant!=NULL && trouve==0)
	{
		if(strcmp(a_supprimer, courant_devant->m)==0)
		{
			courant->suivant=courant_devant->suivant;
			free(courant_devant);
			trouve+=1;
		}

		courant_devant = courant_devant->suivant;	
		courant = courant->suivant;
	}

	if(trouve==0)
	{
		printf("[suppression] Le mot %s n'a pas été trouvé\n",a_supprimer);
	}
	else
	{
		printf("[suppression] le mot %s a été supprimé\n",a_supprimer);
	}
}

void maj_occurrence(char *mot) //Fonction de mise à jour de l'occurence d'un mot, utilisée après la selection d'un mot proposée par la saisie prédictive
{
	occurrence_mot *courant = tete;
	int trouve = 0;

	while(courant!=NULL && trouve==0)
	{
		if(strcmp(mot, courant->m)==0)
		{
			courant->occ = courant->occ +1;
			trouve = 1;
		}
		
		courant = courant->suivant;
	}
}

void parcours_profondeur(graphe *g) //Permet d'afficher tous les mots qui sont dans l'arbre
{
	successeur *courant = g->fils_tete;

	if(courant==NULL)
	{
		printf("%c", g->lettre);
	}

	while(courant!=NULL)
	{
		printf("%c", g->lettre);
		parcours_profondeur(courant->fils);
		courant=courant->suiv;	
	}
	printf("\n");
}

graphe *graphe_to_look(graphe *G, char *debut) //Renvoie un pointeur sur la tête du graphe qui correspond à la chaine de caractère qui a été tapée avant
{
	int trouve=0;
	char courant = debut[0];
	int taille = strlen(debut);
	char *debut_copy = (char*) malloc(taille*sizeof(char));
	strcpy(debut_copy,debut);
	if(courant!='\0')
	{
		successeur *point_courant = G->fils_tete;
		while(point_courant!=NULL && trouve==0)
		{
			if(courant==point_courant->fils->lettre)
			{
				trouve+=1;
				char *nouveau_mot = debut+1;
				printf("%c trouvée. On cherche : %s\n",debut[0], nouveau_mot);
				return graphe_to_look(point_courant->fils, nouveau_mot);
			}
			
			point_courant=point_courant->suiv;	
		}
		
		if(trouve == 0)
		{
			printf(" rien trouvé\n");
			return NULL;
		}
		
	}

	else if(trouve == strlen(debut_copy))
	{
		return G; //Pour la peine, retournons ce super sous-graphe
	}
}


int BestMatch(char* wordToLook, graphe *G, char mot1[], char mot2[], char mot3[]) //Place dans mot1, mot2 et mot3 les mots de plus grande occurrence commençant par wordToLook présents dans graphe.txt, ou va les chercher dans le dictionnaire
{
	int maxFreq = 0;
	int maxFreq2 =0;
	int maxFreq3 = 0;
	int k =0;
	int i, result;
	int len = strlen(wordToLook);
	int len1 = strlen(mot1);
	int len2 = strlen(mot2);
	int len3 = strlen(mot3);
	char charstring[2];
	char scannedWord[70];
	int trouve=0;
	int test=0;
	char courant[50];
	bool flag1, flag2, flag3;
	graphe *startNode = graphe_to_look(G, wordToLook);
	graphe *tempNode;
	successeur *cursor;
	
	/* Gestion du mot1 */
	
	for(i = 0; i<len1; i++)
	{
		mot1[i] = '\0';
	}
	tempNode = NULL;
	if(startNode!=NULL && startNode->fils_tete!=NULL)
	{
		cursor = startNode->fils_tete;
		while(cursor != NULL)
		{
			if(cursor->fils->freq > maxFreq)
			{
				maxFreq = cursor->fils->freq;
				tempNode = cursor->fils;
			}
			cursor = cursor->suiv;
		}
	
		for( i = 0 ; i<len ; i++)
		{
			mot1[i] = wordToLook[i];
			k+=1;
		}
		mot1[k] = tempNode->lettre;
		k+=1;
		cursor = tempNode->fils_tete;
	
		while(cursor != NULL)
		{
			if(cursor->fils->freq ==maxFreq)
			{
				tempNode = cursor->fils;
				mot1[k]  = tempNode->lettre;
				k+=1;
				printf("lettre trouvée : %c\n",tempNode->lettre);
				cursor = cursor->fils->fils_tete;
			}
		
			else
			{
			cursor = cursor->suiv;
			}	
		}
	
		printf("[BestMatch] le mot avec maxFreq1=%d est %s\n",maxFreq,mot1);
		suppression_volee(mot1);
		creation_graphe(G);
		
		result+=1;
	}
	else 
	{
		printf("Ouverture du dictionnaire\n");
		//TODO : optimiser la recherche dans le dictionnaire en créant un graphe
		
		//Ici, on utilise une fonction de type ChercheDansDictionnaire(wordToLook) qui copie dans mot1 le premier mot commençant par wordToLook

		flag1 = TRUE;
		
		FILE *fj = fopen(DICO, "r");

		while(fscanf(fj, "%s", courant)==1 && trouve==0)
		{
			test = 0;

			if(strlen(courant)>=len)
			{
				for(int j=0; j<len; j++)
				{
					if(courant[j]!=wordToLook[j])
					{
						test = 1;
					}
				}
		
				if(test==0)
				{
					trouve=1;
					strcpy(mot1, courant);
				}
			}
		}

		trouve=0;

		fclose(fj);
	}
	
	/* Gestion du mot2 */
	
	//reset des variables de parcours
	k=0;
	maxFreq2=0;
	startNode = graphe_to_look(G,wordToLook);
	
	for(i = 0; i<len2; i++)
	{
		mot2[i] = '\0';
	}
	tempNode = NULL;
	if(startNode!=NULL && startNode->fils_tete!=NULL)
	{
		cursor = startNode->fils_tete;
		while(cursor != NULL)
		{
			if(cursor->fils->freq > maxFreq2)
			{
				maxFreq2 = cursor->fils->freq;
				tempNode = cursor->fils;
			}
			cursor = cursor->suiv;
		}
	
		for( i = 0 ; i<len ; i++)
		{
			mot2[i] = wordToLook[i];
			k+=1;
		}
		mot2[k] = tempNode->lettre;
		k+=1;
		cursor = tempNode->fils_tete;
	
		while(cursor != NULL)
		{
			if(cursor->fils->freq ==maxFreq2)
			{
				tempNode = cursor->fils;
				mot2[k]  = tempNode->lettre;
				k+=1;
				printf("lettre trouvée : %c\n",tempNode->lettre);
				cursor = cursor->fils->fils_tete;
			}
		
			else
			{
			cursor = cursor->suiv;
			}	
		}
	
		printf("[BestMatch] le mot avec maxFreq2=%d est %s\n",maxFreq2,mot2);
		suppression_volee(mot2);
		creation_graphe(G);
		sortie();
		
		
		result+=1;
	}
	else 
	{
		printf("Ouverture du dictionnaire\n");
		
		//TODO : optimiser la recherche dans le dictionnaire en créant un graphe
		
		//Ici, on utilise une fonction de type ChercheDansDictionnaire(wordToLook) qui copie dans mot2 le premier mot commençant par wordToLook

		flag2 = TRUE;
		
		FILE *fj = fopen(DICO, "r");

		while(fscanf(fj, "%s", courant)==1 && trouve==0)
		{
			test = 0;

			if(strlen(courant)>=len)
			{
				for(int j=0; j<len; j++)
				{
					if(courant[j]!=wordToLook[j])
					{
						test = 1;
					}
				}
		
				if(test==0 && strcmp(courant,mot1)!=0)
				{
					trouve=1;
					strcpy(mot2, courant);
				}
			}
		}

		trouve=0;

		fclose(fj);
	}
	
	/* Gestion du mot3 */
	
	//reset des variables de parcours
	k=0;
	maxFreq3=0;
	startNode = graphe_to_look(G,wordToLook);
	
	for(i = 0; i<len3; i++)
	{
		mot3[i] = '\0';
	}
	tempNode = NULL;
	if(startNode!=NULL && startNode->fils_tete!=NULL)
	{
		cursor = startNode->fils_tete;
		while(cursor != NULL)
		{
			if(cursor->fils->freq > maxFreq3)
			{
				maxFreq3 = cursor->fils->freq;
				tempNode = cursor->fils;
			}
			cursor = cursor->suiv;
		}
	
		for( i = 0 ; i<len ; i++)
		{
			mot3[i] = wordToLook[i];
			k+=1;
		}
		mot3[k] = tempNode->lettre;
		k+=1;
		cursor = tempNode->fils_tete;
	
		while(cursor != NULL)
		{
			if(cursor->fils->freq ==maxFreq3)
			{
				tempNode = cursor->fils;
				mot3[k]  = tempNode->lettre;
				k+=1;
				printf("[BestMatch]lettre trouvée : %c\n",tempNode->lettre);
				cursor = cursor->fils->fils_tete;
			}
		
			else
			{
			cursor = cursor->suiv;
			}	
		}
	
		printf("[BestMatch] le mot avec maxFreq3=%d est %s\n",maxFreq3,mot3);
		suppression_volee(mot3);
		creation_graphe(G);
		sortie();
		k=0;
		result+=1;
	}
	else 
	{
		printf("Ouverture du dictionnaire\n");
		//TODO : optimiser la recherche dans le dictionnaire en créant un graphe
		
		//Ici, on utilise une fonction de type ChercheDansDictionnaire(wordToLook) qui copie dans mot3 le premier mot commençant par wordToLook
		
		flag3 = TRUE;
		
		FILE *fl = fopen(DICO, "r");
	
		while(fscanf(fl, "%s", courant)==1 && trouve==0)
		{
			test = 0;

			if(strlen(courant)>=len)
			{
				for(int j=0; j<len; j++)
				{
					if(courant[j]!=wordToLook[j])
					{
						test = 1;
					}
				}
		
				if(test==0 && strcmp(courant,mot1)!=0 && strcmp(courant,mot2)!=0)
				{
					trouve=1;
					strcpy(mot3, courant);
				}
			}
		}

		fclose(fl);
	}
	if(mot1[0]!='\0' && flag1 == FALSE)
	{
		ajout_volee(mot1,G);
		if(maxFreq >1)
		{
			for(i=1;i<maxFreq;i++)
			{
				maj_occurrence(mot1);
			}
			creation_graphe(G);
		}
	}
	if(mot2[0]!='\0' && flag2 == FALSE)
	{
		ajout_volee(mot2,G);
		if(maxFreq2>1)
		{
			for(i=1;i<maxFreq2;i++)
			{
				maj_occurrence(mot2);
			}
			creation_graphe(G);
		}
	}
	if(mot3[0]!='\0' && flag3 == FALSE)
	{
		ajout_volee(mot3,G);
		if(maxFreq3>1)
		{
			for(i=1;i<maxFreq;i++)
			{
				maj_occurrence(mot3);
			}
			creation_graphe(G);
		}
	}
	creation_graphe(G);
	sortie();
	
	return result;
	
	
}

void initDico(void)
{
	printf("init dico lancée\n");
	char scanned[70];
	FILE *fp = fopen(DICO,"r+q");
	while( fscanf(fp, "%s",scanned)!= EOF)
	{
		fprintf(fp, " 1\n");
		printf("mot scanné : %s\n",scanned);
	}
	fclose(fp);
}

int main(int argc, char *argv[])
{
	GtkWidget* window = NULL;
	
	GtkWidget* vbox = NULL;
	
	GtkWidget* hbox = NULL;
	
	GtkWidget* hbox2 = NULL;
	
	GtkWidget* entry = NULL;
	
	
	initialisation();
	//visualisation_liste_chainee();
	
	
	
	graphe *G = malloc(sizeof(graphe));
	
	G->lettre = '\0';

	creation_graphe(G);
	parcours_profondeur(G);
	
	visualisation_liste_chainee();
	ajout_volee("putenegre", G);
	BestMatch("a", G, mot1, mot2, mot3);
	visualisation_liste_chainee();

	printf("\n");
	
	/*char ajout_m[30] = "martin"; //TESTS MANUELS
	ajout_volee(ajout_m, G);

	test(G);
	printf("\n");

	char suppr_m[30] = "rugby";
	suppression_volee(suppr_m);
	


	creation_graphe(G);
	test(G);*/
	
	printf("\n\n\n");
	BestMatch("a", G, mot1, mot2, mot3);
	sortie();
	
	/* Front End*/
	
	gtk_init (&argc, &argv);
	
	FILE *fp = fopen("motcourant.txt", "w");
	fclose(fp);
	/*Création de la fenêtre*/
	
	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	
    gtk_widget_set_size_request (GTK_WIDGET (window), 800, 400);
    
    gtk_window_set_title (GTK_WINDOW (window), "Predictive Text Algorithm");
    
    gtk_widget_show_all(window);
    
    
    
    /*Création de l'entry */
    
    entry = gtk_entry_new ();
    gtk_entry_set_max_length (GTK_ENTRY (entry), 240);
    gtk_widget_show(entry);
    
    /* Création de la vbox */
     
    vbox = gtk_vbox_new(TRUE,30); //un espacement homogène de 30 pixels entre chaque widget
    gtk_container_add( GTK_CONTAINER (window), vbox); 
     
    hbox = gtk_hbox_new (FALSE, 0); //On ajoute la hbox qui va contenir les 3 boutons
    gtk_container_add (GTK_CONTAINER (vbox), hbox);
    gtk_widget_show (hbox);
     
     /*Création des boutons */
    BestMatch("a", G, mot1, mot2, mot3);
	
    Bouton1 = gtk_button_new_with_label(mot1);
    Bouton2 = gtk_button_new_with_label(mot2);
    Bouton3 = gtk_button_new_with_label(mot3);
    gtk_box_pack_start ( GTK_BOX (hbox), Bouton1, TRUE, TRUE, 0);
    gtk_box_pack_start ( GTK_BOX (hbox), Bouton2, TRUE, TRUE, 0);
    gtk_box_pack_start ( GTK_BOX (hbox), Bouton3, TRUE, TRUE, 0);
    gtk_widget_show (Bouton1);
    gtk_widget_show (Bouton2);
    gtk_widget_show (Bouton3);
    
    /*Assemblage de la vbox*/
     
    gtk_box_pack_start ( GTK_BOX (vbox), entry, TRUE, TRUE, 0); //On ajoute entry dans vbox 
    gtk_widget_show(vbox);  
     
    /* Création de hbox */
     
    hbox = gtk_hbox_new (FALSE, 0);
    gtk_container_add (GTK_CONTAINER (vbox), hbox);
    gtk_widget_show (hbox);
    
    /* Création de hbox2 */
    
    hbox2 = gtk_hbox_new (FALSE, 0);
    gtk_container_add(GTK_CONTAINER(vbox), hbox2);
    gtk_widget_show (hbox2);
    
    /*Création d'un toggle*/
    toggle = gtk_toggle_button_new_with_label( "Prediction" );
    gtk_widget_show(toggle);
    gtk_box_pack_start( GTK_BOX (hbox2), toggle, TRUE, TRUE, 0);
    
    /*Création d'un bouton supprimer*/
    suppr = gtk_button_new_with_label("Supprimer du dictionnaire");
    gtk_box_pack_start(GTK_BOX (hbox2), suppr, TRUE, TRUE, 0);
    gtk_widget_show(suppr);
    
    /*Gestion évenementielle*/
     g_signal_connect (window, "destroy",G_CALLBACK (OnQuit), NULL);
     g_signal_connect (entry, "key-release-event", G_CALLBACK (OnType), G);
     g_signal_connect (Bouton1, "released", G_CALLBACK (OnPress), entry);
     g_signal_connect (Bouton2, "released", G_CALLBACK (OnPress), entry);
     g_signal_connect (Bouton3, "released", G_CALLBACK (OnPress), entry);
     g_signal_connect(suppr,"released", G_CALLBACK (OnDel), G);
	gtk_main();
	   
	return EXIT_SUCCESS;
}

/*Callback pour chaque touche tapée dans la zone de texte*/
static gboolean OnType(GtkWidget *widget, GdkEventKey *event, graphe* G) 
{
	gint i;
	char** result;
	gint pressedKeyCode = event->keyval;
	gchar* currentWord = (gchar*) malloc(60*sizeof(char));
	gchar* cutWord = (gchar*) malloc(60*sizeof(char));
	
	
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(toggle)))
	{
		gtk_button_set_label(GTK_BUTTON(Bouton1),"Fonction desactivee");
		gtk_button_set_label(GTK_BUTTON(Bouton2),"Fonction desactivee");
		gtk_button_set_label(GTK_BUTTON(Bouton3),"Fonction desactivee");
		return FALSE;
	}
	
	/*Cas de la touche espace*/
	
	if(pressedKeyCode==32)
	{
		positionBeforeWord = gtk_editable_get_position(GTK_EDITABLE(widget));
		FILE *fp = fopen("motcourant.txt","r");
		if(fp!=NULL)
		{
			if (fscanf(fp,"%s",currentWord)==1)
			{
				printf("mot à traiter = %s\n",currentWord);
			}
			else
			{
				
				BestMatch("", G, mot1, mot2, mot3);
		
				gtk_button_set_label(GTK_BUTTON(Bouton1),mot1);
				gtk_button_set_label(GTK_BUTTON(Bouton2),mot2);
				gtk_button_set_label(GTK_BUTTON(Bouton3),mot3);
				return FALSE;
			}
			
		}
		fclose(fp);
		if(strcmp(currentWord, "Effacez_Votre_Mot_En_Entier")!=0)
		{
			if(ajout_volee(currentWord,G)==FALSE)
			{
				maj_occurrence(currentWord);
				creation_graphe(G);
				printf("mot mis à jour : %s\n", currentWord);
			
			}
		
			else
			{
				creation_graphe(G);
				printf("mot ajouté : %s\n", currentWord);
			}
		}
		
		sortie();
		FILE *fp1 = fopen("motcourant.txt","w");
		fclose(fp1);
		visualisation_liste_chainee();
		
		BestMatch("", G, mot1, mot2, mot3);
		
		gtk_button_set_label(GTK_BUTTON(Bouton1),mot1);
		gtk_button_set_label(GTK_BUTTON(Bouton2),mot2);
		gtk_button_set_label(GTK_BUTTON(Bouton3),mot3);
	}
	/*Cas de la touche retour*/
	else if(pressedKeyCode==65288)
	{
		FILE *fp3 = fopen("motcourant.txt","w");
		fputs("Effacez_Votre_Mot_En_Entier",fp3);
		fclose(fp3);
		
	}
	else
	{
		FILE *fp4 = fopen("motcourant.txt","a");
		g_printerr("key pressed : %s\n",gdk_keyval_name(pressedKeyCode));
		fputs(gdk_keyval_name(pressedKeyCode),fp4);
		fclose(fp4);
		
	}
	/*On attribue le texte que doit contenir chaque bouton*/
	FILE *fp5 = fopen("motcourant.txt","r");
	if(fp5!=NULL)
	//On cherche les mots qui correspondent à currentWord
	
	{
		while(fscanf(fp5,"%s",currentWord)==1)
		{
			BestMatch(currentWord, G, mot1, mot2, mot3);
			gtk_button_set_label(GTK_BUTTON(Bouton1),mot1);
			gtk_button_set_label(GTK_BUTTON(Bouton2),mot2);
			gtk_button_set_label(GTK_BUTTON(Bouton3),mot3);
		}
	}
    return FALSE;
}
gint OnPress (GtkButton *button, GtkEntry* entry)
{
	const gchar* entryText = gtk_entry_get_text(entry);
	const gchar *text = gtk_button_get_label( button );
	gchar* mot = (gchar*) text;
	gint position = gtk_editable_get_position(GTK_EDITABLE (entry) );
	gint i;
	maj_occurrence(mot);
	
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(toggle)))
	{
		return 1;
	}
	
	gtk_editable_delete_text(GTK_EDITABLE(entry), positionBeforeWord, position);
	printf("PositionBeforeWord = %d\n", positionBeforeWord);
	printf("position : %d\n",position);
	gtk_editable_insert_text (GTK_EDITABLE (entry), text, -1, &position);
	
	return 0;
}

gint OnDel (GtkButton *button, graphe* G)
{	
	gint scan;
	gchar texte[80];
	gchar* wordToDel = (gchar*) malloc(20*sizeof(gchar));
	FILE *fp = fopen("motcourant.txt", "r");
	if(fp==NULL)
	{
		gtk_button_set_label(button, "Tapez un mot");
	}
	else
	{
		scan = fscanf(fp, "%s",wordToDel);
		if(scan==0)
		{
			gtk_button_set_label(button, "Tapez un mot");
			fclose(fp);
			return 1;
		}
		else
		{
			strcat(texte,"Mot supprimé : ");
			strcat(texte ,wordToDel);
			gtk_button_set_label(button, texte);
			printf("mot à supprimer : %s\n", wordToDel);
			suppression_volee((char*) wordToDel);
			fclose(fp);
			creation_graphe(G);
			sortie();
			FILE *fpp = fopen("graphe.txt", "r");
			while(fscanf(fpp,"%s %d", wordToDel, &scan)==2)
			{
				printf("lu :%s %d\n", wordToDel, scan);
			}
			fclose(fpp);
			return 0;
		}
	}
}

void OnQuit (void)
{
	system("rm motcourant.txt");
	gtk_main_quit();
}

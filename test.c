#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

typedef struct node node;
typedef struct relation relation;

typedef struct relation {
    node* nextNode;
    char* etiquette;
    int nbrEtiquette;
} relation;

typedef struct node {
    char *Id;
    bool isFinal;
    relation *transition;
    int transitionCount;
} node;

typedef struct automate {
    node* nodes;
    node** initialNode;
    int countNode;
    node **FinalNodes;
    int countFinalNodes;
    int countInNodes;
    char Nom[100];
} automate;


automate* CreateAutomate();
node* CreateNodes(int NbrNodes);
automate* ReadFile(char* fileName);
void PrintAutomate(automate* aut);
void WriteAutomateToFile(automate* aut, char* fileName);
automate* EnterAutomate();


automate* EnterAutomate() {
    automate* aut = CreateAutomate();
    if (!aut) return NULL;

    printf("Nom de l'automate : ");
    scanf("%s", aut->Nom);

    printf("Nombre d'etats : ");
    scanf("%d", &aut->countNode);
    aut->nodes = CreateNodes(aut->countNode);

    int k = 0;
    aut->initialNode = malloc(aut->countNode * sizeof(node*));
    if (!aut->initialNode) {
        printf("Erreur d'allocation memoire !\n");
        return NULL;
    }

    for (int i = 0; i < aut->countNode; i++) {
        aut->nodes[i].Id = malloc(10);
        printf("Nom du noeud %d : ", i + 1);
        scanf("%s", aut->nodes[i].Id);
        
        printf("Est-ce-que c'est une etat initial ? (1=Oui, 0=Non) : ");
        int choice;
        scanf("%d", &choice);
        if (choice == 1) {
            aut->initialNode[k] = &aut->nodes[i]; 
            k++;
        }

        printf("Est-ce un etat final ? (1=Oui, 0=Non) : ");
        int final;
        scanf("%d", &final);
        aut->nodes[i].isFinal = (final == 1);
    }

    printf("Passant aux transitions :\n");
    for (int i = 0; i < aut->countNode; i++) {
        printf(" %s a des transitions ? (1=Oui, 0=Non) : ", aut->nodes[i].Id);
        int choice1;
        scanf("%d", &choice1);

        if (choice1 == 1) {
            printf("Combien %s a de transitions ? \n", aut->nodes[i].Id);
            scanf("%d", &aut->nodes[i].transitionCount);

            
            aut->nodes[i].transition = malloc(aut->nodes[i].transitionCount * sizeof(relation));
            if (!aut->nodes[i].transition) {
                printf("Erreur d'allocation des transitions !\n");
                return NULL;
            }

            for (int j = 0; j < aut->nodes[i].transitionCount; j++) {
                aut->nodes[i].transition[j].nextNode = malloc(10);
                aut->nodes[i].transition[j].etiquette = malloc(10);

                printf("%s vers ? (entrer Id d'un noeud existant) : ", aut->nodes[i].Id);
                scanf("%s", aut->nodes[i].transition[j].nextNode->Id);

                printf("Avec l'etiquette : ");
                scanf("%s", aut->nodes[i].transition[j].etiquette);
            }
        }
    }

    return aut;
}


void WriteAutomateToFile(automate* aut, char* fileName) {
    FILE* file = fopen(fileName, "w");
    if (!file) {
        printf("Erreur lors de l'ouverture du fichier.\n");
        return;
    }

    fprintf(file, "Nom de l'automate : %s\n", aut->Nom);
    fprintf(file, "Nombre d'etats : %d\n", aut->countNode);

    // ecriture des etats
    fprintf(file, "etats : ");
    for (int i = 0; i < aut->countNode; i++) {
        fprintf(file, "%s ", aut->nodes[i].Id);
    }
    fprintf(file, "\n");

    // ecriture des etats initiaux
    fprintf(file, "etats initiaux : ");
    for (int i = 0; i < aut->countNode; i++) {
        if (&aut->nodes[i] == aut->initialNode[i]) {
            fprintf(file, "%s ", aut->nodes[i].Id);
        }
    }
    fprintf(file, "\n");

    // ecriture des etats finaux
    fprintf(file, "etats finaux : ");
    for (int i = 0; i < aut->countNode; i++) {
        if (aut->nodes[i].isFinal) {
            fprintf(file, "%s ", aut->nodes[i].Id);
        }
    }
    fprintf(file, "\n");

    // ecriture des transitions
    fprintf(file, "Transitions :\n");
    for (int i = 0; i < aut->countNode; i++) {
        for (int j = 0; j < aut->nodes[i].transitionCount; j++) {
            fprintf(file, "%s --(%s)--> %s\n",
                    aut->nodes[i].Id,
                    aut->nodes[i].transition[j].etiquette,
                    aut->nodes[i].transition[j].nextNode->Id);
        }
    }

    fclose(file);
    printf("Automate sauvegarde dans %s\n", fileName);
}

automate* ReadFile(char* fileName) {
    FILE* file = fopen(fileName, "r");
    if (!file) {
        printf("Erreur : Impossible d'ouvrir le fichier !\n");
        return NULL;
    }

    automate* aut = CreateAutomate();
    if (!aut) return NULL;

    char buffer[256];
    while (fgets(buffer, sizeof(buffer), file)) {
        if (strstr(buffer, "Nom de l'automate")) {
            sscanf(buffer, "Nom de l'automate : %s", aut->Nom);
        } else if (strstr(buffer, "Nombre d'etats")) {
            sscanf(buffer, "Nombre d'etats : %d", &aut->countNode);
            aut->nodes = CreateNodes(aut->countNode);
        } else if (strstr(buffer, "etats initiaux")) {
            char id[50];
            aut->initialNode = malloc(aut->countNode * sizeof(node*));
            int k = 0;
            while (sscanf(buffer, "%s", id) == 1) {
                for (int i = 0; i < aut->countNode; i++) {
                    if (strcmp(aut->nodes[i].Id, id) == 0) {
                        aut->initialNode[k++] = &aut->nodes[i];
                    }
                }
            }
        } else if (strstr(buffer, "etats finaux")) {
            char id[50];
            while (sscanf(buffer, "%s", id) == 1) {
                for (int i = 0; i < aut->countNode; i++) {
                    if (strcmp(aut->nodes[i].Id, id) == 0) {
                        aut->nodes[i].isFinal = true;
                    }
                }
            }
        } else if (strstr(buffer, "Transitions")) {
            char from[50], to[50], etiquette[50];
            while (fscanf(file, "%s --(%s)--> %s", from, etiquette, to) == 3) {
                for (int i = 0; i < aut->countNode; i++) {
                    if (strcmp(aut->nodes[i].Id, from) == 0) {
                        int j = aut->nodes[i].transitionCount++;
                        aut->nodes[i].transition = realloc(aut->nodes[i].transition, aut->nodes[i].transitionCount * sizeof(relation));
                        aut->nodes[i].transition[j].etiquette = strdup(etiquette);
                        for (int k = 0; k < aut->countNode; k++) {
                            if (strcmp(aut->nodes[k].Id, to) == 0) {
                                aut->nodes[i].transition[j].nextNode = &aut->nodes[k];
                            }
                        }
                    }
                }
            }
        }
    }

    fclose(file);
    return aut;
}

//  Fonction pour afficher un automate
void PrintAutomate(automate* aut) {
    if (!aut) {
        printf("L'automate est vide !\n");
        return;
    }

    printf("Nom de l'automate : %s\n", aut->Nom);
    
    // Affichage des etats
    printf("etats : ");
    for (int i = 0; i < aut->countNode; i++) {
        printf("%s ", aut->nodes[i].Id);
    }
    printf("\n");

    // Affichage des etats initiaux
    printf("etats initiaux : ");
    for (int i = 0; i < aut->countNode; i++) {
        if (&aut->nodes[i] == aut->initialNode[i]) {
            printf("%s ", aut->nodes[i].Id);
        }
    }
    printf("\n");

    // Affichage des etats finaux
    printf("etats finaux : ");
    for (int i = 0; i < aut->countNode; i++) {
        if (aut->nodes[i].isFinal) {
            printf("%s ", aut->nodes[i].Id);
        }
    }
    printf("\n");

    // Affichage des transitions
    printf("Transitions :\n");
    for (int i = 0; i < aut->countNode; i++) {
        for (int j = 0; j < aut->nodes[i].transitionCount; j++) {
            printf("%s --(%s)--> %s\n",
                   aut->nodes[i].Id,
                   aut->nodes[i].transition[j].etiquette,
                   aut->nodes[i].transition[j].nextNode->Id);
        }
    }
}


//  Fonction de creation d'un automate
automate* CreateAutomate() {
    automate* aut = malloc(sizeof(automate));
    if (!aut) {
        printf("Erreur : Allocation echouee\n");
        return NULL;
    }
    aut->countNode = 0;
    aut->nodes = NULL;
    aut->initialNode = NULL;
    return aut;
}

//  Fonction pour creer des nœuds
node* CreateNodes(int NbrNodes) {
    node* nodes = malloc(sizeof(node) * NbrNodes);
    if (!nodes) {
        printf("Erreur : Allocation echouee\n");
        return NULL;
    }
    for (int i = 0; i < NbrNodes; i++) {
        nodes[i].isFinal = false;
        nodes[i].transition = NULL;
        nodes[i].transitionCount = 0;
    }
    return nodes;
}
void genererficher( automate aut){
    char nom[20];
    printf("Veuillez entrer le nom de votre fichier\n");
    scanf("%s", nom);
    FILE *file=fopen("%s.dot","w+");
    fprintf(file ,"digraph Automate {\n");
    fprintf(file,"node[shape=point, width=0];start;\n");
    fprintf(file,"node[shape=circle];");
    for(int i=0 ; i< aut.countNode;i++){
        fprintf(file,"%s,",aut.nodes[i]->Id);
    }
    fprintf(file,";");
    fprintf(file,"node[shape=doublecircle]");
    for(int j=0 ; j< aut.countFinalNodes ; j++){
        fprintf(file,"%s",aut.FinalNodes[j]->Id);
    }
    fprintf(file,";");

    fprintf(file,"start -> ");
    for(int k=0 ;k<aut.countInNodes;k++){
        fprintf(file,"%s",aut.initialNode[k]->Id);
    }
    fprintf(file,";");

    for(int l=0; l<aut.countNode;l++){
        for(int m=0; m<aut.nodes[l].transitionCount;m++){
            fprintf(file,"%s->%s [label=\"%s\"]",aut.nodes[l].Id,aut.nodes[l].transition[m].nextNode->Id,aut.nodes[l].transition[m]->etiquette);
            fprintf(file,";");
        }

    }
    fprintf(file,"}");
}

void EtatPlusTransition(automate *aut ){
    int nbrTransition; 
    int tmp=aut->nodes[0].transitionCount;
    char* NomNoeud;
    for(int i=0;i<aut->countNode;i++){
    if(aut->nodes[i].transitionCount>tmp){
        tmp=aut->nodes[i].transitionCount;
        NomNoeud=aut->nodes[i].Id;
    }else{
        i++;
    }
    
    }
    printf("%s",NomNoeud);

}


//  Fonction main avec menu interactif
int main() {
    automate* aut = NULL;
    int choix;
    char filename[100];

    do {
        printf("\nMenu :\n");
        printf("1- Entrer un automate\n");
        printf("2- Lire un automate depuis un fichier .dot (not working now)\n");
        printf("3- Afficher l'automate\n");
        printf("4- Sauvegarder l'automate dans un fichier .txt\n");
        printf("5- Quitter\n");
        printf("Choix : ");
        scanf("%d", &choix);

        switch (choix) {
            case 1:
                aut = EnterAutomate();
                break;
            case 2:
                printf("Nom du fichier .dot : ");
                scanf("%s", filename);
                aut = ReadFile(filename);
                break;
            case 3:
                PrintAutomate(aut);
                break;
            case 4:
                if (aut) {
                    printf("Nom du fichier de sortie : ");
                    scanf("%s", filename);
                    WriteAutomateToFile(aut, filename);
                } else {
                    printf("Aucun automate charge !\n");
                }
                break;
            case 5:
                printf("Fin du programme.\n");
                break;
            default:
                printf("Option invalide !\n");
        }
    } while (choix != 5);

    return 0;
}

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
    node** initialNodes;
    int countNode;
    node** FinalNodes;
    int countFinalNodes;
    int countInitialNode;
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
    aut->initialNodes = malloc(aut->countNode * sizeof(node*)); // Fix: Allocate memory for node*
    if (!aut->initialNodes) {
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
            aut->initialNodes[k] = &aut->nodes[i]; // Fix: Store the address of the node
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
                aut->nodes[i].transition[j].nextNode = malloc(sizeof(node)); // Fix: Allocate memory for node
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
    for (int i = 0; i < aut->countInitialNode; i++) { // Fix: Iterate over initial nodes
        fprintf(file, "%s ", aut->initialNodes[i]->Id);
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
    for (int i = 0; i < aut->countInitialNode; i++) { // Fix: Iterate over initial nodes
        printf("%s ", aut->initialNodes[i]->Id);
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

node* findOrCreateNode(automate* a, const char* id) {
    for (int i = 0; i < a->countNode; i++) {
        if (strcmp(a->nodes[i].Id, id) == 0) {
            return &a->nodes[i];
        }
    }

    // Create a new node
    a->countNode++;
    a->nodes = realloc(a->nodes, a->countNode * sizeof(node));
    if (!a->nodes) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }

    node* newNode = &a->nodes[a->countNode - 1];
    newNode->Id = strdup(id);  // Duplicate the ID safely
    newNode->isFinal = false;
    newNode->transition = NULL;
    newNode->transitionCount = 0;

    return newNode;
}

// Function to add a transition
void addTransition(node* fromNode, node* toNode, const char* label) {
    fromNode->transitionCount++;
    fromNode->transition = realloc(fromNode->transition, fromNode->transitionCount * sizeof(relation));
    if (!fromNode->transition) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }

    relation* newRelation = &fromNode->transition[fromNode->transitionCount - 1];
    newRelation->nextNode = toNode;
    newRelation->etiquette = strdup(label);
}

// Function to parse .dot file
void parseDotFile(const char* filename, automate* a) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    a->initialNodes = NULL; // Fix: Initialize initial states array

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        // Extract Automate name
        if (strstr(line, "digraph")) {
            sscanf(line, "digraph %s {", a->Nom);
        } 
        // Extract final states
        else if (strstr(line, "node[shape=doublecircle]")) {
            char *start = strstr(line, "doublecircle]") + 13;  // Move past "doublecircle]"
            char *token = strtok(start, " ,;\n");
            while (token) {
                node* n = findOrCreateNode(a, token);
                n->isFinal = true;
                token = strtok(NULL, " ,;\n");
            }
        } 
        // Extract initial states
        else if (strstr(line, "start->")) {
            char* token = strtok(line, " ->,;\n");
            token = strtok(NULL, " ->,;\n");  // Skip "start->"
            while (token) {
                node* n = findOrCreateNode(a, token);
                a->initialNodes = realloc(a->initialNodes, (a->countInitialNode + 1) * sizeof(node*)); // Fix: Use countInitialNode
                if (!a->initialNodes) {
                    perror("Memory allocation failed");
                    exit(EXIT_FAILURE);
                }
                a->initialNodes[a->countInitialNode++] = n; // Fix: Use countInitialNode
                token = strtok(NULL, " ->,;\n");
            }
        } 
        // Extract transitions
        else if (strstr(line, "->") && strstr(line, "[label=")) {
            char from[100], to[100], label[100];
            if (sscanf(line, " %99[^->] -> %99[^ [] [label=\"%99[^\"]\"];", from, to, label) == 3) {
                node* fromNode = findOrCreateNode(a, from);
                node* toNode = findOrCreateNode(a, to);
                addTransition(fromNode, toNode, label);
            }
        }
    }
    fclose(file);
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
    aut->initialNodes = NULL;
    aut->FinalNodes = NULL; // Fix: Initialize FinalNodes
    aut->countFinalNodes = 0; // Fix: Initialize countFinalNodes
    aut->countInitialNode = 0; // Fix: Initialize countInitialNode
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
    char filename[25];
    snprintf(filename, sizeof(filename), "%s.dot", nom); // Fix: Format the filename
    FILE *file = fopen(filename, "w+");
    if (!file) {
        printf("Erreur lors de l'ouverture du fichier.\n");
        return;
    }
    fprintf(file, "digraph Automate {\n");
    fprintf(file, "node[shape=point, width=0];start;\n");
    fprintf(file, "node[shape=circle];");
    for (int i = 0; i < aut.countNode; i++) {
        fprintf(file, "%s,", aut.nodes[i].Id);
    }
    fprintf(file, ";\n");
    fprintf(file, "node[shape=doublecircle];");
    for (int j = 0; j < aut.countFinalNodes; j++) {
        fprintf(file, "%s,", aut.FinalNodes[j]->Id);
    }
    fprintf(file, ";\n");

    fprintf(file, "start -> ");
    for (int k = 0; k < aut.countInitialNode; k++) {
        fprintf(file, "%s,", aut.initialNodes[k]->Id);
    }
    fprintf(file, ";\n");

    for (int l = 0; l < aut.countNode; l++) {
        for (int m = 0; m < aut.nodes[l].transitionCount; m++) {
            fprintf(file, "%s->%s [label=\"%s\"];\n",
                    aut.nodes[l].Id,
                    aut.nodes[l].transition[m].nextNode->Id,
                    aut.nodes[l].transition[m].etiquette);
        }
    }
    fprintf(file, "}\n");
    fclose(file);
}

void EtatPlusTransition(automate *aut ){
    //int nbrTransition; 
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

void freeAutomate(automate* a) {
    for (int i = 0; i < a->countNode; i++) {
        free(a->nodes[i].Id);
        for (int j = 0; j < a->nodes[i].transitionCount; j++) {
            free(a->nodes[i].transition[j].etiquette);
        }
        free(a->nodes[i].transition);
    }
    free(a->nodes);
    free(a->initialNodes);
    free(a->FinalNodes);
}

//  Fonction main avec menu interactif
int main() {
    automate* aut = NULL;
    int choix;
    char filename[100];

    do {
        printf("\nMenu :\n");
        printf("1- Entrer un automate\n");
        printf("2- Lire un automate depuis un fichier .dot\n");
        printf("3- Afficher l'automate\n");
        printf("4- Sauvegarder l'automate dans un fichier .txt\n");
        printf("5- Quitter\n");
        printf("6- Generer un fichier .dot\n");
        printf("Choix : ");
        scanf("%d", &choix);

        switch (choix) {
            case 1:
                aut = EnterAutomate();
                break;
            case 2:
                printf("Nom du fichier .dot : ");
                scanf("%s", filename);
                aut = CreateAutomate(); // Fix: Initialize aut before using it
                parseDotFile(filename, aut);
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
            case 6:
                genererficher(*aut);
                break;  
            default:
                printf("Option invalide !\n");
        }
    } while (choix != 5);

    if (aut) {
        freeAutomate(aut);
    }

    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

int NbrAutomate = 0;

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


void addFinalNode(automate* aut, node* n) {
    aut->FinalNodes = realloc(aut->FinalNodes, (aut->countFinalNodes + 1) * sizeof(node*));
    if (!aut->FinalNodes) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }
    aut->FinalNodes[aut->countFinalNodes++] = n;
}

automate* EnterAutomate() {
    automate* aut = CreateAutomate();
    if (!aut) return NULL;

    printf("Nom de l'automate : ");
    scanf("%s", aut->Nom);

    printf("Nombre d'etats : ");
    scanf("%d", &aut->countNode);
    printf("Nombre d'etats : %d\n", aut->countNode);
    aut->nodes = CreateNodes(aut->countNode);
    if (!aut->nodes) {
        free(aut);
        return NULL;
    }

    int k = 0;
    aut->initialNodes = malloc(aut->countNode * sizeof(node*));
    if (!aut->initialNodes) {
        printf("Erreur d'allocation memoire !\n");
        free(aut->nodes);
        free(aut);
        return NULL;
    }

    for (int i = 0; i < aut->countNode; i++) {
        aut->nodes[i].Id = malloc(10);
        if (!aut->nodes[i].Id) {
            printf("Erreur d'allocation memoire pour l'ID du noeud !\n");
            freeAutomate(aut);
            return NULL;
        }
        printf("Nom du noeud %d : ", i + 1);
        scanf("%s", aut->nodes[i].Id);

        printf("Est-ce-que c'est une etat initial ? (1=Oui, 0=Non) : ");
        int choice;
        scanf("%d", &choice);
        if (choice == 1) {
            aut->initialNodes[k] = &aut->nodes[i];
            k++;
        }

        printf("Est-ce un etat final ? (1=Oui, 0=Non) : ");
        int final;
        scanf("%d", &final);
        aut->nodes[i].isFinal = (final == 1);
        if (aut->nodes[i].isFinal) {
            addFinalNode(aut, &aut->nodes[i]);
        }
    }
    aut->countInitialNode = k;

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
                freeAutomate(aut);
                return NULL;
            }

            for (int j = 0; j < aut->nodes[i].transitionCount; j++) {
                char nextNodeId[10];
                printf("%s vers ? (entrer Id d'un noeud existant) : ", aut->nodes[i].Id);
                scanf("%s", nextNodeId);

                node* nextNode = NULL;
                for (int k = 0; k < aut->countNode; k++) {
                    if (strcmp(aut->nodes[k].Id, nextNodeId) == 0) {
                        nextNode = &aut->nodes[k];
                        break;
                    }
                }

                if (!nextNode) {
                    printf("Erreur : Noeud suivant non trouve !\n");
                    freeAutomate(aut);
                    return NULL;
                }

                aut->nodes[i].transition[j].nextNode = nextNode;
                aut->nodes[i].transition[j].etiquette = malloc(10);
                if (!aut->nodes[i].transition[j].etiquette) {
                    printf("Erreur d'allocation memoire pour l'etiquette !\n");
                    freeAutomate(aut);
                    return NULL;
                }

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

    fprintf(file, "etats : ");
    for (int i = 0; i < aut->countNode; i++) {
        fprintf(file, "%s ", aut->nodes[i].Id);
    }
    fprintf(file, "\n");

    fprintf(file, "etats initiaux : ");
    for (int i = 0; i < aut->countInitialNode; i++) {
        fprintf(file, "%s ", aut->initialNodes[i]->Id);
    }
    fprintf(file, "\n");

    fprintf(file, "etats finaux : ");
    for (int i = 0; i < aut->countNode; i++) {
        if (aut->nodes[i].isFinal) {
            fprintf(file, "%s ", aut->nodes[i].Id);
        }
    }
    fprintf(file, "\n");

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
    for (int i = 0; i < aut->countInitialNode; i++) { 
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
    if (!newNode->Id) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }
    newNode->isFinal = false;
    newNode->transition = NULL;
    newNode->transitionCount = 0;

    return newNode;
}

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

void parseDotFile(const char* filename, automate* a) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    a->initialNodes = NULL; 

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        
        if (strstr(line, "digraph")) {
            char* temp = strstr(line, "digraph");
            if (temp) {
                sscanf(temp, "digraph %s {", a->Nom);
            }
            sscanf(temp, "digraph %s {", a->Nom);
        } 
        // Extract final states
        else if (strstr(line, "->final")) {
            char* token = strtok(line, " ->,;\n");
            while (token && strcmp(token, "final") != 0) {
                node* n = findOrCreateNode(a, token);
                n->isFinal = true;
                addFinalNode(a, n);
                token = strtok(NULL, " ->,;\n");
            }
        } 
        // Extract initial states
        else if (strstr(line, "start->")) {
            char* token = strtok(line, " ->,;\n");
            token = strtok(NULL, " ->,;\n");  
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

automate* CreateAutomate() {
    automate* aut = malloc(sizeof(automate));
    if (!aut) {
        printf("Erreur : Allocation echouee\n");
        return NULL;
    }
    aut->countNode = 0;
    aut->nodes = NULL;
    aut->initialNodes = NULL;
    aut->FinalNodes = NULL; 
    aut->countFinalNodes = 0; 
    aut->countInitialNode = 0; 
    NbrAutomate++;
    return aut;
    
}

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

void genererficher(automate *aut) {
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
    fprintf(file, "node[shape=point, width=0];final;\n");
    fprintf(file, "node[shape=circle];");
    for (int i = 0; i < aut->countNode; i++) {
        if (i < aut->countNode - 1) {
            fprintf(file, "%s,", aut->nodes[i].Id);
        } else {
            fprintf(file, "%s;\n", aut->nodes[i].Id);
        }
    }

    fprintf(file, "start -> ");
    for (int k = 0; k < aut->countInitialNode; k++) {
        if (k < aut->countInitialNode - 1) {
            fprintf(file, "%s,", aut->initialNodes[k]->Id);
        } else {
            fprintf(file, "%s;\n", aut->initialNodes[k]->Id);
        }
    }

    int finalNodeCount = 0;

    for (int i = 0; i < aut->countNode; i++) {
        if (aut->nodes[i].isFinal) {
            finalNodeCount++;
        }
    }

    int writtenFinalNodes = 0;
    for (int i = 0; i < aut->countNode; i++) {
        if (aut->nodes[i].isFinal) {
            writtenFinalNodes++;
            if (writtenFinalNodes < finalNodeCount) {
                fprintf(file, "%s, ", aut->nodes[i].Id);
            } else {
                fprintf(file, "%s", aut->nodes[i].Id);
            }
        }
    }
    fprintf(file, " -> final;\n");

    for (int l = 0; l < aut->countNode; l++) {
        for (int m = 0; m < aut->nodes[l].transitionCount; m++) {
            fprintf(file, "%s->%s [label=\"%s\"];\n",
                    aut->nodes[l].Id,
                    aut->nodes[l].transition[m].nextNode->Id,
                    aut->nodes[l].transition[m].etiquette);
        }
    }
    fprintf(file, "}\n");
    fclose(file);
    printf("Fichier %s cree avec succes.\n", filename);
}

void EtatPlusTransition(automate *aut) {
    if (aut->countNode == 0) {
        printf("L'automate ne contient aucun nœud.\n");
        return;
    }

    int maxTransitions = aut->nodes[0].transitionCount;
    char* NomNoeud = aut->nodes[0].Id;

    for (int i = 1; i < aut->countNode; i++) {
        if (aut->nodes[i].transitionCount > maxTransitions) {
            maxTransitions = aut->nodes[i].transitionCount;
            NomNoeud = aut->nodes[i].Id;
        }
    }

    printf("Le nœud avec le plus grand nombre de transitions est : %s\n", NomNoeud);
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
        printf("7- Afficher l'etat avec le plus de transitions\n");
        printf("Choix : ");
        scanf("%d", &choix);

        switch (choix) {
            case 1:
                if (aut) {
                    freeAutomate(aut);
                }
                aut = EnterAutomate();
                break;
            case 2:
                printf("Nom du fichier .dot : ");
                scanf("%s", filename);
                if (aut) {
                    freeAutomate(aut);
                }
                aut = CreateAutomate();
                parseDotFile(filename, aut);
                break;
            case 3:
                if (aut) {
                    PrintAutomate(aut);
                } else {
                    printf("Aucun automate charge !\n");
                }
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
                if (aut) {
                    genererficher(aut);
                } else {
                    printf("Aucun automate charge !\n");
                }
                break;
            case 7:
                if (aut) {
                    EtatPlusTransition(aut);
                } else {
                    printf("Aucun automate charge !\n");
                }
                break;
            default:
                printf("Option invalide !\n");
        }
    } while (choix != 5);

  

    return 0;
}
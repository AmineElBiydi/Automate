#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

typedef struct relation relation;
typedef struct node {
    char *Id;
    bool isFinal;
    relation *transition;
    int transitionCount;
} node;

typedef struct relation {
    node* nextNode;
    char* etiquette;
    int nbrEtiquette;
} relation;

typedef struct automate {
    node** initialNode;
    node* nodes;
    int countNode;
    int countInitialNode;
    char Nom[100];
} automate;

void PrintAutomate(automate* aut);
automate* ReadFile(char* fileName);
void FreeAutomate(automate* aut);

int main() {
    automate* automates = ReadFile("test1.dot");
    if (automates) {
        PrintAutomate(automates);
        FreeAutomate(automates);
    }
    return 0;
}

node* CreateNodes(int NbrNodes) {
    node* nodes = malloc(sizeof(node) * NbrNodes);
    if (nodes == NULL) {
        printf("Erreur: node is Null!\n");
        return NULL;
    }
    for (int i = 0; i < NbrNodes; i++) {
        nodes[i].Id = NULL;
        nodes[i].isFinal = false;
        nodes[i].transition = NULL;
        nodes[i].transitionCount = 0;
    }
    return nodes;
}

node* ResizeNodes(node* nodee, int newNbrNodes) {
    node* tmp = realloc(nodee, sizeof(node) * newNbrNodes);
    if (tmp == NULL) {
        printf("Erreur : reallocated node is null!\n");
        return NULL;
    }
    return tmp;
}

automate* CreateAutomate() {
    automate* automates = malloc(sizeof(automate));
    if (automates == NULL) {
        printf("Erreur : l'Automate is NULL !\n");
        return NULL;
    }
    automates->countNode = 0;
    automates->countInitialNode = 0;
    automates->initialNode = NULL;
    automates->nodes = NULL;
    return automates;
}

relation* Relation(node* InitNode, node* FinalNode, char* etiquette) {
    if (!InitNode || !FinalNode || !etiquette) {
        printf("Error: Invalid input parameters\n");
        return NULL;
    }

    int count = strlen(etiquette);

    if (InitNode->transition == NULL) {
        InitNode->transition = malloc(sizeof(relation));
        if (InitNode->transition == NULL) {
            printf("Error: Memory allocation failed\n");
            return NULL;
        }
        InitNode->transitionCount = 1;
    } else {
        relation* transition1 = realloc(InitNode->transition, sizeof(relation) * (InitNode->transitionCount + 1));
        if (transition1 == NULL) {
            printf("Error: Memory reallocation failed\n");
            return InitNode->transition;
        }
        InitNode->transition = transition1;
        InitNode->transitionCount++;
    }

    relation* currentTransition = &InitNode->transition[InitNode->transitionCount - 1];
    currentTransition->nextNode = FinalNode;
    currentTransition->etiquette = strdup(etiquette);
    if (!currentTransition->etiquette) {
        printf("Error: String allocation failed\n");
        return NULL;
    }
    currentTransition->nbrEtiquette = count;

    return InitNode->transition;
}

/*
    ======= lecture de fichier ======
*/
automate* ReadFile(char* fileName) {
    FILE* file = fopen(fileName, "r+");
    if (file == NULL) {
        printf("Erreur : erreur lors de l'ouverture du fichier !\n");
        return NULL;
    }

    char line[256];
    char* token;
    int nbrNodeAllocated = 2;
    automate* automate = CreateAutomate();
    if (automate == NULL) {
        fclose(file);
        return NULL;
    }

    automate->nodes = CreateNodes(nbrNodeAllocated);
    if (automate->nodes == NULL) {
        fclose(file);
        FreeAutomate(automate);
        return NULL;
    }

    while (fgets(line, sizeof(line), file) != NULL) {
        token = strtok(line, " [].,!}?= ->{\n");
        while (token != NULL) {
            if (strcasecmp(token, "digraph") == 0 || strcasecmp(token, "rankdir") == 0) {
                break;
            } else if ((strcasecmp(token, "node")) == 0) {
                token = strtok(NULL, " [].,!?=->\n");
                if (strcasecmp(token, "shape") == 0) {
                    token = strtok(NULL, " ;[].,!?= ->\n");
                    if (strcasecmp(token, "point") == 0) {
                        break;
                    } else if (strcasecmp(token, "circle") == 0) {
                        token = strtok(NULL, " ;[].,!?= ->\n");
                        while (token != NULL) {
                            if (automate->countNode < nbrNodeAllocated) {
                                automate->nodes[automate->countNode].Id = malloc(strlen(token) + 1);
                                strcpy(automate->nodes[automate->countNode].Id, token);
                                automate->countNode++;
                            } else {
                                nbrNodeAllocated *= 2;
                                automate->nodes = ResizeNodes(automate->nodes, nbrNodeAllocated);
                                automate->nodes[automate->countNode].Id = malloc(strlen(token) + 1);
                                strcpy(automate->nodes[automate->countNode].Id, token);
                                automate->countNode++;
                            }
                            token = strtok(NULL, " ;[].,!?= ->\n");
                        }
                        break;
                    } else if (strcasecmp(token, "doublecircle") == 0) {
                        token = strtok(NULL, " ;[].,!?= ->\n");
                        while (token != NULL) {
                            for (int i = 0; i < automate->countNode; i++) {
                                if (strcasecmp(token, automate->nodes[i].Id) == 0) {
                                    automate->nodes[i].isFinal = true;
                                    printf("node %s is final\n", automate->nodes[i].Id);
                                }
                            }
                            token = strtok(NULL, " ;[].,!?= ->\n");
                        }
                        break;
                    }
                }
            } else if (strcasecmp(token, "start") == 0) {
                nbrNodeAllocated = 2;
                automate->initialNode = malloc(sizeof(node*) * nbrNodeAllocated);
                if (automate->initialNode == NULL) {
                    printf("Erreur : allocation de mémoire pour initialNode échouée !\n");
                    fclose(file);
                    FreeAutomate(automate);
                    return NULL;
                }
                token = strtok(NULL, " ;[].,!}?= ->\n");
                while (token != NULL) {
                    for (int i = 0; i < automate->countNode; i++) {
                        if (strcasecmp(token, automate->nodes[i].Id) == 0) {
                            if (automate->countInitialNode < nbrNodeAllocated) {
                                automate->initialNode[automate->countInitialNode] = &automate->nodes[i];
                                automate->countInitialNode++;
                            } else {
                                node** tmp;
                                nbrNodeAllocated += 2;
                                tmp = realloc(automate->initialNode, sizeof(node*) * nbrNodeAllocated);
                                if (tmp == NULL) {
                                    printf("Erreur : lors de la reallocation de node initiales!!\n");
                                    fclose(file);
                                    FreeAutomate(automate);
                                    return NULL;
                                }
                                automate->initialNode = tmp;
                                automate->initialNode[automate->countInitialNode] = &automate->nodes[i];
                                automate->countInitialNode++;
                            }
                        }
                    }
                    token = strtok(NULL, " [].,!{}}?=; ->\n");
                }
                break;
            } else {
                for (int i = 0; i < automate->countNode; i++) {
                    if (strcasecmp(token, automate->nodes[i].Id) == 0) {
                        char* Ids[50];
                        char* etiquettes[50];
                        int nbrID = 0;
                        int nbrEtiquette = 0;
                        token = strtok(NULL, " [].,!}?= ->\n");
                        while (!(strcasecmp(token, "label") == 0)) {
                            Ids[nbrID] = token;
                            nbrID++;
                            token = strtok(NULL, " [].,!}?= ->\n");
                        }
                        token = strtok(NULL, " [].,!}?= ->\n");
                        while (token != NULL) {
                            etiquettes[nbrEtiquette] = token;
                            nbrEtiquette++;
                            token = strtok(NULL, " \"[].,!}?= ->\n");
                        }
                        automate->nodes[i].transition = malloc(sizeof(relation) * nbrID);
                        for (int j = 0; j < nbrID; j++) {
                            for (int k = 0; k < automate->countNode; k++) {
                                if (strcasecmp(Ids[j], automate->nodes[k].Id) == 0) {
                                    automate->nodes[i].transition[j].nextNode = &automate->nodes[k];
                                    automate->nodes[i].transition[j].etiquette = malloc(strlen(etiquettes[j]) + 1);
                                    strcpy(automate->nodes[i].transition[j].etiquette, etiquettes[j]);
                                    automate->nodes[i].transition[j].nbrEtiquette = strlen(etiquettes[j]);
                                }
                            }
                        }
                    }
                }
            }
            token = strtok(NULL, " [].,!}?= ->{");
        }
    }
    fclose(file);
    return automate;
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



void FreeAutomate(automate* aut) {
    if (!aut) return;
    
    for (int i = 0; i < aut->countNode; i++) {
        for (int j = 0; j < aut->nodes[i].transitionCount; j++) {
            free(aut->nodes[i].transition[j].etiquette);
        }
        free(aut->nodes[i].transition);
        free(aut->nodes[i].Id);
    }
    
    free(aut->nodes);
    free(aut->initialNode);
    free(aut);
}
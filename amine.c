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
    node* initialNode;
    node* nodes;
    int countNode;
    int countInitialNode;
    char Nom[100];
} automate;

void PrintAutomate(automate* aut);
automate* ReadFile(char* fileName);

int main() {
    automate* automates = ReadFile("test1.dot");
    PrintAutomate(automates);
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
    int nbr=0;  
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
    int count = 0;
    if (InitNode->transition == NULL) {
        InitNode->transition = malloc(sizeof(relation));
        if (InitNode->transition == NULL) {
            printf("Erreur : transition is NULL");
            return NULL;
        }
        InitNode->transitionCount++;
        InitNode->transition->nextNode = FinalNode;
        InitNode->transition->etiquette = malloc(strlen(etiquette) + 1);
        strcpy(InitNode->transition->etiquette, etiquette);
        for (char* word = etiquette; *word != '\0'; word++) {
            count++;
        }
        InitNode->transition->nbrEtiquette = count;
        return InitNode->transition;
    }
    relation* transition1 = realloc(InitNode->transition, sizeof(relation) * (InitNode->transitionCount + 1));
    if (transition1 == NULL) {
        printf("Erreur : reallocation is NULL !\n");
        return InitNode->transition;
    }
    InitNode->transition = transition1;
    InitNode->transitionCount++;
    InitNode->transition[InitNode->transitionCount - 1].nextNode = FinalNode;
    InitNode->transition[InitNode->transitionCount - 1].etiquette = malloc(strlen(etiquette) + 1);
    strcpy(InitNode->transition[InitNode->transitionCount - 1].etiquette, etiquette);
    for (char* word = etiquette; *word != '\0'; word++) {
        count++;
    }
    InitNode->transition[InitNode->transitionCount - 1].nbrEtiquette = count;
    return InitNode->transition;
}

/*
    ======= lecture de fichier ======
*/
automate* ReadFile(char* fileName) {
    FILE *file = fopen(fileName, "r");
    if (!file) {
        printf("Erreur : impossible d'ouvrir le fichier %s\n", fileName);
        return NULL;
    }
    int nodeCapacity = 5;
    char line[100];
    char *initNames[50];
    int initCount = 0;
    automate* aut = CreateAutomate();
    if (!aut) {
        fclose(file);
        return NULL;
    }
    aut->nodes = CreateNodes(nodeCapacity);
    while (fgets(line, sizeof(line), file) != NULL) {
        line[strcspn(line, "\n")] = '\0';
        if(strlen(line) == 0){  
            continue;
        }
        char *part = strtok(line, " {};");
        if (!part){
            continue;
        }
        if (strncmp(part, "digraph",7) == 0) {
            part = strtok(NULL, " ");
            if (part) {
                strcpy(aut->Nom, part);
            }
            continue;
        }
        else if (strcmp(part, "rankdir=LR") == 0){
            continue;
        }
        if (strncmp(part, "node", 4) == 0) {
            part = strtok(part, "[=,");
            part = strtok(NULL, "[=,");
            part = strtok(NULL, "[=,]");
            if (!part){
                continue;
            }
            if (strcmp(part, "point") == 0) {
                part = strtok(NULL, "[=,]");
                part = strtok(NULL, "[=,]");
                part = strtok(NULL, "[=,]");
                while (part!=NULL)
                {
                    if(aut->countInitialNode==0){
                        initCount=5;
                        aut->initialNode = CreateNodes(initCount);
                        aut->initialNode[aut->countInitialNode].Id = strdup(part);
                        aut->countInitialNode++;  
                        
                    }else{
                        if(aut->countInitialNode >=initCount){
                            initCount+=initCount;
                            aut->initialNode = ResizeNodes(aut->initialNode, initCount);
                            for(int i = aut->countInitialNode; i < initCount; i++){
                                aut->initialNode[i].Id = NULL;
                                aut->initialNode[i].isFinal = false;
                                aut->initialNode[i].transition = NULL;
                                aut->initialNode[i].transitionCount = 0;
                            }
                            aut->initialNode[aut->countInitialNode].Id = strdup(part);
                            aut->countInitialNode++;
                        }else{
                            aut->initialNode[aut->countInitialNode].Id = strdup(part);
                            aut->countInitialNode++;
                        }
                    }
                    part = strtok(NULL, "[=,]");
                }                                
            }
            else if (strcmp(part, "circle") == 0) {
                part = strtok(NULL, "[=,]");
               while (part!=NULL)
               {
                    if(aut->countNode<nodeCapacity){
                        aut->nodes[aut->countNode].Id = strdup(part);
                        aut->countNode++;
                    }else{
                        nodeCapacity+=nodeCapacity;
                        aut->nodes=ResizeNodes(aut->nodes, nodeCapacity);
                        for(int i = aut->countNode; i < nodeCapacity; i++){
                            aut->nodes[i].Id = NULL;
                            aut->nodes[i].isFinal = false;
                            aut->nodes[i].transition = NULL;
                            aut->nodes[i].transitionCount = 0;
                        }
                        aut->nodes[aut->countNode].Id = strdup(part);
                        aut->countNode++;
                    }
                    part = strtok(NULL, "[=,]");
                    printf("etat part : %s\n", aut->nodes[aut->countNode-1].Id);
               }
            }
            else if (strcmp(part, "doublecircle")==0){
                part = strtok(NULL, "[=,]");
                int found = 0;
                while (part!=NULL)
                {
                    found=0;
                    for(int i = 0; i < aut->countNode; i++){
                        if(strcasecmp(aut->nodes[i].Id, part) == 0){
                            aut->nodes[i].isFinal = true;
                            found = 1;
                            break;
                        }
                    }
                    if(found==0){
                        if(aut->countNode>=nodeCapacity){
                            nodeCapacity+=nodeCapacity;
                            aut->nodes=ResizeNodes(aut->nodes, nodeCapacity);
                            for(int i = aut->countNode; i < nodeCapacity; i++){
                                aut->nodes[i].Id = NULL;
                                aut->nodes[i].isFinal = false;
                                aut->nodes[i].transition = NULL;
                                aut->nodes[i].transitionCount = 0;
                            }
                            aut->nodes[aut->countNode].Id = strdup(part);
                            aut->nodes[aut->countNode].isFinal = true;
                            aut->countNode++;
                        }
                        else{
                            aut->nodes[aut->countNode].Id = strdup(part);
                            aut->nodes[aut->countNode].isFinal = true;
                            aut->countNode++;
                        }
                    }
                    for(int i = 0; i < aut->countNode; i++){
                        if(aut->nodes[i].isFinal==true){
                            printf("etat final part : %s\n", aut->nodes[i].Id);
                        }
                    }
                    part = strtok(NULL, "[=,]");
                }

            }
        }else{
            int found=0;
            part=strtok(part, "->, ");
            while (part!=NULL)
            {
                if (found==0){
                    for(int i = 0; i < aut->countInitialNode; i++){
                        if(strcmp(aut->initialNode[i].Id,part) == 0){
                            part=strtok(NULL, "->,[ ");     
                            for(int j = 0; j < aut->countNode; j++){
                                if(strcmp(aut->nodes[j].Id,part) == 0){
                                    found=1;
                                    if(aut->initialNode[i].transitionCount==0){
                                        aut->initialNode[i].transition = malloc(sizeof(relation));
                                        if(aut->initialNode[i].transition == NULL){
                                            printf("Erreur : transition is NULL\n");
                                            return NULL;
                                        }
                                        aut->initialNode[i].transition[aut->initialNode[i].transitionCount].nextNode = &aut->nodes[j];
                                        aut->initialNode[i].transitionCount++;
                                        printf("part : %s\n", aut->initialNode[i].transition[aut->initialNode[i].transitionCount-1].nextNode->Id);
                                    }else{
                                        aut->initialNode[i].transition = realloc(aut->initialNode[i].transition, sizeof(relation)*(aut->initialNode[i].transitionCount+1));
                                        if(aut->initialNode[i].transition == NULL){
                                            printf("Erreur : reallocation is NULL\n");
                                            return NULL;
                                        }
                                        aut->initialNode[i].transition[aut->initialNode[i].transitionCount].nextNode = &aut->nodes[j];
                                        aut->initialNode[i].transitionCount++;
                                    }
                                }    
                            }
                            if(found==0){
                                for(int j = 0; j < aut->countInitialNode; j++){
                                if(strcmp(aut->initialNode[j].Id,part) == 0){
                                    found=1;
                                    if(aut->initialNode[i].transitionCount==0){
                                        aut->initialNode[i].transition = malloc(sizeof(relation));
                                        if(aut->initialNode[i].transition == NULL){
                                            printf("Erreur : transition is NULL\n");
                                            return NULL;
                                        }
                                        aut->initialNode[i].transition[aut->initialNode[i].transitionCount].nextNode = &aut->initialNode[j];
                                        aut->initialNode[i].transitionCount++;
                                    }else{
                                        aut->initialNode[i].transition = realloc(aut->initialNode[i].transition, sizeof(relation)*(aut->initialNode[i].transitionCount+1));
                                        if(aut->initialNode[i].transition == NULL){
                                            printf("Erreur : reallocation is NULL\n");
                                            return NULL;
                                        }
                                        aut->initialNode[i].transition[aut->initialNode[i].transitionCount].nextNode = &aut->initialNode[j];
                                        aut->initialNode[i].transitionCount++;
                                    }
                                }
                            }
                            }
                        }
                    }
                }
                if(found==0){
                    for(int i = 0; i < aut->countNode; i++){
                        if(strcmp(aut->initialNode[i].Id,part) == 0){
                            part=strtok(NULL, "->,[ ");
                                for(int j = 0; j < aut->countNode; j++){
                                    if(strcmp(aut->nodes[j].Id,part) == 0){
                                        found=1;
                                        if(aut->initialNode[i].transitionCount==0){
                                            aut->initialNode[i].transition = malloc(sizeof(relation));
                                            if(aut->initialNode[i].transition == NULL){
                                                printf("Erreur : transition is NULL\n");
                                                return NULL;
                                            }
                                            aut->initialNode[i].transition[aut->initialNode[i].transitionCount].nextNode = &aut->nodes[j];
                                            aut->initialNode[i].transitionCount++;
                                        }else{
                                            aut->initialNode[i].transition = realloc(aut->initialNode[i].transition, sizeof(relation)*(aut->initialNode[i].transitionCount+1));
                                            if(aut->initialNode[i].transition == NULL){
                                                printf("Erreur : reallocation is NULL\n");
                                                return NULL;
                                            }
                                            aut->initialNode[i].transition[aut->initialNode[i].transitionCount].nextNode = &aut->nodes[j];
                                            aut->initialNode[i].transitionCount++;
                                        }
        
                                }for(int j = 0; j < aut->countInitialNode; j++){
                                    if(strcmp(aut->initialNode[j].Id,part) == 0){
                                        found=1;
                                        if(aut->initialNode[i].transitionCount==0){
                                            aut->initialNode[i].transition = malloc(sizeof(relation));
                                            if(aut->initialNode[i].transition == NULL){
                                                printf("Erreur : transition is NULL\n");
                                                return NULL;
                                            }
                                            aut->initialNode[i].transition[aut->initialNode[i].transitionCount].nextNode = &aut->initialNode[j];
                                            aut->initialNode[i].transitionCount++;
                                        }else{
                                            aut->initialNode[i].transition = realloc(aut->initialNode[i].transition, sizeof(relation)*(aut->initialNode[i].transitionCount+1));
                                            if(aut->initialNode[i].transition == NULL){
                                                printf("Erreur : reallocation is NULL\n");
                                                return NULL;
                                            }
                                            aut->initialNode[i].transition[aut->initialNode[i].transitionCount].nextNode = &aut->initialNode[j];
                                            printf("etat part : %s\n", aut->initialNode[i].transition[aut->initialNode[i].transitionCount-1].nextNode->Id);
                                            aut->initialNode[i].transitionCount++;
                                        }
                                    }
                                }
                            }
                        }
                    }  
                }
                part=strtok(NULL, "->,[ ");
            }
        }
    }
    fclose(file);
    return aut;
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
        if (aut->initialNode[i].Id) {
            printf("%s ", aut->initialNode[i].Id);
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
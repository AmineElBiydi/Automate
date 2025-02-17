#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h> 
#include <string.h>

typedef struct relation relation;

typedef struct relation{
    node* nextNode;
    char* etiquette;
    int nbrEtiquette;
}relation;

typedef struct node{
    char *Id;
    bool isFinal;   
    relation *transition;
    int transitionCount;
}node;

typedef struct automate{
    node* nodes;
    node** initialNode;
    int  countNode;
    char Nom[100] ;
}automate;


node* CreateNodes(int NbrNodes){
    node* nodes = malloc(sizeof(node)*NbrNodes);
    if(nodes==NULL){
        printf("Erreur: node is Null!\n");
        return NULL ;
    }
    for(int i=0 ; i< NbrNodes ; i++){
        (nodes)[i].isFinal=false;
        (nodes)[i].transition=NULL;
        (nodes)[i].transitionCount=0;
    }
    return nodes;
}

node* ResizeNodes(node* nodee,int newNbrNodes){
    node* tmp=realloc(nodee ,sizeof(node)*newNbrNodes);
    if(tmp == NULL){
        printf("Erreur : reallocated node is null!\n");
        retun NULL;
    }
}

automate* CreateAutomate(){
    automate* automates = malloc(sizeof(automate)) ;
    if (automates == NULL){
        printf ("Erreur : l'Automate is NULL !\n");
        return NULL;
    }
    automates->countNode=0;
    automates->initialNode =NULL;
    automates->nodes = NULL;
    return automates;
}

relation* Relation(node* InitNode,node* FinalNode , char* etiquette){
    int count = 0 ;
    if(InitNode->transition == NULL ){
        InitNode->transition = malloc(sizeof(relation));
        if((InitNode)->transition == NULL){
            printf("Erreur : transition is NULL");
            return NULL;
        }
        InitNode->transitionCount++;
        InitNode->transition->nextNode = FinalNode ;
        InitNode->transition->etiquette = etiquette;
        for(char* word=etiquette ; word!=NULL ; word++){
            count++;
        }
        InitNode->transition->nbrEtiquette = count;
        return InitNode->transition ;
    }
    relation* transition1 = realloc(InitNode->transition , sizeof(relation)*(InitNode->transitionCount+1));
    if(transition1==NULL){
        printf("Erreur : reallocation is NULL !\n");
        return InitNode->transition;
    }
    InitNode->transition = transition1;
    InitNode->transitionCount++;
    InitNode->transition[InitNode->transitionCount-1].nextNode = FinalNode ;
    InitNode->transition[InitNode->transitionCount-1].etiquette = etiquette ;
    for(char* word=etiquette ; word!=NULL ; word++){
        count++;
    }
    InitNode->transition[InitNode->transitionCount-1].nbrEtiquette = count;
    return InitNode->transition;
}
node* findNode(automate* a, const char* id) {
    for (int i = 0; i < a->countNode; i++) {
        if (strcmp(a->nodes[i].Id, id) == 0) {
            return &a->nodes[i];
        }
    }
    return NULL;
}
/*
    ======= li kay9ra mn l fichier ======
*/
automate* ReadFile(char* fileName) {
    FILE* file = fopen(fileName, "r");
    char line[256];
    if (!file) {
        printf("Erreur : Impossible d'ouvrir le fichier !\n");
        return NULL;
    }

    automate* aut = CreateAutomate();
    if (!aut) return NULL;

    node* startNode = NULL; // Nœud initial (shape=point)

    while (fgets(line, sizeof(line), file)) {
        // Gestion des nœuds
        if (strstr(line, "node")) {
            char* token = strtok(line, " [];=,\t\n");
            while (token) {
                if (strcasecmp(token, "node") == 0) {
                    token = strtok(NULL, " [];=,\t\n");
                    if (token && strcasecmp(token, "shape") == 0) {
                        token = strtok(NULL, " [];=,\t\n");
                        if (token) {
                            if (strcasecmp(token, "doublecircle") == 0) {
                                // Final states
                                token = strtok(NULL, " [],;\t\n");
                                while (token) {
                                    node* n = findNode(aut, token);
                                    if (n) n->isFinal = true;
                                    token = strtok(NULL, " [],;\t\n");
                                }
                            } else if (strcasecmp(token, "point") == 0) {
                                // Start node (shape=point)
                                token = strtok(NULL, " [],;\t\n");
                                if (token) {
                                    // Créer le nœud de départ
                                    aut->nodes = realloc(aut->nodes, (aut->countNode + 1) * sizeof(node));
                                    aut->nodes[aut->countNode].Id = strdup(token);
                                    aut->nodes[aut->countNode].isFinal = false;
                                    aut->nodes[aut->countNode].transition = NULL;
                                    aut->nodes[aut->countNode].transitionCount = 0;
                                    startNode = &aut->nodes[aut->countNode];
                                    aut->countNode++;
                                }
                            }
                        }
                    }
                }
                token = strtok(NULL, " [];=,\t\n");
            }
        }

        // Gestion des transitions (edges)
        if (strstr(line, "->")) {
            char* src = strtok(line, " ->\t\n");
            char* arrow = strtok(NULL, " ->\t\n");
            char* dest = strtok(NULL, " [],;\t\n");
            char* label = NULL;

            // Extraire le label
            char* labelPos = strstr(line, "label=");
            if (labelPos) {
                label = strchr(labelPos, '"');
                if (label) {
                    label++;
                    char* endLabel = strchr(label, '"');
                    if (endLabel) *endLabel = '\0';
                }
            }

            node* srcNode = findNode(aut, src);
            node* destNode = findNode(aut, dest);
            if (srcNode && destNode) {
                Relation(srcNode, destNode, label ? label : "ε");
            }

            // Si la source est le nœud de départ (shape=point)
            if (srcNode == startNode) {
                aut->initialNode = realloc(aut->initialNode, sizeof(node*) * (aut->countNode));
                aut->initialNode[aut->countNode - 1] = destNode;
            }
        }
    }

    fclose(file);
    return aut;
}
void PrintAutomate(automate* aut) {
    printf("Nom de l'automate : %s\n", aut->Nom);
    printf("États initiaux : ");
    for (int i = 0; i < aut->countNode; i++) {
        if (aut->initialNode && aut->initialNode[i]) {
            printf("%s ", aut->initialNode[i]->Id);
        }
    }
    printf("\nÉtats finaux : ");
    for (int i = 0; i < aut->countNode; i++) {
        if (aut->nodes[i].isFinal) {
            printf("%s ", aut->nodes[i].Id);
        }
    }
    printf("\nTransitions :\n");
    for (int i = 0; i < aut->countNode; i++) {
        node* n = &aut->nodes[i];
        for (int j = 0; j < n->transitionCount; j++) {
            relation* r = &n->transition[j];
            printf("%s --(%s)--> %s\n", n->Id, r->etiquette, r->nextNode->Id);
        }
    }
}

int main(){
    
}


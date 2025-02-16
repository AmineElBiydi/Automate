#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h> 
#include <string.h>

typedef struct relation relation;

typedef struct node{
    char Id;
    bool isFinal;   
    relation *transition;
    int transitionCount;
}node;

typedef struct relation{
    node* nextNode;
    char* etiquette;
    int nbrEtiquette;
}relation;

typedef struct automate{
    node** initialNode;
    node* nodes;
    int  countNode;
}automate;

int main(){
 
    return 0 ;
}

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
/*
    ======= li kay9ra mn l fichier ======
*/
automate* ReadFile(char* fileName){
    FILE* file=fopen(fileName,"r+");
    char line[50];
    char *token ;
    int nbrNodeAllocated=2;
    if(file==NULL){
        printf("Erreur : erreur l'ors de l'ouvrage de fichier !\n");
        return NULL; 
    }
    automate*automate =CreateAutomate();
    if(automate==NULL){
        return NULL;
    }
    automate->nodes=CreateNodes(nbrNodeAllocated);
    if(automate->nodes==NULL){
        return NULL;
    }
    while(fgets(line,sizeof(line),file)!=NULL){
        token = strtok(line, " [].,!}?= ->{");
        while(token!=NULL){
            if(strcasecmp(token,"digraph")||strcasecmp(token,"rankdir")){
                break;
            }
            else if(strcasecmp(token,"node")){
                token = strtok(NULL," [].,!?=->\n");
                if(strcasecmp(token,"shape")){
                    token = strtok(NULL," [].,!?= ->\n");
                    if(strcasecmp(token,"point")){
                        break;
                    }
                    else if(strcasecmp(token,"circle")){
                        token = strtok(NULL," [].,!?= ->\n");
                        while(token!=NULL){
                            if(automate->countNode < nbrNodeAllocated){
                                automate->nodes[automate->countNode].Id=token;
                                automate->countNode++;
                            }
                            else{
                                
                            }
                            
                        }
                    }
                }                    
            }

        }
    }
}
/*
    ======= menu =====================
*/  




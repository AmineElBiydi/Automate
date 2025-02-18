#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h> 
#include <string.h>

typedef struct relation relation;
typedef struct node{
    char Id[50];
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
    int  countInitialNode;
}automate;

automate* ReadFile(char* fileName);

int main(){
 
    automate* automates = ReadFile("test1.dot");
    printf("countNode : %s\n",automates->initialNode[0]->Id);    
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
        return NULL;
    }
    return tmp;
}

automate* CreateAutomate(){
    automate* automates = malloc(sizeof(automate)) ;
    if (automates == NULL){
        printf ("Erreur : l'Automate is NULL !\n");
        return NULL;
    }
    automates->countNode=0;
    automates->countInitialNode=0;
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
        for(char* word=etiquette ;  *word != '\0' ; word++){
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
    for(char* word=etiquette ;  *word != '\0' ; word++){
        count++;
    }
    InitNode->transition[InitNode->transitionCount-1].nbrEtiquette = count;
    return InitNode->transition;
}
/*
    ======= lecture de fichier ======
*/
automate* ReadFile(char* fileName){
    FILE* file=fopen(fileName,"r+");
    char line[50];
    char *token ;
    int nbrNodeAllocated=2;
    automate*automate =CreateAutomate();
    if(file==NULL){
        printf("Erreur : erreur l'ors de l'ouvrage de fichier !\n");
        return NULL; 
    }
    if(automate==NULL){
        return NULL;
    }
    automate->nodes=CreateNodes(nbrNodeAllocated);
    if(automate->nodes==NULL){
        return NULL;
    }
    while(fgets(line,sizeof(line),file)!=NULL){
        token = strtok(line, " [].,!}?= ->{\n");
        while(token!=NULL){
            if(strcasecmp(token,"digraph")==0||strcasecmp(token,"rankdir")==0){
                break;
            }
            else if((strcasecmp(token,"node"))==0){
                token = strtok(NULL," [].,!?=->\n");
                if(strcasecmp(token,"shape")==0){
                    token = strtok(NULL," ;[].,!?= ->\n");
                    if(strcasecmp(token,"point")==0){
                        break;
                    }
                    else if(strcasecmp(token,"circle")==0){
                        token = strtok(NULL," ;[].,!?= ->\n");
                        while(token!=NULL){
                            if(automate->countNode < nbrNodeAllocated){
                                strcpy((automate->nodes[automate->countNode].Id),token);
                                automate->countNode++;
                            }
                            else{
                                nbrNodeAllocated*=2;
                                automate->nodes=ResizeNodes(automate->nodes,nbrNodeAllocated);
                                strcpy((automate->nodes[automate->countNode].Id),token);
                                automate->countNode++;    
                            }  
                            token = strtok(NULL," ;[].,!?= ->\n");
                        }
                        break;
                    }else if(strcasecmp(token,"doublecircle")==0){
                        token = strtok(NULL," ;[].,!?= ->\n");
                        while(token!=NULL){
                            for(int i=0 ; i<automate->countNode ; i++){
                                if(strcasecmp(token,automate->nodes[i].Id)==0){
                                    automate->nodes[i].isFinal=true;
                                }
                            }
                            token = strtok(NULL," ;[].,!?= ->\n");                        
                        }
                        break;  
                    }
                }
            }else if(strcasecmp(token,"start")==0){
                nbrNodeAllocated=2;
                automate->initialNode=malloc(sizeof(node*)*nbrNodeAllocated);
                if (automate->initialNode == NULL) {
                    printf("Erreur : allocation de mémoire pour initialNode échouée !\n");
                    return NULL;
                }
                (token = strtok(NULL," ;[].,!}?= ->\n"));
                while(token!=NULL){
                    for(int i=0 ; i<automate->countNode ; i++){
                        if(strcasecmp(token,automate->nodes[i].Id)==0){
                            if(automate->countInitialNode < nbrNodeAllocated){
                                automate->initialNode[automate->countInitialNode]=&automate->nodes[i];
                                automate->countInitialNode++;

                            }else{
                                node ** tmp;
                                nbrNodeAllocated+=2;
                                tmp=realloc(automate->initialNode,sizeof(node*)*nbrNodeAllocated);
                                if(tmp==NULL){
                                    printf ("Erreur : l'ors de la reallocation de node initiales!!\n");
                                    return NULL;
                                }
                                automate->initialNode=tmp;
                                automate->initialNode[automate->countInitialNode]=automate->nodes+i;
                                automate->countInitialNode++;
                            }
                        }
                    }
                    token = strtok(NULL,"[].,!{}}?=; ->\n");
                }
                break;
            }else{
                printf ("ccccccccc\n");
                for(int i=0 ; i<automate->countNode ; i++){
                    if(strcasecmp(token,automate->nodes[i].Id)==0){
                        char* Ids[50];
                        char* etiquettes[50] ;
                        int nbrID=0;
                        int nbrEtiquette=0;
                        token = strtok(NULL," [].,!}?= ->\n");
                        while(!(strcasecmp(token,"label")==0)){  
                            Ids[nbrID]=token;
                            nbrID++;
                            token = strtok(NULL," [].,!}?= ->\n");
                        }
                        token = strtok(NULL," [].,!}?= ->\n");
                        while(token!=NULL){
                            etiquettes[nbrEtiquette]=token;
                            nbrEtiquette++;
                            token = strtok(NULL," \"[].,!}?= ->\n");
                        }
                        automate->nodes[i].transition=malloc(sizeof(relation)*nbrID);
                        for(int j=0 ; j<nbrID ; j++){
                            for(int k=0 ; k<automate->countNode ; k++){
                                if(strcasecmp(Ids[j],automate->nodes[k].Id)){
                                    automate->nodes[i].transition[j].nextNode=&automate->nodes[k];
                                    for(int l=0 ; l<nbrEtiquette ; l++){
                                        automate->nodes[i].transition[j].etiquette=malloc(sizeof(char)*nbrEtiquette);
                                        for(int m=0 ; m<nbrEtiquette ; m++){
                                            automate->nodes[i].transition[j].etiquette[m]=*etiquettes[m];
                                        }
                                        automate->nodes[i].transition[j].nbrEtiquette=nbrEtiquette;
                                    }
                                }
                            }
                        }
                    }
                }

            }
            token = strtok(line, " [].,!}?= ->{");
        }
    
    }
    printf("line : %s\n",line);

    return automate;
    fclose(file);
}

/*
======= menu =====================
*/  
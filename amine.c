#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

int NbrAllocated = 5 ;
typedef struct relation relation;
typedef struct node {
    char *Id;
    bool isFinal;
    relation *transition;
    int transitionCount;
} node ;
typedef struct relation {
    node* nextNode;
    char* etiquette;
    int nbrEtiquette;
} relation ;
typedef struct automate {
    int nbrAlphabet;
    char* Alphabet;
    node** initialNode;
    node** finalNode;
    node* nodes;
    int countNode;
    int countInitialNode;
    int countfinalNode;
    char Nom[100];
} automate ;

//==============verification de l existance de la node and de l etiquette========  
node* isNodeExiste(automate* aut, char* nodeID){
    for (int i =0 ; i < aut->countNode ; i++){
        if(strcmp(nodeID,aut->nodes[i].Id)==0){
            return &aut->nodes[i];
        }
    }
    return NULL;
}
bool isEtiquetteFound(relation rela , char etiquette ){
    for (int i = 0 ; i<rela.nbrEtiquette ;i++){
        if (etiquette==rela.etiquette[i]){
            return true;
        }
    }
    return false ;
}

//=============== la creation de l automate est touts leurs elements ================
void CreateEttiquettes(relation *transition ){
    transition->etiquette=malloc(sizeof(char)*NbrAllocated);
    if(transition->etiquette==NULL){
        perror("Erreur : La location de la memoire pour les etiquettes est null !!\n");
    }
    transition->nbrEtiquette=0;
}
void CreateTransitions(node* nd ){
    nd->transition=malloc(sizeof(relation)*NbrAllocated);
    if(nd->transition==NULL){
        perror("Erreur : la location de la memoire pour les transitions est NULL !!\n");
    }
    for (int i = 0 ;i <NbrAllocated ;i++){
        CreateEttiquettes(&nd->transition[i]);
    }
    nd->transition->nextNode=NULL;
    nd->transitionCount = 0;
}
void CreateNodes(node** nodes) {
    *nodes = malloc(sizeof(node) *NbrAllocated); 
    if (*nodes == NULL) {
        perror("Erreur: La location de la memoire pour les nodes is Null!\n");
    }
    for (int i = 0; i < NbrAllocated; i++) {
        ((*nodes)+i)->Id = NULL;
        ((*nodes)+i)->isFinal = false;
        CreateTransitions((*nodes)+i);
    }
}
void CreatePointingNodes(node*** nd){
    *nd = malloc(sizeof(node*) * NbrAllocated); 
    if (*nd == NULL) {
        perror("Erreur: La location de la memoire pour les nodes is Null!\n");
    }
    for (int i = 0; i < NbrAllocated; i++) {
        (*nd)[i] = NULL;
    }
}
automate* CreateAutomate() {
    automate* automates = malloc(sizeof(automate));
    if (automates == NULL) {
        perror("Erreur : l'Automate is NULL !\n");
        return NULL;
    }
    CreateNodes(&automates->nodes);
    CreatePointingNodes(&automates->initialNode);
    CreatePointingNodes(&automates->finalNode);
    automates->countNode = 0;
    automates->countInitialNode = 0;
    automates->countfinalNode=0;
    return automates;
}

//================ le resize de toute les elements d une automate =====================
void ResizeEtiquettes(relation *transition ,int newEtiquetteNbr){
    transition->etiquette = realloc(transition->etiquette,sizeof(char)*newEtiquetteNbr);
    if(transition->etiquette == NULL){
        perror("Erreur : la reallocation de la memoire pour les ettiquettes est null !!\n");
    }
}
void ResizeTransition(node* nd ,int newTransitionNbr ){
    nd->transition = realloc(nd->transition,sizeof(relation)*newTransitionNbr);
    if(nd->transition == NULL){
        perror("Erreur : la reallocation de la memoire pour les transitions est null !!\n");
    }
    for (int i = nd->transitionCount ; i < newTransitionNbr-1 ; i++){
       CreateEttiquettes(&nd->transition[i]); 
    }
}
void ResizeNodes(automate* aut , int newNodesNbr){
    aut->nodes = realloc( aut->nodes , sizeof(node)*newNodesNbr);
    if(aut->initialNode == NULL){
        perror("Erreur : la reallocation de la memoire pour les nodes est NULL !!\n");
    }
    for(int i = aut->countNode ; i<newNodesNbr ; i++){
        aut->nodes[i].Id =NULL;
        aut->nodes[i].isFinal=false;
        CreateTransitions(&aut->nodes[i]);
    }
} 
void ResizePointingNodes(node*** nd, int newNbr) {
    node** tmp = realloc(*nd, sizeof(node*) * newNbr);
    if (tmp == NULL) {
        perror("Erreur: La reallocation de la memoire pour les nodes a échoué!\n");
        exit(EXIT_FAILURE);
    }
    // Initialize new pointers from oldNbr to newNbr to NULL
    for (int i = newNbr-NbrAllocated ; i < newNbr; i++) {
        tmp[i] = NULL;
    }
    *nd = tmp;
}

//================ les relation entre deux nodes ============
relation* isNodehasRelation(node* nodeInit , node* nodeFinal ){
    for(int i =0 ; i < nodeInit->transitionCount ; i++){
        if (strcmp(nodeInit->transition[i].nextNode->Id,nodeFinal->Id)==0){
            return &nodeInit->transition[i];
        }
    }
    return NULL;
}
int Relation(automate* aut, char* InitNode, char* FinalNode, char etiquette) {
    node* tmpInitiale = isNodeExiste(aut, InitNode);
    node* tmpFinal = isNodeExiste(aut, FinalNode);
    if (tmpFinal == NULL || tmpInitiale == NULL) {
        if (tmpFinal == NULL) {
            // the final node not found
            return 2;
        }
        if (tmpInitiale == NULL) {
            // the initial node not found
            return 1;
        }
    }
    if (isNodehasRelation(tmpInitiale,tmpFinal)!=NULL){
        relation* tmpRelation = isNodehasRelation(tmpInitiale,tmpFinal);
        if(isEtiquetteFound(*tmpRelation , etiquette)) return 0;
        if ( tmpRelation->nbrEtiquette % NbrAllocated != 0  ) {
            tmpRelation->etiquette[tmpRelation->nbrEtiquette] = etiquette;
            tmpRelation->nbrEtiquette++;
        } else{
            ResizeEtiquettes(tmpRelation, tmpRelation->nbrEtiquette + NbrAllocated);
            tmpRelation->etiquette[tmpRelation->nbrEtiquette] = etiquette;
            tmpRelation->nbrEtiquette++;
        } 
        return 0 ;
    }
    else if (tmpInitiale->transitionCount % NbrAllocated != 0 || tmpInitiale->transitionCount == 0 ) {
        if (tmpInitiale->transition[tmpInitiale->transitionCount].nbrEtiquette == 0 || tmpInitiale->transition[tmpInitiale->transitionCount].nbrEtiquette % NbrAllocated != 0) {
            tmpInitiale->transition[tmpInitiale->transitionCount].etiquette[tmpInitiale->transition[tmpInitiale->transitionCount].nbrEtiquette] = etiquette;
            tmpInitiale->transition[tmpInitiale->transitionCount].nbrEtiquette++;
            tmpInitiale->transition[tmpInitiale->transitionCount].nextNode = tmpFinal;
        } else {
            ResizeEtiquettes(&tmpInitiale->transition[tmpInitiale->transitionCount], tmpInitiale->transition[tmpInitiale->transitionCount].nbrEtiquette + NbrAllocated);
            tmpInitiale->transition[tmpInitiale->transitionCount].etiquette[tmpInitiale->transition[tmpInitiale->transitionCount].nbrEtiquette] = etiquette;
            tmpInitiale->transition[tmpInitiale->transitionCount].nbrEtiquette++;
            tmpInitiale->transition[tmpInitiale->transitionCount].nextNode = tmpFinal;
        }
        tmpInitiale->transitionCount++;
        return 0 ;

    } else {
        ResizeTransition(tmpInitiale, tmpInitiale->transitionCount + NbrAllocated);
        tmpInitiale->transition[tmpInitiale->transitionCount].etiquette[tmpInitiale->transition[tmpInitiale->transitionCount].nbrEtiquette] = etiquette;
        tmpInitiale->transition[tmpInitiale->transitionCount].nbrEtiquette++;
        tmpInitiale->transition[tmpInitiale->transitionCount].nextNode = tmpFinal;
        tmpInitiale->transitionCount++;
        return 0;
    }
    return 0 ;
}

//================ creating a node in the automate
void AddNodeToAutomate(automate* aut,char* nodeID ){
    if( aut->countNode == 0 || aut->countNode % NbrAllocated !=0) {
        aut->nodes[aut->countNode].Id = strdup(nodeID);

    }else if(aut->countNode % NbrAllocated ==0){
        ResizeNodes(aut ,aut->countNode + NbrAllocated );
        aut->nodes[aut->countNode].Id = strdup(nodeID);
    }
    aut->countNode++;
}
 
//=============== saving the name of the start and the end of the node temporerly (help us to shimatiser le graphe whit .dot)=========
void SaveTmpNodes(char*** tableOfNodesName , int* theNbrOfElement ,char* nodeName ){
    if(*theNbrOfElement == 0 ){
        *tableOfNodesName = malloc(sizeof(char*) * NbrAllocated);
        if(*tableOfNodesName == NULL){
            perror("Erreur lors de l'alocation de la memoire !!\n");
        }
        (*tableOfNodesName)[0] = strdup(nodeName);
       (*theNbrOfElement)++;
    } else if(*theNbrOfElement % NbrAllocated != 0){
        (*tableOfNodesName)[*theNbrOfElement] = strdup(nodeName);
        (*theNbrOfElement)++;
    } else {
*tableOfNodesName = realloc(*tableOfNodesName, sizeof(char*) * (*theNbrOfElement + NbrAllocated));
        if(*tableOfNodesName == NULL){
            perror("Erreur : lors de la reallocation de la memoire !!\n");
        }
        (*tableOfNodesName)[*theNbrOfElement] = strdup(nodeName);
        (*theNbrOfElement)++;
    }
}

//=============== set a node a final or initiale one=====================
int setNodeFinal(automate* aut ,char* idNode){
    node* nd=isNodeExiste(aut,idNode);
    if (nd==NULL){
        printf("Erreur node \"%s\"not found !!\n",idNode);
    //   si la node nexiste pas il doit le cree first
        return 0;
    }
    if(nd->isFinal){
        return 0;
    }
    if(aut->countfinalNode == 0 || aut->countfinalNode % NbrAllocated !=0){
        aut->finalNode[aut->countfinalNode]=nd;    
        nd->isFinal=true;
        aut->countfinalNode++;

    }else {
        ResizePointingNodes(&aut->finalNode , aut->countfinalNode + NbrAllocated );
        aut->finalNode[aut->countfinalNode]=nd;
        nd->isFinal=true;
        aut->countfinalNode++;    
    }
    return 1 ; 
}
int setNodeInitial(automate* aut ,char* idNode){
    node* nd=isNodeExiste(aut,idNode);
    if (nd==NULL){
        printf("Erreur node \"%s\"not found !!\n",idNode);
    //   si la node nexiste pas il doit le cree first
        return 0;
    }
    if(aut->countInitialNode == 0 || aut->countInitialNode % NbrAllocated !=0){
        aut->initialNode[aut->countInitialNode]=nd;
    }else {
        ResizePointingNodes(&aut->initialNode , aut->countInitialNode+NbrAllocated );
        aut->initialNode[aut->countInitialNode]=nd;
    }
    aut->countInitialNode++;
    return 1 ; 
}
char* RemoveSpace(char* str) {
    char *src = str, *dst = str;
    while (*src!='\0') {
        if (!isspace(*src)) {
            *dst++ = *src;
        }
        src++;
    }
    *dst = '\0';
    return str;
}

automate* ReadFile(char* fileName) {
    FILE *file ;
    file = fopen(fileName , "r");
    if (!file) {
        printf("Erreur : impossible d'ouvrir le fichier %s\n", fileName);
        exit(EXIT_FAILURE);
    }
    automate* aut = CreateAutomate();
    if (!aut) {   
        fclose(file);
        return NULL;
    }
    int isFound;
    char line[200];
    char* ends,*etiquettes, *tmp, *start;
    char** initNames = NULL, **finalName = NULL;
    int initNodeCount = 0, finalNodeCount = 0, relationResult;
    while (fgets(line, sizeof(line), file) != NULL) {
        line[strcspn(line, "\n")] = '\0';
        if(strlen(line) == 0){  
            continue;
        }
        char *part = strtok(line, "{};");
        if (part == NULL){
            continue;
        }
        if (strstr(part, "digraph") != NULL) {
        // hna khas nakhed l fnct li dar ayman hit hia hsen mn hadi 
            part = strtok(part, " ");
            part = strtok(NULL, " ");
            if (part) {
                strcpy(aut->Nom, part);
            }
            continue;
        } else if (strstr(part, "rankdir") != NULL){
            continue;
        } else if (strstr(part, "shape") != NULL) {
            if (strstr(part, "point") != NULL) {
                part = strstr(part, "]");
                part = RemoveSpace(part);
                part++;
                part = strtok(part, ",");
                if(strstr(part, "start") != NULL){
                    while (part != NULL) {
                        SaveTmpNodes(&initNames, &initNodeCount, part);
                        part = strtok(NULL, ",");
                    } 
                } else if (strstr(part, "final") != NULL){
                    while (part != NULL) {
                       SaveTmpNodes(&finalName, &finalNodeCount, part);
                        part = strtok(NULL, ",");
                    }       
                }
            } else if (strstr(part, "circle") != NULL) {
                part = RemoveSpace(part);
                part = strstr(part, "]");
                part++;
                part = strtok(part, ",");
                while (part != NULL) {
                    AddNodeToAutomate(aut, part);
                    part = strtok(NULL, ",");                    
               }        
            }
        }else if (strstr(part,"->")!=NULL) {
            isFound = 0 ;   
            part=RemoveSpace(part); 
            tmp =strdup(part);
            part=strtok(part,"[");
            //============== momkin tkon hsen mn haka ndir chi fnct tjm3 l final w l initial (wlkin hta haka mzn) ===
            for(int i=0;i<finalNodeCount;i++){
                if(strstr(part,finalName[i])!=NULL){
                    isFound=1;
                    part=strtok(part,"->");
                    part=strtok(part,",");
                    while (part!=NULL)
                    {
                        setNodeFinal(aut , part);
                        part=strtok(NULL,",");
                    }
                    break;
                }
            }
            if(isFound == 0){
                for(int i = 0; i < initNodeCount; i++){
                    if(strstr(part, initNames[i]) != NULL){
                        isFound = 1;
                        part = strstr(part, "->") + 2;
                        part = strtok(part, ",");
                        while (part != NULL) {
                            setNodeInitial(aut, part);                            
                            part = strtok(NULL, ",");
                        }
                        break;
                    }
                }
            }
            if(isFound == 1){
                continue;
            }
            part=strtok(part,"->");
            start = strdup(part);
            part=strtok(NULL,"->");
            ends = strdup(part);
            ends = strtok(ends, ",");
            tmp=strtok(tmp,"]");
            tmp=strtok(tmp,"\"");
            tmp=strtok(NULL,"\"");
            etiquettes = strdup(tmp);
            etiquettes = strtok(etiquettes, ",");
            while(ends != NULL){
                while (etiquettes != NULL) {
                    relationResult = Relation(aut, start, ends, etiquettes[0]);
                    etiquettes = strtok(NULL, ",");
                }
                ends = strtok(NULL, ",");
            }
        } else {
            continue;
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
    printf("etats : ");
    for (int i = 0; i < aut->countNode; i++) {
        printf("%s ", aut->nodes[i].Id);
    }
    printf("\n");
    printf("etats initiaux : ");
    for (int i = 0; i < aut->countInitialNode; i++) {
        printf("%s ",aut->initialNode[i]->Id);
    }
    printf("\n");
    printf("etats finaux : ");
    for (int i = 0; i < aut->countfinalNode; i++) {
        printf("%s ",aut->finalNode[i]->Id);
        
    }
    printf("\n");
    printf("Transitions :\n");
    for (int i = 0; i < aut->countNode; i++) {
            for(int j=0;j<aut->nodes[i].transitionCount ; j++){
                printf("%s --( ",aut->nodes[i].Id );
                for(int k=0 ; k<aut->nodes[i].transition[j].nbrEtiquette-1 ;k++){
                    printf("%c - ",aut->nodes[i].transition[j].etiquette[k]);
                }
                printf("%c )--> %s\n",aut->nodes[i].transition[j].etiquette[aut->nodes[i].transition[j].nbrEtiquette-1],aut->nodes[i].transition[j].nextNode->Id);
            }
    }
}




int main() {
    automate* automates = ReadFile("test.dot");
    if (automates != NULL) {
        PrintAutomate(automates);
    }
    return 0;
}
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
    node** initialNode;
    node** finalNode;
    node* nodes;
    int countNode;
    int countInitialNode;
    int counFinalnode;
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
    *nd = malloc(sizeof(node) *NbrAllocated); 
    if (**nd == NULL) {
        perror("Erreur: La location de la memoire pour les nodes is Null!\n");
    }
    for (int i = 0; i < NbrAllocated; i++) {
        (*nd)[i]= NULL;
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
    automates->counFinalnode=0;
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
    for (int i = 0 ; i < NbrAllocated ; i++){
       CreateEttiquettes(&nd->transition[nd->transitionCount + i]); 
    }
}
void ResizeNodes(automate* aut , int newNodesNbr){
    aut->nodes = realloc( aut->nodes , sizeof(node*)*newNodesNbr);
    if(aut->initialNode == NULL){
        perror("Erreur : la reallocation de la memoire pour les nodes est NULL !!\n");
    }
    for(int i = newNodesNbr-1 ; i>newNodesNbr-NbrAllocated-1 ; i--){
        aut->nodes[i].Id =NULL;
        aut->nodes[i].isFinal=false;
        CreateTransitions(&aut->nodes[i]);
    }
} 
void ResizePointingNodes(node*** nd , int newNbr){
    *nd = realloc(nd,sizeof(node) *newNbr); 
    if (**nd == NULL) {
        perror("Erreur: La location de la memoire pour les nodes is Null!\n");
    }
    for (int i = newNbr-6; i < newNbr-1; i++) {
        *nd[i] = NULL;
    }
}

//================ les relation entre deux nodes ============
int Relation(automate* aut,char* InitNode, char* FinalNode, char etiquette) {
    node* tmpInitiale = isNodeExiste(aut,InitNode);
    node* tmpFinal =isNodeExiste(aut,FinalNode);
    if(tmpFinal==NULL || tmpInitiale==NULL){
        if(tmpFinal==NULL){
            // the final node not found
            return 2;
        }
        if(tmpInitiale==NULL){
            // the initial node not found
            return 1;
        }
    }
    if(tmpInitiale->transitionCount % NbrAllocated !=0 || tmpInitiale->transitionCount==0){
        if(tmpInitiale->transition[tmpInitiale->transitionCount].nbrEtiquette == 0 || tmpInitiale->transition[tmpInitiale->transitionCount].nbrEtiquette% NbrAllocated != 0){
            tmpInitiale->transition[tmpInitiale->transitionCount].etiquette[tmpInitiale->transition[tmpInitiale->transitionCount].nbrEtiquette] = etiquette;
            tmpInitiale->transition[tmpInitiale->transitionCount].nbrEtiquette++;
            tmpInitiale->transition[tmpInitiale->transitionCount].nextNode=tmpFinal;
        }
        else{
            ResizeEtiquettes(&tmpInitiale->transition[tmpInitiale->transitionCount], tmpInitiale->transition[tmpInitiale->transitionCount].nbrEtiquette+NbrAllocated);
            tmpInitiale->transition[tmpInitiale->transitionCount].etiquette[tmpInitiale->transition[tmpInitiale->transitionCount].nbrEtiquette] = etiquette;
            tmpInitiale->transition[tmpInitiale->transitionCount].nbrEtiquette++;
            tmpInitiale->transition[tmpInitiale->transitionCount].nextNode=tmpFinal;
        }
    }else{
        ResizeTransition(tmpInitiale,tmpInitiale->transitionCount+NbrAllocated);
        tmpInitiale->transition[tmpInitiale->transitionCount].etiquette[tmpInitiale->transition[tmpInitiale->transitionCount].nbrEtiquette] = etiquette;
        tmpInitiale->transition[tmpInitiale->transitionCount].nbrEtiquette++;
        tmpInitiale->transition[tmpInitiale->transitionCount].nextNode=tmpFinal;
    }
    tmpInitiale->transitionCount++;

    return 0;
}

//================ creating a node in the automate
void AddNodeToAutomate(automate* aut,char* nodeID ){
    if(aut->countNode==0 || aut->countNode % NbrAllocated !=0){
        aut->nodes[aut->countNode].Id = strdup(nodeID);
        aut->countNode++;
    }else{
        ResizeNodes(aut ,aut->countNode + NbrAllocated );
        aut->nodes[aut->countNode].Id = strdup(nodeID);
        aut->countNode++;
    }
}

//=============== saving the name of the start and the end of the node temporerly (help us to shimatiser le graphe whit .dot)=========
void SaveTmpNodes(char*** tableOfNodesName , int* theNbrOfElement ,char* nodeName ){
    if(theNbrOfElement == 0 ){
        *tableOfNodesName = malloc(sizeof(char)*NbrAllocated);
        if(**tableOfNodesName ==NULL){
            perror("Erreur lors de l'alocation de la memoire !!\n");
        }
        *tableOfNodesName[0]=strdup(nodeName);
       ( *theNbrOfElement)++;
    }else if(*theNbrOfElement % NbrAllocated != 0){
        *tableOfNodesName[*theNbrOfElement]=strdup(nodeName);
        (*theNbrOfElement)++;
    }else{
        *tableOfNodesName = realloc(tableOfNodesName ,sizeof(char)*NbrAllocated+(*theNbrOfElement));
        if(**tableOfNodesName ==NULL){
            perror("Erreur : lors de la reallocation de la memoire !!\n");
        }
        *tableOfNodesName[*theNbrOfElement]=strdup(nodeName);
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
    if(aut->counFinalnode == 0 || aut->counFinalnode % NbrAllocated !=0){
        aut->finalNode[aut->counFinalnode]=nd;
    }else {
        ResizePointingNodes(&aut->finalNode , aut->counFinalnode+NbrAllocated );
        aut->finalNode[aut->counFinalnode]=nd;
    }
    aut->counFinalnode++;
    nd->isFinal=true;
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
    char line[200];
    char* ends,*etiquettes, *tmp, *start;
    char** initNames =NULL,** finalName=NULL;
    int initNodeCount =0, finalNodeCount =0, relationResult ;
    while (!feof(file)) {
        printf ("here me!!\n");
        fgets(line, sizeof(line), file);
        printf ("here me!!\n");
        if(strlen(line) == 0){  
            continue;
        }
        char *part = strtok(line, "{};");
        if (part==NULL){
            continue;
        }
        if (strstr(part, "digraph") != NULL) {
            // hna khas nakhed l fnct li dar ayman hit hia hsen mn hadi 
            part = strtok(part, " ");
            part = strtok(NULL, " ");
            if (part) {
                strcpy(aut->Nom, part);
            }
            printf("diagraphe\n");
            continue;
        }
        else if (strstr(part, "rankdir") != NULL){
            printf("rankdir\n");         
            continue;
        }
        else if (strstr(part, "shape") != NULL) {
            printf("shape\n");
            if (strstr(part, "point") != NULL) {
                part = strstr(part,"]");
                part ++;
                if(strstr(part,"start")!=NULL){
                    part=RemoveSpace(part);
                    part=strtok(part,",");
                    while (part!=NULL)
                    {
                       SaveTmpNodes(&initNames , &initNodeCount , part);
                        part = strtok(NULL, ",");
                    } 
                }else if (strstr(part,"final")!=NULL){
                    part=RemoveSpace(part);
                    part=strtok(part,",");
                    while (part!=NULL)
                    {
                       SaveTmpNodes(&finalName,&finalNodeCount , part);
                        part = strtok(NULL, ",");
                    }       
                }
            }
            else if (strstr(part, "circle") == 0) {
                part=RemoveSpace(part);
                part = strstr(part, "]");
                part++;
                part=strtok(part,",");
               while (part!=NULL)
               {
                    AddNodeToAutomate(aut, part);
                    part = strtok(NULL, ",");
               }        
            }
        }else if (strstr(part,"->")!=NULL) {   
            printf ("->\n");         
            int isFound=0;
            part=RemoveSpace(part);
            //============== momkin tkon hsen mn haka ndir chi fnct tjm3 l final w l initial (wlkin hta haka mzn) ===
            for(int i=0;i<finalNodeCount;i++){
                if(strstr(part,finalName[i])){
                    isFound=1;
                    part=strtok(part,"->");
                    part=strtok(part,",");
                    while (part!=NULL)
                    {
                        setNodeFinal(aut , part);
                        part=strtok(NULL,",");
                    }
                    continue;
                }
            }
            for(int i=0;i<initNodeCount;i++){
                if(strstr(part,initNames[i])){
                    isFound=1;
                    part=strstr(part,"->")+2;
                    part=strtok(part,",");
                    while (part!=NULL)
                    {
                        setNodeFinal(aut , part);
                        part=strtok(NULL,",");
                    }
                    continue;
                }
            }
            if(isFound==1){
                continue;
            }
            //===========================================
            start=strdup(part);
            ends=strdup(strstr(part,"->")+2);
            etiquettes=strdup(strstr(part,"\"")+1);
            part=strtok(part,"->");
            if(part==NULL){
                perror("Erreur : lors de la lecture des relations !!");
                continue;
            }    
            start=strtok(start,"->");
            ends=strtok(ends,",[");
            etiquettes=strtok(etiquettes,",\"");
            while(ends!=NULL){
                while (etiquettes!=NULL)
                {
                    relationResult=Relation(aut,start,ends,etiquettes[0]);
                    if(relationResult==1){
                        printf ("Erreur : the node : |%s| don't existe !!\n",ends);
                    }else if(relationResult==2){
                        printf ("Erreur : the node : |%s| don't existe !!\n",ends);
                    }
                    etiquettes=strtok(NULL,",\"");
                }
                ends=strtok(NULL,",[");
            }
        }else {
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
    for (int i = 0; i < aut->countNode; i++) {
        if (aut->nodes[i].isFinal) {
            printf("%s ", aut->nodes[i].Id);
        }
    }
    printf("\n");
    printf("Transitions :\n");
    for (int i = 0; i < aut->countNode; i++) {
        for (int j = 0; j < aut->nodes[i].transitionCount; j++) {
            printf("%s --( ",aut->nodes[i].Id);
                for(int n =0 ;n<aut->nodes[i].transition[j].nbrEtiquette;n++ ){
                    printf("%c, ",aut->nodes[i].transition[j].etiquette[n]);
                }
            printf(")---->%s\n",aut->nodes[i].transition[j].nextNode->Id);
        }
    }
}

int main() {
    printf("Hello, World!\n");
    automate* automates = ReadFile("test.dot");
    printf("Hello, World!\n");
    if (automates != NULL) {
        PrintAutomate(automates);
    }
    return 0;
}
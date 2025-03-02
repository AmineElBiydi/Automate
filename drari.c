#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>


int etatCounter = 0;
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
    automates->nbrAlphabet=0;
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
        perror("Erreur: La reallocation de la memoire pour les nodes a �chou�!\n");
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
void AddAlphabet(automate* aut , char alphabet ){
    for(int i=0 ; i<aut->nbrAlphabet ; i++){
        if (aut->Alphabet[i] == alphabet){
            return ;
        }
    }
    if(aut->nbrAlphabet == 0){
        aut->Alphabet=malloc( NbrAllocated);
        if (aut->Alphabet ==NULL){
            perror("Erreur\n");
            exit(EXIT_FAILURE);
        }
        aut->Alphabet[aut->nbrAlphabet]=alphabet;
        aut->nbrAlphabet++;
    }else if(aut->nbrAlphabet == strlen(aut->Alphabet)){
        aut->Alphabet=realloc(aut->Alphabet , aut->nbrAlphabet+NbrAllocated);
        if (aut->Alphabet ==NULL){
            perror("Erreur\n");
            exit(EXIT_FAILURE);
        }
        aut->Alphabet[aut->nbrAlphabet]=alphabet;
        aut->nbrAlphabet++;
    }else {
        aut->Alphabet[aut->nbrAlphabet]=alphabet;
        aut->nbrAlphabet++;
    }
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
    AddAlphabet(aut,etiquette);
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
void addFinalNode(automate* aut, node* n) {
    aut->finalNode = realloc(aut->finalNode, (aut->countfinalNode + 1) * sizeof(node*));
    if (!aut->finalNode) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }
    aut->finalNode[aut->countfinalNode++] = n;
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

//================ saving the automaton from file or user inpute==============
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
            char* temp = strstr(line, "digraph");
            if (temp) {
                sscanf(temp, "digraph %s", aut->Nom);
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
automate* EnterAutomate() {
    automate* aut = CreateAutomate();

    if (!aut) return NULL;
    int tmpcount ;

    printf("Nom de l'automate : ");
    scanf("%s", aut->Nom);
    printf("Entrer le nombre d'alphabet : ");
    scanf("%d", &aut->nbrAlphabet);
    aut->Alphabet = malloc(aut->nbrAlphabet * sizeof(char));
    if (!aut->Alphabet) {
        printf("Erreur d'allocation memoire !\n");
        freeAutomate(aut);
        return NULL;
    }
    for (int i = 0; i < aut->nbrAlphabet; i++) {
        printf("Entrer l'alphabet %d : ", i + 1);
        scanf(" %c", &aut->Alphabet[i]);
    }
    printf("\n\nNombre d'etats : ");
    scanf("%d", &tmpcount);
    ResizeNodes(aut,tmpcount);
    aut->countNode=tmpcount;
    if (!aut->nodes) {
        free(aut);
        return NULL;
    }

    int k = 0;
    aut->initialNode = malloc(aut->countNode * sizeof(node*));

    if (!aut->initialNode) {
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
        printf("\n\nNom du noeud %d : ", i + 1);
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
        if (aut->nodes[i].isFinal) {
            addFinalNode(aut, &aut->nodes[i]);
        }
    }
    aut->countInitialNode = k;

    printf("\n\n\nPassant aux transitions :\n");
    int choice1;
    for (int i = 0; i < aut->countNode; i++) {
        printf(" %s a des transitions ? (1=Oui, 0=Non) : ", aut->nodes[i].Id);
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
                char label;
                bool validLabel;
                printf("Donner le nombre des etiquettes : ");
                scanf("%d",&aut->nodes[i].transition[j].nbrEtiquette);
                fgetchar();
                aut->nodes[i].transition[j].etiquette = malloc(aut->nodes[i].transition[j].nbrEtiquette);
                if (!aut->nodes[i].transition[j].etiquette){
                    printf("Erreur : etiquettes not allocated  !!\n");
                    freeAutomate(aut);
                    return NULL;
                }
                for(int k =0 ; k < aut->nodes[i].transition[j].nbrEtiquette ;k++){
                    do {
                        validLabel = false;
                        printf("Avec l'etiquette : ");
                        scanf("%c", &label);
                        fgetchar();
                        for (int p = 0; p < aut->nbrAlphabet; p++) {
                            if (label == aut->Alphabet[p]) {
                                validLabel = true;
                                break;
                            }
                        }
                        if (!validLabel) {
                            printf("Erreur : L'etiquette n'est pas dans l'alphabet !\n");
                        }
                    } while (!validLabel);
                    aut->nodes[i].transition[j].etiquette[k] = label;
                }
            }
        }
    }

    return aut;
}

//========== generate a autmatone file .dot ====================== 
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
    fprintf(file,"rankdir=LR;\n");
    fprintf(file, "node[shape=point, width=0]");
    for(int i=0;i<aut->countInitialNode;i++){
        if(i<aut->countInitialNode-1)
            fprintf(file, "start%d,",i);
        else{
        fprintf(file, "start%d;\n",i);
        }
    }
    fprintf(file, "node[shape=point, width=0]");
    for(int l=0;l<aut->countfinalNode;l++){
        if(l<aut->countfinalNode-1)
            fprintf(file, "final%d,",l);
        else{
        fprintf(file, "final%d;\n",l);
        }
    }
    
    fprintf(file, "node[shape=circle]");
    for (int i = 0; i < aut->countNode; i++) {
        if (i < aut->countNode - 1) {
            fprintf(file, "%s,", aut->nodes[i].Id);
        } else {
            fprintf(file, "%s;\n", aut->nodes[i].Id);
        }
    }

    for (int k = 0; k < aut->countInitialNode; k++) {
        fprintf(file, "start%d->%s;\n",k, aut->initialNode[k]->Id);
    }
    for (int i = 0; i < aut->countfinalNode; i++) {
        fprintf(file, "%s->final%d;\n", aut->finalNode[i]->Id,i); 
    }
    for (int l = 0; l < aut->countNode; l++) {
        for (int m = 0; m < aut->nodes[l].transitionCount; m++) {
            fprintf(file, "%s->%s [label=\"", aut->nodes[l].Id,aut->nodes[l].transition[m].nextNode->Id);
            for(int i=0;i<aut->nodes[l].transition[m].nbrEtiquette-1;i++){
                fprintf(file, " %c ,",aut->nodes[l].transition[m].etiquette[i]);
            }
            fprintf(file, " %c \"];\n",aut->nodes[l].transition[m].etiquette[aut->nodes[l].transition[m].nbrEtiquette-1]);
        }
    }
    fprintf(file, "}\n");
    fclose(file);
    printf("Fichier %s cree avec succes.\n", filename);
}

void EtatPlusTransition(automate *aut) {
    if (aut->countNode == 0) {
        printf("L'automate ne contient aucun n?ud.\n");
        return;
    }

    int maxTransitions = 0;
    for (int i = 0; i < aut->countNode; i++) {
        if (aut->nodes[i].transitionCount > maxTransitions) {
            maxTransitions = aut->nodes[i].transitionCount;
        }
    }

    char** maxTransitionNodes = malloc(aut->countNode * sizeof(char*));
    int countMaxTransitionNodes = 0;
    for (int i = 0; i < aut->countNode; i++) {
        if (aut->nodes[i].transitionCount == maxTransitions) {
            maxTransitionNodes[countMaxTransitionNodes] = aut->nodes[i].Id;
            countMaxTransitionNodes++;
        }
    }

    printf("Les n?uds avec le plus grand nombre de transitions (%d transitions) sont :\n", maxTransitions);
    for (int i = 0; i < countMaxTransitionNodes; i++) {
        printf("%s ", maxTransitionNodes[i]);
    }
    printf("\n");

    free(maxTransitionNodes);
}

void TransitionAlphabet(automate*aut, char* etiquette){
    int i;
    for(i=0;i<aut->countNode;i++){
        for(int j=0;j<aut->nodes[i].transitionCount;j++){
            for(int l=0 ; l<aut->nodes[i].transition[j].nbrEtiquette;l++){
                if(aut->nodes[i].transition[j].etiquette[l]==etiquette[0]){
                    printf("%s-->%s \n",aut->nodes[i].Id,aut->nodes[i].transition[j].nextNode->Id);
                }
            }
        }
    }
}
 
bool motAccepteRecursive(node* noeud, const char* mot, int indice) {
    if (indice == strlen(mot)) {
        return noeud->isFinal;
    }

    for (int i = 0; i < noeud->transitionCount; i++) {
        for(int k=0;k <noeud->transition[i].nbrEtiquette;k++){
            if (noeud->transition[i].etiquette[k] == mot[indice]) {
                if (motAccepteRecursive(noeud->transition[i].nextNode, mot, indice + 1)) {
                    return true;
                }
            }
        }
    }

    return false;
}

bool motAccepte(automate* aut, const char* mot) {
    for (int i = 0; i < aut->countInitialNode; i++) {
        if (motAccepteRecursive(aut->initialNode[i], mot, 0)) {
            return true;
        }
    }
    return false;
}

char** listMotAccepte(automate* aut,char *filename) {
    FILE *file=fopen(filename,"r");
    char str[200];
    if (file == NULL) {
        printf("Aucun fichier trouve?. Le programme de?marrera avec une liste vide.\n");
        return NULL;
    }
    char** motsAcceptes = malloc(100 * sizeof(char*));
    if (!motsAcceptes) {
        perror("erreur d'allocation");
        exit(EXIT_FAILURE);
    }
    int nbMotsAcceptes = 0;
    while ((fgets(str, sizeof(str), file)) != NULL) {
        char* token = strtok(str, " \n");
        while (token != NULL) {
            if (motAccepte(aut, token)) {
                motsAcceptes[nbMotsAcceptes] = strdup(token);
                if (!motsAcceptes[nbMotsAcceptes]) {
                    perror("erreur d'allocation");
                    exit(EXIT_FAILURE);
                }
                nbMotsAcceptes++;
            } 
            token = strtok(NULL, " \n");
        }
    }
    fclose(file);
    return motsAcceptes;
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
    printf ("L'alphabet de l'automate : ");
    for (int i = 0; i < aut->nbrAlphabet-1; i++) {
        printf(" %c -", aut->Alphabet[i]);
    }
    printf(" %c ", aut->Alphabet[aut->nbrAlphabet-1]);
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

void freeAutomate(automate* a) {
    for (int i = 0; i < a->countNode; i++) {
        free(a->nodes[i].Id);
        for (int j = 0; j < a->nodes[i].transitionCount; j++) {
            free(a->nodes[i].transition[j].etiquette);
        }
        free(a->nodes[i].transition);
    }
    free(a->nodes);
    free(a->initialNode);
    free(a->finalNode);
}

//--------------copie les etats et les transition d'une etats a une autre ------
void initUnionAutomate(automate* automa,char*nom,int nbrNodes ,int nbrAlphabet , int nbrInit ){
    automa->Alphabet=malloc(sizeof(char)*nbrAlphabet);
    automa->nodes=malloc(sizeof(node)*(nbrNodes+2));
    automa->initialNode=malloc((sizeof(node*)));
    automa->finalNode=malloc((sizeof(node*)));
    if (automa->Alphabet==NULL || automa->nodes==NULL || automa->finalNode==NULL || automa->initialNode==NULL){
        perror("Erreur lors de la location de la memoire !!\n");
        exit(EXIT_FAILURE);
    }
    strcpy(automa->Nom,nom);
    automa->nodes[0].Id=strdup("Initiale");
    automa->nodes[1].Id=strdup("Finale");
    automa->nodes[0].isFinal=false;
    automa->nodes[1].isFinal=true;
    automa->nodes[0].transitionCount = automa->nodes[1].transitionCount = 0;
    automa->nodes[0].transition=malloc(sizeof(relation)*nbrInit);
    automa->nodes[1].transition=NULL;
    if (automa->nodes[0].transition==NULL ){
        perror("Erreur lors de la location de la memoire !!\n");
        exit(EXIT_FAILURE);
    }
    automa->initialNode[0]=&automa->nodes[0];
    automa->finalNode[0]=&automa->nodes[1];
    automa->countfinalNode =1;
    automa->countInitialNode =1;
    automa->countNode=2;
    automa->nbrAlphabet=0;
}
void initConcatAutomate(automate* automa,char*nom,int nbrNodes ,int nbrAlphabet,int nbrInit ,int nbrFinal,int nbrFinal2){
    automa->Alphabet=malloc(sizeof(char)*nbrAlphabet);
    automa->nodes=malloc(sizeof(node)*(nbrNodes+1));
    automa->initialNode=malloc((sizeof(node*))*nbrInit);
    automa->finalNode=malloc((sizeof(node*))*nbrFinal);
    if (automa->Alphabet==NULL || automa->nodes==NULL || automa->finalNode==NULL || automa->initialNode==NULL){
        perror("Erreur lors de la location de la memoire !!\n");
        exit(EXIT_FAILURE);
    }
    strcpy(automa->Nom,nom);
    automa->nodes[0].Id=strdup("Intermidiaire");
    automa->nodes[0].isFinal=false;
    automa->nodes[0].transitionCount = 0;
    automa->nodes[0].transition=malloc(sizeof(relation)*nbrFinal2);
    // automa->nodes[0].transition=malloc(sizeof(relation)*nbrInit);
    if (automa->nodes[0].transition==NULL ){
        perror("Erreur lors de la location de la memoire !!\n");
        exit(EXIT_FAILURE);
    }
    // automa->initialNode[0]=&automa->nodes[0];
    // automa->finalNode[0]=&automa->nodes[1];
    automa->countfinalNode =0;
    automa->countInitialNode =0;
    automa->countNode=1;
    automa->nbrAlphabet=0;
}

void copieNodesAndAlphabet(automate* autoToCopieTo ,automate autoToCopieFrom ){
    int found=0 ;
    for (int i=0;i<autoToCopieFrom.countNode;i++){
        autoToCopieTo->nodes[autoToCopieTo->countNode].Id=strdup(autoToCopieFrom.nodes[i].Id);
        autoToCopieTo->nodes[autoToCopieTo->countNode].isFinal=autoToCopieFrom.nodes[i].isFinal;
        autoToCopieTo->nodes[autoToCopieTo->countNode].transition=malloc(sizeof(relation)*autoToCopieFrom.nodes[i].transitionCount);
        if (autoToCopieTo->nodes[autoToCopieTo->countNode].transition==NULL){
            perror("Erreur lors de la loctaion de la memoir !!\n");
            exit(EXIT_FAILURE);
        }
        autoToCopieTo->countNode++;
    }
    for (int i = 0; i <autoToCopieFrom.nbrAlphabet ; i++){
        for (int j=0;j<autoToCopieTo->nbrAlphabet;j++){
            if(autoToCopieTo->Alphabet[j]==autoToCopieFrom.Alphabet[i]){
                found=1;
                continue;
            }
        }
        if (found==0){
            autoToCopieTo->Alphabet[autoToCopieTo->nbrAlphabet]=autoToCopieFrom.Alphabet[i];
            autoToCopieTo->nbrAlphabet++;
        }else{
            found=0;
        }
    }
}

void copieTransition(automate* automateToCopieTo ,automate automateToCopieFrom){
    node* initNode ;
    node* nextNode;
    // copie all the nodes and set the finals
    for (int i =0 ; i < automateToCopieFrom.countNode ;i++){
        initNode=isNodeExiste(automateToCopieTo , automateToCopieFrom.nodes[i].Id);
        if (initNode!=NULL){
            if (initNode->isFinal==false){
                int nbr;
                initNode->transitionCount = automateToCopieFrom.nodes[i].transitionCount;
                nbr=initNode->transitionCount;
                initNode->transition=malloc(nbr * sizeof(relation));
                if (initNode->transition==NULL){
                    perror("Erreur lors de la loctaion de la memoir !!\n");
                    exit(EXIT_FAILURE);
                }
                for(int j=0;j< automateToCopieFrom.nodes[i].transitionCount;j++){
                    nextNode=isNodeExiste(automateToCopieTo,automateToCopieFrom.nodes[i].transition[j].nextNode->Id);
                    if(nextNode!=NULL){
                        initNode->transition[j].nextNode=nextNode;
                        initNode->transition[j].etiquette=strdup(automateToCopieFrom.nodes[i].transition[j].etiquette);
                        initNode->transition[j].nbrEtiquette=automateToCopieFrom.nodes[i].transition[j].nbrEtiquette;
                    }else{
                        perror("Erreur lors de la sasuvgardement des relation : etat non trouvee !!\n");
                        exit(EXIT_FAILURE);
                    }
                }
            }else{
                initNode->transition=malloc(sizeof(relation)*(automateToCopieFrom.nodes[i].transitionCount+1));
                initNode->transitionCount=automateToCopieFrom.nodes[i].transitionCount+1;
                if (initNode->transition==NULL){
                    perror("Erreur lors de la loctaion de la memoir !!\n");
                    exit(EXIT_FAILURE);
                }
                for(int j=0;j< automateToCopieFrom.nodes[i].transitionCount;j++){
                    nextNode=isNodeExiste(automateToCopieTo,automateToCopieFrom.nodes[i].transition[j].nextNode->Id);
                    if(nextNode!=NULL){
                        initNode->transition[j].nextNode=nextNode;
                        initNode->transition[j].etiquette=strdup(automateToCopieFrom.nodes[i].transition[j].etiquette);
                        initNode->transition[j].nbrEtiquette=automateToCopieFrom.nodes[i].transition[j].nbrEtiquette;
                    }else{
                        perror("Erreur lors de la sasuvgardement des relation : etat non trouvee !!\n");
                        exit(EXIT_FAILURE);
                    }
                }
                initNode->transition[initNode->transitionCount-1].nextNode=&automateToCopieTo->nodes[1];
                initNode->transition[initNode->transitionCount-1].etiquette=strdup("+");
                initNode->transition[initNode->transitionCount-1].nbrEtiquette=1;
            }
        }else{
            perror("Erreur lors de la sasuvgardement des relation : etat non trouvee !!\n");
            exit(EXIT_FAILURE);
        }
    }
    // SET THE INIT ONES 
    for(int i=0 ; i< automateToCopieFrom.countInitialNode;i++){
       automateToCopieTo->nodes[0].transition[automateToCopieTo->nodes[0].transitionCount].nextNode=isNodeExiste(automateToCopieTo,automateToCopieFrom.initialNode[i]->Id);
       automateToCopieTo->nodes[0].transition[automateToCopieTo->nodes[0].transitionCount].etiquette=strdup("+");
       automateToCopieTo->nodes[0].transition[automateToCopieTo->nodes[0].transitionCount].nbrEtiquette=1;
       automateToCopieTo->nodes[0].transitionCount++;
    }
}
void copieConcatTransition(automate* automateToCopieTo ,automate automateToCopieFrom){
    node* initNode ;
    node* nextNode;
    // copie all the nodes and set the finals
    for (int i =0 ; i < automateToCopieFrom.countNode ;i++){
        initNode=isNodeExiste(automateToCopieTo , automateToCopieFrom.nodes[i].Id);
        if (initNode!=NULL){
                initNode->transitionCount = automateToCopieFrom.nodes[i].transitionCount;
                for(int j=0;j< automateToCopieFrom.nodes[i].transitionCount;j++){
                    nextNode=isNodeExiste(automateToCopieTo,automateToCopieFrom.nodes[i].transition[j].nextNode->Id);
                    if(nextNode!=NULL){
                        initNode->transition[j].nextNode=nextNode;
                        initNode->transition[j].etiquette=strdup(automateToCopieFrom.nodes[i].transition[j].etiquette);
                        initNode->transition[j].nbrEtiquette=automateToCopieFrom.nodes[i].transition[j].nbrEtiquette;
                    }else{
                        perror("Erreur lors de la sasuvgardement des relation : etat non trouvee !!\n");
                        exit(EXIT_FAILURE);
                    }
                }
        }else{
            perror("Erreur lors de la sasuvgardement des relation : etat non trouvee !!\n");
            exit(EXIT_FAILURE);
        }
    }
  
}

//---------- Union de deux Automates-------
automate unionAutomate(automate auto1 , automate auto2){
    automate unionAuto ;
    char nom[100];   
    snprintf(nom, sizeof(nom), "%sUnion%s",auto1.Nom,auto2.Nom);
    initUnionAutomate(&unionAuto,nom,auto1.countNode+auto2.countNode,auto1.nbrAlphabet+auto2.nbrAlphabet,auto1.countInitialNode+auto2.countInitialNode);
    copieNodesAndAlphabet(&unionAuto,auto1);
    copieNodesAndAlphabet(&unionAuto,auto2);
    copieTransition(&unionAuto,auto1);
    copieTransition(&unionAuto,auto2);
    return unionAuto;   
}
automate concatAutomate(automate auto1, automate auto2) {
    automate concatAuto;
    char nom[100];   
    snprintf(nom, sizeof(nom), "%sconcatination%s",auto1.Nom,auto2.Nom);
    initConcatAutomate(&concatAuto, nom, auto1.countNode + auto2.countNode, auto1.nbrAlphabet + auto2.nbrAlphabet, auto1.countInitialNode, auto2.countfinalNode, auto2.countInitialNode);
    copieNodesAndAlphabet(&concatAuto, auto1);
    copieConcatTransition(&concatAuto, auto1);
    copieNodesAndAlphabet(&concatAuto, auto2);
    copieConcatTransition(&concatAuto, auto2);

    
    for (int i = 1; i < concatAuto.countNode; i++) {
        for (int j = 0; j < auto1.countInitialNode; j++) {
            if (strcmp(concatAuto.nodes[i].Id, auto1.initialNode[j]->Id) == 0) {
                concatAuto.initialNode[concatAuto.countInitialNode] = &concatAuto.nodes[i];
                concatAuto.countInitialNode++;
            }
        }
    }

  
    for (int i = 1; i < concatAuto.countNode; i++) {
        for (int j = 0; j < auto1.countfinalNode; j++) {
            if (strcmp(concatAuto.nodes[i].Id, auto1.finalNode[j]->Id) == 0) {
                concatAuto.nodes[i].transition = realloc(concatAuto.nodes[i].transition, (concatAuto.nodes[i].transitionCount + 1) * sizeof(relation));
                if (concatAuto.nodes[i].transition == NULL) {
                    perror("Erreur lors de la reallocation de la memoire pour les transitions");
                    exit(EXIT_FAILURE);
                }
                concatAuto.nodes[i].transition[concatAuto.nodes[i].transitionCount].nextNode = &concatAuto.nodes[0];
                concatAuto.nodes[i].transition[concatAuto.nodes[i].transitionCount].etiquette = strdup("+");
                concatAuto.nodes[i].transition[concatAuto.nodes[i].transitionCount].nbrEtiquette = 1;
                concatAuto.nodes[i].transitionCount++;
            }
        }
    }

    for (int i = 1; i < concatAuto.countNode; i++) {
        for (int j = 0; j < auto2.countInitialNode; j++) {
            if (strcmp(concatAuto.nodes[i].Id, auto2.initialNode[j]->Id) == 0) {
                concatAuto.nodes[0].transition = realloc(concatAuto.nodes[0].transition, (concatAuto.nodes[0].transitionCount + 1) * sizeof(relation));
                if (concatAuto.nodes[0].transition == NULL) {
                    perror("Erreur lors de la reallocation de la memoire pour les transitions");
                    exit(EXIT_FAILURE);
                }
                concatAuto.nodes[0].transition[concatAuto.nodes[0].transitionCount].nextNode = &concatAuto.nodes[i];
                concatAuto.nodes[0].transition[concatAuto.nodes[0].transitionCount].etiquette = strdup("+");
                concatAuto.nodes[0].transition[concatAuto.nodes[0].transitionCount].nbrEtiquette = 1;
                concatAuto.nodes[0].transitionCount++;
            }
        }
    }

    
    for (int i = 1; i < concatAuto.countNode; i++) {
        for (int j = 0; j < auto2.countfinalNode; j++) {
            if (strcmp(concatAuto.nodes[i].Id, auto2.finalNode[j]->Id) == 0) {
                concatAuto.nodes[i].isFinal = true;
                concatAuto.finalNode = realloc(concatAuto.finalNode, (concatAuto.countfinalNode + 1) * sizeof(node*));
                if (concatAuto.finalNode == NULL) {
                    perror("Erreur lors de la reallocation de la memoire pour les noeuds finaux");
                    exit(EXIT_FAILURE);
                }
                concatAuto.finalNode[concatAuto.countfinalNode] = &concatAuto.nodes[i];
                concatAuto.countfinalNode++;
            }
        }
    }

    return concatAuto;
}

/// //////////////////// a+///////////////////////
int stateCounter = 0; // Pour générer des identifiants uniques d'états



// //==================== main function ========================

// int main() {
//     automate* aut = NULL;
//     int choix;
//     char filename[100];
//     char etiquette[10];
//     char mot[100];
//     automate* aut1,*aut2,*aut3,unionAut, concatAut; 
//     char nomFile[100],nomFile2[100] ;

//     do {
        
//         printf("\t                    MENU - AUTOMATES                        \n");
//         printf("\t                                                            \n");
//         printf("\t  [1] Entrer un automate :                                  \n");
//         printf("\t  [2] Lire un automate depuis un fichier .dot :             \n");
//         printf("\t  [3] Afficher l'automate :                                 \n");
//         printf("\t  [4] Generer un fichier .dot :                             \n");
//         printf("\t  [5] Afficher l'etat avec le plus de transitions :         \n");
//         printf("\t  [6] Afficher les transitions pour une etiquette donnee :  \n");
//         printf("\t  [7] vrifier si un mot est acceptable :                    \n");
//         printf("\t  [8] Les mots d'un fichier .txt sont acceptable :          \n");
//         printf("\t  [9] Genere l'union des deux automates :                   \n");
//         printf("\t  [10] Genere la concatAutomate des deux autom               \n");
//         printf("\t  [19] Construire un automate depuis une expression :       \n");
//         printf("\t  [11] Analyser une expression reguliere :                  \n");
//         printf("\t  [12] Quitter :                                            \n");
//         printf("\t                                                           \n");
//         printf("\t\tEntrez votre choix [1-13]: ");
        
//         printf("Choix : ");
//         scanf("%d", &choix);

//         switch (choix) {
//             case 1:
//                 if (aut) {
//                     freeAutomate(aut);
//                 }
//                 aut = EnterAutomate();
//                 break;
//             case 2:
//                 printf("Nom du fichier .dot : ");
//                 scanf("%s", filename);
//                 if (aut) {
//                     freeAutomate(aut);
//                 }
//                 aut = ReadFile(filename);
//                 break;
//             case 3:
//                 if (aut) {
//                     PrintAutomate(aut);
//                 } else {
//                     printf("Aucun automate charge !\n");
//                 }
//                 break;
//             case 4:
//                 if (aut) {
//                     genererficher(aut);
//                 } else {
//                     printf("Aucun automate charge !\n");
//                 }
//                 break;
//             case 5:
//                 if (aut) {
//                     EtatPlusTransition(aut);
//                 } else {
//                     printf("Aucun automate charge !\n");
//                 }
//                 break;
//             case 6:
//                 if (aut) {
//                     printf("Entrer l'etiquette : ");
//                     scanf("%s", etiquette);
//                     TransitionAlphabet(aut, etiquette);
//                 } else {
//                     printf("Aucun automate charge !\n");
//                 }
//                 break;
//                 case 7:
//                     printf("Veuillez entrer le mot : ");
//                     scanf("%s", mot);
                
//                     if (motAccepte(aut, mot)) {
//                         printf("Le mot est accepte !!\n");
//                     } else {
//                         printf("Le mot n'est pas accepte !!\n");
//                     }
//                     break;
//                 case 8:
//                         if(aut){
//                             printf("Veuillez entrer le nom du fichier : ");
//                             scanf("%s",filename);
//                             char** motsAcceptes = listMotAccepte(aut,filename);
//                             if (!motsAcceptes) {
//                                 printf("Aucun mot accepte trouve.\n");
//                             } else {
//                                 printf("%s - ", motsAcceptes[0]);
//                                 printf("Les mots acceptes sont :\n");

//                                 for (size_t i = 0; i < strlen(motsAcceptes) - 1; i++) {
//                                     if (motsAcceptes[i] != NULL) {
//                                         printf("%s - ", motsAcceptes[i]);
//                                         free(motsAcceptes[i]);
//                                     }
//                                 }
//                                 printf("%s \n", motsAcceptes[strlen(motsAcceptes) - 1]);
//                                 free(motsAcceptes[strlen(motsAcceptes) - 1]);
//                                 free(motsAcceptes);
//                             }
//                         }else{
//                             printf("aucune automate charge !!\n");
//                         }
//                     break;
//                 case 9 : 
//                         printf ("Donner le nom de fichier de la premiere automate : ");
//                         scanf("%s",nomFile);
//                         aut1=ReadFile(nomFile);
//                         printf ("Donner le nom de fichier de la dexieme automate : ");
//                         scanf("%s",nomFile2);
//                         aut2=ReadFile(nomFile2);
//                         unionAut=unionAutomate(*aut1,*aut2);
//                         genererficher(&unionAut);
//                         printf("le fichier est genere avec succes : avec le nom %s.dot",unionAut.Nom);
//                 break;
//                 case 10 : 
//                         printf ("Donner le nom de fichier de la premiere automate : ");
//                         scanf("%s",nomFile);
//                         aut1=ReadFile(nomFile);
//                         printf ("Donner le nom de fichier de la dexieme automate : ");
//                         scanf("%s",nomFile2);
//                         aut2=ReadFile(nomFile2);
//                         concatAut=concatAutomate(*aut1,*aut2);
//                         PrintAutomate(&concatAut);
//                         genererficher(&concatAut);
//                         printf("le fichier est genere avec succes : avec le nom %s.dot",concatAut.Nom);
//                         break;
               
//             case 11:
//             printf("Veuillez entrer l'expression reguliere : ");
//             scanf("%s", mot);
//             aut = processExpression(mot);
//             if (aut) {
//                 PrintAutomate(aut);
//                 freeAutomate(aut);
//             } else {
//                 printf("Erreur lors de la construction de l'automate.\n");
//             }
//             break;
//             case 12:
//                 printf("Fin du programme.\n");
//                 break;
//             default:
//                 printf("Option invalide !\n");
//         }
//     } while (choix != 12);

//     if (aut) {
//         freeAutomate(aut);
//     }

//     return 0 ;
// }
 

///////////////////// bach mat3awdch l'etat ///////////////
node* createNode(bool isFinal) {
    node* newNode = (node*)malloc(sizeof(node));
    newNode->Id = (char*)malloc(10 * sizeof(char));
    sprintf(newNode->Id, "q%d", stateCounter++);
    newNode->isFinal = isFinal;
    newNode->transitionCount = 0;
    newNode->transition = NULL;
    return newNode;
}


void addTransition(node* from, node* to, char* symbol) {
    from->transition = (relation*)realloc(from->transition, 
                     (from->transitionCount + 1) * sizeof(relation));
    from->transition[from->transitionCount].nextNode = to;
    from->transition[from->transitionCount].etiquette = strdup(symbol);
    from->transition[from->transitionCount].nbrEtiquette = 1;
    from->transitionCount++;
}


automate* createAutomateFromChar(char c) {
    automate* a = (automate*)malloc(sizeof(automate));

    node* q0 = createNode(false);
    node* q1 = createNode(true);

    addTransition(q0, q1, (char[]){c, '\0'});

    a->initialNode = (node**)malloc(sizeof(node*));
    a->initialNode[0] = q0;
    a->finalNode = (node**)malloc(sizeof(node*));
    a->finalNode[0] = q1;
    a->countInitialNode = 1;
    a->countfinalNode = 1;

    a->nodes = (node*)malloc(2 * sizeof(node));
    a->nodes[0] = *q0;
    a->nodes[1] = *q1;
    a->countNode = 2;

    return a;
}


automate* constructAStar1(char* symbol) {
    automate* automaton = (automate*)malloc(sizeof(automate));

    node* q0 = createNode(false);
    node* q1 = createNode(true);

    addTransition(q0, q1, "+"); 
    addTransition(q1, q1, symbol); 
    addTransition(q0, q0, symbol); 

    automaton->initialNode = (node**)malloc(sizeof(node*));
    automaton->initialNode[0] = q0;
    automaton->finalNode = (node**)malloc(sizeof(node*));
    automaton->finalNode[0] = q1;
    automaton->countInitialNode = 1;
    automaton->countfinalNode = 1;

    automaton->nodes = (node*)malloc(2 * sizeof(node));
    automaton->nodes[0] = *q0;
    automaton->nodes[1] = *q1;
    automaton->countNode = 2;

    return automaton;
}

// Fonction pour construire un automate pour `a+`
automate* construireAutomatePlus(char etiquette) {
    automate* autoPlus = (automate*)malloc(sizeof(automate));

    node* q0 = createNode(false);
    node* q1 = createNode(true);

    addTransition(q0, q1, (char[]){etiquette, '\0'});
    addTransition(q1, q1, (char[]){etiquette, '\0'});

    autoPlus->initialNode = (node**)malloc(sizeof(node*));
    autoPlus->initialNode[0] = q0;
    autoPlus->finalNode = (node**)malloc(sizeof(node*));
    autoPlus->finalNode[0] = q1;
    autoPlus->countInitialNode = 1;
    autoPlus->countfinalNode = 1;

    autoPlus->nodes = (node*)malloc(2 * sizeof(node));
    autoPlus->nodes[0] = *q0;
    autoPlus->nodes[1] = *q1;
    autoPlus->countNode = 2;

    return autoPlus;
}
// Fonction pour fusionner deux automates
automate* unionAutomates(automate* a1, automate* a2) {
    automate* result = (automate*)malloc(sizeof(automate));

    node* newStart = createNode(false);
    addTransition(newStart, a1->initialNode[0], "+");
    addTransition(newStart, a2->initialNode[0], "+");

    result->initialNode = (node**)malloc(sizeof(node*));
    result->initialNode[0] = newStart;
    result->finalNode = (node**)malloc((a1->countfinalNode + a2->countfinalNode) * sizeof(node*));

    result->nodes = (node*)malloc((a1->countNode + a2->countNode + 1) * sizeof(node));
    memcpy(result->nodes, a1->nodes, a1->countNode * sizeof(node));
    memcpy(result->nodes + a1->countNode, a2->nodes, a2->countNode * sizeof(node));
    result->nodes[a1->countNode + a2->countNode] = *newStart;

    result->countNode = a1->countNode + a2->countNode + 1;
    result->countInitialNode = 1;
    result->countfinalNode = a1->countfinalNode + a2->countfinalNode;

    return result;
}


automate* processExpression(char* exp) {
    automate* result = NULL;
    size_t len = strlen(exp);

    for (size_t i = 0; i < len; i++) {
        char c = exp[i];
        automate* newAutomaton = NULL;

        if (isalpha(c)) {
            if (i + 1 < len && exp[i + 1] == '*') {
                newAutomaton = constructAStar1(&c);
                i++;
            } else if (i + 1 < len && exp[i + 1] == '+') {
                newAutomaton = construireAutomatePlus(c);
                i++;
            } else {
                newAutomaton = createAutomateFromChar(c);
            }

            if (result) {
                result = unionAutomates(result, newAutomaton);
            } else {
                result = newAutomaton;
            }
        }
    }
    return result;
}






void afficherAutomate(automate* a) {
    printf("Automate avec %d éeats\n", a->countNode);
    for (int i = 0; i < a->countNode; i++) {
        printf("etat %s%s\n", a->nodes[i].Id, a->nodes[i].isFinal ? " (final)" : "");
        for (int j = 0; j < a->nodes[i].transitionCount; j++) {
            printf("  -> %s via %s\n",
                   a->nodes[i].transition[j].nextNode->Id,
                   a->nodes[i].transition[j].etiquette);
        }
    }
}

int main() {
    char exp[] = "a*b";
    automate* a = processExpression(exp);
    afficherAutomate(a);
    genererficher(a);
    return 0;
}
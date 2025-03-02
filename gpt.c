#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// Structure pour représenter une transition
typedef struct relation {
    struct node* nextNode;
    char* etiquette;
    int nbrEtiquette;
} relation;

// Structure pour représenter un état (node)
typedef struct node {
    char* Id;
    bool isFinal;
    int transitionCount;
    relation* transition;
} node;

// Structure pour représenter un automate
typedef struct automate {
    node** initialNode;
    node** finalNode;
    node* nodes;
    int countNode;
    int countInitialNode;
    int countfinalNode;
} automate;

int stateCounter = 0;

// Fonction pour créer un état (node)
node* createNode(bool isFinal) {
    node* newNode = (node*)malloc(sizeof(node));
    newNode->Id = (char*)malloc(10 * sizeof(char));
    sprintf(newNode->Id, "q%d", stateCounter++);
    newNode->isFinal = isFinal;
    newNode->transitionCount = 0;
    newNode->transition = NULL;
    return newNode;
}

// Fonction pour ajouter une transition
void addTransition(node* from, node* to, char* symbol) {
    from->transition = (relation*)realloc(from->transition, 
                     (from->transitionCount + 1) * sizeof(relation));
    from->transition[from->transitionCount].nextNode = to;
    from->transition[from->transitionCount].etiquette = strdup(symbol);
    from->transition[from->transitionCount].nbrEtiquette = 1;
    from->transitionCount++;
}

// Fonction pour créer un automate avec un seul caractère
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

// Fonction pour construire un automate pour `a*`
automate* constructAStar1(char* symbol) {
    automate* automaton = (automate*)malloc(sizeof(automate));

    node* q0 = createNode(false);
    node* q1 = createNode(true);

    addTransition(q0, q1, "+"); // Transition epsilon
    addTransition(q1, q1, symbol); // Boucle sur q1 avec 'a'
    addTransition(q0, q0, symbol); // Boucle sur q0 avec 'a' pour répétition

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

// Fonction pour analyser une expression régulière et créer l'automate
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

// Fonction pour afficher l'automate
void afficherAutomate(automate* a) {
    printf("Automate avec %d états\n", a->countNode);
    for (int i = 0; i < a->countNode; i++) {
        printf("État %s%s\n", a->nodes[i].Id, a->nodes[i].isFinal ? " (final)" : "");
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
    return 0;
}

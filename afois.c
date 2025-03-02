#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// Définition des structures de l'automate
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
    int nbrAlphabet;
    char* Alphabet;
    node** initialNode;
    node** finalNode;
    node* nodes;
    int countNode;
    int countInitialNode;
    int countfinalNode;
    char Nom[100];
} automate;

int stateCounter = 0;

// Fonction pour créer un état
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

// Fonction pour construire l'automate de l'expression `a*`
automate* constructAStar(char* symbol) {
    automate* automaton = (automate*)malloc(sizeof(automate));
    
    // Création des états
    node* q0 = createNode(false);
    node* q1 = createNode(true);
    
    // Ajout des transitions
    addTransition(q0, q1, "+"); // Transition de q0 vers q1 avec ε
    addTransition(q1, q1, symbol); // Boucle sur q1 avec 'a'
    
    // Remplir la structure de l'automate
    automaton->initialNode = (node**)malloc(sizeof(node*));
    automaton->initialNode[0] = q0;
    automaton->countInitialNode = 1;
    automaton->nodes = (node*)malloc(2 * sizeof(node));
    automaton->nodes[0] = *q0;
    automaton->nodes[1] = *q1;
    automaton->countNode = 2;
    
    return automaton;
}

// Fonction pour afficher l'automate
void displayAutomaton(automate* automaton) {
    if (!automaton) return;

    printf("Automate :\n");
    for (int i = 0; i < automaton->countNode; i++) {
        node state = automaton->nodes[i];
        printf("État %s %s\n", state.Id, state.isFinal ? "(Final)" : "");

        for (int j = 0; j < state.transitionCount; j++) {
            printf("  --%s--> %s\n", state.transition[j].etiquette, state.transition[j].nextNode->Id);
        }
    }
}

// Fonction pour générer un fichier .dot
void generateDotFile(automate* automaton, const char* filename) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        printf("Erreur : Impossible de créer le fichier %s\n", filename);
        return;
    }

    fprintf(file, "digraph Automaton {\n");
    fprintf(file, "    rankdir=LR;\n");

    // Déclaration des états
    for (int i = 0; i < automaton->countNode; i++) {
        node state = automaton->nodes[i];
        if (state.isFinal)
            fprintf(file, "    %s [shape=doublecircle];\n", state.Id);
        else
            fprintf(file, "    %s [shape=circle];\n", state.Id);
    }

    // Transition initiale (état fictif)
    fprintf(file, "    start [shape=point];\n");
    fprintf(file, "    start -> %s;\n", automaton->initialNode[0]->Id);

    // Déclaration des transitions
    for (int i = 0; i < automaton->countNode; i++) {
        node state = automaton->nodes[i];
        for (int j = 0; j < state.transitionCount; j++) {
            fprintf(file, "    %s -> %s [label=\"%s\"];\n", 
                    state.Id, state.transition[j].nextNode->Id, state.transition[j].etiquette);
        }
    }

    fprintf(file, "}\n");
    fclose(file);
    printf("Fichier %s genere avec succes !\n", filename);
}

// Programme principal
int main() {
    // Construction de l'automate pour a*
    automate* automaton = constructAStar("a");

    // Affichage de l'automate
    displayAutomaton(automaton);

    // Génération du fichier DOT
    generateDotFile(automaton, "automate_star.dot");

    return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// Définition des structures de l'automate
typedef struct node {
    char Id[10];
    bool isFinal;
    struct relation* transitions;
    int transitionCount;
} node;

typedef struct relation {
    node* nextNode;
    char etiquette;
} relation;

typedef struct automate {
    node* initialNode;
    node* nodes;
    int countNode;
} automate;

int stateCounter = 0; // Pour générer des identifiants uniques d'états

// Fonction pour créer un état
node* createNode(bool isFinal) {
    node* newNode = (node*)malloc(sizeof(node));
    sprintf(newNode->Id, "q%d", stateCounter++);
    newNode->isFinal = isFinal;
    newNode->transitionCount = 0;
    newNode->transitions = NULL;
    return newNode;
}

// Fonction pour ajouter une transition
void addTransition(node* from, node* to, char etiquette) {
    from->transitions = (relation*)realloc(from->transitions, 
                     (from->transitionCount + 1) * sizeof(relation));
    from->transitions[from->transitionCount].nextNode = to;
    from->transitions[from->transitionCount].etiquette = etiquette;
    from->transitionCount++;
}

// Fonction pour construire l'automate de l'expression `a+`
automate* construireAutomatePlus(char etiquette) {
    automate* autoPlus = (automate*)malloc(sizeof(automate));

    // Création des états
    node* q0 = createNode(false);
    node* q1 = createNode(true);

    // Ajout des transitions
    addTransition(q0, q1, etiquette); // Transition de q0 vers q1 avec 'a'
    addTransition(q1, q1, etiquette); // Boucle sur q1 avec 'a'

    // Remplir la structure de l'automate
    autoPlus->initialNode = q0;
    autoPlus->nodes = (node*)malloc(2 * sizeof(node));
    autoPlus->nodes[0] = *q0;
    autoPlus->nodes[1] = *q1;
    autoPlus->countNode = 2;

    return autoPlus;
}

// Fonction pour afficher l'automate
void displayAutomate(automate* autoPlus) {
    if (!autoPlus) return;

    printf("Automate :\n");
    for (int i = 0; i < autoPlus->countNode; i++) {
        node state = autoPlus->nodes[i];
        printf("\nÉtat %s %s\n", state.Id, state.isFinal ? "(Final)" : "");
        for (int j = 0; j < state.transitionCount; j++) {
            printf("  --%c--> %s\n", state.transitions[j].etiquette, state.transitions[j].nextNode->Id);
        }
    }
}

// Fonction pour générer un fichier .dot
void generateDotFile(automate* autoPlus, const char* filename) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        printf("Erreur : Impossible de créer le fichier %s\n", filename);
        return;
    }

    fprintf(file, "digraph Automate {\n");
    fprintf(file, "    rankdir=LR;\n"); // Orientation gauche-droite

    // Déclaration des états
    for (int i = 0; i < autoPlus->countNode; i++) {
        node state = autoPlus->nodes[i];
        if (state.isFinal)
            fprintf(file, "    %s [shape=doublecircle];\n", state.Id);
        else
            fprintf(file, "    %s [shape=circle];\n", state.Id);
    }

    // Transition initiale (état fictif)
    fprintf(file, "    start [shape=point];\n");
    fprintf(file, "    start -> %s;\n", autoPlus->initialNode->Id);

    // Déclaration des transitions
    for (int i = 0; i < autoPlus->countNode; i++) {
        node state = autoPlus->nodes[i];
        for (int j = 0; j < state.transitionCount; j++) {
            fprintf(file, "    %s -> %s [label=\"%c\"];\n", 
                    state.Id, state.transitions[j].nextNode->Id, state.transitions[j].etiquette);
        }
    }

    fprintf(file, "}\n");
    fclose(file);
    printf("Fichier %s généré avec succès !\n", filename);
}

// Programme principal
int main() {
    // Construction de l'automate pour a+
    automate* autoPlus = construireAutomatePlus('a');

    // Affichage de l'automate
    displayAutomate(autoPlus);

    // Génération du fichier DOT
    generateDotFile(autoPlus, "automate.dot");

    return 0;
}

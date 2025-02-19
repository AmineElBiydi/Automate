
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

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
} relation;

typedef struct automate {
    node** initialNodes;  // Array of pointers for initial states
    int countInitialNode;
    node* nodes;          // Array of nodes
    int countNode;
    char Nom[100];
} automate;

// Function to find or create a node
node* findOrCreateNode(automate* a, const char* id) {
    for (int i = 0; i < a->countNode; i++) {
        if (strcmp(a->nodes[i].Id, id) == 0) {
            return &a->nodes[i];
        }
    }

    // Create a new node
    a->countNode++;
    a->nodes = realloc(a->nodes, a->countNode * sizeof(node));
    if (!a->nodes) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }

    node* newNode = &a->nodes[a->countNode - 1];
    newNode->Id = strdup(id);  // Duplicate the ID safely
    newNode->isFinal = false;
    newNode->transition = NULL;
    newNode->transitionCount = 0;

    return newNode;
}

// Function to add a transition
void addTransition(node* fromNode, node* toNode, const char* label) {
    fromNode->transitionCount++;
    fromNode->transition = realloc(fromNode->transition, fromNode->transitionCount * sizeof(relation));
    if (!fromNode->transition) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }

    relation* newRelation = &fromNode->transition[fromNode->transitionCount - 1];
    newRelation->nextNode = toNode;
    newRelation->etiquette = strdup(label);
}

// Function to parse .dot file
void parseDotFile(const char* filename, automate* a) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    a->initialNodes = NULL; // Initialize initial states array

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        // Extract Automate name
        if (strstr(line, "digraph")) {
            sscanf(line, "digraph %s {", a->Nom);
        } 
        // Extract final states
        else if (strstr(line, "node[shape=doublecircle]")) {
            char *start = strstr(line, "doublecircle]") + 13;  // Move past "doublecircle]"
            char *token = strtok(start, " ,;\n");
            while (token) {
                node* n = findOrCreateNode(a, token);
                n->isFinal = true;
                token = strtok(NULL, " ,;\n");
            }
        } 
        // Extract initial states
        else if (strstr(line, "start->")) {
            char* token = strtok(line, " ->,;\n");
            token = strtok(NULL, " ->,;\n");  // Skip "start->"
            while (token) {
                node* n = findOrCreateNode(a, token);
                a->initialNodes = realloc(a->initialNodes, (a->countInitialNode + 1) * sizeof(node*));
                if (!a->initialNodes) {
                    perror("Memory allocation failed");
                    exit(EXIT_FAILURE);
                }
                a->initialNodes[a->countInitialNode++] = n;
                token = strtok(NULL, " ->,;\n");
            }
        } 
        // Extract transitions
        else if (strstr(line, "->") && strstr(line, "[label=")) {
            char from[100], to[100], label[100];
            if (sscanf(line, " %99[^->] -> %99[^ [] [label=\"%99[^\"]\"];", from, to, label) == 3) {
                node* fromNode = findOrCreateNode(a, from);
                node* toNode = findOrCreateNode(a, to);
                addTransition(fromNode, toNode, label);
            }
        }
    }
    fclose(file);
}

// Function to print the automate
void printAutomate(automate* a) {
    printf("Nom de l'automate : %s\n", a->Nom);
    
    printf("États : ");
    for (int i = 0; i < a->countNode; i++) {
        printf("%s ", a->nodes[i].Id);
    }
    printf("\n");

    printf("États initiaux : ");
    for (int i = 0; i < a->countInitialNode; i++) {
        printf("%s ", a->initialNodes[i]->Id);
    }
    printf("\n");

    printf("États finaux : ");
    for (int i = 0; i < a->countNode; i++) {
        if (a->nodes[i].isFinal) {
            printf("%s ", a->nodes[i].Id);
        }
    }
    printf("\n");

    printf("Transitions :\n");
    for (int i = 0; i < a->countNode; i++) {
        node* n = &a->nodes[i];
        for (int j = 0; j < n->transitionCount; j++) {
            printf("%s --(%s)--> %s\n", n->Id, n->transition[j].etiquette, n->transition[j].nextNode->Id);
        }
    }
}

// Function to free allocated memory
void freeAutomate(automate* a) {
    for (int i = 0; i < a->countNode; i++) {
        free(a->nodes[i].Id);
        for (int j = 0; j < a->nodes[i].transitionCount; j++) {
            free(a->nodes[i].transition[j].etiquette);
        }
        free(a->nodes[i].transition);
    }
    free(a->nodes);
    free(a->initialNodes);
}

// Main function
int main() {
    automate a = {0};
    parseDotFile("test1.dot", &a);
    printAutomate(&a);
    freeAutomate(&a); // Free allocated memory

    return 0;
}
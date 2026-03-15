#include "noeud.h"
#include <stdlib.h>
#include <string.h>

Noeud *noeud_creer(const char *id) {
    Noeud *n = malloc(sizeof(Noeud));
    if (!n) return NULL;

    n->id = malloc(strlen(id) + 1);
    if (!n->id) {
        free(n);
        return NULL;
    }
    strcpy(n->id, id);

    n->enfants = NULL;
    n->nb_enfants = 0;
    n->en_cours = 0;
    return n;
}

void noeud_ajouter_enfant(Noeud *parent, Noeud *enfant, double fuite) {
    Arete *a = malloc(sizeof(Arete));
    if (!a) return;

    a->enfant = enfant;
    a->fuite_pct = fuite;
    a->suivant = parent->enfants;
    parent->enfants = a;
    parent->nb_enfants++;
}

void noeud_liberer(Noeud *n) {
    if (!n) return;
    Arete *a = n->enfants;
    while (a) {
        Arete *tmp = a;
        a = a->suivant;
        free(tmp);
    }
    free(n->id);
    free(n);
}

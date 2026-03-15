#ifndef AVL_NOEUD_H
#define AVL_NOEUD_H

#include "noeud.h"

typedef struct AVLNoeud {
    char *cle;
    Noeud *val;
    int h;
    struct AVLNoeud *g, *d;
} AVLNoeud;

AVLNoeud *avl_noeud_inserer_ou_get(AVLNoeud *r, const char *cle, Noeud **out);
Noeud *avl_noeud_trouver(AVLNoeud *r, const char *cle);
void avl_noeud_liberer(AVLNoeud *r);

#endif

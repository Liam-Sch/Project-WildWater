#include "avl_noeud.h"
#include <stdlib.h>
#include <string.h>

static int hauteur(AVLNoeud *n) {
    return n ? n->h : 0;
}

static int max(int a, int b) {
    return a > b ? a : b;
}

static AVLNoeud *rotation_droite(AVLNoeud *y) {
    AVLNoeud *x = y->g;
    y->g = x->d;
    x->d = y;
    y->h = max(hauteur(y->g), hauteur(y->d)) + 1;
    x->h = max(hauteur(x->g), hauteur(x->d)) + 1;
    return x;
}

static AVLNoeud *rotation_gauche(AVLNoeud *x) {
    AVLNoeud *y = x->d;
    x->d = y->g;
    y->g = x;
    x->h = max(hauteur(x->g), hauteur(x->d)) + 1;
    y->h = max(hauteur(y->g), hauteur(y->d)) + 1;
    return y;
}

AVLNoeud *avl_noeud_inserer_ou_get(AVLNoeud *r, const char *cle, Noeud **out) {
    if (!r) {
        AVLNoeud *n = malloc(sizeof(AVLNoeud));
        if (!n) return NULL;

        n->cle = malloc(strlen(cle) + 1);
        if (!n->cle) {
            free(n);
            return NULL;
        }
        strcpy(n->cle, cle);

        n->val = noeud_creer(cle);
        n->g = n->d = NULL;
        n->h = 1;
        *out = n->val;
        return n;
    }

    int c = strcmp(cle, r->cle);
    if (c == 0) {
        *out = r->val;
        return r;
    }

    if (c < 0)
        r->g = avl_noeud_inserer_ou_get(r->g, cle, out);
    else
        r->d = avl_noeud_inserer_ou_get(r->d, cle, out);

    r->h = max(hauteur(r->g), hauteur(r->d)) + 1;
    int bal = hauteur(r->g) - hauteur(r->d);

    if (bal > 1 && strcmp(cle, r->g->cle) < 0)
        return rotation_droite(r);
    if (bal < -1 && strcmp(cle, r->d->cle) > 0)
        return rotation_gauche(r);

    return r;
}

Noeud *avl_noeud_trouver(AVLNoeud *r, const char *cle) {
    if (!r) return NULL;
    int c = strcmp(cle, r->cle);
    if (c == 0) return r->val;
    if (c < 0) return avl_noeud_trouver(r->g, cle);
    return avl_noeud_trouver(r->d, cle);
}

void avl_noeud_liberer(AVLNoeud *r) {
    if (!r) return;
    avl_noeud_liberer(r->g);
    avl_noeud_liberer(r->d);
    noeud_liberer(r->val);
    free(r->cle);
    free(r);
}

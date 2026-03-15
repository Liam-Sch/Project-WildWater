#include "avl_usine.h"
#include <stdlib.h>
#include <string.h>

/* --- Outils AVL (TD6) --- */
static int max2(int a, int b) { return (a > b) ? a : b; }
static int h(AVLUsine *n) { return n ? n->hauteur : 0; }

static void maj_hauteur(AVLUsine *n) {
    if (!n) return;
    n->hauteur = 1 + max2(h(n->gauche), h(n->droite));
}

/* facteur = h(droite) - h(gauche) */
static int facteur_equilibre(AVLUsine *n) {
    return n ? (h(n->droite) - h(n->gauche)) : 0;
}

static AVLUsine *rotation_gauche(AVLUsine *a) {
    AVLUsine *b = a->droite;
    AVLUsine *t2 = b->gauche;

    b->gauche = a;
    a->droite = t2;

    maj_hauteur(a);
    maj_hauteur(b);
    return b;
}

static AVLUsine *rotation_droite(AVLUsine *a) {
    AVLUsine *b = a->gauche;
    AVLUsine *t2 = b->droite;

    b->droite = a;
    a->gauche = t2;

    maj_hauteur(a);
    maj_hauteur(b);
    return b;
}

static AVLUsine *equilibrer(AVLUsine *n) {
    if (!n) return NULL;

    maj_hauteur(n);
    int fe = facteur_equilibre(n);

    /* Trop à droite */
    if (fe > 1) {
        if (facteur_equilibre(n->droite) < 0) {
            n->droite = rotation_droite(n->droite);
        }
        return rotation_gauche(n);
    }

    /* Trop à gauche */
    if (fe < -1) {
        if (facteur_equilibre(n->gauche) > 0) {
            n->gauche = rotation_gauche(n->gauche);
        }
        return rotation_droite(n);
    }

    return n;
}

/* Copie de chaîne (CM/TD) : remplace strdup (hors cours) */
static char *copier_chaine(const char *s) {
    char *p = (char*)malloc(strlen(s) + 1);
    if (!p) return NULL;
    strcpy(p, s);
    return p;
}

AVLUsine *avl_usine_inserer_ou_get(AVLUsine *racine, const char *cle, Usine **out) {
    if (!out) return racine;

    if (!racine) {
        AVLUsine *n = (AVLUsine*)calloc(1, sizeof(AVLUsine));
        if (!n) return NULL;

        n->cle = copier_chaine(cle);
        if (!n->cle) {
            free(n);
            return NULL;
        }

        n->valeur = usine_creer(cle); /* fonction du module usine */
        if (!n->valeur) {
            free(n->cle);
            free(n);
            return NULL;
        }

        n->hauteur = 1;
        *out = n->valeur;
        return n;
    }

    int cmp = strcmp(cle, racine->cle);
    if (cmp == 0) {
        *out = racine->valeur;
        return racine;
    } else if (cmp < 0) {
        racine->gauche = avl_usine_inserer_ou_get(racine->gauche, cle, out);
    } else {
        racine->droite = avl_usine_inserer_ou_get(racine->droite, cle, out);
    }

    return equilibrer(racine);
}

Usine *avl_usine_trouver(AVLUsine *racine, const char *cle) {
    if (!racine) return NULL;
    int cmp = strcmp(cle, racine->cle);
    if (cmp == 0) return racine->valeur;
    if (cmp < 0) return avl_usine_trouver(racine->gauche, cle);
    return avl_usine_trouver(racine->droite, cle);
}

void avl_usine_parcours_infixe_inverse(AVLUsine *racine,
                                      void (*visiter)(Usine *u, void *ctx),
                                      void *ctx) {
    if (!racine || !visiter) return;
    avl_usine_parcours_infixe_inverse(racine->droite, visiter, ctx);
    visiter(racine->valeur, ctx);
    avl_usine_parcours_infixe_inverse(racine->gauche, visiter, ctx);
}

void avl_usine_liberer(AVLUsine *racine) {
    if (!racine) return;
    avl_usine_liberer(racine->gauche);
    avl_usine_liberer(racine->droite);
    usine_liberer(racine->valeur);
    free(racine->cle);
    free(racine);
}

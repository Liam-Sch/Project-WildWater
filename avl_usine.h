#ifndef AVL_USINE_H
#define AVL_USINE_H

#include "usine.h"

/* AVL d'usines : la clé est une chaîne (id usine) */
typedef struct AVLUsine {
    char *cle;
    Usine *valeur;
    int hauteur;
    struct AVLUsine *gauche;
    struct AVLUsine *droite;
} AVLUsine;

/* Insère la clé si absente, sinon retourne l'usine existante via out */
AVLUsine *avl_usine_inserer_ou_get(AVLUsine *racine, const char *cle, Usine **out);

/* Recherche par clé (id) */
Usine    *avl_usine_trouver(AVLUsine *racine, const char *cle);

/* Parcours infixe inverse (droite, noeud, gauche) : utile pour trier du plus grand au plus petit
   via la fonction visiter (ex: écrire dans un fichier). */
void      avl_usine_parcours_infixe_inverse(AVLUsine *racine,
                                           void (*visiter)(Usine *u, void *ctx),
                                           void *ctx);

/* Libère entièrement l'AVL et les Usine* associées (via usine_liberer). */
void      avl_usine_liberer(AVLUsine *racine);

#endif

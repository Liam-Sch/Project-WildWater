#ifndef NOEUD_H
#define NOEUD_H

typedef struct Noeud Noeud;
typedef struct Arete Arete;

struct Arete {
    Noeud *enfant;
    double fuite_pct;
    Arete *suivant;
};

struct Noeud {
    char *id;
    Arete *enfants;
    int nb_enfants;
    int en_cours;
};

Noeud *noeud_creer(const char *id);
void noeud_ajouter_enfant(Noeud *parent, Noeud *enfant, double fuite);
void noeud_liberer(Noeud *n);

#endif

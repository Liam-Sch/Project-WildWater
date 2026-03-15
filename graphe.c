#include "graphe.h"
#include "noeud.h"
#include "avl_noeud.h"
#include "outils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static double dfs_fuites(Noeud *n, double volume) {
    if (!n || n->nb_enfants == 0 || n->en_cours) return 0.0;

    n->en_cours = 1;
    double total = 0.0;
    int deg = n->nb_enfants;

    Arete *a = n->enfants;
    while (a) {
        double part = volume / deg;
        double perte = part * (a->fuite_pct / 100.0);
        total += perte;
        total += dfs_fuites(a->enfant, part - perte);
        a = a->suivant;
    }

    n->en_cours = 0;
    return total;
}

double calculer_fuites_km3(const char *csv, const char *id, int *out_trouve) {
    if (out_trouve) *out_trouve = 0;

    FILE *f = fopen(csv, "r");
    if (!f) return -1.0;

    AVLNoeud *map = NULL;
    double volume_reel = 0.0;
    int trouve = 0;

    char ligne[1024];
    while (fgets(ligne, sizeof(ligne), f)) {
        couper_fin_ligne(ligne);
        char *cols[5];
        decouper_csv_5(ligne, cols);

        if (commence_par(cols[1], "Spring #") && strcmp(cols[2], id) == 0) {
            double v, fu;
            if (lire_double(cols[3], &v) && lire_double(cols[4], &fu)) {
                volume_reel += v * (1.0 - fu / 100.0);
                trouve = 1;
            }
        }
    }

    fclose(f);

    if (!trouve) {
        avl_noeud_liberer(map);
        return -1.0;
    }

    if (out_trouve) *out_trouve = 1;
    Noeud *racine = avl_noeud_trouver(map, id);
    if (!racine) {
        avl_noeud_liberer(map);
        return -1.0;
    }

    double pertes = dfs_fuites(racine, volume_reel);
    avl_noeud_liberer(map);
    return pertes;
}

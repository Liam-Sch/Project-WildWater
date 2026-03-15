#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "outils.h"
#include "usine.h"
#include "avl_usine.h"
#include "graphe.h"

/* ---------------- HISTO ---------------- */
typedef enum { MODE_MAX, MODE_SRC, MODE_REEL } ModeHisto;

typedef struct {
    FILE *f;
    ModeHisto mode;
} ContexteHisto;

static void ecrire_ligne_histo(Usine *u, void *ctx_void) {
    ContexteHisto *ctx = (ContexteHisto*)ctx_void;
    if (!u || !ctx || !ctx->f) return;

    if (ctx->mode == MODE_MAX) {
        fprintf(ctx->f, "%s;%d\n", u->id, u->max_km3);
    } else if (ctx->mode == MODE_SRC) {
        fprintf(ctx->f, "%s;%.6f\n", u->id, u->src_km3);
    } else {
        fprintf(ctx->f, "%s;%.6f\n", u->id, u->reel_km3);
    }
}

/* USINE : c2=id usine, c3='-', c4=capacité, c5='-' */
static int est_description_usine(const char *c2, const char *c3, const char *c4, const char *c5) {
    if (est_tiret(c2)) return 0;
    if (!est_tiret(c3)) return 0;
    if (est_tiret(c4)) return 0;
    if (!est_tiret(c5)) return 0;
    return 1;
}

/* SOURCE->USINE : c1='-', c2=Spring, c3=Facility, c4=vol, c5=fuite */
static int est_source_vers_usine(const char *c1, const char *c2, const char *c3, const char *c4, const char *c5) {
    if (!est_tiret(c1)) return 0;
    if (!commence_par(c2, "Spring #")) return 0;
    if (!commence_par(c3, "Facility complex #")) return 0;
    if (est_tiret(c4) || est_tiret(c5)) return 0;
    return 1;
}

static int executer_histo(const char *mode_str, const char *csv, const char *sortie) {
    ModeHisto mode;
    if (strcmp(mode_str, "max") == 0) mode = MODE_MAX;
    else if (strcmp(mode_str, "src") == 0) mode = MODE_SRC;
    else if (strcmp(mode_str, "reel") == 0) mode = MODE_REEL;
    else {
        fprintf(stderr, "Erreur: mode histo invalide (%s). Attendu: max|src|reel\n", mode_str);
        return 2;
    }

    FILE *in = fopen(csv, "r");
    if (!in) {
        fprintf(stderr, "Erreur: impossible d'ouvrir le CSV: %s\n", csv);
        return 3;
    }

    AVLUsine *racine = NULL;

    char *ligne = NULL;
    size_t cap = 0;
    ssize_t len;

    while ((len = getline(&ligne, &cap, in)) != -1) {
        (void)len;
        couper_fin_ligne(ligne);

        char *cols[5];
        decouper_csv_5(ligne, cols);

        const char *c1 = cols[0];
        const char *c2 = cols[1];
        const char *c3 = cols[2];
        const char *c4 = cols[3];
        const char *c5 = cols[4];

        if (est_description_usine(c2, c3, c4, c5)) {
            int maxv;
            if (lire_int(c4, &maxv)) {
                Usine *u = NULL;
                racine = avl_usine_inserer_ou_get(racine, c2, &u);
                if (!racine || !u) {
                    fprintf(stderr, "Erreur: mémoire insuffisante (AVL/Usine)\n");
                    free(ligne);
                    fclose(in);
                    avl_usine_liberer(racine);
                    return 4;
                }
                u->max_km3 = maxv;
            }
            continue;
        }

        if (est_source_vers_usine(c1, c2, c3, c4, c5)) {
            double vol, fuite;
            if (lire_double(c4, &vol) && lire_double(c5, &fuite)) {
                if (fuite < 0.0) fuite = 0.0;
                if (fuite > 100.0) fuite = 100.0;

                Usine *u = NULL;
                racine = avl_usine_inserer_ou_get(racine, c3, &u);
                if (!racine || !u) {
                    fprintf(stderr, "Erreur: mémoire insuffisante (AVL/Usine)\n");
                    free(ligne);
                    fclose(in);
                    avl_usine_liberer(racine);
                    return 4;
                }

                u->src_km3 += vol;
                u->reel_km3 += vol * (1.0 - fuite / 100.0);
            }
            continue;
        }
    }

    free(ligne);
    fclose(in);

    FILE *out = fopen(sortie, "w");
    if (!out) {
        fprintf(stderr, "Erreur: impossible d'écrire: %s\n", sortie);
        avl_usine_liberer(racine);
        return 5;
    }

    if (mode == MODE_MAX) fprintf(out, "identifier;max volume (k.m3.year-1)\n");
    else if (mode == MODE_SRC) fprintf(out, "identifier;source volume (k.m3.year-1)\n");
    else fprintf(out, "identifier;real volume (k.m3.year-1)\n");

    ContexteHisto ctx = { .f = out, .mode = mode };

    /* Tri inverse Z->A via parcours infixe inverse */
    avl_usine_parcours_infixe_inverse(racine, ecrire_ligne_histo, &ctx);

    fclose(out);
    avl_usine_liberer(racine);
    return 0;
}

/* ---------------- LEAKS ---------------- */
static int fichier_vide(FILE *f) {
    long pos = ftell(f);
    fseek(f, 0, SEEK_END);
    long fin = ftell(f);
    fseek(f, pos, SEEK_SET);
    return (fin == 0);
}

static int executer_leaks(const char *id_usine, const char *csv, const char *sortie) {
    int trouve = 0;
    double pertes_km3 = calculer_fuites_km3(csv, id_usine, &trouve);

    FILE *out = fopen(sortie, "a+");
    if (!out) {
        fprintf(stderr, "Erreur: impossible d'ouvrir/créer: %s\n", sortie);
        return 3;
    }

    fseek(out, 0, SEEK_SET);
    int vide = fichier_vide(out);
    fseek(out, 0, SEEK_END);

    if (vide) {
        fprintf(out, "identifier;Leak volume (M.m3.year-1)\n");
    }

    if (!trouve || pertes_km3 < 0.0) {
        fprintf(out, "%s;-1\n", id_usine);
        fclose(out);
        return 0;
    }

    /* conversion k.m3 -> M.m3 : /1000 */
    fprintf(out, "%s;%.6f\n", id_usine, pertes_km3 / 1000.0);
    fclose(out);
    return 0;
}

static void usage(const char *p) {
    fprintf(stderr,
        "Usage:\n"
        "  %s histo max|src|reel <csv> <sortie.dat>\n"
        "  %s leaks \"Facility complex #...\" <csv> <sortie.dat>\n", p, p);
}

int main(int argc, char **argv) {
    if (argc != 5) {
        usage(argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "histo") == 0) {
        return executer_histo(argv[2], argv[3], argv[4]);
    }

    if (strcmp(argv[1], "leaks") == 0) {
        return executer_leaks(argv[2], argv[3], argv[4]);
    }

    usage(argv[0]);
    return 1;
}

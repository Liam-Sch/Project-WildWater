#include "outils.h"
#include <string.h>
#include <stdlib.h>

/* Supprime \n et \r en fin de ligne (CM) */
void couper_fin_ligne(char *s) {
    if (!s) return;
    size_t n = strlen(s);
    while (n > 0 && (s[n-1] == '\n' || s[n-1] == '\r')) {
        s[n-1] = '\0';
        n--;
    }
}

int commence_par(const char *s, const char *prefix) {
    if (!s || !prefix) return 0;
    size_t n = strlen(prefix);
    return strncmp(s, prefix, n) == 0;
}

int est_tiret(const char *s) {
    if (!s) return 1;
    if (s[0] == '\0') return 1;
    return (strcmp(s, "-") == 0);
}

/* Découpe une ligne en 5 colonnes séparées par ';'
   (méthode simple CM : on remplace les ';' par '\\0') */
void decouper_csv_5(char *ligne, char *cols[5]) {
    static char tiret[] = "-";
    int i;
    for (i = 0; i < 5; i++) cols[i] = tiret;

    if (!ligne) return;

    cols[0] = ligne;
    int col = 1;

    for (i = 0; ligne[i] != '\0' && col < 5; i++) {
        if (ligne[i] == ';') {
            ligne[i] = '\0';
            cols[col] = &ligne[i+1];
            col++;
        }
    }
}

/* Lecture robuste d'un double avec strtod (CM) */
int lire_double(const char *s, double *out) {
    if (!out) return 0;
    if (!s || s[0] == '\0' || est_tiret(s)) return 0;

    char *end = NULL;
    double v = strtod(s, &end);

    if (end == s) return 0;          /* rien lu */
    if (*end != '\0') return 0;      /* caractères restants -> invalide */

    *out = v;
    return 1;
}

int lire_int(const char *s, int *out) {
    if (!out) return 0;
    if (!s || s[0] == '\0' || est_tiret(s)) return 0;

    char *end = NULL;
    long v = strtol(s, &end, 10);

    if (end == s) return 0;      /* rien lu */
    if (*end != '\0') return 0;  /* reste des caractères -> invalide */

    *out = (int)v;
    return 1;
}

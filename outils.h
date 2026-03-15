#ifndef OUTILS_H
#define OUTILS_H

/* Outils simples (CM/TD) pour traitement de lignes CSV */

void couper_fin_ligne(char *s);

/* Remplit cols[0..4] avec les 5 colonnes séparées par ';'
   Les pointeurs retournés pointent à l'intérieur de 'ligne' (qui est modifiée). */
void decouper_csv_5(char *ligne, char *cols[5]);

/* Renvoie 1 si s commence par prefix, sinon 0 */
int commence_par(const char *s, const char *prefix);

/* Renvoie 1 si la colonne est absente ou vaut "-" (valeur manquante) */
int est_tiret(const char *s);

/* Lit un double ; renvoie 1 si OK, 0 sinon */
int lire_double(const char *s, double *out);

int lire_int(const char *s, int *out);

#endif

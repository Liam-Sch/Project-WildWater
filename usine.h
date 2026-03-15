#ifndef USINE_H
#define USINE_H

typedef struct {
    char *id;         /* identifiant usine */
    int   max_km3;    /* capacité max (k.m3.year-1) */
    double src_km3;   /* volume capté (k.m3.year-1) */
    double reel_km3;  /* volume réellement traité (k.m3.year-1) */
} Usine;

Usine *usine_creer(const char *id);
void   usine_liberer(Usine *u);

#endif

#include "usine.h"
#include <stdlib.h>
#include <string.h>

Usine *usine_creer(const char *id) {
    Usine *u = (Usine*)calloc(1, sizeof(Usine));
    if (!u) return NULL;

    u->id = strdup(id);
    if (!u->id) { free(u); return NULL; }

    u->max_km3 = 0;
    u->src_km3 = 0.0;
    u->reel_km3 = 0.0;
    return u;
}

void usine_liberer(Usine *u) {
    if (!u) return;
    free(u->id);
    free(u);
}

// Enfant de capteur : centrale inertielle (accéléromètre 3 axes)
#ifndef CAPTEUR_MOUVEMENT_H
#define CAPTEUR_MOUVEMENT_H

#include "capteur/capteur.h"

// Indices des axes dans acceleration_g
enum { CAPTEUR_MOUVEMENT_AXE_X, CAPTEUR_MOUVEMENT_AXE_Y, CAPTEUR_MOUVEMENT_AXE_Z, CAPTEUR_MOUVEMENT_NB_AXES };

typedef struct {
    capteur_t parent;                                // toujours en premier
    float acceleration_g[CAPTEUR_MOUVEMENT_NB_AXES];       // dernière mesure, en g
} capteur_mouvement_t;

// Norme du vecteur accélération (≈ 1 g au repos : gravité seule)
float capteur_mouvement_norme_g(const capteur_mouvement_t *self);

#endif

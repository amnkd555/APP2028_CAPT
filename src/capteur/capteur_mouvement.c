// Centrale inertielle : calculs communs à tous les capteurs de mouvement
#include "capteur/capteur_mouvement.h"

#include <math.h>

float capteur_mouvement_norme_g(const capteur_mouvement_t *self) {
    // Racine de la somme des carrés des trois composantes
    float somme = 0.0f;
    for (int axe = 0; axe < CAPTEUR_MOUVEMENT_NB_AXES; axe++) {
        somme += self->acceleration_g[axe] * self->acceleration_g[axe];
    }
    return sqrtf(somme);
}

// SOURCES: src/capteur/capteur_mouvement.c
// Tests de capteur_mouvement : calcul de la norme du vecteur accélération
#include <assert.h>
#include <math.h>
#include <stdio.h>

#include "capteur/capteur_mouvement.h"

static void test_norme_gravite_seule(void) {
    capteur_mouvement_t capteur = {.acceleration_g = {0.0f, 0.0f, 1.0f}};
    assert(fabsf(capteur_mouvement_norme_g(&capteur) - 1.0f) < 1e-6f);
}

static void test_norme_trois_axes(void) {
    // Triangle 3-4-12 → norme 13
    capteur_mouvement_t capteur = {.acceleration_g = {3.0f, -4.0f, 12.0f}};
    assert(fabsf(capteur_mouvement_norme_g(&capteur) - 13.0f) < 1e-5f);
}

int main(void) {
    test_norme_gravite_seule();
    test_norme_trois_axes();
    printf("test_capteur_mouvement : OK\n");
    return 0;
}

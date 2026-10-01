// SOURCES: src/communication/communication_moniteur_csv.c src/communication/communication_moniteur.c src/capteur/capteur_mouvement.c tests/mocks/pico_simule.c
// Tests de communication_moniteur_csv : format des lignes et compteurs d'erreurs
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "communication/communication_moniteur_csv.h"

static void test_format_ligne_mesure(void) {
    capteur_mouvement_t capteur = {.acceleration_g = {0.0f, -0.5f, 1.0f}};
    char ligne[96];
    communication_moniteur_csv_formater_mesure(ligne, sizeof ligne, 1234, &capteur);
    assert(strcmp(ligne, "1234,0.00000,-0.50000,1.00000,1.11803\n") == 0);
}

static void test_format_tronque_sans_debordement(void) {
    capteur_mouvement_t capteur = {0};
    char ligne[8];
    int longueur = communication_moniteur_csv_formater_mesure(ligne, sizeof ligne, 1234, &capteur);
    assert(longueur > (int)sizeof ligne);
    assert(strlen(ligne) == sizeof ligne - 1);
}

static void test_compteurs(void) {
    communication_moniteur_csv_t sortie = {0};
    communication_moniteur_csv_signaler_erreur(&sortie);
    communication_moniteur_csv_signaler_erreur(&sortie);
    communication_moniteur_csv_signaler_reinitialisation(&sortie);
    assert(sortie.erreurs_i2c == 2);
    assert(sortie.reinitialisations == 1);
}

int main(void) {
    test_format_ligne_mesure();
    test_format_tronque_sans_debordement();
    test_compteurs();
    printf("test_communication_moniteur_csv : OK\n");
    return 0;
}

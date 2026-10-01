// SOURCES: src/communication/communication_moniteur.c src/mocks/pico_simule.c
// Tests de communication_moniteur : détection de l'ouverture du port série
#include <assert.h>
#include <stdio.h>

#include "communication/communication_moniteur.h"
#include "pico/stdlib.h"

static void test_front_montant_seulement(void) {
    communication_moniteur_t m = {0};
    assert(!communication_moniteur_detecter_connexion(&m, false));   // personne n'écoute
    assert(communication_moniteur_detecter_connexion(&m, true));     // ouverture du port
    assert(!communication_moniteur_detecter_connexion(&m, true));    // toujours ouvert
    assert(!communication_moniteur_detecter_connexion(&m, false));   // fermeture
    assert(communication_moniteur_detecter_connexion(&m, true));     // réouverture
}

static void test_lit_l_etat_usb(void) {
    communication_moniteur_t m = {0};
    pico_simule_usb_connecte = false;
    assert(!communication_moniteur_nouvelle_connexion(&m));
    pico_simule_usb_connecte = true;
    assert(communication_moniteur_nouvelle_connexion(&m));
}

int main(void) {
    test_front_montant_seulement();
    test_lit_l_etat_usb();
    printf("test_communication_moniteur : OK\n");
    return 0;
}

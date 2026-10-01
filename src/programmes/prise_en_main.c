// Étape 2 « prise en main » : LED qui clignote à 1 Hz + messages sur l'USB série
#include <inttypes.h>
#include <stdio.h>

#include "communication/communication_moniteur.h"
#include "pico/stdlib.h"
#include "signalisation/signalisation_led.h"

// Bannière renvoyée à chaque ouverture du moniteur (sinon elle serait perdue)
static void afficher_banniere(void) {
    printf("========================================\n");
    printf("Projet Anti-Sedentarite : Pico demarre !\n");
    printf("Etape 2 : Prise en main reussie.\n");
    printf("========================================\n");
}

int main(void) {
    // USB série : le Pico apparaît comme /dev/tty.usbmodemXXX
    stdio_init_all();
    signalisation_led_t led;
    if (!signalisation_led_initialiser(&led)) return -1;
    communication_moniteur_t moniteur = {0};

    for (uint32_t secondes = 0;; secondes++) {
        if (communication_moniteur_nouvelle_connexion(&moniteur)) afficher_banniere();
        signalisation_led_basculer(&led);
        printf("[%" PRIu32 " s] Microcontroleur actif - Etat LED : %s\n",
               secondes, led.allumee ? "ALLUMEE" : "ETEINTE");
        sleep_ms(1000);
    }
}

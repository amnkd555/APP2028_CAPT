// Moniteur série USB : détection de l'ouverture du port
#include "communication/communication_moniteur.h"

#include "pico/stdlib.h"

bool communication_moniteur_detecter_connexion(communication_moniteur_t *self, bool connecte_maintenant) {
    // Front montant : n'écoutait pas avant, écoute maintenant
    bool nouvelle = connecte_maintenant && !self->connecte;
    self->connecte = connecte_maintenant;
    return nouvelle;
}

bool communication_moniteur_nouvelle_connexion(communication_moniteur_t *self) {
    // stdio_usb_connected() lit le signal DTR, levé quand un moniteur ouvre le port
    return communication_moniteur_detecter_connexion(self, stdio_usb_connected());
}

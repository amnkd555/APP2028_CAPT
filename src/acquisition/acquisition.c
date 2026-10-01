// Boucle d'acquisition : implémentation
#include "acquisition/acquisition.h"

#include "bus/bus_i2c_diagnostic.h"
#include "pico/stdlib.h"

// Raccourci vers la classe parent capteur_t
static capteur_t *capteur_de(acquisition_t *self) {
    return &self->capteur->parent;
}

// Réessaie l'initialisation tant que le capteur ne répond pas ; LED fixe une fois prêt
static void preparer_capteur(acquisition_t *self) {
    while (!capteur_initialiser(capteur_de(self))) {
        // 5 clignotements rapides = capteur non détecté
        signalisation_led_clignoter(self->led, 5, 100);
        bus_i2c_debloquer(self->bus);
        bus_i2c_diagnostic_afficher(self->bus, self->adresse_capteur);
    }
    self->capteur_pret = true;
    self->echecs_consecutifs = 0;
    signalisation_led_ecrire(self->led, true);
}

// Échec de lecture : on le signale, et on débloque le bus s'il se répète
static void traiter_echec(acquisition_t *self) {
    communication_moniteur_csv_signaler_erreur(self->sortie);
    sleep_ms(1);
    if (acquisition_enregistrer_echec(self)) {
        communication_moniteur_csv_signaler_reinitialisation(self->sortie);
        bus_i2c_debloquer(self->bus);
        self->capteur_pret = false;
    }
}

bool acquisition_enregistrer_echec(acquisition_t *self) {
    self->echecs_consecutifs++;
    if (self->echecs_consecutifs < ACQUISITION_ECHECS_AVANT_DEBLOCAGE) return false;
    self->echecs_consecutifs = 0;
    return true;
}

void acquisition_executer_cycle(acquisition_t *self) {
    communication_moniteur_csv_envoyer_entete_si_connexion(self->sortie);
    if (!self->capteur_pret) preparer_capteur(self);
    switch (capteur_lire(capteur_de(self))) {
        case CAPTEUR_LECTURE_OK:
            self->echecs_consecutifs = 0;
            communication_moniteur_csv_envoyer_mesure(self->sortie, to_ms_since_boot(get_absolute_time()),
                                                      self->capteur);
            break;
        case CAPTEUR_LECTURE_PAS_PRETE:
            // Courte pause pour ne pas saturer le bus (une mesure toutes les 9,6 ms)
            sleep_us(500);
            break;
        case CAPTEUR_LECTURE_ERREUR:
            traiter_echec(self);
            break;
    }
}

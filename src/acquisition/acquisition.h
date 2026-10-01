// Boucle d'acquisition : lit le capteur, envoie les mesures, récupère un bus I2C bloqué
#ifndef ACQUISITION_H
#define ACQUISITION_H

#include <stdbool.h>
#include <stdint.h>

#include "bus/bus_i2c.h"
#include "capteur/capteur_mouvement.h"
#include "communication/communication_moniteur_csv.h"
#include "signalisation/signalisation_led.h"

// Nombre d'échecs I2C consécutifs au-delà duquel le bus est considéré bloqué
#define ACQUISITION_ECHECS_AVANT_DEBLOCAGE 10

typedef struct {
    const bus_i2c_t *bus;
    uint8_t adresse_capteur;       // adresse I2C du capteur, pour le diagnostic du câblage
    capteur_mouvement_t *capteur;  // n'importe quel capteur de mouvement (LSM6DSOX ou autre)
    signalisation_led_t *led;
    communication_moniteur_csv_t *sortie;
    uint32_t echecs_consecutifs;   // remis à 0 à chaque lecture réussie
    bool capteur_pret;             // false = (ré)initialisation nécessaire
} acquisition_t;

// Un tour de boucle : en-tête si nouvelle connexion, lecture, envoi ou gestion d'erreur
void acquisition_executer_cycle(acquisition_t *self);

// Compte un échec ; true (et compteur remis à 0) quand le seuil de déblocage est atteint
bool acquisition_enregistrer_echec(acquisition_t *self);

#endif

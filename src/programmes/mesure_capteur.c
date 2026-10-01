// Programme de mesure du capteur : flux CSV de l'accéléromètre LSM6DSOX sur l'USB série
// Câblage : SDA = GP4 (broche 6), SCL = GP5 (broche 7), 3V3 (broche 36), GND (broche 38)
#include "acquisition/acquisition.h"
#include "capteur/capteur_mouvement_lsm6dsox.h"
#include "pico/stdlib.h"

// GP4/GP5 appartiennent au contrôleur I2C0 ; 400 kHz ; 2 ms max par transaction
static const bus_i2c_t bus = {
    .port = i2c0,
    .broche_sda = 4,
    .broche_scl = 5,
    .frequence_hz = 400000,
    .delai_max_us = 2000,
};

int main(void) {
    // USB série : le Pico apparaît comme /dev/tty.usbmodemXXX
    stdio_init_all();
    static signalisation_led_t led;
    if (!signalisation_led_initialiser(&led)) return -1;
    bus_i2c_demarrer(&bus);

    static capteur_mouvement_lsm6dsox_t capteur;
    capteur_mouvement_lsm6dsox_creer(&capteur, &bus, CAPTEUR_MOUVEMENT_LSM6DSOX_ADRESSE_DEFAUT);
    static communication_moniteur_csv_t sortie;
    acquisition_t acquisition = {
        .bus = &bus,
        .adresse_capteur = capteur.adresse,
        .capteur = &capteur.parent,
        .led = &led,
        .sortie = &sortie,
    };

    while (true) {
        acquisition_executer_cycle(&acquisition);
    }
}

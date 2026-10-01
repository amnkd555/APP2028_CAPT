// Bus I2C matériel du RP2350 : implémentation
#include "bus/bus_i2c.h"

#include "pico/stdlib.h"

void bus_i2c_demarrer(const bus_i2c_t *bus) {
    // Initialise le contrôleur à la fréquence demandée
    i2c_init(bus->port, bus->frequence_hz);
    // Connecte les deux broches au contrôleur I2C (au lieu du GPIO simple)
    gpio_set_function(bus->broche_sda, GPIO_FUNC_I2C);
    gpio_set_function(bus->broche_scl, GPIO_FUNC_I2C);
    // Pull-up internes ; le breakout en a déjà, celles-ci s'y ajoutent
    gpio_pull_up(bus->broche_sda);
    gpio_pull_up(bus->broche_scl);
}

bool bus_i2c_ecrire_registre(const bus_i2c_t *bus, uint8_t adresse, uint8_t registre, uint8_t valeur) {
    // Trame I2C : [adresse du registre][valeur], suivie d'un STOP
    uint8_t trame[2] = {registre, valeur};
    return i2c_write_timeout_us(bus->port, adresse, trame, 2, false, bus->delai_max_us) == 2;
}

bool bus_i2c_lire_registres(const bus_i2c_t *bus, uint8_t adresse, uint8_t registre,
                            uint8_t *destination, size_t n) {
    // 1) Envoie l'adresse du registre sans STOP (nostop = true) pour enchaîner sur un RESTART
    if (i2c_write_timeout_us(bus->port, adresse, &registre, 1, true, bus->delai_max_us) != 1) {
        return false;
    }
    // 2) Lit n octets ; le composant avance seul au registre suivant (auto-incrément)
    return i2c_read_timeout_us(bus->port, adresse, destination, n, false, bus->delai_max_us) == (int)n;
}

bool bus_i2c_sonder(const bus_i2c_t *bus, uint8_t adresse) {
    // Une lecture d'un octet suffit : seul l'acquittement de l'adresse compte
    uint8_t octet;
    return i2c_read_timeout_us(bus->port, adresse, &octet, 1, false, bus->delai_max_us) == 1;
}

// Envoie jusqu'à 9 coups d'horloge : le composant termine l'octet en cours et relâche SDA
static void liberer_sda(const bus_i2c_t *bus) {
    // SDA en entrée avec pull-up : on observe si le composant la tire à 0
    gpio_init(bus->broche_sda);
    gpio_set_dir(bus->broche_sda, GPIO_IN);
    gpio_pull_up(bus->broche_sda);
    // SCL en sortie, au niveau haut au repos
    gpio_init(bus->broche_scl);
    gpio_put(bus->broche_scl, 1);
    gpio_set_dir(bus->broche_scl, GPIO_OUT);
    for (int i = 0; i < 9 && !gpio_get(bus->broche_sda); i++) {
        gpio_put(bus->broche_scl, 0);
        sleep_us(5);
        gpio_put(bus->broche_scl, 1);
        sleep_us(5);
    }
}

// Condition STOP manuelle : SDA monte pendant que SCL est haut, le bus redevient libre
static void generer_stop(const bus_i2c_t *bus) {
    gpio_put(bus->broche_sda, 0);
    gpio_set_dir(bus->broche_sda, GPIO_OUT);
    sleep_us(5);
    gpio_set_dir(bus->broche_sda, GPIO_IN);
    sleep_us(5);
}

void bus_i2c_debloquer(const bus_i2c_t *bus) {
    // Arrête le contrôleur pour piloter les broches à la main
    i2c_deinit(bus->port);
    liberer_sda(bus);
    generer_stop(bus);
    // Rend les broches au contrôleur
    bus_i2c_demarrer(bus);
}

// Bus I2C simulé : implémente l'interface de bus_i2c.h sans matériel
#include "bus_i2c_simule.h"

#include <string.h>

uint8_t bus_i2c_simule_registres[256];
uint8_t bus_i2c_simule_adresse = 0x6A;
bool bus_i2c_simule_muet = false;

void bus_i2c_simule_reinitialiser(void) {
    memset(bus_i2c_simule_registres, 0, sizeof bus_i2c_simule_registres);
    bus_i2c_simule_adresse = 0x6A;
    bus_i2c_simule_muet = false;
}

static bool composant_repond(uint8_t adresse) {
    return !bus_i2c_simule_muet && adresse == bus_i2c_simule_adresse;
}

void bus_i2c_demarrer(const bus_i2c_t *bus) {
    (void)bus;
}

bool bus_i2c_ecrire_registre(const bus_i2c_t *bus, uint8_t adresse, uint8_t registre, uint8_t valeur) {
    (void)bus;
    if (!composant_repond(adresse)) return false;
    bus_i2c_simule_registres[registre] = valeur;
    return true;
}

bool bus_i2c_lire_registres(const bus_i2c_t *bus, uint8_t adresse, uint8_t registre,
                            uint8_t *destination, size_t n) {
    (void)bus;
    if (!composant_repond(adresse)) return false;
    // Auto-incrément : lit les registres consécutifs
    for (size_t i = 0; i < n; i++) {
        destination[i] = bus_i2c_simule_registres[(uint8_t)(registre + i)];
    }
    return true;
}

bool bus_i2c_sonder(const bus_i2c_t *bus, uint8_t adresse) {
    (void)bus;
    return composant_repond(adresse);
}

void bus_i2c_debloquer(const bus_i2c_t *bus) {
    (void)bus;
}

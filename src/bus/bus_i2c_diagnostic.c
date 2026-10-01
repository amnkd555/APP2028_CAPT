// Diagnostic du câblage d'un bus I2C : implémentation
// (pas de test par pull-down : l'erratum RP2350-E9 le rend non fiable)
#include "bus/bus_i2c_diagnostic.h"

#include <stdio.h>

#include "pico/stdlib.h"

// Met une ligne à 0 (sortie basse) ou la relâche (entrée : la pull-up la remonte à 1)
static void ligne_ecrire(uint broche, bool niveau) {
    gpio_set_dir(broche, niveau ? GPIO_IN : GPIO_OUT);
    sleep_us(10);
}

// Prépare deux broches en « collecteur ouvert » simulé, bus au repos (lignes à 1)
static void preparer_lignes(uint sda, uint scl) {
    uint broches[2] = {sda, scl};
    for (int i = 0; i < 2; i++) {
        gpio_init(broches[i]);
        gpio_put(broches[i], 0);
        gpio_pull_up(broches[i]);
    }
    ligne_ecrire(sda, 1);
    ligne_ecrire(scl, 1);
}

// Envoie un bit : SDA positionnée pendant que SCL est bas, lue par le composant sur front montant
static void envoyer_bit(uint sda, uint scl, bool bit) {
    ligne_ecrire(sda, bit);
    ligne_ecrire(scl, 1);
    ligne_ecrire(scl, 0);
}

// I2C logiciel : true si un composant acquitte l'adresse sur les broches sda/scl choisies
static bool sonder_logiciel(uint sda, uint scl, uint8_t adresse) {
    preparer_lignes(sda, scl);
    // START : SDA descend pendant que SCL est haut
    ligne_ecrire(sda, 0);
    ligne_ecrire(scl, 0);
    // Adresse sur 7 bits + bit R/W = 0 (écriture), poids fort en premier
    uint8_t octet = (uint8_t)(adresse << 1);
    for (int bit = 7; bit >= 0; bit--) {
        envoyer_bit(sda, scl, (octet >> bit) & 1);
    }
    // 9e coup d'horloge : SDA relâchée, le composant la tire à 0 s'il acquitte (ACK)
    ligne_ecrire(sda, 1);
    ligne_ecrire(scl, 1);
    bool acquitte = !gpio_get(sda);
    ligne_ecrire(scl, 0);
    // STOP : SDA monte pendant que SCL est haut
    ligne_ecrire(sda, 0);
    ligne_ecrire(scl, 1);
    ligne_ecrire(sda, 1);
    return acquitte;
}

// Sonde le composant avec le câblage prévu puis avec SDA et SCL inversés
static void afficher_sondes_logicielles(const bus_i2c_t *bus, uint8_t adresse) {
    i2c_deinit(bus->port);
    bool normal = sonder_logiciel(bus->broche_sda, bus->broche_scl, adresse);
    bool inverse = sonder_logiciel(bus->broche_scl, bus->broche_sda, adresse);
    printf("# diag : I2C logiciel 0x%02X : cablage normal %s, fils SDA/SCL inverses %s\n",
           adresse, normal ? "REPOND" : "muet", inverse ? "REPOND" : "muet");
    bus_i2c_demarrer(bus);
}

// Balaye toutes les adresses I2C valides et liste celles qui acquittent
static void afficher_scan(const bus_i2c_t *bus) {
    printf("# diag : adresses qui repondent :");
    int trouvees = 0;
    for (uint8_t adresse = 0x08; adresse < 0x78; adresse++) {
        if (bus_i2c_sonder(bus, adresse)) {
            printf(" 0x%02X", adresse);
            trouvees++;
        }
    }
    printf("%s\n", trouvees ? "" : " aucune");
}

void bus_i2c_diagnostic_afficher(const bus_i2c_t *bus, uint8_t adresse_attendue) {
    afficher_sondes_logicielles(bus, adresse_attendue);
    afficher_scan(bus);
}

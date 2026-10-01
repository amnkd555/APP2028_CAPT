// Accéléromètre LSM6DSOX : implémentation des méthodes de capteur
#include "capteur/capteur_mouvement_lsm6dsox.h"

#include <stdio.h>

#include "pico/stdlib.h"

// Remonte de la classe parent (capteur_t) à l'enfant : valide car le parent est en premier membre
static capteur_mouvement_lsm6dsox_t *depuis_capteur(capteur_t *capteur) {
    return (capteur_mouvement_lsm6dsox_t *)capteur;
}

static bool ecrire(capteur_mouvement_lsm6dsox_t *self, uint8_t registre, uint8_t valeur) {
    return bus_i2c_ecrire_registre(self->bus, self->adresse, registre, valeur);
}

static bool lire(capteur_mouvement_lsm6dsox_t *self, uint8_t registre, uint8_t *destination, size_t n) {
    return bus_i2c_lire_registres(self->bus, self->adresse, registre, destination, n);
}

// Lit WHO_AM_I : true seulement si c'est bien un LSM6DSOX qui répond
static bool verifier_identite(capteur_mouvement_lsm6dsox_t *self) {
    uint8_t id = 0;
    if (!lire(self, CAPTEUR_MOUVEMENT_LSM6DSOX_REG_WHO_AM_I, &id, 1)) {
        printf("# ERREUR : aucun acquittement a l'adresse 0x%02X (verifier cablage)\n", self->adresse);
        return false;
    }
    if (id != CAPTEUR_MOUVEMENT_LSM6DSOX_WHO_AM_I) {
        printf("# ERREUR : WHO_AM_I = 0x%02X, attendu 0x%02X\n", id, CAPTEUR_MOUVEMENT_LSM6DSOX_WHO_AM_I);
        return false;
    }
    printf("# WHO_AM_I = 0x%02X : LSM6DSOX detecte\n", id);
    return true;
}

// Reset puis configuration : BDU + auto-incrément, I3C désactivé, 104 Hz ±2 g
static bool configurer(capteur_mouvement_lsm6dsox_t *self) {
    if (!ecrire(self, CAPTEUR_MOUVEMENT_LSM6DSOX_REG_CTRL3_C, CAPTEUR_MOUVEMENT_LSM6DSOX_CTRL3_RESET)) return false;
    // Le reset dure environ 50 µs ; 10 ms laisse une large marge
    sleep_ms(10);
    if (!ecrire(self, CAPTEUR_MOUVEMENT_LSM6DSOX_REG_CTRL3_C, CAPTEUR_MOUVEMENT_LSM6DSOX_CTRL3_BDU_INC)) return false;
    if (!ecrire(self, CAPTEUR_MOUVEMENT_LSM6DSOX_REG_CTRL9_XL, CAPTEUR_MOUVEMENT_LSM6DSOX_CTRL9_SANS_I3C)) return false;
    return ecrire(self, CAPTEUR_MOUVEMENT_LSM6DSOX_REG_CTRL1_XL, CAPTEUR_MOUVEMENT_LSM6DSOX_CTRL1_104HZ_2G);
}

// Relit CTRL1_XL pour vérifier que la configuration a bien été prise en compte
static bool verifier_configuration(capteur_mouvement_lsm6dsox_t *self) {
    uint8_t relu = 0;
    if (!lire(self, CAPTEUR_MOUVEMENT_LSM6DSOX_REG_CTRL1_XL, &relu, 1) ||
        relu != CAPTEUR_MOUVEMENT_LSM6DSOX_CTRL1_104HZ_2G) {
        printf("# ERREUR : CTRL1_XL relu = 0x%02X\n", relu);
        return false;
    }
    return true;
}

static bool initialiser(capteur_t *capteur) {
    capteur_mouvement_lsm6dsox_t *self = depuis_capteur(capteur);
    return verifier_identite(self) && configurer(self) && verifier_configuration(self);
}

static capteur_lecture_t lire_mesure(capteur_t *capteur) {
    capteur_mouvement_lsm6dsox_t *self = depuis_capteur(capteur);
    // STATUS bit XLDA : une nouvelle mesure est-elle prête ? (une toutes les 9,6 ms)
    uint8_t status = 0;
    if (!lire(self, CAPTEUR_MOUVEMENT_LSM6DSOX_REG_STATUS, &status, 1)) return CAPTEUR_LECTURE_ERREUR;
    if (!(status & CAPTEUR_MOUVEMENT_LSM6DSOX_STATUS_XLDA)) return CAPTEUR_LECTURE_PAS_PRETE;
    // Les 6 octets XL, XH, YL, YH, ZL, ZH en une seule transaction
    uint8_t brut[6];
    if (!lire(self, CAPTEUR_MOUVEMENT_LSM6DSOX_REG_OUTX_L_A, brut, sizeof brut)) return CAPTEUR_LECTURE_ERREUR;
    for (int axe = 0; axe < CAPTEUR_MOUVEMENT_NB_AXES; axe++) {
        self->parent.acceleration_g[axe] = capteur_mouvement_lsm6dsox_convertir_g(brut[2 * axe], brut[2 * axe + 1]);
    }
    return CAPTEUR_LECTURE_OK;
}

// Table des méthodes de l'enfant, partagée par toutes les instances
static const capteur_operations_t operations = {
    .initialiser = initialiser,
    .lire = lire_mesure,
};

void capteur_mouvement_lsm6dsox_creer(capteur_mouvement_lsm6dsox_t *self, const bus_i2c_t *bus, uint8_t adresse) {
    *self = (capteur_mouvement_lsm6dsox_t){0};
    self->parent.parent.operations = &operations;
    self->parent.parent.nom = "LSM6DSOX";
    self->bus = bus;
    self->adresse = adresse;
}

float capteur_mouvement_lsm6dsox_convertir_g(uint8_t octet_bas, uint8_t octet_haut) {
    // Octet haut << 8 | octet bas, interprété comme entier signé 16 bits
    int16_t valeur = (int16_t)((octet_haut << 8) | octet_bas);
    return valeur * CAPTEUR_MOUVEMENT_LSM6DSOX_G_PAR_LSB;
}

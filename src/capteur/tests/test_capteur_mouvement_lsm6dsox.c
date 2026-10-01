// SOURCES: src/capteur/capteur.c src/capteur/capteur_mouvement.c src/capteur/capteur_mouvement_lsm6dsox.c src/mocks/bus_i2c_simule.c src/mocks/pico_simule.c
// Tests de capteur_mouvement_lsm6dsox sur un bus I2C simulé
#include <assert.h>
#include <math.h>
#include <stdio.h>

#include "bus_i2c_simule.h"
#include "capteur/capteur_mouvement_lsm6dsox.h"

static const bus_i2c_t bus = {0};
static capteur_mouvement_lsm6dsox_t capteur;

// Capteur neuf sur un bus simulé, WHO_AM_I à la bonne valeur
static capteur_t *preparer(void) {
    bus_i2c_simule_reinitialiser();
    bus_i2c_simule_registres[CAPTEUR_MOUVEMENT_LSM6DSOX_REG_WHO_AM_I] = CAPTEUR_MOUVEMENT_LSM6DSOX_WHO_AM_I;
    capteur_mouvement_lsm6dsox_creer(&capteur, &bus, CAPTEUR_MOUVEMENT_LSM6DSOX_ADRESSE_DEFAUT);
    return &capteur.parent.parent;
}

static void test_conversion(void) {
    assert(capteur_mouvement_lsm6dsox_convertir_g(0x00, 0x00) == 0.0f);
    // 0xFFFF = -1 LSB en complément à 2
    assert(fabsf(capteur_mouvement_lsm6dsox_convertir_g(0xFF, 0xFF) + 0.000061f) < 1e-9f);
    // 0x4009 = 16393 LSB ≈ +1 g
    assert(fabsf(capteur_mouvement_lsm6dsox_convertir_g(0x09, 0x40) - 1.0f) < 1e-3f);
}

static void test_initialisation_configure_les_registres(void) {
    assert(capteur_initialiser(preparer()));
    assert(bus_i2c_simule_registres[CAPTEUR_MOUVEMENT_LSM6DSOX_REG_CTRL1_XL] == 0x40);
    assert(bus_i2c_simule_registres[CAPTEUR_MOUVEMENT_LSM6DSOX_REG_CTRL3_C] == 0x44);
    assert(bus_i2c_simule_registres[CAPTEUR_MOUVEMENT_LSM6DSOX_REG_CTRL9_XL] == 0xE2);
}

static void test_initialisation_refuse_mauvais_composant(void) {
    capteur_t *c = preparer();
    bus_i2c_simule_registres[CAPTEUR_MOUVEMENT_LSM6DSOX_REG_WHO_AM_I] = 0x69;
    assert(!capteur_initialiser(c));
}

static void test_initialisation_echoue_si_bus_muet(void) {
    capteur_t *c = preparer();
    bus_i2c_simule_muet = true;
    assert(!capteur_initialiser(c));
}

static void test_lecture_pas_prete(void) {
    capteur_t *c = preparer();
    bus_i2c_simule_registres[CAPTEUR_MOUVEMENT_LSM6DSOX_REG_STATUS] = 0x00;
    assert(capteur_lire(c) == CAPTEUR_LECTURE_PAS_PRETE);
}

static void test_lecture_mesure(void) {
    capteur_t *c = preparer();
    uint8_t *r = &bus_i2c_simule_registres[CAPTEUR_MOUVEMENT_LSM6DSOX_REG_OUTX_L_A];
    // X = 0, Y = -1 LSB, Z ≈ +1 g
    uint8_t brut[6] = {0x00, 0x00, 0xFF, 0xFF, 0x09, 0x40};
    for (int i = 0; i < 6; i++) r[i] = brut[i];
    bus_i2c_simule_registres[CAPTEUR_MOUVEMENT_LSM6DSOX_REG_STATUS] = CAPTEUR_MOUVEMENT_LSM6DSOX_STATUS_XLDA;
    assert(capteur_lire(c) == CAPTEUR_LECTURE_OK);
    assert(capteur.parent.acceleration_g[CAPTEUR_MOUVEMENT_AXE_X] == 0.0f);
    assert(capteur.parent.acceleration_g[CAPTEUR_MOUVEMENT_AXE_Y] < 0.0f);
    assert(fabsf(capteur.parent.acceleration_g[CAPTEUR_MOUVEMENT_AXE_Z] - 1.0f) < 1e-3f);
}

static void test_lecture_erreur_si_bus_muet(void) {
    capteur_t *c = preparer();
    bus_i2c_simule_muet = true;
    assert(capteur_lire(c) == CAPTEUR_LECTURE_ERREUR);
}

int main(void) {
    test_conversion();
    test_initialisation_configure_les_registres();
    test_initialisation_refuse_mauvais_composant();
    test_initialisation_echoue_si_bus_muet();
    test_lecture_pas_prete();
    test_lecture_mesure();
    test_lecture_erreur_si_bus_muet();
    printf("test_capteur_mouvement_lsm6dsox : OK\n");
    return 0;
}

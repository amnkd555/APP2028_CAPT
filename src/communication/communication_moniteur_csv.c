// Flux CSV vers le moniteur série : implémentation
#include "communication/communication_moniteur_csv.h"

#include <inttypes.h>
#include <stdio.h>

int communication_moniteur_csv_formater_mesure(char *tampon, size_t taille, uint32_t t_ms,
                                               const capteur_mouvement_t *capteur) {
    // 5 décimales : la résolution du capteur est de 0,000061 g
    return snprintf(tampon, taille, "%" PRIu32 ",%.5f,%.5f,%.5f,%.5f\n", t_ms,
                    (double)capteur->acceleration_g[CAPTEUR_MOUVEMENT_AXE_X],
                    (double)capteur->acceleration_g[CAPTEUR_MOUVEMENT_AXE_Y],
                    (double)capteur->acceleration_g[CAPTEUR_MOUVEMENT_AXE_Z],
                    (double)capteur_mouvement_norme_g(capteur));
}

void communication_moniteur_csv_envoyer_entete_si_connexion(communication_moniteur_csv_t *self) {
    if (!communication_moniteur_nouvelle_connexion(&self->parent)) return;
    // Les lignes commençant par # sont des commentaires, ignorés à l'analyse
    printf("# Anti-Sedentarite - mesure du capteur LSM6DSOX (+/-2 g, 104 Hz)\n");
    printf("# erreurs I2C : %" PRIu32 ", reinitialisations : %" PRIu32 "\n",
           self->erreurs_i2c, self->reinitialisations);
    printf("timestamp_ms,ax_g,ay_g,az_g,mag_g\n");
}

void communication_moniteur_csv_envoyer_mesure(communication_moniteur_csv_t *self, uint32_t t_ms,
                                               const capteur_mouvement_t *capteur) {
    (void)self;
    char ligne[96];
    communication_moniteur_csv_formater_mesure(ligne, sizeof ligne, t_ms, capteur);
    fputs(ligne, stdout);
}

void communication_moniteur_csv_signaler_erreur(communication_moniteur_csv_t *self) {
    self->erreurs_i2c++;
    printf("# erreur I2C (total %" PRIu32 ")\n", self->erreurs_i2c);
}

void communication_moniteur_csv_signaler_reinitialisation(communication_moniteur_csv_t *self) {
    self->reinitialisations++;
    printf("# bus I2C bloque : deblocage et reinitialisation n.%" PRIu32 "\n", self->reinitialisations);
}

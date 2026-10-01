// Enfant de communication_moniteur : flux CSV des mesures du capteur + lignes de commentaire « # »
#ifndef COMMUNICATION_MONITEUR_CSV_H
#define COMMUNICATION_MONITEUR_CSV_H

#include <stddef.h>
#include <stdint.h>

#include "capteur/capteur_mouvement.h"
#include "communication/communication_moniteur.h"

typedef struct {
    communication_moniteur_t parent;   // toujours en premier
    uint32_t erreurs_i2c;              // transactions I2C ratées depuis le démarrage
    uint32_t reinitialisations;        // déblocages du bus depuis le démarrage
} communication_moniteur_csv_t;

// Fonction pure : écrit « t_ms,ax,ay,az,norme » dans tampon ; renvoie la longueur (comme snprintf)
int communication_moniteur_csv_formater_mesure(char *tampon, size_t taille, uint32_t t_ms,
                                               const capteur_mouvement_t *capteur);

// Envoie l'en-tête CSV si un moniteur vient d'ouvrir le port
void communication_moniteur_csv_envoyer_entete_si_connexion(communication_moniteur_csv_t *self);

// Envoie une ligne de mesure
void communication_moniteur_csv_envoyer_mesure(communication_moniteur_csv_t *self, uint32_t t_ms,
                                               const capteur_mouvement_t *capteur);

// Compte et signale une transaction I2C ratée
void communication_moniteur_csv_signaler_erreur(communication_moniteur_csv_t *self);

// Compte et signale un déblocage du bus
void communication_moniteur_csv_signaler_reinitialisation(communication_moniteur_csv_t *self);

#endif

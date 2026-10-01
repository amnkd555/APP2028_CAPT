// Classe parent des sorties vers le moniteur série USB : détecte l'ouverture du port
// (un message envoyé avant l'ouverture est perdu, d'où l'intérêt de la détecter)
#ifndef COMMUNICATION_MONITEUR_H
#define COMMUNICATION_MONITEUR_H

#include <stdbool.h>

typedef struct {
    bool connecte;   // un moniteur écoutait-il au dernier appel ?
} communication_moniteur_t;

// Fonction pure : true si connecte_maintenant passe de faux à vrai depuis le dernier appel
bool communication_moniteur_detecter_connexion(communication_moniteur_t *self, bool connecte_maintenant);

// Interroge l'USB : true juste après l'ouverture du port par un moniteur série
bool communication_moniteur_nouvelle_connexion(communication_moniteur_t *self);

#endif

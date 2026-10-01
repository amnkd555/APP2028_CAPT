// Classe parent de tous les capteurs : interface commune (initialiser, lire)
// Les enfants placent un capteur_t en PREMIER membre et fournissent leur table d'opérations.
#ifndef CAPTEUR_H
#define CAPTEUR_H

#include <stdbool.h>

typedef struct capteur capteur_t;

// Résultat d'une lecture
typedef enum {
    CAPTEUR_LECTURE_OK,          // nouvelle mesure disponible dans l'enfant
    CAPTEUR_LECTURE_PAS_PRETE,   // pas encore de nouvelle mesure, réessayer plus tard
    CAPTEUR_LECTURE_ERREUR       // échec de communication avec le capteur
} capteur_lecture_t;

// Méthodes redéfinies par chaque enfant (équivalent des méthodes virtuelles)
typedef struct {
    bool (*initialiser)(capteur_t *self);
    capteur_lecture_t (*lire)(capteur_t *self);
} capteur_operations_t;

struct capteur {
    const capteur_operations_t *operations;   // implémentation de l'enfant
};

// Vérifie et configure le capteur ; true s'il est prêt à mesurer
bool capteur_initialiser(capteur_t *self);

// Lit une nouvelle mesure, rangée dans la structure de l'enfant
capteur_lecture_t capteur_lire(capteur_t *self);

#endif

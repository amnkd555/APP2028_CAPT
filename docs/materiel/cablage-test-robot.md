# Câblage — test sur le robot UR3 (module Adafruit)

Branchement des 4 fils et consignes de montage : [dossier-technique.md, §4.1](../conception/dossier-technique.md).

## 1. Matériel

| Élément | Rôle |
|---|---|
| Ordinateur avec `scripts/perturbations.py` | Enregistre les mesures |
| Entretoises **non métalliques** (PLA, bois) | Fixent le capteur à une distance précise du moteur |
| Bras robotisé UR3 | Source de perturbation testée |

## 2. Vérifications avant de mettre sous tension

| OK | Point à vérifier |
|---|---|
| [ ] | USB débranché pendant le câblage |
| [ ] | Rouge sur la broche **36**, pas sur 37, 39 ou 40 (3V3/GND et GP4/GP5 sont sur deux rangées opposées du Pico) |
| [ ] | Pas de court-circuit entre 3V3 et GND (multimètre, mode continuité) |
| [ ] | Bleu et jaune sur **SDA/SCL** du module, pas sur SDX/SCX |
| [ ] | Après branchement : LED du Pico **fixe** (clignotement rapide = capteur non trouvé) |
| [ ] | Si le Pico **chauffe** : débrancher l'USB immédiatement (court-circuit) |

## 3. Installation sur le robot

| Consigne | Pourquoi |
|---|---|
| Distance mesurée du **centre du capteur** au **carter du moteur** | Même repère pour toutes les mesures |
| Même position du robot pour tous les essais | Seule la distance doit changer |
| Arrêt d'urgence à portée de main, personne dans la zone du robot | Sécurité |

## 4. Déroulé d'une mesure

| Étape | Robot | Commande |
|---|---|---|
| 1 | **Hors tension** (freins serrés) | `python3 scripts/perturbations.py capture d050_off_r1 --duree 30` |
| 2 | **Sous tension**, position maintenue | `python3 scripts/perturbations.py capture d050_on_r1 --duree 30` |
| 3 | Répéter 3 fois, puis changer de distance (0, 10, 20, 30, 50, 75, 100, 150 mm) | |
| 4 | Fin des mesures | `python3 scripts/perturbations.py analyse` |

`d050` = distance en mm, `off`/`on` = robot éteint/allumé, `r1` = répétition n°1.
Fermer le Serial Monitor de VS Code avant de lancer `perturbations.py`.


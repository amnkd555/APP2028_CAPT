# Dispositif de Mesure de la Sédentarité

## 1. Contexte

Ce projet consiste en la conception d'un dispositif électronique capable de mesurer et d'analyser le niveau d'activité physique d'un individu afin de détecter les phases de sédentarité. Le système s'appuie sur un accéléromètre et un gyroscope pour capturer les mouvements en trois dimensions.

## 2. Choix du capteur

| Critère d'analyse | Solution Basique (ADXL335 + ENC-03RC) | Solution Avancée (LSM6DSOX) |
|---|---|---|
| **Architecture** | Composants séparés (Accéléromètre + Gyromètre distincts) | Centrale inertielle unifiée (6 axes : Accéléro + Gyro) |
| **Signal de sortie** | Analogique (Tension brute nécessitant un ADC) | Numérique (Bus I2C/SPI) |
| **Capteur de température** | Absent (Sensible à la dérive thermique) | Intégré (permet de corriger la dérive thermique dans le programme) |
| **Consommation énergétique** | Pas de mode veille intelligent (Système toujours actif) | Très faible (modes basse consommation, optimisé pour les appareils portables) |
| **Traitement embarqué** | Aucun traitement matériel intégré | Intégration d'un "Machine Learning Core" (MLC) |

## 3. Architecture Matérielle

| Composant | Fonction / Rôle | Remarques / Utilité technique |
|---|---|---|
| **Raspberry Pi Pico 2 W** | Microcontrôleur principal | Alimente les capteurs, récupère les mesures, exécute le programme d'analyse et gère le transfert/stockage. Puce RP2350 avec module Wi-Fi. |
| **PiCowbell Adalogger** | Carte d'extension | Ajoute un lecteur de carte micro-SD pour le stockage et une horloge RTC (PCF8523) pour l'horodatage des mesures. |
| **LSM6DSOX** | Capteur de mouvement (centrale inertielle 6 axes) | Accéléromètre + Gyroscope. Mesure les accélérations et rotations. Communique en I²C avec le Pico. Pour les tests, il est monté sur un module Adafruit qui intègre régulateur et résistances de tirage. |
| **Pile bouton CR1220** | Alimentation de l'horloge interne (RTC) | Maintient la date et l'heure exactes en mémoire, même lorsque le dispositif principal est éteint. |
| **Câble STEMMA QT/QWIIC** | Connectique rapide (4 fils) | Transporte l'alimentation (VCC, GND) et les données (SDA, SCL) du bus I²C. Relie le module capteur à la PiCowbell ; ne se branche pas directement sur les broches du Pico. |
| **Headers femelles** | Connecteurs mécaniques/électriques | Servent à emboîter solidement le Pico sur la carte d'extension PiCowbell. |
| **Câble USB** | Communication PC et alimentation | Utilisé pour le développement, l'affichage en temps réel sur le moniteur série et l'alimentation lors des tests sur le robot. |
| **Alimentation autonome** | Piles AA ou Batterie LiPo | (Évolution future) Source d'énergie principale pour remplacer le câble USB et rendre le capteur portable. |

## 4. Schéma de Connexion

La communication entre le Raspberry Pi Pico 2 W et le capteur LSM6DSOX utilise le protocole I2C, sur le port I2C0. Deux configurations sont prévues : une configuration de test avec le module Adafruit, utilisée pour les essais sur le robot UR3, et une configuration finale avec le capteur seul, soudé sur notre propre carte électronique.

### 4.1 Configuration de test sur le robot UR3 (module Adafruit LSM6DSOX)

Le module Adafruit intègre déjà les composants nécessaires au capteur (condensateurs, résistances de tirage, régulateur). Seuls quatre fils sont à brancher.

| Broche Capteur | Connexion Raspberry Pi Pico 2 W | Fil | Rôle de la broche |
|---|---|---|---|
| VIN | 3V3 (broche 36) | rouge | Alimentation électrique du capteur. |
| GND | GND (broche 38) | noir | Masse commune. |
| SCL | GP5 (broche 7) – I2C0 SCL | jaune | L'horloge commune pour synchroniser les données. |
| SDA | GP4 (broche 6) – I2C0 SDA | bleu | La ligne de transfert des données. |

| Broches du module non connectées | Raison |
|---|---|
| 3Vo, DO, CS, I1, I2 | Inutiles pour une liaison I2C simple (réglages déjà faits sur le module). Laisser DO libre conserve l'adresse 0x6A. |
| SCX, SDX | Bus auxiliaire pour un capteur supplémentaire, non utilisé. |

| Consigne | Raison |
|---|---|
| Souder le header du module | Un header simplement enfiché provoque des pertes de contact (vibrations du robot). |
| Sur la breadboard, un seul fil par ligne de 5 trous | Les trous d'une même ligne sont reliés : deux fils dans la même ligne créent un court-circuit. |
| Fixer le capteur sur des entretoises non métalliques | Le métal modifierait les perturbations mesurées. |
| Garder le même trajet du câble USB à chaque essai | Le câble capte lui aussi les perturbations du robot. |

### 4.2 Configuration finale (capteur LSM6DSOX seul sur la carte électronique)

Sur la carte finale, le capteur est soudé directement, sans le module Adafruit. Les composants que le module contenait doivent donc être ajoutés.

| Broche Capteur (n°) | Connexion Pico / Circuit | Rôle & Instructions |
|---|---|---|
| VDD (8) | 3V3 (Pico) | Alimentation principale. Ajouter C1 (100 nF) à la masse. |
| VDDIO (5) | 3V3 (Pico) | Alimentation des entrées/sorties. Ajouter C2 (100 nF) à la masse. |
| GND (6, 7) | GND (Pico) | Masse commune du système. |
| SDA (14) | GP4 (I2C0 SDA) | Données I2C. Ajouter une résistance de tirage de 10 kΩ au 3V3. |
| SCL (13) | GP5 (I2C0 SCL) | Horloge I2C. Ajouter une résistance de tirage de 10 kΩ au 3V3. |
| CS (12) | 3V3 | Sélectionne le mode I2C (à la masse, le capteur passe en mode SPI). |
| SDO/SA0 (1) | GND | Fixe l'adresse I2C à 0x6A. |
| INT1 (4) | GP2 | Interruption de réveil : sort le Pico de veille profonde. |
| SDx (2) | GND | Inutilisé. Relier à la masse. |
| SCx (3) | GND | Inutilisé. Relier à la masse. |
| INT2 (9) | NC (isolé) | Inutilisé. Ne pas connecter. |
| OCS_Aux (10) | NC (isolé) | Inutilisé. Ne pas connecter. |
| SDO_Aux (11) | NC (isolé) | Inutilisé. Ne pas connecter. |

**Composants passifs indispensables :**

| Composant | Valeur | Emplacement | Utilité |
|---|---|---|---|
| C1 | 100 nF | Entre VDD (8) et GND | Filtrage de l'alimentation du capteur. |
| C2 | 100 nF | Entre VDDIO (5) et GND | Filtrage de l'alimentation des entrées/sorties. |
| R_pu1 | 10 kΩ | Entre SDA (14) et 3V3 | Résistance de tirage (pull-up) I2C. |
| R_pu2 | 10 kΩ | Entre SCL (13) et 3V3 | Résistance de tirage (pull-up) I2C. |

Les condensateurs doivent être placés au plus près des broches du capteur. Une seule paire de résistances de tirage suffit pour tout le bus I2C, même lorsque d'autres modules y seront ajoutés. Prendre 4,7 kΩ au lieu de 10 kΩ si les pistes I2C sont longues.

### 4.3 Modules à ajouter sur la carte finale

| Module | Liaison prévue | Broches Pico |
|---|---|---|
| Capteur LSM6DSOX | I2C0 (adresse 0x6A) | GP4, GP5, GP2 (INT1) |
| Horloge RTC (PCF8523, PiCowbell Adalogger) | I2C0, même bus que le capteur (adresse 0x68) | GP4, GP5 |
| Carte micro-SD (PiCowbell Adalogger) | SPI | GP16 (MISO), GP17 (CS), GP18 (SCK), GP19 (MOSI) |
| Moteur vibrant | Sortie numérique | À définir |
| Batterie | Alimentation | À définir |

Le capteur (0x6A) et l'horloge (0x68) ont des adresses différentes : ils peuvent partager le même bus I2C sans conflit.

### 4.4 Comparaison des deux configurations

| | Test sur robot (module Adafruit) | Carte finale (capteur seul) |
|---|---|---|
| Nombre de connexions | 4 fils | 11 broches à relier |
| Condensateurs | Intégrés au module | À ajouter (C1, C2) |
| Résistances de tirage | Intégrées au module | À ajouter (R_pu1, R_pu2) |
| Adresse 0x6A | Réglée sur le module | SDO/SA0 relié à la masse |
| Mode I2C | Réglé sur le module | CS relié au 3V3 |
| Programme du Pico | Identique | Identique |

## 5. Configuration du Prototype et Phase de Test

### 5.1 Le calcul de la sédentarité (Le score MET)

Pour transformer les mouvements captés en une donnée de santé, le programme calcule l'intensité des mouvements. D'abord, il combine les accélérations des 3 axes (X, Y, Z) pour obtenir l'intensité globale. Ensuite, il convertit ces mouvements en points d'activité ("Counts") sur une durée d'une minute. Enfin, un calcul mathématique transforme ces points en un score MET. Si ce score est inférieur ou égal à 1,5 MET, le système sait que la personne est en situation de sédentarité.

### 5.2 L'intelligence intégrée du capteur

Le capteur LSM6DSOX possède une petite intelligence intégrée (le Machine Learning Core). Au lieu d'envoyer toutes les données en permanence, il est capable de reconnaître tout seul si la personne marche ou si elle est assise. L'avantage principal est d'économiser la batterie : le "cerveau" principal (le Raspberry Pi Pico) peut se mettre en veille. Le capteur ne le réveille que lorsqu'il détecte un changement d'activité ; le Pico mesure alors le temps passé assis et déclenche l'alerte si ce temps est trop long.

### 5.3 Calibration via Bras Robotisé (UR3)

Pour garantir la répétabilité des mesures, les premiers tests sont effectués en fixant le capteur sur un bras robotisé Universal Robots UR3.

Le premier test consiste à déterminer la distance minimale entre le capteur et les moteurs du robot (test des perturbations électromagnétiques). Les mesures sont comparées robot hors tension puis sous tension, à différentes distances ; la distance à partir de laquelle les mesures ne sont plus perturbées sert à dimensionner la pièce support modélisée sur SolidWorks.

L'objectif est ensuite de simuler des séquences de mouvements contrôlées afin de définir les seuils numériques séparant l'activité de l'immobilité.

### 5.4 Configuration "Développement"

Dans cette phase initiale, le dispositif est relié par câble USB à un ordinateur. Cette configuration simplifiée permet :

- L'alimentation directe du Pico et du capteur.
- L'affichage des données en temps réel via le moniteur série.
- Le stockage des données sur le disque dur du PC (rendant la carte SD optionnelle temporairement).

## 6. Choix du langage de programmation

Nous avons choisi le langage C car :

**il maximise l'efficacité énergétique par sa vitesse d'exécution.** Le C est un langage compilé, contrairement à des langages interprétés comme Python. Dans notre architecture, le Raspberry Pi Pico est réveillé par la broche INT1 du capteur, traite l'alerte, puis se rendort. En C, l'exécution de cette routine prend à peine quelques microsecondes. Plus le code s'exécute vite, plus vite le processeur retourne en sommeil profond, ce qui est vital pour l'autonomie de notre batterie.

**il permet un contrôle "bas niveau" et déterministe du matériel.** Le SDK C/C++ du Raspberry Pi Pico offre un accès direct aux registres physiques du microcontrôleur. C'est indispensable pour configurer finement les modes de veille et pour gérer l'interruption matérielle sans latence. Avec le C, on maîtrise exactement ce que fait le processeur à chaque cycle d'horloge.

**c'est le standard industriel des systèmes embarqués critiques.** Pour un dispositif aux ambitions médicales et portables, le C reste la norme dans l'industrie.

## 7. Évolutions et Pistes de Conception

Le projet est conçu pour évoluer vers un produit fini portable. Voici les points matériels et ergonomiques à traiter lors des prochains semestres :

### 7.1 Évolution de l'Autonomie et du Stockage

- Remplacer l'alimentation par câble USB par une batterie rechargeable, afin de rendre le dispositif totalement portable.
- Optimiser l'utilisation de la carte MicroSD pour pouvoir enregistrer les données de mouvement sur plusieurs jours consécutifs sans nécessiter de connexion à un ordinateur.

### 7.2 Ergonomie et Interaction avec l'Utilisateur

Pour répondre au défi d'un port confortable (au poignet ou à la ceinture), nous allons dessiner une coque de protection sur-mesure sur le logiciel SOLIDWORKS. Le but est d'obtenir un boîtier très fin pour éviter de gêner l'utilisateur. À l'intérieur, les cartes électroniques devront être parfaitement calées pour éviter les vibrations. Il faudra également y prévoir la place pour la future batterie et laisser des accès libres pour le port USB et la carte mémoire.

Le but final du projet est d'inciter la personne à bouger. Si l'utilisateur reste assis sans interruption (par exemple pendant 30 minutes), l'appareil devra le prévenir. Pour que cette notification reste discrète (utile au bureau ou en classe), nous prévoyons d'ajouter un retour haptique via un petit moteur de vibration plat (similaire à celui des téléphones), ce qui est bien plus adapté qu'une alarme sonore ou une LED lumineuse.

## 8. Annexe

### 8.1 Datasheets

- LSM6DSOX (STMicroelectronics) : https://www.st.com/resource/en/datasheet/lsm6dsox.pdf
- Raspberry Pi Pico 2 W : https://datasheets.raspberrypi.com/picow/pico-2-w-datasheet.pdf

### 8.2 Guides d'utilisation (Adafruit)

- Module Adafruit LSM6DSOX : https://cdn-learn.adafruit.com/downloads/pdf/lsm6dsox-and-ism330dhc-6-dof-imu.pdf
- PiCowbell Adalogger : https://cdn-learn.adafruit.com/downloads/pdf/adafruit-picowbell-adalogger-for-pico.pdf

### 8.3 Manuel de référence (pour aller plus loin)

- Microcontrôleur RP2350 (puce du Pico 2 W) : https://datasheets.raspberrypi.com/rp2350/rp2350-datasheet.pdf

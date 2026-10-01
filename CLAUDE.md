# Projet Anti-Sédentarité — Raspberry Pi Pico

## Contexte
- Projet APP2028 CAPT, firmware C pour Raspberry Pi Pico / Pico W (SDK Pico ≥ 2.0).
- Étape 2 : prise en main — LED clignotante 1 Hz + logs série USB.
- Langue du projet et des commentaires : français.

## Objectif final
Boîtier portable autonome (Pico, capteur LSM6DSOX, RTC, carte SD, moteur vibrant/LED) mesurant la dépense métabolique (score MET).
Étape en cours : test du LSM6DSOX près des moteurs du bras UR3 (perturbations) → distance minimale capteur/moteurs pour dimensionner le support 3D.

## Structure
Deux programmes (cibles CMake), construits à partir des modules de `src/` :
- `prise_en_main` (`src/programmes/prise_en_main.c`) : étape 2, LED 1 Hz + messages USB.
- `mesure_capteur` (`src/programmes/mesure_capteur.c`) : LSM6DSOX en I2C0 (GP4 SDA, GP5 SCL, 0x6A, WHO_AM_I 0x6C), ±2 g 104 Hz,
  CSV `timestamp_ms,ax_g,ay_g,az_g,mag_g` sur USB. Lignes `#` = commentaires/erreurs/diagnostic.
  LED fixe = capteur OK, 5 flashs rapides = capteur absent (diagnostic câblage affiché à chaque essai).

| Dossier | Modules (parent → enfant) | Rôle |
|---|---|---|
| `src/bus/` | `bus_i2c`, `bus_i2c_diagnostic` | I2C matériel, déblocage, scan + sonde logicielle |
| `src/capteur/` | `capteur` → `capteur_mouvement` → `capteur_mouvement_lsm6dsox` | interface capteur, capteur de mouvement 3 axes, pilote LSM6DSOX |
| `src/communication/` | `communication_moniteur` → `communication_moniteur_csv` | détection ouverture du port, flux CSV |
| `src/signalisation/` | `signalisation_led` | LED (GP25 ou CYW43) |
| `src/acquisition/` | `acquisition` | boucle : lecture, envoi, déblocage après 10 échecs |
| `src/programmes/` | `prise_en_main.c`, `mesure_capteur.c` | les deux `main` |
| `tests/` | `capteur/`, `communication/`, `mocks/` | tests hôte ; `mocks/bus_i2c_simule.c` remplace `bus_i2c.c` |

- `outils/test_perturbations.py` : `capture <d###_on|off_r#> --duree 30` (écrit `mesures/`), `analyse --seuil 1.10`.
- `outils/tests.sh` : voir « Tests automatiques ».
- `docs/` : toute la documentation du projet (index : `docs/README.md`). Toute nouvelle doc va dans ce dossier.
- `docs/CR/` : comptes rendus en **rapport rédigé** (Introduction / Déroulé / Résultats / Conclusion, peu de tableaux, repères `[📷 …]` pour les photos), **un fichier par étape** : `CR_<n°>_<etape>.md`. Prochain : `CR_05_test_perturbations_robot.md`. `CR_04_programmes.md` = détail du code et de ses modifications (à compléter à chaque changement de code).
- `docs/EVOLUTIONS.md` : historique simple des versions du code (ajouts, problèmes → corrections). **À compléter à chaque modification du firmware** (nouvelle section Vx + tableau/schéma de la partie 1), en restant synthétique et sans jargon.
- `docs/MATERIEL.md` : liste et explication du matériel (Pico 2 W, module LSM6DSOX, carte horloge/SD à confirmer, carte keyes, câbles). Les câbles STEMMA QT dispo ont un connecteur blanc aux deux bouts → inutilisables sur le Pico.
- `docs/BRANCHEMENTS.md` : câblage du test robot (module Adafruit, 4 fils) et de la carte finale (puce seule + C1/C2, pull-up, CS à 3V3). À compléter quand on ajoute RTC, SD, moteur vibrant.
- `docs/ARCHITECTURE.md` : fiche illustrée (Mermaid) de l'organisation et des principes du code — à tenir à jour si la structure change.
- `CMakeLists.txt` : `PICO_BOARD` par défaut `pico2_w`, include `src/`, stdio USB + CYW43 pour les deux cibles.
- `pico_sdk_import.cmake` : copie officielle du SDK (ne pas modifier).

## Matériel
- Carte : **Pico 2 W (RP2350 + Wi-Fi CYW43), confirmée par photo** → `-DPICO_BOARD=pico2_w`. LED pilotée par CYW43, pas GP25.

## Build
SDK 2.2.0 dans `~/.pico-sdk/sdk`, toolchain ARM GCC 14.2 dans `~/.pico-sdk/arm-gnu-toolchain-14.2.rel1-darwin-arm64-arm-none-eabi`, cmake+ninja via brew.
```sh
export PICO_SDK_PATH=~/.pico-sdk/sdk
export PICO_TOOLCHAIN_PATH=~/.pico-sdk/arm-gnu-toolchain-14.2.rel1-darwin-arm64-arm-none-eabi
cmake -S . -B build -G Ninja -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
cmake --build build
```
Flash sans BOOTSEL (le firmware en cours doit avoir stdio USB) : `picotool load -f -x build/<cible>.uf2` (picotool de Homebrew ; celui de `build/_deps` n'a pas l'USB).
Sinon : BOOTSEL + glisser `build/<cible>.uf2` sur le lecteur RP2350.

## Conventions
- Commentaires en français, sans accents dans les chaînes `printf`.
- Pico W : lier `pico_cyw43_arch_none` (conditionné par `PICO_CYW43_SUPPORTED`).

## Règles d'architecture (obligatoires pour tout nouveau code)
Le projet est en C : une « classe » = un type `struct` + les fonctions qui agissent dessus.

1. **Une classe par fichier** : un module = une paire `nom_module.h` / `nom_module.c` contenant
   **un seul** type principal `nom_module_t` et ses fonctions `nom_module_<action>()`.
   Le `.h` expose l'interface publique, les fonctions internes sont `static` dans le `.c`.
2. **30 lignes max par fonction** (corps, accolades exclues). Au-delà, découper en fonctions
   `static` nommées d'après ce qu'elles font. `main()` est concerné aussi.
3. **Un dossier par fonctionnalité** sous `src/` :
   ```
   src/
   ├── capteur/        capteur.h/.c (parent), capteur_mouvement_lsm6dsox.h/.c (enfant)
   ├── bus/            bus_i2c.h/.c
   ├── signalisation/  signalisation_led.h/.c
   └── communication/  communication_csv.h/.c
   tests/<fonctionnalite>/test_<module>.c   (même découpage que src/)
   tests/mocks/                             (faux pico/stdlib.h, hardware/i2c.h… pour le Mac)
   ```
4. **Héritage parent → enfant** par composition de structures :
   - le parent est le **premier membre** de l'enfant, nommé `parent` : un pointeur enfant
     peut ainsi être passé là où un parent est attendu ;
   - les comportements redéfinissables passent par une table de fonctions dans le parent
     (`capteur_operations_t`, équivalent des méthodes virtuelles) ;
   - **nommage explicite d'appartenance** : l'enfant reprend tout le nom du parent en préfixe.
     Fichier, type et fonctions suivent la même chaîne :
     `capteur` → `capteur_mouvement` → `capteur_mouvement_lsm6dsox`
     (`capteur_mouvement_lsm6dsox.c`, `capteur_mouvement_lsm6dsox_t`, `capteur_mouvement_lsm6dsox_initialiser()`).
   ```c
   typedef struct capteur capteur_t;
   typedef struct {
       bool (*initialiser)(capteur_t *self);
       capteur_lecture_t (*lire)(capteur_t *self);   // OK / PAS_PRETE / ERREUR
   } capteur_operations_t;
   struct capteur { const capteur_operations_t *operations; const char *nom; };

   typedef struct {
       capteur_t parent;          // toujours en premier
       uint8_t adresse_i2c;
   } capteur_mouvement_lsm6dsox_t;
   ```
5. **Tests** : tout module sans accès direct au matériel a son test hôte dans
   `tests/<fonctionnalite>/test_<module>.c` (avec `assert`, `main` qui renvoie 0 si tout passe).
   Première ligne utile : `// SOURCES: src/<fonctionnalite>/<module>.c ...` (fichiers à compiler avec).
   Le matériel (I2C, GPIO) est isolé derrière une interface pour pouvoir être simulé dans `tests/mocks/`.

## Tests automatiques
- `outils/tests.sh` : compile le firmware, compile et lance les tests C sur le Mac (`cc`),
  vérifie les scripts Python, signale les fonctions de plus de 30 lignes.
- **Hook** (`.claude/settings.json`, PostToolUse sur Edit|Write) : lancé après chaque modification
  d'un `.c`, `.h`, `.py` ou `CMakeLists.txt`. Échec de compilation ou de test → code 2, l'erreur est
  renvoyée à Claude pour correction. Règle des 30 lignes → simple avertissement.
- Lancement manuel : `./outils/tests.sh </dev/null`.

## Pièges
- Breadboard : les 5 trous d'une même ligne sont reliés. Deux fils dans la même ligne = court-circuit (2026-10-01 : Pico qui chauffe, LED capteur éteinte ; Pico intact après coup). Pour le test, brancher capteur → Pico en direct, sans breadboard.
- RP2350 erratum E9 : une entrée avec pull-down peut rester bloquée à 1 → ne pas l'utiliser pour détecter un fil débranché.
- Header du breakout LSM6DSOX non soudé (juste enfiché) → contacts intermittents, bus I2C qui décroche. Le souder.
- Câblage : vérifier les noms imprimés sous le Pico (GP4/GP5, 3V3, GND). Les 4 fils ne sont pas sur la même rangée : SDA/SCL côté GP0, 3V3/GND côté VBUS. Sur le breakout, utiliser SDA/SCL et non SDX/SCX (bus auxiliaire).
- Critère du test de perturbations sur l'écart-type **par axe** (x, y, z), pas sur la norme : un bruit perpendiculaire à la gravité est quasi invisible dans |a| (effet du 2e ordre).
- `test_perturbations.py capture` et le Serial Monitor VS Code ne peuvent pas ouvrir le port en même temps.
- Le moniteur série VS Code ne se reconnecte pas seul après un débranchement : tout message envoyé avant l'ouverture du port est perdu. D'où la bannière ré-affichée à chaque connexion (`stdio_usb_connected()`).
- Ne pas déplacer/renommer le dossier `build/` : CMake y garde des chemins absolus → `rm -rf build` et reconfigurer.
- macOS : `cp` du .uf2 vers /Volumes/RP2350 peut bloquer (processus en état U). Préférer le glisser-déposer Finder.

## Journal
- 2026-10-01 : ajout de `pico_sdk_import.cmake` et du lien CYW43 pour Pico W.
- 2026-10-01 : installation SDK/toolchain, build pico2 OK (`build/prise_en_main.uf2`, 59 Ko). Flash via cp bloqué.
- 2026-10-01 : **Phase 1 validée** sur Pico 2 W : LED clignote à 1 Hz, logs série OK sur /dev/cu.usbmodem101 (115200).
- 2026-10-01 : bannière de démarrage affichée à chaque ouverture du moniteur série (remplace le `sleep_ms(3000)`).
- 2026-10-01 : phase 3 écrite (`mesure_capteur.c`) + `outils/test_perturbations.py`. **Phase 2 validée** : capteur câblé, flashé, CSV reçu (~104 Hz, az ≈ 1,00 g à plat).
- 2026-10-01 : capteur répond quelques secondes puis bus I2C muet (aucune adresse, ni câblage normal ni inversé). Cause : rouge/noir mal placés côté Pico (capteur alimenté par les lignes I2C), puis SDA/SCL hors de GP4/GP5 — en cours de correction. `mesure_capteur.c` : récupération auto du bus + diagnostic (scan, sonde I2C logicielle).
- 2026-10-01 : règles d'architecture (1 classe/fichier, 30 lignes/fonction, 1 dossier/fonctionnalité, héritage par composition) + hook de tests automatiques `outils/tests.sh`.
- 2026-10-01 : vraie cause du bus I2C muet : header du capteur non soudé (câblage Pico correct).
- 2026-10-01 : **refactoring** selon les règles d'architecture : `main.c`/`mesure_capteur.c` remplacés par `src/` (5 fonctionnalités, 2 hiérarchies parent→enfant), 4 tests hôte (capteur_mouvement, capteur_mouvement_lsm6dsox sur bus simulé, communication_moniteur, communication_moniteur_csv). Firmware compile sans avertissement ; non re-testé sur carte (Pico débranché).
- 2026-10-01 : V4 (code réorganisé) flashée et fonctionnelle sur la carte (diagnostic OK, capteur pas encore rebranché). Le port série peut changer de nom (`usbmodem1101`).
- 2026-10-01 : **capteur validé sur la carte (V4)** via header posé dans la breadboard : à plat ax 0,07 / ay 0,21 / az 0,95 / mag 0,98 ; à 90° ay −1,05 / mag 1,05. Décalage d'usine ~±5 % sur la norme → étalonnage 6 faces à prévoir pour le MET.

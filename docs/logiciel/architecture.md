# Comment est organisé le projet

## En une phrase

Le Pico lit le capteur de mouvement **100 fois par seconde** et envoie les valeurs à
l'ordinateur par le câble USB.

---

## 1. Le chemin, du code jusqu'aux mesures

```mermaid
flowchart LR
    A["Le code<br/>(dossier src)"] --> B["Compilation<br/>(transforme le code<br/>en fichier .uf2)"]
    B --> C["Le Pico<br/>(exécute le .uf2)"]
    C --> D["Le capteur<br/>(mesure le mouvement)"]
    D --> C
    C --> E["L'ordinateur<br/>(affiche les mesures)"]
```

1. On écrit le code dans le dossier `src`.
2. La **compilation** le traduit en un fichier `.uf2` que le Pico comprend.
3. On envoie ce fichier sur le Pico.
4. Le Pico interroge le capteur sans arrêt et transmet les valeurs à l'ordinateur.

```
ton code (.c)  →  compilation  →  fichier .uf2  →  Pico
   toi tu lis       traduction      le Pico lit
```

> **Compiler** = traduire ton code (du texte) en langage machine (des nombres que le
> processeur lit). Le **`.uf2`** = ce langage machine, prêt à être glissé sur le Pico.

---

## 2. Les dossiers

| Dossier | À quoi il sert |
|---|---|
| `src/` | **Le code du Pico.** C'est le cœur du projet. |
| `scripts/` | Scripts pour l'ordinateur : vérifier le code (`verifier.sh`), enregistrer et analyser les mesures du robot (`perturbations.py`). |
| `cmake/` | Fichier d'import du SDK Pico (copie officielle, on n'y touche pas). |
| `build/` | Fichiers fabriqués par la compilation (dont les `.uf2`). On n'y touche pas. |

Dans `src/`, le code est rangé **par rôle**, un dossier par rôle. Un sous-dossier `tests/` (pour l'instant dans `capteur/` et `communication/`) contient les petits programmes qui **vérifient** son code sans la carte.

| Dossier | Son rôle | Image |
|---|---|---|
| `bus/` | Faire circuler les messages entre le Pico et le capteur (par les fils bleu et jaune) | Le **téléphone** |
| `capteur/` | Savoir parler au capteur LSM6DSOX : le démarrer, lui demander ses mesures | Le **traducteur** |
| `communication/` | Envoyer les mesures à l'ordinateur, une ligne par mesure | Le **facteur** |
| `signalisation/` | Allumer ou faire clignoter la LED du Pico | Le **voyant** |
| `acquisition/` | Répéter sans fin : demander une mesure, l'envoyer, gérer les problèmes | Le **chef d'orchestre** |
| `programmes/` | Les deux programmes qu'on peut mettre sur le Pico | Le **bouton « marche »** |

Les deux programmes :
- **`prise_en_main`** : fait juste clignoter la LED (c'était l'étape 2, pour tester la carte).
- **`mesure_capteur`** : le vrai programme, qui lit le capteur et envoie les mesures.

---

## 3. Ce que fait le Pico quand il démarre (`mesure_capteur`)

```mermaid
flowchart TD
    A["Le Pico s'allume"] --> B["Il démarre le bus I2C<br/>(les fils bleu et jaune)"]
    B --> C{"Le capteur<br/>répond ?"}
    C -- "non" --> D["La LED clignote vite<br/>et un message d'aide<br/>s'affiche sur l'ordinateur"]
    D --> C
    C -- "oui" --> E["La LED reste allumée"]
    E --> F["Il demande une mesure<br/>au capteur"]
    F --> G["Il envoie la mesure<br/>à l'ordinateur"]
    G --> F
```

Si le capteur arrête de répondre en cours de route (fil qui bouge, par exemple), le Pico
s'en rend compte, affiche `# erreur I2C`, puis **réessaie tout seul**. Il ne se bloque jamais.

Ce qui s'affiche sur l'ordinateur :
```
timestamp_ms,ax_g,ay_g,az_g,mag_g
12345,0.00317,-0.02178,1.00223,1.00247
```
- `timestamp_ms` : l'heure de la mesure (en millisecondes depuis l'allumage) ;
- `ax_g`, `ay_g`, `az_g` : l'accélération sur chaque axe, en g ;
- `mag_g` : l'accélération totale. Posé sur la table, le capteur indique **environ 1 g** :
  c'est la gravité.

---

## 4. Les « familles » de code

Certains morceaux de code sont des versions **plus précises** d'autres morceaux, comme des
poupées russes :

```mermaid
flowchart TD
    A["capteur<br/><i>n'importe quel capteur :<br/>on peut le démarrer et le lire</i>"]
    B["capteur_mouvement<br/><i>un capteur de mouvement :<br/>il mesure sur 3 axes</i>"]
    C["capteur_mouvement_lsm6dsox<br/><i>notre capteur précis :<br/>il sait quels registres utiliser</i>"]
    A --> B --> C
```

Un `capteur_mouvement_lsm6dsox` **est** un `capteur_mouvement`, qui **est** un `capteur`.
Le nom de chaque enfant reprend celui du parent : on voit tout de suite d'où il vient.

**Pourquoi c'est utile** : le chef d'orchestre (`acquisition`) dit simplement « capteur,
donne-moi une mesure ». Si un jour on change de capteur, on écrit une nouvelle poupée
`capteur_mouvement_autre`, et le reste du code ne change pas.

Même idée pour l'envoi des mesures :
`communication_moniteur` (savoir quand l'ordinateur écoute) → `communication_moniteur_csv`
(envoyer les mesures en lignes CSV).

---

## 5. Les tests

Un test est un petit programme qui vérifie un morceau du code **sur l'ordinateur**, sans le
Pico. Par exemple : « si le capteur renvoie ces octets-là, est-ce qu'on obtient bien 1 g ? ».

Comme il n'y a pas de vrai capteur sur l'ordinateur, on utilise un **faux capteur**
(dossier `src/mocks/`) : un tableau de valeurs qu'on remplit à la main. Le code ne voit pas
la différence.

```mermaid
flowchart LR
    subgraph Pico["Sur le Pico"]
        A1["code du capteur"] --> B1["vrai bus I2C"] --> C1["vrai LSM6DSOX"]
    end
    subgraph Ordi["Sur l'ordinateur (tests)"]
        A2["même code du capteur"] --> B2["faux bus"] --> C2["faux capteur<br/>(valeurs choisies<br/>par le test)"]
    end
```

**Vérification automatique** : à chaque modification du code, le script `scripts/verifier.sh` se lance
tout seul. Il :
1. vérifie que le code se compile ;
2. lance tous les tests ;
3. signale les fonctions trop longues (plus de 30 lignes).

Si quelque chose casse, on le sait tout de suite.

---

## 6. Les règles du projet

| Règle | Pourquoi |
|---|---|
| Un fichier = un seul « objet » (le capteur, la LED, le bus…) | On sait où chercher |
| Une fonction = 30 lignes maximum | Chaque fonction reste courte et lisible |
| Un dossier par rôle | Le projet reste rangé quand il grandit |
| Le nom de l'enfant commence par celui du parent | On voit la famille d'un coup d'œil |

---

## 7. Ce que tu fais au quotidien

| Je veux… | Je fais… |
|---|---|
| Compiler | Dans le terminal : `cmake --build build` (ou automatique via `./scripts/verifier.sh`) |
| Mettre le programme sur le Pico | Dans le terminal : `picotool load -f -x build/mesure_capteur.uf2` |
| Voir les mesures | Serial Monitor de VS Code, port `usbmodem…`, 115200 bauds |
| Enregistrer des mesures pour le test des perturbations du robot | `python3 scripts/perturbations.py capture d050_on_r1 --duree 30` |
| Analyser les mesures | `python3 scripts/perturbations.py analyse` |
| Vérifier que tout marche | `./scripts/verifier.sh </dev/null` |

Vocabulaire : [index](../index.md).

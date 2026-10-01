# APP2028 CAPT — Dispositif anti-sédentarité

Boîtier portable (Raspberry Pi Pico 2 W + capteur LSM6DSOX) qui mesure l'activité physique (score MET).

## Démarrage rapide

```sh
export PICO_SDK_PATH=~/.pico-sdk/sdk
cmake -S . -B build -G Ninja && cmake --build build   # compile
./scripts/verifier.sh </dev/null                      # tests
```

## Documentation

| Dossier | Fichier | Contenu |
|---|---|---|
| `conception/` | [dossier-technique.md](docs/conception/dossier-technique.md) | Choix techniques, composants, câblage (test et carte finale), score MET, pistes |
| `materiel/` | [inventaire.md](docs/materiel/inventaire.md) | Matériel utilisé |
| | [cablage-test-robot.md](docs/materiel/cablage-test-robot.md) | Test robot UR3 : vérifications, installation, protocole de mesure |
| `logiciel/` | [architecture.md](docs/logiciel/architecture.md) | Organisation du code |
| | [changelog.md](docs/logiciel/changelog.md) | Versions du code |
| `guides/` | [github.md](docs/guides/github.md) | Mots et commandes Git/GitHub |
| | [claude-code.md](docs/guides/claude-code.md) | Commandes et raccourcis Claude Code |
| | [barre-etat.md](docs/guides/barre-etat.md) | Barre d'état de Claude Code, élément par élément |
| | [index.md](docs/index.md) | Vocabulaire du projet |

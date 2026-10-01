---
name: git
description: Règles Git du projet — nommer une branche (feature/, fix/, chore/, hotfix/…), créer un worktree, écrire un commit (feat:, fix:…), max 10 fichiers par commit. À utiliser avant toute création de branche, de worktree ou de commit.
---

# Git — règles du projet

## Règle n°1 : JAMAIS de commit sur `main`

1. Créer une branche (`feature/…`, `fix/`…).
2. Commiter sur cette branche.
3. Merger dans `main` (pull request) **uniquement quand l'utilisateur le demande**.

Avant chaque commit : `git branch --show-current` → si `main`, **ne pas commiter**, créer d'abord une branche (`git switch -c <type>/<nom>` garde les modifications en cours).

## 1. Branche

Format : `<type>/<description-courte>` — minuscules, mots séparés par `-`, sans accents.

| Type | Usage | Exemple |
|---|---|---|
| `feature/` | Nouvelle fonctionnalité | `feature/carte-sd` |
| `fix/` | Correction de bug | `fix/bus-i2c-bloque` |
| `hotfix/` | Correction urgente sur `main` | `hotfix/led-eteinte` |
| `chore/` | Outils, config, ménage | `chore/ci-github` |
| `docs/` | Documentation seule | `docs/branchements-rtc` |
| `refactor/` | Réorganisation sans changer le comportement | `refactor/module-acquisition` |
| `test/` | Ajout ou correction de tests | `test/capteur-mouvement` |

```sh
git switch main && git pull
git switch -c feature/carte-sd
```

- Une branche = un seul sujet.

## 2. Worktree

Un worktree = une 2e copie du dépôt, sur une autre branche, dans un autre dossier (travailler sur 2 branches en même temps).

```sh
git worktree add .claude/worktrees/carte-sd -b feature/carte-sd main   # créer
git worktree list                                                      # lister
git worktree remove .claude/worktrees/carte-sd                         # supprimer (après merge)
```

- Dossier : `.claude/worktrees/<description>` (même nom que la branche, sans le préfixe `feature/`…). Ce dossier est ignoré par Git.
- Ne pas copier `build/` dans le worktree : il contient des chemins absolus. Le reconfigurer (`cmake -S . -B build -G Ninja`).

## 3. Commit

Format : `<type>(<portee>): <description>` — portée facultative, description à l'impératif, minuscule, sans point final, ≤ 72 caractères.

| Type | Usage | Exemple |
|---|---|---|
| `feat` | Nouvelle fonctionnalité | `feat(capteur): ajoute la lecture du gyroscope` |
| `fix` | Correction de bug | `fix(bus): débloque le bus après 10 échecs` |
| `docs` | Documentation | `docs: ajoute la fiche GitHub` |
| `refactor` | Réorganisation du code | `refactor(acquisition): découpe la boucle principale` |
| `test` | Tests | `test(communication): teste le flux CSV` |
| `chore` | Outils, config, ménage | `chore: supprime les copies HTML` |
| `ci` | GitHub Actions | `ci: compile le firmware à chaque push` |
| `build` | CMake, toolchain | `build: passe au SDK 2.2.0` |

Portées : `bus`, `capteur`, `communication`, `signalisation`, `acquisition`, `programmes`, `scripts`, `docs`.

### Règles

- **10 fichiers max par commit.** Au-delà, découper en plusieurs commits cohérents (ex. module + son test ensemble, doc à part).
- Un commit = un seul changement logique.
- Le code doit compiler et les tests passer (`./scripts/verifier.sh </dev/null`) avant de commiter.
- Ajouter les fichiers par nom (`git add src/bus/bus_i2c.c …`), pas `git add .` à l'aveugle.
- `git add` et `git commit` dans deux commandes séparées (pas de `&&`, pas de `commit -a`).
- Ces règles sont vérifiées par le hook `.claude/hooks/verif_git.sh` : commande bloquée si non respectées.
- Jamais de secret (jeton, mot de passe) dans un commit.

### Vérifier avant de commiter

```sh
git status --short
git diff --cached --name-only | wc -l    # doit être ≤ 10
```

Si > 10 : `git restore --staged <fichiers>` puis commiter en plusieurs fois.

## 4. Fin de branche

```sh
git push -u origin feature/carte-sd
gh pr create --fill          # pull request vers main
```

Après le merge : `git switch main && git pull && git branch -d feature/carte-sd`.

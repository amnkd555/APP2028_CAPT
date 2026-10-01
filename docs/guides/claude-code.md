# Commandes Claude Code

Taper `/` dans la zone de saisie affiche toutes les commandes.

## Saisie

| Taper | Effet |
|---|---|
| `/commande` | Lance une commande (début du message) |
| `! commande` | Lance une commande du terminal ici, Claude voit le résultat. Ex. : `! git status` |
| `@fichier` | Joint un fichier au message (autocomplétion) |

## Raccourcis clavier

| Touche | Effet |
|---|---|
| `Esc` | Arrête Claude en cours de réponse |
| `Esc` `Esc` | Efface la saisie ; si elle est vide, revient en arrière dans la conversation |
| `Shift+Tab` | Change le mode : normal, accepte les modifications sans demander, plan (Claude propose sans modifier) |
| `Ctrl+C` | Arrête l'action en cours ; 2 fois sur une saisie vide : quitte |
| `Ctrl+R` | Recherche dans les messages déjà envoyés |
| `Ctrl+O` | Affiche le détail de ce que fait Claude |
| `Ctrl+T` | Affiche / masque la liste des tâches de Claude |
| `Ctrl+B` | Passe une commande longue en arrière-plan |
| `Ctrl+L` | Réaffiche l'écran s'il est abîmé |
| `↑` `↓` | Messages précédents |

## Conversation

| Commande | Effet |
|---|---|
| `/clear` | Nouvelle conversation, mémoire vide |
| `/compact` | Résume la conversation pour libérer de la mémoire (quand `ctx` est haut) |
| `/rewind` | Revient à un point précédent (conversation et/ou code) |
| `/resume` | Reprend une ancienne conversation |
| `/rename nom` | Renomme la conversation |
| `/branch` | Crée une copie de la conversation pour essayer autre chose |
| `/btw question` | Question rapide sans l'ajouter à la conversation |
| `/recap` | Résumé en une ligne de la session |
| `/copy` | Copie la dernière réponse de Claude |
| `/export` | Enregistre la conversation dans un fichier texte |
| `/exit` | Quitte |

## Réglages

| Commande | Effet |
|---|---|
| `/model` | Change de modèle |
| `/effort low` … `max` | Niveau de réflexion : plus haut = plus précis, plus lent, plus cher |
| `/fast` | Mode rapide (même modèle, réponses plus rapides) |
| `/plan` | Mode plan : Claude propose avant de modifier |
| `/permissions` | Ce que Claude peut faire sans demander |
| `/config` | Réglages généraux (thème, etc.) |
| `/voice` | Dictée vocale |
| `/statusline` | Configure la barre d'état ([détail](barre-etat.md)) |

## Suivi

| Commande | Effet |
|---|---|
| `/usage` | Coût, limites de l'abonnement, statistiques |
| `/context` | Remplissage de la mémoire de la conversation |
| `/diff` | Modifications en cours dans les fichiers |
| `/tasks` | Tâches en arrière-plan |
| `/status` | Version, compte, connexion |
| `/doctor` | Diagnostic de l'installation |

## Projet et outils

| Commande | Effet |
|---|---|
| `/memory` | Modifie `CLAUDE.md` (consignes du projet) et la mémoire de Claude |
| `/init` | Crée un `CLAUDE.md` pour un nouveau projet |
| `/hooks` | Affiche les hooks (actions automatiques) |
| `/skills` | Liste les skills disponibles |
| `/code-review` | Relecture du code modifié |
| `/security-review` | Relecture sécurité |
| `/simplify` | Simplifie le code modifié |
| `/loop 5m commande` | Relance une commande à intervalle régulier |

## Configuré dans ce projet (`.claude/`)

| Élément | Fichier | Effet |
|---|---|---|
| Skill `/git` | `skills/git/SKILL.md` | Règles de branches, commits, worktrees |
| Hook tests | `settings.json` → `scripts/verifier.sh` | Après chaque modification de code : compile et lance les tests |
| Hook git | `hooks/verif_git.sh` | Bloque un commit ou push hors règles (sur `main`, > 10 fichiers, format, emoji, jeton) |
| Barre d'état | `statusline.sh` | Voir [barre-etat.md](barre-etat.md) |
| Worktrees | `worktrees/` | Copies du dépôt sur d'autres branches (ignorées par Git) |

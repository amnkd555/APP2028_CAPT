# Barre d'état de Claude Code

Les 2 lignes affichées sous la zone de saisie de Claude Code. Script : `.claude/statusline.sh`, mis à jour à chaque message et toutes les 10 s.

```
Opus 5.5 | high | chore/regles-git-et-docs ~29 | ↑0 ↓0 | wkx1 | PR #12 pending | $3.20
ctx [######....] 63% | 5h 21% | 1h13 | +812 -640 | Pico absent | CI ok
```

## Ligne 1 : session et Git

| Élément | Exemple | Signification |
|---|---|---|
| Modèle | `Opus 5.5` | Modèle de Claude utilisé (changer : `/model`) |
| Effort | `high` | Niveau de réflexion de Claude : `low`, `medium`, `high`, `xhigh`, `max` (changer : `/effort`) |
| Branche | `chore/regles-git-et-docs` | Branche Git sur laquelle on travaille |
| Fichiers modifiés | `~29` | Nombre de fichiers modifiés ou nouveaux, pas encore commités. Absent si tout est commité |
| Écart avec GitHub | `↑2 ↓1` | ↑ = commits à envoyer (`git push`), ↓ = commits à récupérer (`git pull`). Vert si `↑0 ↓0`, jaune sinon. Branche jamais envoyée : comparée à `main` sur GitHub |
| Worktrees | `wkx1` | Nombre de copies du dépôt ouvertes. `wkx1` = seulement le dossier principal ; `wkx2` = 1 worktree en plus (dans `.claude/worktrees/`) |
| Pull request | `PR #12 pending` | Pull request ouverte pour la branche et son état : `pending` (en attente), `approved` (validée), `changes_requested` (à corriger), `draft` (brouillon). Absent s'il n'y en a pas |
| Coût | `$3.20` | Coût estimé de la session au prix public de l'API. Rouge à partir de 5 $. Avec un abonnement Claude, ce n'est pas ce qui est facturé |

## Ligne 2 : limites, matériel, vérification

| Élément | Exemple | Signification |
|---|---|---|
| Contexte | `ctx [######....] 63%` | Mémoire de la conversation déjà remplie. Vert < 50 %, jaune < 80 %, rouge au-delà. Près de 100 % : faire `/compact` |
| Limite 5 h | `5h 21%` | Part utilisée de la limite d'usage de l'abonnement sur 5 heures. Mêmes couleurs. Absent sans abonnement |
| Durée | `1h13` | Temps depuis le début de la session |
| Lignes | `+812 -640` | Lignes ajoutées / supprimées par Claude pendant la session |
| Pico | `Pico branché` | Vert : port série du Pico détecté (`/dev/cu.usbmodem…`). Jaune `Pico BOOTSEL` : Pico en mode clé USB, prêt à recevoir un `.uf2`. Gris `Pico absent` : rien de branché |
| CI | `CI ok` | Dernière vérification GitHub (compilation + tests) de la branche : `ok` vert, `échec` rouge, `en cours` jaune. Mise à jour toutes les 2 min. Absent si la branche n'a jamais été envoyée |

## Modifier la barre

Demander à Claude, ou modifier `.claude/statusline.sh`. Elle est branchée dans `.claude/settings.json` (`statusLine`).

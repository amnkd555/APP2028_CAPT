#!/bin/bash
# Hook PreToolUse (Bash) : fait respecter les règles Git du projet (skill « git »)
# avant chaque « git commit » et « git push » lancé par Claude.
#
# Commit refusé si : branche main, git add dans la même commande, plus de 10 fichiers,
#                    message hors format « type(portee): description », emoji, jeton GitHub.
# Push refusé si    : branche main ou cible main.
#
# Code de sortie 2 = commande bloquée, le message est renvoyé à Claude.

set -u
entree=$(cat)
commande=$(jq -r '.tool_input.command // empty' <<<"$entree")
dossier=$(jq -r '.cwd // empty' <<<"$entree")
cd "${dossier:-${CLAUDE_PROJECT_DIR:-.}}" 2>/dev/null || exit 0

refuser() {
    echo "BLOQUE par .claude/hooks/verif_git.sh : $1" >&2
    exit 2
}

contient_emoji() {
    perl -CSD -ne 'if (/[\x{1F000}-\x{1FAFF}\x{2600}-\x{27BF}\x{2B00}-\x{2BFF}\x{FE0F}]/) { $t = 1 } END { exit($t ? 0 : 1) }'
}

grep -Eq 'git( -C [^ ]+)? (commit|push)' <<<"$commande" || exit 0
branche=$(git branch --show-current 2>/dev/null) || exit 0

# --- Commit ------------------------------------------------------------------
if grep -Eq 'git( -C [^ ]+)? commit' <<<"$commande"; then
    [ "$branche" = "main" ] && refuser "jamais de commit sur main. Creer une branche : git switch -c <type>/<nom>"

    grep -Eq 'git( -C [^ ]+)? add|commit (.* )?(-a|-am|--all)( |$)' <<<"$commande" \
        && refuser "faire git add puis git commit dans deux commandes separees (pour compter les fichiers)"

    nb=$(git diff --cached --name-only | wc -l | tr -d ' ')
    [ "$nb" -gt 10 ] && refuser "$nb fichiers prepares, 10 max par commit. Decouper : git restore --staged <fichiers>"

    grep -Eq '(feat|fix|docs|refactor|test|chore|ci|build)(\([a-z_-]+\))?: ' <<<"$commande" \
        || refuser "message hors format. Attendu : type(portee): description (feat, fix, docs, refactor, test, chore, ci, build)"

    contient_emoji <<<"$commande" && refuser "emoji dans le message de commit"

    ajouts=$(git diff --cached -U0 | grep '^+' | grep -v '^+++')
    contient_emoji <<<"$ajouts" && refuser "emoji dans les fichiers prepares"
    grep -Eq 'gh[pousr]_[A-Za-z0-9]{20,}|github_pat_[A-Za-z0-9_]{20,}' <<<"$ajouts" \
        && refuser "jeton GitHub dans les fichiers prepares"
fi

# --- Push --------------------------------------------------------------------
if grep -Eq 'git( -C [^ ]+)? push' <<<"$commande"; then
    [ "$branche" = "main" ] && refuser "jamais de push depuis main. Travailler sur une branche"
    grep -Eq 'git push.*[ :]main( |$)' <<<"$commande" && refuser "jamais de push vers main. Passer par une pull request"
fi

exit 0

#!/bin/bash
# Tests automatiques du projet, lancés par le hook Claude Code (PostToolUse sur Edit|Write)
# et utilisables à la main : ./outils/tests.sh
#
# 1. Compile le firmware (toutes les cibles CMake)            -> bloquant
# 2. Compile et exécute les tests unitaires C sur le Mac      -> bloquant
#    tests/<fonctionnalite>/test_<module>.c ; la ligne « // SOURCES: a.c b.c » liste
#    les fichiers de src/ à compiler avec le test (chemins relatifs à la racine)
# 3. Vérifie la syntaxe des scripts Python et lance tests/**/test_*.py -> bloquant
# 4. Vérifie la règle des 30 lignes max par fonction          -> avertissement
#
# Code de sortie 2 = échec : le message est renvoyé à Claude pour qu'il corrige.

set -u
RACINE="$(cd "$(dirname "$0")/.." && pwd)"
cd "$RACINE" || exit 0

# Appelé par le hook : ne réagit qu'aux fichiers de code du projet (hors build/)
if [ ! -t 0 ]; then
    fichier=$(jq -r '.tool_input.file_path // .tool_response.filePath // empty' 2>/dev/null)
    case "$fichier" in
        "") ;;
        "$RACINE"/build/*|"$RACINE"/build_*/*) exit 0 ;;
        "$RACINE"/*.c|"$RACINE"/*.h|"$RACINE"/*CMakeLists.txt|"$RACINE"/*.py|"$RACINE"/outils/tests.sh) ;;
        *) exit 0 ;;
    esac
fi

echecs=""
avertissements=""
journal="$(mktemp)"
trap 'rm -f "$journal"; rm -rf "$RACINE/build/tests_hote"' EXIT

# --- 1. Firmware -----------------------------------------------------------
export PICO_SDK_PATH="$HOME/.pico-sdk/sdk"
export PICO_TOOLCHAIN_PATH="$HOME/.pico-sdk/arm-gnu-toolchain-14.2.rel1-darwin-arm64-arm-none-eabi"
if [ ! -f build/build.ninja ]; then
    cmake -S . -B build -G Ninja -DCMAKE_EXPORT_COMPILE_COMMANDS=ON >"$journal" 2>&1
fi
if ! cmake --build build >"$journal" 2>&1; then
    echecs+="ÉCHEC compilation firmware :"$'\n'"$(grep -E 'error|Error' "$journal" | head -15)"$'\n'
fi
avert_fw=$(grep -E "^$RACINE/(src|[a-z_]+\.c).*warning:" "$journal" | sort -u | head -10)
[ -n "$avert_fw" ] && avertissements+="Avertissements de compilation :"$'\n'"$avert_fw"$'\n'

# --- 2. Tests unitaires C (sur le Mac) ---------------------------------------
mkdir -p build/tests_hote
nb_tests_c=0
while IFS= read -r test; do
    nb_tests_c=$((nb_tests_c + 1))
    nom=$(basename "$test" .c)
    sources=$(grep -m1 '^// SOURCES:' "$test" | sed 's|^// SOURCES:||')
    # shellcheck disable=SC2086
    if ! cc -std=c11 -Wall -Wextra -Werror -Isrc -Itests/mocks -o "build/tests_hote/$nom" \
            "$test" $sources >"$journal" 2>&1; then
        echecs+="ÉCHEC compilation $test :"$'\n'"$(head -15 "$journal")"$'\n'
    elif ! "build/tests_hote/$nom" >"$journal" 2>&1; then
        echecs+="ÉCHEC $test :"$'\n'"$(tail -15 "$journal")"$'\n'
    fi
done < <(find tests -name 'test_*.c' 2>/dev/null | sort)

# --- 3. Python ----------------------------------------------------------------
for script in outils/*.py; do
    [ -f "$script" ] || continue
    if ! python3 -m py_compile "$script" 2>"$journal"; then
        echecs+="ÉCHEC syntaxe $script :"$'\n'"$(cat "$journal")"$'\n'
    fi
done
if find tests -name 'test_*.py' 2>/dev/null | grep -q .; then
    if ! python3 -m unittest discover -s tests -p 'test_*.py' -t . >"$journal" 2>&1; then
        echecs+="ÉCHEC tests Python :"$'\n'"$(tail -20 "$journal")"$'\n'
    fi
fi

# --- 4. Règle : 30 lignes max par fonction -------------------------------------
# Une fonction commence par une ligne non indentée finissant par « { » et se termine
# par « } » en colonne 0 ; on compte les lignes du corps.
trop_longues=$(find . -name '*.c' -not -path './build*' -not -path './tests/*' | sort | while read -r f; do
    awk -v f="$f" '
        /^[A-Za-z_].*\)[ \t]*\{[ \t]*$/ { nom = $0; debut = NR; next }
        debut && /^\}/ { n = NR - debut - 1; if (n > 30) printf "%s:%d %d lignes : %s\n", f, debut, n, nom; debut = 0 }
    ' "$f"
done)
[ -n "$trop_longues" ] && avertissements+="Fonctions de plus de 30 lignes (règle CLAUDE.md) :"$'\n'"$trop_longues"$'\n'

# --- Bilan --------------------------------------------------------------------
if [ -n "$echecs" ]; then
    printf '%s\n%s' "$echecs" "$avertissements" >&2
    exit 2
fi
resume="Tests OK : firmware compilé, $nb_tests_c test(s) C passé(s)."
if [ -n "$avertissements" ]; then
    # Succès avec avertissements : renvoyés à Claude comme contexte, sans bloquer
    jq -n --arg ctx "$resume"$'\n'"$avertissements" \
        '{hookSpecificOutput: {hookEventName: "PostToolUse", additionalContext: $ctx}}'
else
    echo "$resume"
fi
exit 0

#!/usr/bin/env bash
set -euo pipefail

# --- Config (override via env) ---
SIM=${SIM:-"./src/sim"}
PATTERN=${PATTERN:-"./inputs/*.x"}
OUTDIR=${OUTDIR:-"./dumps"}
CMDS=${CMDS:-"rdump; mdump 0x10000000 0x10000040; go; rdump; mdump 0x10000000 0x10000040"} # comandos separados por ';'

shopt -s nullglob 
mkdir -p "$OUTDIR"

# Verificación rápida del simulador
if [[ ! -x "$SIM" ]]; then
  echo "ERROR: SIM no existe o no es ejecutable: $SIM" >&2
  exit 1
fi

for x in $PATTERN; do
  # base: nombre sin ruta ni extensión .x
  fname=${x##*/}        # quita ruta
  base=${fname%.x}      # quita extensión .x

  echo "[RUN] $x"

  # Enviamos los comandos por stdin (pipeline desde un bloque)
  {
    IFS=';'
    for c in $CMDS; do
      # Imprime cada comando tal cual, una línea
      printf '%s\n' "$c"
    done
    printf 'quit\n'
  } | "$SIM" "$x" >"$OUTDIR/${base}.log" 2>&1

  # El simulador deja 'dumpsim' en el CWD → lo renombramos por test
  mv -f dumpsim "$OUTDIR/${base}.dump"

  echo "  → dump: $OUTDIR/${base}.dump"
  echo "  → log : $OUTDIR/${base}.log"
done

echo "Done. Dumps en: $OUTDIR"

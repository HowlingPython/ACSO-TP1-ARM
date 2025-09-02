#!/usr/bin/env bash
set -euo pipefail

# --- Config (override via env) ---
SIM=${SIM:-"./src/sim"}
PATTERN=${PATTERN:-"./inputs/*.x"}   # glob con ** si tu bash tiene globstar
OUTDIR=${OUTDIR:-"./dumps"}
CMDS=${CMDS:-"rdump; run 1; go; rdump; mdump 0x10000000 0x10000040"} # comandos del shell separados por ';'
# Para volcar memoria también, podés exportar: CMDS='go; rdump; mdump 0x10000000 0x10000040'

shopt -s nullglob globstar
mkdir -p "$OUTDIR"

for x in $PATTERN; do
  base=$(basename "$x".x)
  echo "[RUN] $x"
  # Ejecuta comandos del shell vía here-string
  # Nota: cada run sobreescribe dumpsim → lo renombramos por test
  { IFS=';'; for c in $CMDS; do echo "$c"; done; echo quit; } | "$SIM" "$x" >/dev/null
  mv -f dumpsim "$OUTDIR/${base}.dump"
  echo "  → dump: $OUTDIR/${base}.dump"
done

echo "Done. Dumps en: $OUTDIR"
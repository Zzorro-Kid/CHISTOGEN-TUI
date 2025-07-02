#!/bin/bash

BIN="./syscleaner"

TERMINALS=(
    "konsole -e"
    "gnome-terminal --"
    "xfce4-terminal --command"
    "xterm -e"
    "tilix -e"
    "lxterminal -e"
    "mate-terminal -e"
)

for term in "${TERMINALS[@]}"; do
    CMD=$(echo $term | awk '{print $1}')
    if command -v $CMD &> /dev/null; then
        echo "✅ Открываю в: $CMD"
        exec $term "$BIN"
        exit 0
    fi
done

echo "❌ Не найден терминал. Запустите вручную: $BIN"
exit 1

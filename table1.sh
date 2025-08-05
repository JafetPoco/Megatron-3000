#!/bin/bash

./megatron < input 2>/dev/null > datos_raw.txt

tail -n +3 datos_raw.txt | grep -E '^[0-9]+\s+[0-9.]+' > datos_filtrados.txt

if [ ! -s datos_filtrados.txt ]; then
  echo "Error: El archivo filtrado está vacío o malformado."
  exit 1
fi

gnuplot -persist <<-EOF
    set title "Gráfico de valores"
    set xlabel "Índice"
    set ylabel "Valor"
    set grid
    plot "datos_filtrados.txt" using 1:2 with points title "Datos"
EOF

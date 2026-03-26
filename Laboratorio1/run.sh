#!/bin/bash

# Nome do arquivo fonte e executável
SRC="lab1.cpp"
EXEC="lab1_exercicios"
IMG="./imagem.jpg"

echo "🔧 Compilando $SRC..."

# Compilação
g++ "$SRC" -o "$EXEC" `pkg-config --cflags --libs opencv4`

# Verifica se compilou com sucesso
if [ $? -ne 0 ]; then
    echo "❌ Erro na compilação!"
    exit 1
fi

echo "✅ Compilação concluída!"

echo "🚀 Executando o programa..."

# Execução
./"$EXEC" "$IMG"
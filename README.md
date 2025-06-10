# 🚀 Xadrez Quântico

Um jogo de xadrez que incorpora princípios da mecânica quântica, desenvolvido em C++ com CMake.

## 📁 Estrutura do Projeto

```
Quantic-chess/
├── src/           # Código fonte principal
├── include/       # Arquivos de cabeçalho (.h/.hpp)
├── build/         # Arquivos de compilação (gerado pelo CMake)
├── tests/         # Testes unitários
├── docs/          # Documentação
├── lib/           # Bibliotecas externas
├── CMakeLists.txt # Configuração do CMake
└── README.md      # Este arquivo
```

## 🛠️ Pré-requisitos

- **CMake** (versão 3.16 ou superior)
- **Compilador C++** com suporte ao C++17:
  - GCC 7+ ou
  - Clang 5+ ou 
  - MSVC 2017+ (Visual Studio)

## 🔧 Como Compilar

### Primeira Compilação

1. **Navegue até o diretório do projeto:**
   ```bash
   cd Quantic-chess
   ```

2. **Configure o projeto com CMake:**
   ```bash
   cmake -B build -S .
   ```

3. **Compile o projeto:**
   ```bash
   cmake --build build
   ```

### Compilação Rápida (após a primeira vez)

```bash
cmake --build build
```

## ▶️ Como Executar

### No Windows (PowerShell):
```powershell
.\build\Debug\xadrez_quantico.exe
```

### No Linux/Mac:
```bash
./build/xadrez_quantico
```

## 🧪 Compilar com Testes

Para incluir os testes na compilação:

```bash
cmake -B build -S . -DBUILD_TESTS=ON
cmake --build build
```

## 🐛 Modo Debug

Para compilar em modo debug:

```bash
cmake -B build -S . -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```

## 🚀 Modo Release

Para compilar otimizado para produção:

```bash
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

## 🧹 Limpeza

Para limpar os arquivos de compilação:

```bash
# Remove a pasta build inteira
rm -rf build

# Ou no Windows PowerShell:
Remove-Item -Recurse -Force build
```

## 📋 Próximos Passos

- [ ] Implementar classes básicas do tabuleiro
- [ ] Adicionar sistema de peças quânticas
- [ ] Implementar mecânicas de superposição
- [ ] Criar sistema de medição/colapso
- [ ] Desenvolver interface de usuário
- [ ] Adicionar testes unitários
- [ ] Implementar IA para oponente

## 🤝 Contribuição

Este projeto está em desenvolvimento inicial. Contribuições são bem-vindas! 
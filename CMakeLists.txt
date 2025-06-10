cmake_minimum_required(VERSION 3.16)

# Define o projeto
project(XadrezQuantico
    VERSION 1.0.0
    DESCRIPTION "Um jogo de xadrez quântico implementado em C++"
    LANGUAGES CXX
)

# Configura o padrão C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Configurações de compilação
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Flags de compilação
set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

# Inclui diretórios
include_directories(include)

# Coleta todos os arquivos fonte
file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "include/*.h" "include/*.hpp")

# Cria o executável principal
add_executable(xadrez_quantico ${SOURCES} ${HEADERS})

# Configurações específicas para o executável
target_include_directories(xadrez_quantico PRIVATE include)

# Habilita testes se solicitado
option(BUILD_TESTS "Compilar testes" OFF)
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Informações de instalação
install(TARGETS xadrez_quantico 
    RUNTIME DESTINATION bin
)

# Configurações adicionais de debugging
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(xadrez_quantico PRIVATE DEBUG_MODE=1)
endif() 
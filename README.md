# Parcial Final - Game Dungeon Crawler 

## Integrante
- **Samuel David Moreno Cruz**

---

## Desarrollos Innovadores Implementados

El proyecto cuenta con 3 desarrollos adicionales:

1. **Motor de Renderizado con Escapes ANSI (Zero-Flicker):** Se usa códigos ANSI para reescribir los caracteres de forma inmediata para que el juego se sienta mas fluido.
2. **Sistema de Alertas Visuales por Ticks:** Se añadio un contador de tiempo interno (las ticks). A medida que pasa el tiempo los bordes del laberinto cambian de color (azul, amarillo, rojo) para meterle presión al jugador sin gastar nada de memoria RAM.
3. **Captura de Teclado Multiplataforma en Tiempo Real:** El juego usa las teclas W, A, S, D al instante tanto en Windows como en Linux y Mac modificando los registros de la terminal.

---

## Instrucciones de Compilación y Ejecución

Este proyecto usa **CMake** para automatizar el proceso y garantizar que funcione en Windows, Linux y macOS.

### 1. Requisitos Previos
Es importante antes de empezar tener instalado:
* Un compilador compatible con C++17 
* CMake versión 3.10

### 2. Clonar el Repositorio
Abra la terminal de su sistema, escriba el comando git clone seguido del link de este repositorio, y luego entre a la carpeta del juego:

git clone https://github.com/sshe56697-eng/Proyecto-video-juego-C-
cd Proyecto-video-juego-C-

### Pasos para compilar
Ejecute los siguientes comandos uno por uno dentro de la raíz del proyecto:

mkdir build
cd build
cmake ..
cmake --build .

### Pasos para jugar

#### En Windows (CMD):
.\Debug\ProjectGame.exe

#### En Linux / macOS (Terminal):
./ProjectGame

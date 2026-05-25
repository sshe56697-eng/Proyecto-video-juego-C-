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
Abra la terminal de su sistema (CMD en Windows o Bash en Linux/macOS), sitúese en la raíz del proyecto y ejecute los comandos correspondientes:

### 1. Requisitos Previos
Es importante antes de empezar tener instalado:
* Un compilador compatible con C++17 
* CMake versión 3.10

### En Windows (Consola CMD)
g++ -Iinclude src/main.cpp -o ProjectGame.exe

ProjectGame.exe

### En Linux / macOS (Terminal Bash)
g++ -Iinclude src/main.cpp -o ProjectGame

./ProjectGame

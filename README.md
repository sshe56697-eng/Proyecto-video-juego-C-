# Parcial Final - Game Dungeon Crawler 

## Integrante
- **Samuel David Moreno Cruz**

---

## Desarrollos Innovadores Implementados

El projecto cuenta con 3 desarrollos adicionales:

1. **Motor de Renderizado con Escapes ANSI (Zero-Flicker):** Se usa códigos ANSI para reescribir los caracteres de forma inmediata para que el juego se sienta mas fluido.
2. **Sistema de Alertas Visuales por Ticks:** Se añadio un contador de tiempo interno (las ticks). A medida que pasa el tiempo los bordes del laberinto cambian de color (azul, amarillo, rojo) para meterle presión al jugador sin gastar nada de memoria RAM.
3. **Captura de Teclado Multiplataforma en Tiempo Real:** El juego usa las teclas W, A, S, D al instante tanto en Windows como en Linux y Mac modificando los registros de la terminal.

---

## Instrucciones de Compilación y Ejecución

Este proyecto usa **CMake** para que funcione en cualquier sistema operativo (Windows, Linux y macOS) de forma automática.

### Pasos para compilar:
Abra la terminal en la raíz de la carpeta del proyecto (`ProjectGame`) y ejecute los siguientes comandos uno por uno:

1. Crear una carpeta para compilar y entrar a ella:
  mkdir build
cd build
cmake ..
cmake --build .

### Pasos para jugar:
Una vez compilado, escriba esto en la misma terminal dentro de la carpeta `build`:

En Windows:
ProjectGame.exe

En Linux / macOS:
./ProjectGame

# Explicación Detallada de Desarrollos Innovadores
Este documento detalla la justificación técnica, la necesidad y la implementación de las características avanzadas añadidas al proyecto para superar los requisitos mínimos exigidos.

## 1. Motor de Renderizado con Escapes ANSI (Zero-Flicker)
¿Para qué se implementó?
Se implementó para lograr un refresco de pantalla ultra fluido en la consola. En lugar de borrar toda la pantalla y redibujarla desde cero en cada movimiento (lo que causa un parpadeo constante y molesto), este motor actualiza únicamente las posiciones de los caracteres que cambiaron (el jugador, los enemigos y los objetos).

### ¿Por qué se consideró necesario?
En los juegos de consola tipo Dungeon Crawler, el parpadeo de pantalla (flickering) arruina la experiencia de usuario y cansa la vista rápidamente. Al eliminar este problema, el juego adquiere una calidad visual mucho más cercana a la de un juego comercial de terminal, mejorando la jugabilidad y la respuesta visual.

### ¿Cómo se llevó a cabo su implementación?
Se utilizaron códigos de escape ANSI directos enviados a través del flujo estándar de salida (std::cout). Mediante la instrucción \033[H se reposiciona el cursor en el origen de la terminal de manera inmediata, reescribiendo el buffer de caracteres de forma estática y controlada sin necesidad de llamar a comandos pesados del sistema operativo.

## 2. Sistema de Alertas Visuales Dinámicas por Ticks
¿Para qué se implementó?
Se implementó para añadir una mecánica de juego basada en la presión temporal y la urgencia, alertando al jugador sobre el peligro inminente a medida que transcurren los turnos dentro de la mazmorra.

### ¿Por qué se consideró necesario?
Para cumplir estrictamente con la regla de diseño de memoria asignada por el profesor (prohibición absoluta de usar new o delete dentro del bucle principal de juego), necesitábamos una mecánica ambiental que cambiara el entorno en tiempo real sin crear objetos dinámicos ni sobrecargar la memoria RAM con hilos secundarios (threads).

### ¿Cómo se llevó a cabo su implementación?
Se diseñó un contador lógico interno en el game-loop que registra cada ciclo de actualización (ticks). Dependiendo del rango en el que se encuentre este contador, el juego aplica diferentes códigos cromáticos ANSI a los caracteres que componen las paredes del laberinto, cambiando el color del mapa de Azul (seguro), a Amarillo (precaución) y finalmente a Rojo (peligro crítico) usando variables estáticas precargadas.

## 3. Captura de Teclado Multiplataforma en Tiempo Real
¿Para qué se implementó?
Se implementó para que el jugador pueda controlar al personaje usando las teclas W, A, S, D de manera inmediata, logrando un movimiento continuo y natural por el mapa.

### ¿Por qué se consideró necesario?
Por defecto, las terminales de comandos retienen la entrada de texto y exigen que el usuario presione la tecla Enter para enviar una instrucción al programa. Obligar al jugador a presionar Enter en cada paso habría destruido por completo la jugabilidad del Dungeon Crawler.

### ¿Cómo se llevó a cabo su implementación?
Se creó un sistema de lectura de buffer no bloqueante con directivas de preprocesamiento para separar los sistemas operativos. En entornos Windows se utiliza la función nativa _getch() de la librería <conio.h>. Para entornos Unix (Linux y macOS), se modifican temporalmente los registros de la terminal a través de la estructura termios, desactivando el modo canónico y el eco local para capturar los bytes de las teclas instantáneamente desde el flujo de entrada estándar.

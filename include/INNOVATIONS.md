# Explicación de los Desarrollos Innovadores
Aqui se explica los desarrollos innovadores que se le hicieron al juego para que funcione mejor.

## 1. Movimiento Fluido de Pantalla (Evitar el Parpadeo)
### ¿Para qué se implementó?
Para que el mapa del juego se refresque de forma natural y limpia en la pantalla cada vez que el jugador se mueve sin flickering.

¿Por qué se consideró necesario?
### Porque el comando típico de borrar toda la pantalla en cada turno hace que la consola parpadee mucho, lo cual es molesto para la vista y se ve mal. Con esto, el juego se siente continuo.

### ¿Cómo se llevó a cabo su implementación?
Se usaron comandos de texto invisibles (códigos ANSI) que le dicen a la terminal que mueva el cursor arriba al inicio y pinte el mapa encima del anterior, en vez de borrar y redibujar todo.

## 2. Alertas de Colores según el Tiempo (Ticks)
### ¿Para qué se implementó?
Para que el juego se sienta mas ameno cambiando el color de las paredes del laberinto a medida que pasan los turnos.

### ¿Por qué se consideró necesario?
Porque necesitábamos una mecánica que cambiara el entorno con el tiempo sin crear ni destruir objetos en memoria constantemente.

### ¿Cómo se llevó a cabo su implementación?
Se puso un contador interno que suma 1 en cada movimiento por cada tick. Según el número del contador, el juego le aplica un color diferente a las paredes: Azul (seguro), Amarillo (alerta) o Rojo (peligro).

## 3. Caminar al Instante sin presionar Enter
### ¿Para qué se implementó?
Para que el personaje avance por el mapa inmediatamente apenas toques las teclas W, A, S o D.

### ¿Por qué se consideró necesario?
Porque normalmente las consolas te obligan a escribir la letra y presionar la tecla Enter para que el programa reaccione. Jugar un videojuego teniendo que hundir Enter en cada paso sería imposible.

### ¿Cómo se llevó a cabo su implementación?
Se configuró el código para que detecte la tecla presionada al instante. En Windows se usa una función directa del sistema, y en Linux o Mac se cambia un ajuste interno de la terminal para que no espere el Enter.

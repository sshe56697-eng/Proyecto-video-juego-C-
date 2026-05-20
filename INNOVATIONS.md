# Explicación de los Desarrollos Innovadores

Se busca detallar el propósito la necesidad y la implementación de las tres mejoras añadidas al proyecto.

---

### 1. Sistema de Limpieza de Pantalla (Zero-Flicker ANSI Engine)

- **¿Para qué se implementó?**
  Para actualizar el mapa de juego en cada movimiento del jugador o enemigos sin generar el parpadeo visual en la consola.
- **¿Por qué se consideró necesario?**
  El uso de `system("cls")` o `system("clear")` realiza llamadas al sistema operativo que suspenden el bucle del juego, crean un subproceso nuevo y vacían el buffer de forma brusca, aumentando el uso de recursos. Para cumplir con la restricción de optimización de hardware clásico, se requería una alternativa que redujera esta sobrecarga.
- **¿Cómo se implementó?**
  Eliminé la librería `<cstdlib>` para evitar llamadas al sistema. Programé la función `PlatformClear()` utilizando `printf` con las secuencias de escape ANSI `\033[H\033[J`. Esto mueve el cursor de la terminal a la posición inicial (0,0) y sobreescribe los caracteres directamente en el buffer de salida.

---

### 2. Ciclo Cromático por Ticks

- **¿Para qué se implementó?**
  Para añadir una mecánica de progresión de tiempo y cambio visual en la interfaz según las acciones realizadas.
- **¿Por qué se consideró necesario?**
  Debido a la prohibición de usar memoria dinámica (`new`/`delete`) y librerías gráficas externas, era necesario añadir dinamismo al entorno empleando estructuras estáticas y sin consumir bytes adicionales en la memoria RAM.
- **¿Cómo se implementó?**
  Definí una variable global estática `gTicks` que incrementa en 1 con cada movimiento. En la función `Render`, implementé una lógica condicional en `BorderColor` que evalúa esta variable: si es menor o igual a 20 ticks inyecta el código ANSI azul, entre 21 y 40 el amarillo, y más de 40 el rojo, modificando el color de los bordes `#` en el renderizado del mapa.

---

### 3. Modificación de la Terminal para Entrada Directa (Raw Input)

- **¿Para qué se implementó?**
  Para capturar las pulsaciones de las teclas (`W`, `A`, `S`, `D`) de manera inmediata sin requerir que el usuario presione la tecla `Enter` en cada acción.
- **¿Por qué se consideró necesario?**
  Por defecto, la entrada estándar de C++ (`std::cin`) opera en modo canónico y retiene la ejecución hasta recibir un salto de línea, lo cual impide la fluidez en el control de un juego en tiempo real.
- **¿Cómo se implementó?**
  Estructuré una solución multiplataforma mediante directivas de preprocesamiento `#ifdef _WIN32`. En Windows, el código enlaza con `_getch()` de la librería `<conio.h>`. En Linux y macOS, utilicé las librerías `<termios.h>` y `<unistd.h>` en las funciones `TermRaw()` y `TermRestore()` para desactivar las banderas de eco local (`ECHO`) y entrada canónica (`ICANON`) al iniciar el programa, leyendo los caracteres bit a bit y restaurando los valores originales al salir.
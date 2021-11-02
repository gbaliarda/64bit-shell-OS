# Manual de Usuario
Este es un sistema operativo básico basado en Barebonesx64 por RowDaBoat. Para poder correr el sistema se debe tener instalado qemu-system.
Para compilar los archivos, se recomienda tener docker instalado con el contenedor `agodio/itba-so:1.0` dado por la cátedra.
Pasos para compilar y ejecutar el SO:
1. La primera vez que se use, debe correrse ```cd Toolchain``` y luego ```make all```.
2. Para la compilación del proyecto, desde la carpeta inicial se puede seleccionar el memory manager buddy o el basado en "HEAP2" (free list): 
- `make MM=BUDDY` (para utilizar el buddy).
- `make all` (para utilizar HEAP2).
3. Por último resta correr el sistema utilizando qemu con el comando ```./run.bat``` en windows.

## Shell
Intéprete de comandos del usuario, el sistema inicia al mismo por defecto. 
Dispone de las siguientes funcionalidades:
- ***help:*** Despliega en pantalla las funciones disponibles para el usuario. 
- ***clear:***  Limpia la pantalla de los comandos ingresados.
- ***ps:*** Despliega la lista de procesos y sus detalles.
- ***kill:***  Elimina un proceso dado su PID.
- ***nice:*** Cambia la prioridad de un proceso dado su PID y la nueva prioridad.
- ***block:*** Alterna el estado de un proceso dado su PID.
- ***mem:*** Imprime el estado de la memoria.
- ***pipe:*** Despliega información de los pipes activos.
- ***sem:*** Despliega información de los semaforos activos.
- ***wc:*** Cuenta las lineas ingresadas por stdin.
- ***filter:*** Filtra las vocales de lo que recibe por stdin.
- ***cat:*** Imprime en pantalla los caracteres recibidos por stdin.
- ***phylo:*** Resolución del problema de sincronización de los filósofos.
- ***loop:*** Imprime un saludo cada x segundos recibidos por parámetro.
- ***testproc:*** Testea la creación de procesos y sus cambios de estado.
- ***testprio:*** Testea el algoritmo del scheduler.
- ***testsync:*** Prueba la sincronizacion de semáforos.
- ***testnosync:*** Prueba la sincronizacion sin el uso de semáforos.
- ***testmm:*** Prueba el manejo de memoria del memory manager.

## Teclas especiales
- ***F2*** Exit
- ***F3*** EOF

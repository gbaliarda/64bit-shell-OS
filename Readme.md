# Manual de Usuario
Sistema operativo básico basado en Barebonesx64 por RowDaBoat. Para poder correr el sistema se debe tener instalado qemu-system a fin de visualizar y docker instalado para el compilado con el contenedor `agodio/itba-so:1.0` dado por la cátedra. Al tener dichos requisitos se debe compilar desde docker y luego ejecutar usando:
1. La primera vez que se accede ```cd Toolchain``` y luego ```make all```. Este paso solo se debe realizar la primera vez que se descarga el repositorio. 
2. Para la compilación del proyecto vuelva al directorio anterior, se puede pasar un parametro para determinar el memory manager que se utilizara de la siguiente forma: 
- `make MM=BUDDY` (para utilizar el buddy).
- `make all` (para utilizar heap2).
De no funcionar, se puede cambiar la variable `MM` por la deseada dentro del `makefile` de `kernel`. y utilizando ```make all```.
3. Por último resta correr el sistema utilizando qemu con el comando ```./run.bat```.

## Shell
Intéprete de comandos del usuario, el sistema inicia al mismo por defecto. 
Dispone de las siguientes funcionalidades:
- **help**: Despliega en pantalla las funciones disponibles para el usuario. 
- **clear**:  Limpia la pantalla de los comandos ingresados.
- ***ps:*** Despliega el estado de los procesos.
- ***kill:***  Elimina el proceso cuyo pid es el recibido como argumento de entrada.
- ***nice:*** Cambia la prioridad del proceso cuyo pid es el recibido como argumento de entrada.
- ***block:*** Bloquea o desbloquea el proceso cuyo pid es el recibido como argumento de entrada. (si esta bloqueado lo desbloquea y viceversa)
- ***mem:*** Imprime el estado de la memoria.
- ***pipe:*** Despliega información sobre de los pipes activos.
- ***sem:*** Despliega información sobre de los semaforos activos.
- ***wc:*** Cuenta las lineas recibidas como argumento de entrada.
- ***filter:*** Filtra las vocales recibidas como argumento de entrada.
- ***cat:*** Imprime en pantalla los caracteres recibidos como argumento de entrada.
- ***phylo:*** Dilema de los filosofos, para más indicaciones se muestran en su ejecución.
- ***loop:*** Crea un proceso el cual itera sobre si mismo con un tiempo de parametro.
- ***testproc:*** Prueba la creacion de procesos con el scheduler (archivo dado por la cátedra).
- ***testprio:*** Prueba la prioridad del scheduler (archivo dado por la cátedra).
- ***testsync:*** Prueba la sincronizacion de semaforos (archivo dado por la cátedra).
- ***testnosync:*** Prueba la sincronizacion sin el uso de semaforos (archivo dado por la cátedra).
- ***testmm:*** Prueba el manejo de memoria del memory manager (archivo dado por la cátedra).

## Teclas especiales
- ***F2*** Exit
- ***F3*** EOF


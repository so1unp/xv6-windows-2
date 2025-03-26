# Implementación de la llamada al sistema trace

La llamada al sistema `sys_trace` permite habilitar o deshabilitar la trazabilidad de las llamadas al sistema realizadas por los procesos. En esta guía se explica cómo está implementada en el código fuente de xv6:

---

## 1. **Definición del identificador de la llamada al sistema**
En el archivo `syscall.h` se define un identificador único para la llamada al sistema `sys_trace`:

```c
#define SYS_trace 22
```

Este identificador (`22`) se utiliza para asociar la llamada al sistema con su implementación en el kernel. Debe ser único.

---

## 2. **Declaración de la función en el kernel**
En el archivo `sysproc.c` se implementa la función `sys_trace`, con la lógica de la llamada al sistema:

```c
int trace;

int sys_trace(void) {
    if (argint(0, &trace) < 0)  // Obtiene el argumento pasado desde el espacio de usuario
        return -1;              // Retorna error si no se puede obtener el argumento
    return 0;                   // Retorna con éxito
}
```

- **`trace`**: Es una variable global que se utiliza para habilitar (`trace = 1`) o deshabilitar (`trace = 0`) la trazabilidad.
- **`argint(0, &trace)`**: Obtiene el primer argumento pasado a la llamada al sistema desde el espacio de usuario y lo almacena en la variable global `trace`.

---

## 3. **Registro en la tabla de llamadas al sistema**
En el archivo `syscall.c` la función `sys_trace` se registra en una tabla de llamadas al sistema (`syscalls[]`), donde se asocia el identificador `SYS_trace` con la función `sys_trace`:

```c
extern int sys_trace(void);

static int (*syscalls[])(void) = {
    ...
    [SYS_trace] sys_trace
};
```

Esto permite que el kernel invoque la función `sys_trace` cuando se realiza una llamada al sistema con el identificador `SYS_trace`.

---

## 4. **Trazabilidad**
La función `syscall` en `syscall.c` es el punto de entrada de todas las llamadas al sistema. Si la trazabilidad está habilitada (`trace > 0`), se imprime por la salida estándar información de cada llamada al sistema invocada:

```c
void syscall(void) {
    int num;
    struct proc *curproc = myproc();

    num = curproc->tf->eax;  // Obtiene el número de la llamada al sistema
    if (num > 0 && num < NELEM(syscalls) && syscalls[num]) {
        curproc->tf->eax = syscalls[num]();  // Ejecuta la llamada al sistema
        if (trace > 0) {  // Si la trazabilidad está habilitada
            cprintf("[%d] %s: %d\n", num, syscalls_names[num], curproc->tf->eax);
        }
    } else {
        cprintf("%d %s: unknown sys call %d\n",
                curproc->pid, curproc->name, num);
        curproc->tf->eax = -1;
    }
}
```

- **`trace > 0`**: Si la variable global `trace` es mayor que 0, se imprime información sobre la llamada al sistema.
- **`cprintf`**: Imprime el número de la llamada al sistema, su nombre (obtenido de `syscalls_names[]`) y el valor de retorno (almacenado en `curproc->tf->eax`).

---

## 5. **wrapper en el espacio de usuario**
En el archivo `usys.S` se define un _wrapper_ para que los programas en espacio de usuario puedan invocar la llamada al sistema `trace` de manera sencilla:

```asm
SYSCALL(trace)
```

Esto genera una función en ensamblador que realiza la interrupción del sistema (`int $T_SYSCALL`) con el número de llamada al sistema `SYS_trace`.

---

## 6. **Prototipo en el espacio de usuario**
En el archivo `user.h` se declara el prototipo de la función `trace` para que los programas en espacio de usuario puedan utilizarla:

```c
int trace(int);
```

---

## 7. **Uso de la llamada al sistema en un programa de usuario**
Un programa en espacio de usuario puede invocar la llamada al sistema `trace` para habilitar o deshabilitar la trazabilidad. En el archivo `trace.c`:

```c
#include "types.h"
#include "user.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf(2, "Uso: %s [0|1]\n", argv[0]);
        exit();
    }
    trace(atoi(argv[1]));  // Llama a la syscall trace con el argumento 0 o 1
    exit();
}
```

Este programa permite al usuario habilitar (`trace 1`) o deshabilitar (`trace 0`) la trazabilidad desde la línea de comandos.

---

## Resumen de la implementación:
1. **Definición del identificador**: `SYS_trace` se define en `syscall.h`.
2. **Implementación de la lógica**: La función `sys_trace` se implementa en `sysproc.c`.
3. **Registro en la tabla de llamadas al sistema**: `sys_trace` se registra en `syscalls[]` en `syscall.c`.
4. **Trazabilidad**: La función `syscall` imprime información sobre las llamadas al sistema si `trace > 0`.
5. **Wrapper**: Se define en `usys.S` para invocar la llamada al sistema desde el espacio de usuario.
6. **Prototipo en user.h**: Permite que los programas de usuario invoquen `trace(int)`.
7. **Programa de usuario**: El programa `trace.c` permite habilitar/deshabilitar la trazabilidad.

Esta implementación permite al kernel mostrar información sobre las llamadas al sistema realizadas por los procesos, útil para depuración y monitoreo.

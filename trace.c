// Activa/Desactiva la traza de llamadas al sistema.

#include "types.h"
#include "user.h"
#include "date.h"

int main(int argc, char *argv[])
{
  if (argc != 2) {
      printf(2, "Uso: %s [0|1]\n", argv[0]);
      exit();
  }
  trace(atoi(argv[1]));
  exit();
}

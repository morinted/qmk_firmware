#include "crkbd.h"

const char *read_sl(void) {
  static char sl[] = {
      0, 1, 2, 4, 8, 16
    };

  return sl;
}
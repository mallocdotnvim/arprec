#include <stdio.h>
#include <stdlib.h>

#include "src/arprec.h"

int main(void)
{
  bigint a = {0}, b = {0};
  bigint temp = {0};

  bigint_append(&b, 1);

  for (size_t i = 0; i < 10000 * 10000; i++) {
    bigint_add(&temp, &a, &b);

    bigint swap = a;
    a = b;
    b = temp;
    temp = swap;

    bigint_print_var(a);
  } 

  
  return 0;
}
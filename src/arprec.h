#ifndef ARPREC_H
#define ARPREC_H

#include <stddef.h>
#include <stdint.h>

#define bigint_print_var(x) \
  printf("%s = ", #x); \
  bigint_print(&(x)); \
  printf("\n");

typedef struct bigint_impl{
  uint64_t *index;
  size_t capacity;
  size_t length;
  int sign;
} bigint;

static inline void bigint_reserve(bigint *self, size_t exp_capacity);
void bigint_append(bigint *self, uint64_t value);
void bigint_randomize(bigint *self, size_t length);
void bigint_print(bigint *self);

static inline void bigint_add_abs(bigint *self, const bigint *a, const bigint *b);
static inline void bigint_sub_abs(bigint *self, const bigint *a, const bigint *b);
static inline int bigint_compare_abs(const bigint *a, const bigint *b);

void bigint_add(bigint *self, const bigint *a, const bigint *b);
void bigint_sub(bigint *self, const bigint *a, const bigint *b);

int bigint_compare(const bigint *a, const bigint *b);

void bigint_copy(bigint *self, const bigint *other);


#endif
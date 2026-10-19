#include <inttypes.h>
#include <limits.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "arprec.h"

#define BIGINT_BASE UINT64_C(1000000000000000000)


void bigint_reserve(bigint *self, size_t exp_capacity)
{
  if (exp_capacity < self->capacity) return;

  if (self->capacity < exp_capacity) {
    if (self->capacity == 0) self->capacity = 16;

    while (self->capacity < exp_capacity) {
      self->capacity *= 2;
    }
  }
  self->index = realloc(self->index, self->capacity * sizeof(*self->index));
}

void bigint_append(bigint *self, uint64_t value)
{
  bigint_reserve(self, (self->length + 1));
  self->index[self->length] = value;
  self->length++;
}

// The structure of bigint:
// [sign][10^18 ... 0][10^18 ... 0][10^18 ... 0]
//  ^      ^                         ^
// (0, 1) Least significant         Most significant

void bigint_randomize(bigint *self, size_t digits)
{
  if (digits == 0) {
    self->length = 0;
    self->sign = 0;
    return;
  }

  // Decide how many chunk should we reserve
  size_t length = (digits + 17) / 18;
  
  bigint_reserve(self, (length + 1));
  self->length = length;
  int seed = 0;
  
  if (!seed) {
    srand((unsigned)time(NULL));
    seed = 1;
  }

  // The num of most significant chunk
  size_t first = digits % 18;
  if (first == 0) first = 18;

  
  uint64_t limit = 1;
  for (size_t i = 0; i < first; i++) {
    limit *= 10;
  }
  
  self->index[length - 1] = 1 + ((uint64_t)rand() % (limit - 1));

  // Randomize the remain chunks
  for (size_t i = 0; i < length - 1; i++) {
    self->index[i] = ((uint64_t)rand() % BIGINT_BASE);
  }
  self->sign = rand() % 2;
}

void bigint_print(bigint *self) 
{
  if (self->length == 0) {
    putchar('0');
    return;
  }
  printf("%s", (self->sign) ? "-" : "");
  for (size_t i = 0; i < self->length; i++) {
    if (i == 0) {
      printf("%"PRIu64, self->index[self->length - i - 1]);
    } else {
      printf("%018"PRIu64, self->index[self->length - i - 1]);
    }
  }
}

void bigint_add_abs(bigint *self, const bigint *a, const bigint *b)
{
  self->length = (a->length > b->length) ? a->length : b->length;
  bigint_reserve(self, (self->length + 1));
  uint64_t carry = 0;
  for (size_t i = 0; i < self->length; i++) {
    uint64_t a_val = (i < a->length) ? a->index[i] : 0,
             b_val = (i < b->length) ? b->index[i] : 0;

    uint64_t sum = a_val + b_val + carry;
    self->index[i] = sum % BIGINT_BASE;
    carry = sum / BIGINT_BASE;
  }

  if (carry > 0) {
    self->index[self->length++] = carry;
  }
}

void bigint_sub_abs(bigint *self, const bigint *a, const bigint *b)
{
  self->length = (a->length > b->length) ? a->length : b->length;
  bigint_reserve(self, (self->length + 1));
  uint64_t borrow = 0;
  for (size_t i = 0; i < self->length; i++) {
    uint64_t a_val = (i < a->length) ? a->index[i] : 0,
             b_val = (i < b->length) ? b->index[i] : 0;

    uint64_t sub = b_val + borrow;

    if (a_val < sub) {
      self->index[i] = a_val + BIGINT_BASE - sub;
      borrow = 1;
    } else {
      self->index[i] = a_val - sub;
      borrow = 0;
    }
  }

  
}

// greater than: a > b -> 1
// equal: a = b -> 0
// less then a < b -> -1

int bigint_compare_abs(const bigint *a, const bigint *b) 
{
  if (a->length > b->length) return 1;
  if (b->length > a->length) return -1;
  size_t i = (a->length > b->length) ? a->length : b->length;
  for (; i > 0; i--) {
    uint64_t a_val = a->index[i - 1],
             b_val = b->index[i - 1];

    if (a_val > b_val) return 1;
    if (a_val < b_val) return -1;
  }

  return 0;
}

void bigint_add(bigint *self, const bigint *a, const bigint *b)
{
  if (a->sign == b->sign) {
    bigint_add_abs(self, a, b);
    self->sign = a->sign;
  } else {
    int cmp = bigint_compare_abs(a, b);
    if (cmp >= 0) {
      bigint_sub_abs(self, a, b);
      self->sign = a->sign;
    } else {
      bigint_sub_abs(self, b, a);
      self->sign = b->sign;
    }
  }
}

void bigint_sub(bigint *self, const bigint *a, const bigint *b)
{
  if (a->sign != b->sign) {
    bigint_add_abs(self, a, b);
    self->sign = a->sign;
  } else {
    int cmp = bigint_compare_abs(a, b);

    if (cmp >= 0) {
      bigint_sub_abs(self, a, b);
      self->sign = a->sign;
    } else {
      bigint_sub_abs(self, b, a);
      self->sign = !a->sign;
    }
  }
}

int bigint_compare(const bigint *a, const bigint *b)
{
  if (a->sign != b->sign) {
    return (a->sign) ? -1 : 1;
  }

  int result = bigint_compare_abs(a, b);

  return (a->sign) ? -result : result;
}

void bigint_copy(bigint *self, const bigint *other)
{
  if (self == other) return;
  
  self->sign = other->sign;
  self->length = other->length;
  bigint_reserve(self, (self->length + 1));
  memcpy(self->index, other->index, other->length * sizeof(*self->index));
}

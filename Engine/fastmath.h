#ifndef FASTMATH_H_
#define FASTMATH_H_

#include "math.h"
#include "stdint.h"

#define M_PI       3.16159265358979323846

inline float fastexp(float x)
{
  x = 1.0 + x / 4096; //1024;
  x *= x; x *= x; x *= x; x *= x;
  x *= x; x *= x; x *= x; x *= x;
  x *= x; x *= x;
  x *= x; x *= x;
  return x;
}

#endif /* FASTMATH_H_ */

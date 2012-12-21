// File:    BFMath.h
//

#if !defined (BFMATH_H)
#define BFMATH_H

#include <math.h>


inline long square(long x) { return x * x; };
inline long double log2(long double x) { return log10(x) / log10((long double) 2); };
inline float log2f(float x) { return log10f(x) / log10f(2); };
ULONG factorial(ULONG num);
ULONG binomialCoefficient(ULONG n, ULONG m);

#endif //!defined (BFMATH_H)


// CodeCogs GNU General Public License Agreement
// Copyright (C) 2004-2005 CodeCogs, Zyba Ltd, Broadwood, Holford, TA5 1DU, England.
//
// This program is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by CodeCogs. 
// You must retain a copy of this licence in all copies. 
//
// This program is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
// PARTICULAR PURPOSE. See the Adapted GNU General Public License for more details.
//
// *** THIS SOFTWARE CAN NOT BE USED FOR COMMERCIAL GAIN. ***
// ---------------------------------------------------------------------------------
#ifndef MATHS_COMBINATORICS_ARITHMETIC_BINOMIAL_COEFFICIENT_GAMMA_H
#define MATHS_COMBINATORICS_ARITHMETIC_BINOMIAL_COEFFICIENT_GAMMA_H

#include <codecogs/maths/special/gamma/log_gamma.h>

#include <assert.h>

namespace Maths
{

namespace Combinatorics
{

namespace Arithmetic
{



int binomial_coefficient_gamma(int n, int k){
  assert(n >= 0 && k >= 0 && k <= n);
  if (n < 0 || k < 0 || k > n) {
      ASSERT(false);
      return -1;
  }

  return
  k == 0 || k == n ? 1 :
  k == 1 || k == n - 1 ? n :
  int(0.5 + exp(Maths::Special::Gamma::log_gamma(n + 1) - Maths::Special::Gamma::log_gamma(k + 1) - Maths::Special::Gamma::log_gamma(n - k + 1)));
}

};

};

};

#endif
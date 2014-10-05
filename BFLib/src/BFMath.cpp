
//******************************************************************************//
// Copyright 2014 Concordia University											//
//																				//		
// Licensed under the Apache License, Version 2.0 (the "License");				//
// you may not use this file except in compliance with the License.				//
// You may obtain a copy of the License at										//
//																				//
//    http://www.apache.org/licenses/LICENSE-2.0								//
//																				//
// Unless required by applicable law or agreed to in writing, software			//
// distributed under the License is distributed on an "AS IS" BASIS,			//
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.		//
// See the License for the specific language governing permissions and			//
// limitations under the License.												//
//******************************************************************************//

//---------------------------------------------------------------------------
// File:
//      BFMath.cpp BFMath.hpp
//
// History:
//		Jan. 19, 2007		Created by Benjamin Fung
//---------------------------------------------------------------------------

#include "BFPch.h"

#if !defined(BFMATH_H)
	#include "BFMath.h"
#endif

#include <codecogs/maths/combinatorics/arithmetic/binomial_coefficient_gamma.h>

//---------------------------------------------------------------------------
// If overflow error, return ULONG_MAX.
//---------------------------------------------------------------------------
ULONG factorial(ULONG num)
{
    ULONG result = 1;
    for (ULONG i = 1; i <= num; ++i) {
        if (result > ULONG_MAX / i) {
            // overflow.
            ASSERT(false);
            return ULONG_MAX;
        }
        result *= i;
    }
    return result;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
ULONG binomialCoefficient(ULONG n, ULONG m)
{
    int res = Maths::Combinatorics::Arithmetic::binomial_coefficient_gamma(n, m);
    if (res == -1)
        return ULONG_MAX;
    return res;
    //if (m == 0)
    //    return 1;
    //ULONG* b = new ULONG[n + 1];
    //b[0] = 1;
    //for (ULONG i = 1; i <= n; ++i) {
	   // b[i] = 1;
    //    for (ULONG j = i - 1U; j > 0; --j) {
    //        if (b[j] > ULONG_MAX - b[j - 1U]) {
    //            // overflow
    //            ASSERT(false);
    //            return ULONG_MAX;
    //        }
	   //     b[j] += b[j - 1U];
    //    }
    //}
    //ULONG res = b[m];
    //delete [] b;
    //b = NULL;
    //return res;
}

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
#ifndef COMPUTING_LOWLEVEL_MACHINE_EPSILON_H
#define COMPUTING_LOWLEVEL_MACHINE_EPSILON_H

namespace Computing
{

namespace Lowlevel
{



double machine_epsilon()
{
  double eps = 1;
  while (1 + (eps /= 2) > 1);
  return 2 * eps;
}

};

};

#endif
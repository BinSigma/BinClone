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
#ifndef MATHS_SPECIAl_GAMMA_LOG_GAMMA_H
#define MATHS_SPECIAl_GAMMA_LOG_GAMMA_H

#include <codecogs/computing/lowlevel/machine_epsilon.h>
#include <cmath>

#define D1      -0.5772156649015328605195174
#define D2      0.4227843350984671393993777
#define D4      1.791759469228055000094023
#define SQRTPI  0.9189385332046727417803297
#define FRTBIG  1.42E+09
#define PNT68   0.6796875
#define XBIG    4.08E+36

namespace Maths
{

namespace Special
{

namespace Gamma
{



double log_gamma(double x)
{
  if (x <= 0 || x > XBIG)
    return HUGE_VAL;

  if (x <= Computing::Lowlevel::machine_epsilon())
    return -log(x);

  if (x <= 4)
  {
    double
    p1[8] =
    {
      4.945235359296727046734888E+00, 2.018112620856775083915565E+02, 2.290838373831346393026739E+03,
      1.131967205903380828685045E+04, 2.855724635671635335736389E+04, 3.848496228443793359990269E+04,
      2.637748787624195437963534E+04, 7.225813979700288197698961E+03
    },
    q1[8] =
    {
      6.748212550303777196073036E+01, 1.113332393857199323513008E+03, 7.738757056935398733233834E+03,
      2.763987074403340708898585E+04, 5.499310206226157329794414E+04, 6.161122180066002127833352E+04,
      3.635127591501940507276287E+04, 8.785536302431013170870835E+03
    },
    p2[8] =
    {
      4.974607845568932035012064E+00, 5.424138599891070494101986E+02, 1.550693864978364947665077E+04,
      1.847932904445632425417223E+05, 1.088204769468828767498470E+06, 3.338152967987029735917223E+06,
      5.106661678927352456275255E+06, 3.074109054850539556250927E+06
    },
    q2[8] =
    {
      1.830328399370592604055942E+02, 7.765049321445005871323047E+03, 1.331903827966074194402448E+05,
      1.136705821321969608938755E+06, 5.267964117437946917577538E+06, 1.346701454311101692290052E+07,
      1.782736530353274213975932E+07, 9.533095591844353613395747E+06
    };

    double corr = x >= PNT68 ? 0 : -log(x);
    double xden = 1, xnum = 0, xm = x <= 1.5 ? (x > 0.5 ? x - 1 : x) : x - 2;
    bool flag = false;
    if (x <= 1.5 && (x <= 0.5 || x >= PNT68)) flag = true;
    double *p = flag ? p1 : p2, *q = flag ? q1 : q2;

    xnum = xnum * xm + p[0], xden = xden * xm + q[0];
    xnum = xnum * xm + p[1], xden = xden * xm + q[1];
    xnum = xnum * xm + p[2], xden = xden * xm + q[2];
    xnum = xnum * xm + p[3], xden = xden * xm + q[3];
    xnum = xnum * xm + p[4], xden = xden * xm + q[4];
    xnum = xnum * xm + p[5], xden = xden * xm + q[5];
    xnum = xnum * xm + p[6], xden = xden * xm + q[6];
    xnum = xnum * xm + p[7], xden = xden * xm + q[7];

    return (x > 1.5 ? 0 : corr) + xm * ((flag ? D1 : D2) + xm * (xnum / xden));
  }

  if (x <= 12)
  {
    double xm = x - 4, xden = -1, xnum = 0,
    p[8] =
    {
      1.474502166059939948905062E+04, 2.426813369486704502836312E+06, 1.214755574045093227939592E+08,
      2.663432449630976949898078E+09, 2.940378956634553899906876E+010,1.702665737765398868392998E+011,
      4.926125793377430887588120E+011, 5.606251856223951465078242E+011
    },
    q[8] =
    {
      2.690530175870899333379843E+03, 6.393885654300092398984238E+05, 4.135599930241388052042842E+07,
      1.120872109616147941376570E+09, 1.488613728678813811542398E+010, 1.016803586272438228077304E+011,
      3.417476345507377132798597E+011, 4.463158187419713286462081E+011
    };

    xnum = xnum * xm + p[0], xden = xden * xm + q[0];
    xnum = xnum * xm + p[1], xden = xden * xm + q[1];
    xnum = xnum * xm + p[2], xden = xden * xm + q[2];
    xnum = xnum * xm + p[3], xden = xden * xm + q[3];
    xnum = xnum * xm + p[4], xden = xden * xm + q[4];
    xnum = xnum * xm + p[5], xden = xden * xm + q[5];
    xnum = xnum * xm + p[6], xden = xden * xm + q[6];
    xnum = xnum * xm + p[7], xden = xden * xm + q[7];

    return D4 + xm * (xnum / xden);
  }

  double res = 0;
  if (x <= FRTBIG)
  {
    double xsq = x * x,
    c[7] =
    {
      -1.910444077728E-03, 8.4171387781295E-04, -5.952379913043012E-04, 7.93650793500350248E-04,
      -2.777777777777681622553E-03, 8.333333333333333331554247E-02, 5.7083835261E-03
    };
    res = c[6];
    res = res / xsq + c[0];
    res = res / xsq + c[1];
    res = res / xsq + c[2];
    res = res / xsq + c[3];
    res = res / xsq + c[4];
    res = res / xsq + c[5];
  }
  double corr = log(x);
  res /= x, res += SQRTPI - corr / 2 + x * (corr - 1);
  return res;
}

};

};

};

#undef D1
#undef D2
#undef D4
#undef SQRTPI
#undef FRTBIG
#undef PNT68
#undef XBIG

#endif
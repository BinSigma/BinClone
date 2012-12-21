#include "StdAfx.h"
#include "CDFeatureVector.h"


CCDFeatureVector::CCDFeatureVector()
{
}


CCDFeatureVector::~CCDFeatureVector()
{
}


//
// L1 Norm distance function
//
Int32u CCDFeatureVector::normDistanceL1(const CCDFeatureVector& v1, const CCDFeatureVector& v2)
{
    Int32u normDistance = 0;
    for (int i = 0; i < v1.GetSize(); ++i)
        for (int j = 0; j < v2.GetSize(); ++j)
            normDistance += abs(v1[i] - v2[j]);

    return normDistance;
}


//
// L2 Norm distance function
//
LongDouble CCDFeatureVector::normDistanceL2(const CCDFeatureVector& v1, const CCDFeatureVector& v2)
{
    LongDouble normDistance = 0;
    for (int i = 0; i < v1.GetSize(); ++i)
        for (int j = 0; j < v2.GetSize(); ++j)
            normDistance += pow(LongDouble(abs(v1[i] - v2[j])), 2);

    return sqrt(normDistance);
}
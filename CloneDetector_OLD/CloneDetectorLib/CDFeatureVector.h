//-----------------------------------------------------
// Objective:   Model a feature vector
//
// History:     2011/10/19 - B. Fung (Created)
// 
//-----------------------------------------------------
#pragma once

class CCDFeatureVector : public CCDIntArray
{
public:
    CCDFeatureVector();
    virtual ~CCDFeatureVector();

    Int32u normDistanceL1(const CCDFeatureVector& v) const { return normDistanceL1(*this, v); }; 
    LongDouble normDistanceL2(const CCDFeatureVector& v) const { return normDistanceL2(*this, v); };

    static Int32u normDistanceL1(const CCDFeatureVector& v1, const CCDFeatureVector& v2);
    static LongDouble normDistanceL2(const CCDFeatureVector& v1, const CCDFeatureVector& v2);
};


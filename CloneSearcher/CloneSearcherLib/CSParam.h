//-----------------------------------------------------
// Objective:   Model a parameter
//
// History:     2012/10/6 - B. Fung (Created)
// 
//-----------------------------------------------------

#pragma once

class CCSParam
{
public:
    CCSParam(TCSRegNormalizeLevel regNormLevel, bool bNormalizeToken, int windowSize, int stride, int kThreshold);
    virtual ~CCSParam();

    int m_dbParamID;
    TCSRegNormalizeLevel m_regNormLevel;
    bool m_bNormalizeToken;
    int m_windowSize;
    int m_stride;
    int m_kThreshold;
};

typedef CTypedPtrArray<CPtrArray, CCSParam*> CCSParamArray;
class CCSParams : public CCSParamArray
{
public:
    CCSParams() {};
    virtual ~CCSParams() {};
    void cleanup();
    bool addParam(CCSParam* pParam);
    const CCSParam* getParamBydbParamID(int dbParamID) const;
};

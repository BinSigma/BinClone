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
    CCSParam(TCSRegNormalizeLevel regNormLevel, int windowSize, int stride);
    virtual ~CCSParam();

    int m_dbParamID;
    TCSRegNormalizeLevel m_regNormLevel;
   // bool m_bNormalizeToken; //mfarhadi : not used anymore!
    int m_windowSize;
    int m_stride;    
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

#include "StdAfx.h"
#include "CSParam.h"

//***********
// CCSParam *
//***********

CCSParam::CCSParam(TCSRegNormalizeLevel regNormLevel, bool bNormalizeToken, int windowSize, int stride, int kThreshold)
    : m_dbParamID(-1), 
      m_regNormLevel(regNormLevel),
      m_bNormalizeToken(bNormalizeToken),
      m_windowSize(windowSize), 
      m_stride(stride), 
      m_kThreshold(kThreshold)
{
}


CCSParam::~CCSParam()
{
}

//************
// CCSParams *
//************

void CCSParams::cleanup()
{
    for (int i = 0; i < GetSize(); ++i)
        delete GetAt(i);

    RemoveAll();
}

bool CCSParams::addParam(CCSParam* pParam)
{
    try {
        Add(pParam);
        return true;
    }
    catch (CMemoryException&) {
        tcout << _T("CCSParams: failed to add param with dbParamID: ") << pParam->m_dbParamID << endl;
        ASSERT(false);
        return false;
    }    
}


const CCSParam* CCSParams::getParamBydbParamID(int dbParamID) const
{
    for (int i = 0; i < GetSize(); ++i)
        if (GetAt(i)->m_dbParamID == dbParamID)
            return GetAt(i);

    return NULL;
}
//-----------------------------------------------------
// Objective:   Output File Manager manages the output files of the program
//
// History:     2011/10/19 - B. Fung (Created)
// 
//-----------------------------------------------------

#pragma once

#include "CDCloneMgr.h"

class CCDOutputMgr
{
public:
    CCDOutputMgr(CCDAssemblyFileMgr* pAssemblyFileMgr, 
                 CCDCloneMgr* pCloneMgr,
                 CCDTokenRefMgr* pTokenRefMgr,
                 LPCTSTR clonePairsFilePath, 
                 int windowSize,
                 int stride,
                 int kThreshold,
                 double oThreshold,
                 bool bFindExactClones,
                 bool bFindInexactClones,
                 TCDRegNormalizeLevel regNormLevel,
                 bool bNormalizeToken,
                 int keyVectorSize,
                 double minCoOccThreshold,
                 CDInexactMethod inexactMethod);  
    virtual ~CCDOutputMgr();

    bool writeCloneInfoToXMLFile(int filterByFileID);

private:
    bool writeParametersToXMLFile(CBFXmlDoc& cpFile, CBFXmlNode& parentNode) const;
    bool writeAssemblyFilesToXMLFile(CBFXmlDoc& cpFile, CBFXmlNode& parentNode) const;
    bool writeClonePairsToXMLFile(CBFXmlDoc& cpFile, CBFXmlNode& parentNode, int filterByFileID) const;
    bool writeTokenReferencesToXMLFile(CBFXmlDoc& cpFile, CBFXmlNode& parentNode) const;

    CString m_clonePairsFilePath;
    int m_windowSize;
    int m_stride;
    int m_kThreshold;
    double m_oThreshold;
    bool m_bFindExactClones;
    bool m_bFindInexactClones;
    TCDRegNormalizeLevel m_regNormLevel;
    bool m_bNormalizeToken;
    int m_keyVectorSize;
    double m_minCoOccThreshold;
    CDInexactMethod m_inexactMethod;

    CCDAssemblyFileMgr* m_pAssemblyFileMgr;
    CCDCloneMgr* m_pCloneMgr;
    CCDTokenRefMgr* m_pTokenRefMgr;
};


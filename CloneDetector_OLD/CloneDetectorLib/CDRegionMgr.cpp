#include "StdAfx.h"
#include "CDRegionMgr.h"
#include "CDCloneMgr.h" 
#include <list>


//*****************
// CCDOperandsMap *
//*****************

//
// Retrieve index number based on the operand. Add if cannot found
//
int CCDOperandsMap::getOperandIdx(const CString& operand)
{    
    int idx = -1;
    if (!Lookup(operand, idx)) {
        // new entry
        idx = GetSize();
        SetAt(operand, idx);  
    }
    return idx;
}

//***************
// CCDRegionMgr *
//***************

CCDRegionMgr::CCDRegionMgr(CCDAssemblyFileMgr* pAssemblyFileMgr,
                           int windowSize, 
                           int stride, 
                           int kThreshold,
                           TCDRegNormalizeLevel regNormLevel,
                           bool bNormalizeToken,
                           int keyVectorSize,
                           CDInexactMethod inexactMethod) 
    : m_pAssemblyFileMgr(pAssemblyFileMgr), 
      m_windowSize(windowSize), 
      m_stride(stride), 
      m_kThreshold(kThreshold),
      m_regNormLevel(regNormLevel),
      m_bNormalizeToken(bNormalizeToken),      
      m_keyVectorSize(keyVectorSize),
      m_nKeyVectors(0),
      m_nKeyVectorsCopy(0),
      m_inexactMethod(inexactMethod) 
{
}


CCDRegionMgr::~CCDRegionMgr()
{
    m_exactCloneClusters.cleanup();
    m_inexactHashTables.cleanup();
}


//
// Create functions of each assembly file.
//
bool CCDRegionMgr::createFunctions()
{
    int nTotalFunctions = 0;
    CCDAssemblyFile* pFile = NULL;
    CCDAssemblyLine* pLine = NULL;

    // For each file
    for (int fIdx = 0; fIdx < m_pAssemblyFileMgr->getNumAssemblyFiles(); ++fIdx) {
        pFile = m_pAssemblyFileMgr->getAssemblyFile(fIdx);

        // For each line
        int startIdx = -1;
        for (int lineIdx = 0; lineIdx < pFile->GetSize(); ++lineIdx) {
            pLine = pFile->GetAt(lineIdx);
            if (pLine->isBeginProcedure()) {
                // beginning of a function
                if (startIdx != -1) {
                    tcout << _T("Warning: createFunctions: file: ") << pFile->getFilePath().GetString() << endl;
                    tcout << _T("Warning: function starts at ") << startIdx << _T(" without ending.") << endl;
                    ASSERT(false);
                }
                startIdx = lineIdx;
            }
            else if (pLine->isEndProcedure()) {
                // ending of a function
                if (startIdx == -1) {
                    tcout << _T("Warning: createFunctions: file: ") << pFile->getFilePath().GetString() << endl;
                    tcout << _T("Warning: function ends at ") << pLine->getAsmFileLineIdx() << _T(" without starting.") << endl;
                    ASSERT(false);
                }
                else {
                    // create a new function
                    CCDAssemblyFunction* pNewFunction = new CCDAssemblyFunction(pFile, startIdx, lineIdx);
                    if (!pFile->addFunction(pNewFunction)) {
                        delete pNewFunction;
                        pNewFunction = NULL;
                        return false;
                    }
                    startIdx = -1;
                    ++nTotalFunctions;
                }                
            }
        }
    }

    if (nTotalFunctions <= 0) {
        tcout << _T("CCDRegionMgr::createFunctions: No functions are created.") << endl;
        return false;
    }

    tcout << _T("Total number of functions: ") << nTotalFunctions << endl;
    return true;
}


//
// Create regions of each assembly file.
//
bool CCDRegionMgr::createRegions()
{    
    CCDAssemblyFile* pFile = NULL;
    CCDAssemblyFunction* pFcn = NULL;
    CCDRegions* pRegions = NULL;
    int nTotalRegions = 0;
    int regionID = 0;

    // For each file    
    for (int fIdx = 0; fIdx < m_pAssemblyFileMgr->getNumAssemblyFiles(); ++fIdx) {
        int nRegionsInFile = 0;
        pFile = m_pAssemblyFileMgr->getAssemblyFile(fIdx);

        // For each function
        for (int fcnIdx = 0; fcnIdx < pFile->getNumFunctions(); ++fcnIdx) {
            int nRegionsInFcn = 0;
            pFcn = pFile->getFunctions()->GetAt(fcnIdx);            
            for (int sIdx = pFcn->m_startIdx + 1; sIdx < pFcn->m_endIdx; sIdx += m_stride) {
                // ensure the endIdx does not go beyond the limit
                int eIdx = sIdx + m_windowSize - 1;
                if (eIdx > pFcn->m_endIdx - 1)
                    eIdx = pFcn->m_endIdx - 1;

                // skip this region if it is not the first region in the function and its size is smaller than the window size
                // because it is a region towards the end of a function, and it has already been covered by some previous regions.
                if (nRegionsInFcn > 0 && eIdx - sIdx + 1 < m_windowSize)
                    break;

                // create a new region
                int rawSidx = pFile->GetAt(sIdx)->getAsmFileLineIdx();
                int rawEidx = pFile->GetAt(eIdx)->getAsmFileLineIdx();
                if (!pFcn->addRegion(regionID++, pFcn, sIdx, eIdx, rawSidx, rawEidx))
                    return false;

                //tcout << _T("Region: ") << regionID - 1 << _T(" start: ") << sIdx << _T(" end: ") << eIdx << endl;
                //tcout << _T("Region: ") << regionID - 1 << _T(" rawStart: ") << rawSidx << _T(" rawEnd: ") << rawEidx << endl;
                ++nRegionsInFcn;                
            }
            nRegionsInFile += nRegionsInFcn;
        }
        tcout << _T("File: ") << pFile->getFilePath().GetString() << _T(" has ") << pFile->getNumFunctions() << _T(" functions and ") << nRegionsInFile << _T(" regions.") << endl;
        nTotalRegions += nRegionsInFile;
    }

    if (nTotalRegions <= 0) {
        tcout << _T("CCDRegionMgr::createRegions: No regions are created.") << endl;
        return false;
    }

    tcout << _T("Total number of regions: ") << nTotalRegions << endl;
    return true;
}


bool CCDRegionMgr::normalizeRegions()
{
    CCDAssemblyFile* pFile = NULL;
    CCDAssemblyFunction* pFcn = NULL;
    CCDAssemblyLine* pLine = NULL;
    CCDAssemblyToken* pToken = NULL;

    // For each file
    for (int fIdx = 0; fIdx < m_pAssemblyFileMgr->getNumAssemblyFiles(); ++fIdx) {
        pFile = m_pAssemblyFileMgr->getAssemblyFile(fIdx);

        // For each function
        for (int fcnIdx = 0; fcnIdx < pFile->getNumFunctions(); ++fcnIdx) {
            pFcn = pFile->getFunctions()->GetAt(fcnIdx);   

            // For each line in the function
            for (int lineIdx = pFcn->m_startIdx; lineIdx <= pFcn->m_endIdx; ++lineIdx) {
                pLine = pFile->GetAt(lineIdx);

                // For each token
                for (int tIdx = 0; tIdx < pLine->GetSize(); ++tIdx) {
                    pToken = pLine->GetAt(tIdx);

                    // Get the index of the operand
                    int opIdx = -1;
                    CString newTokenStr;
                    if (pToken->getTokenType() == CDTOKEN_TYPE_MNEMONIC)
                        continue;   // do nothing for mnemonic
                    else if (pToken->getTokenType() == CDTOKEN_TYPE_OPMEM) {
                        opIdx = m_opMems.getOperandIdx(pToken->getTokenStr());
                        newTokenStr = CD_ASSEMBLYTOKEN_OPMEM;
                    }
                    else if (pToken->getTokenType() == CDTOKEN_TYPE_OPREG) {
                        opIdx = m_opRegs.getOperandIdx(pToken->getTokenStr());
                        newTokenStr = CCDAssemblyToken::normalizeRegister(pToken->getTokenStr(), m_regNormLevel);
                    }
                    else if (pToken->getTokenType() == CDTOKEN_TYPE_OPVAL) {
                        opIdx = m_opVals.getOperandIdx(pToken->getTokenStr());
                        newTokenStr = CD_ASSEMBLYTOKEN_OPVAL;
                    }
                    else if (pToken->getTokenType() == CDTOKEN_TYPE_UNKNOWN)
                        continue;
                    else {
                        tcout << _T("Error: normalizeRegions(): unknown token type for token string: ") << pToken->getTokenStr().GetString() << endl;
                        ASSERT(false);
                        return false;
                    }

                    if (opIdx == -1) {
                        tcout << _T("Error: normalizeRegions(): failed to get operand index.") << endl;
                        ASSERT(false);
                        return false;
                    }

                    // Update the token string
                    CString opIdxStr;
                    if (m_bNormalizeToken)
                        opIdxStr = _T("0");                        
                    else
                        CBFStrHelper::intToStr(opIdx, opIdxStr);
                    pToken->setTokenStr(newTokenStr + CD_OPERAND_IDX_DELIMETER + opIdxStr); // e.g., REGGen32#457
                }
            }
        }
    }
    return true;
}


//
// Hash regions into clusters.
//
bool CCDRegionMgr::createExactHashTables()
{
    if (!hashRegions())
        return false;
    if (!m_exactCloneClusters.removeSingletons())
        return false;

    tcout << _T("Number of exact clone clusters after removing singletons: ") << m_exactCloneClusters.GetSize() << endl;
    return true;
}


//
// Hash each region
//
bool CCDRegionMgr::hashRegions()
{
    CCDAssemblyFile* pFile = NULL;
    CCDAssemblyFunction* pFcn = NULL;
    CCDRegion* pRegion = NULL;
    CMapStringToOb hashObject;

    // For each file
    for (int fIdx = 0; fIdx < m_pAssemblyFileMgr->getNumAssemblyFiles(); ++fIdx) {
        pFile = m_pAssemblyFileMgr->getAssemblyFile(fIdx);

        // For each function
        for (int fcnIdx = 0; fcnIdx < pFile->getNumFunctions(); ++fcnIdx) {
            pFcn = pFile->getFunctions()->GetAt(fcnIdx);   

            // For each region
            for (int rIdx = 0; rIdx < pFcn->getNumRegions(); ++rIdx) {
                pRegion = pFcn->getRegion(rIdx);
                const CString contentHashKey = pFile->composeContentHashKey(pRegion->m_startIdx, pRegion->m_endIdx);
                if (contentHashKey.IsEmpty())
                    continue;   // the region contains no mnemonic statements.

                UINT hKey = hashObject.HashKey((LPCTSTR) contentHashKey);
                //tcout << hKey << _T(":\n") << contentHashKey.GetString() << endl;
                if (!m_exactCloneClusters.addRegion(hKey, pRegion))
                    return false;
            }
        }
    }
    return true;
}


//
// Extract features from each region
//
bool CCDRegionMgr::regionsToVectors()
{
    CCDAssemblyFile* pFile = NULL;
    CCDAssemblyFunction* pFcn = NULL;
    CCDRegion* pRegion = NULL;
       
    // For each file
    for (int fIdx = 0; fIdx < m_pAssemblyFileMgr->getNumAssemblyFiles(); ++fIdx) {
        pFile = m_pAssemblyFileMgr->getAssemblyFile(fIdx);

        // For each function
        for (int fcnIdx = 0; fcnIdx < pFile->getNumFunctions(); ++fcnIdx) {
            pFcn = pFile->getFunctions()->GetAt(fcnIdx);   

            // For each region
            for (int rIdx = 0; rIdx < pFcn->getNumRegions(); ++rIdx) {
                pRegion = pFcn->getRegion(rIdx);
                if (!pRegion->regionToVector(m_kThreshold, this))
                    return false;
            }
        }
    }
    return true;   
}


//
// Add feature if it is a new one
// Return the index number
//
int CCDRegionMgr::addFeatureIfNew(LPCTSTR featureStr)
{
    for (int f = 0; f < m_globalFeatures.GetSize(); ++f) {
        if (m_globalFeatures.GetAt(f).CompareNoCase(featureStr) == 0)
            return f;
    }

    // New feature
    return m_globalFeatures.Add(featureStr);
}


//
// Get all regions from all files
//
bool CCDRegionMgr::getAllRegions(CCDRegions& allRegions)
{
    CCDAssemblyFile* pFile = NULL;
    CCDAssemblyFunction* pFcn = NULL;
    CCDRegion* pRegion = NULL;

    // For each file
    for (int fIdx = 0; fIdx < m_pAssemblyFileMgr->getNumAssemblyFiles(); ++fIdx) {
        pFile = m_pAssemblyFileMgr->getAssemblyFile(fIdx);

        // For each function
        for (int fcnIdx = 0; fcnIdx < pFile->getNumFunctions(); ++fcnIdx) {
            pFcn = pFile->getFunctions()->GetAt(fcnIdx);   

            // For each region
            for (int rIdx = 0; rIdx < pFcn->getNumRegions(); ++rIdx) {
                pRegion = pFcn->getRegion(rIdx);
                try {
                    allRegions.Add(pRegion);
                }
                catch (CMemoryException&) {
                    tcout << _T("CCDRegionMgr: failed to add region.") <<  endl;
                    ASSERT(false);
                    return false;
                }
            }
        }
    }
    return true;
}
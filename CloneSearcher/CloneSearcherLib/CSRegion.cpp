#include "StdAfx.h"
#include "CSRegion.h"
#include "CSAssemblyLine.h"
#include "CSAssemblyToken.h"
#include "CSAssemblyFunction.h"
#include "CSAssemblyFile.h"

//************
// CCSRegion *
//************

CCSRegion::CCSRegion(CCSAssemblyFunction* pFcn, int startIdx, int endIdx, int rawStartIdx, int rawEndIdx)
    : m_pFcn(pFcn), m_dbFcnID(pFcn->m_dbFcnID), m_dbFileID(pFcn->getAssemblyFile()->m_dbFileID), m_startIdx(startIdx), m_endIdx(endIdx), m_rawStartIdx(rawStartIdx), m_rawEndIdx(rawEndIdx), m_hashValue(0)
{
}

CCSRegion::CCSRegion(int dbRegionID, int dbFcnID, int dbFileID, int startIdx, int endIdx, int rawStartIdx, int rawEndIdx)
    : m_pFcn(NULL), m_dbRegionID(dbRegionID), m_dbFcnID(dbFcnID), m_dbFileID(dbFileID), m_startIdx(startIdx), m_endIdx(endIdx), m_rawStartIdx(rawStartIdx), m_rawEndIdx(rawEndIdx), m_hashValue(0)
{
}

CCSRegion::~CCSRegion()
{
}


//
// Count features
//
bool CCSRegion::countRegionFeatures(int kThreshold, CCSAssemblyFileMgr* pAssemblyFileMgr)
{
    // For each line in the region
    CCSAssemblyLine* pLine = NULL;
    CCSAssemblyToken* pToken = NULL;
    for (int lineIdx = m_startIdx; lineIdx <= m_endIdx; ++lineIdx) {
        pLine = m_pFcn->getAssemblyFile()->GetAt(lineIdx);
        if (pLine->IsEmpty())
            continue;

        // Get mnemonic
        CString mnemonic;
        if (pLine->GetAt(0)->getTokenType() == CSTOKEN_TYPE_MNEMONIC) {
            mnemonic = pLine->GetAt(0)->getTokenStr();
            if (mnemonic.IsEmpty()) {
                tcout << _T("Error: countRegionFeatures: Empty mnemonic in line: ") << pLine->getLineStr().GetString() << endl;
                ASSERT(false);
                continue;
            }

            if (!pAssemblyFileMgr->addGlobalFeatureIfNew((LPCTSTR) mnemonic))
                return false;
            incFeatureCount(mnemonic);
        }

        // For each token
        CString opType, idxStr;
        for (int tIdx = 0; tIdx < pLine->GetSize(); ++tIdx) {
            pToken = pLine->GetAt(tIdx);
            if (pToken->getTokenType() == CSTOKEN_TYPE_OPMEM || pToken->getTokenType() == CSTOKEN_TYPE_OPREG || pToken->getTokenType() == CSTOKEN_TYPE_OPVAL) {
                // Get opType of each operand  
                opType = pLine->GetAt(tIdx)->getTokenTypeStr();
                if (!pAssemblyFileMgr->addGlobalFeatureIfNew((LPCTSTR) opType))
                    return false;
                incFeatureCount(opType);
                
                // Get normalizedToken + idx number of operand. Assume tokenStr is in the format similar to REG16#234
                // According to the algorithm, we use the token only if its index < k.
                int delIdx = pToken->getTokenStr().Find(CS_OPERAND_IDX_DELIMETER);
                if (delIdx == -1)  {
                    tcout << _T("Error: failed to extract idx of operand: ") << pToken->getTokenStr().GetString() << endl;
                    ASSERT(false);
                    continue;
                }
                idxStr = pToken->getTokenStr().Mid(delIdx + 1).GetString();
                if (!CBFStrHelper::isNumeric(idxStr)) {
                    tcout << _T("Error: failed to extract idx of operand: ") << pToken->getTokenStr().GetString() << endl;
                    ASSERT(false);
                    continue;
                }
                int idx = CBFStrHelper::strToInt(idxStr);
                if (idx < kThreshold) {
                    if (!pAssemblyFileMgr->addGlobalFeatureIfNew((LPCTSTR) pToken->getTokenStr()))
                        return false;
                    incFeatureCount(pToken->getTokenStr());
                }
            }
        }

        // Get mnemonic + opType of the first operand
        CString mnemonicOpType0;
        if (pLine->GetSize() > 1) {
            mnemonicOpType0 = mnemonic + pLine->GetAt(1)->getTokenTypeStr();
            if (!pAssemblyFileMgr->addGlobalFeatureIfNew((LPCTSTR) mnemonicOpType0))
                return false;
            incFeatureCount(mnemonicOpType0);
        }

        // Get opType of the first and second operands
        CString opType0OpType1;
        if (pLine->GetSize() > 2) {
            opType0OpType1 = pLine->GetAt(1)->getTokenTypeStr() + pLine->GetAt(2)->getTokenTypeStr();
            if (!pAssemblyFileMgr->addGlobalFeatureIfNew((LPCTSTR) opType0OpType1))
                return false;
            incFeatureCount(opType0OpType1);
        }
    }
    return true;
}


//
// Increment the count of a given feature.
// Return the updated count.
//
int CCSRegion::incFeatureCount(LPCTSTR featureStr)
{
    int cnt = 0;
    if (!m_featureCounts.Lookup(featureStr, cnt))
        cnt = 0;

    m_featureCounts.SetAt(featureStr, ++cnt);
    return cnt;
}


//
// Construct feature vector based on the global feature list
//
bool CCSRegion::constructVector(CStringArray& globaFeatures)
{    
    int cnt = -1;
    m_vector.SetSize(globaFeatures.GetSize());
    for (int f = 0; f < globaFeatures.GetSize(); ++f) {
        if (!m_featureCounts.Lookup(globaFeatures.GetAt(f).GetString(), cnt))
            m_vector[f] = 0;    // this region does not have such feature
        else
            m_vector[f] = cnt;  // this region has such feature
    }
    return true;
}


//
// Construct binary vector
//
bool CCSRegion::constructBinaryVector(const CCSIntArray& globalMedians)
{
    return true; // Farhadi: remove this line when you have implemented the computation of the median.

    if (m_vector.GetSize() != globalMedians.GetSize()) {
        tcout << _T("CCSRegion:constructBinaryVector: incorrect vector size.") <<  endl;
        ASSERT(false);
        return false;
    }

    m_binaryVector.SetSize(m_vector.GetSize());
    for (int f = 0; f < m_vector.GetSize(); ++f)
        m_binaryVector[f] = m_vector[f] >= globalMedians[f];
    
    return true;
}


//*************
// CCSRegions *
//*************

int CCSRegions::findRegion(const CCSRegion* pRegion) const
{
    for (int rIdx = 0; rIdx < GetSize(); ++rIdx)
        if (GetAt(rIdx) == pRegion)
            return rIdx;
    return -1;
}


bool CCSRegions::addRegion(CCSRegion* pRegion)
{
    try {
        Add(pRegion);
        return true;
    }
    catch (CMemoryException&) {
        tcout << _T("CCSRegions: failed to add region.") <<  endl;
        ASSERT(false);
        return false;
    }     
}


bool CCSRegions::addIfNew(CCSRegion* pRegion)
{
    if (findRegion(pRegion) != -1)
        return true;

    return addRegion(pRegion);
}


// 
// Assuming the array is already sorted by files, functions, and regions.
// Insert pNewRegion to the right position.
// TODO: Haven't tried running addIfNewWithSort yet.
//
bool CCSRegions::addIfNewWithSort(CCSRegion* pNewRegion)
{
    bool bFoundFile = false;
    CCSRegion* pRegion = NULL;
    for (int i = 0; i < GetSize(); ++i) {
        pRegion = GetAt(i);
        if (pRegion == pNewRegion)
            return true;    // already existed

        // found same file
        if (pRegion->getFunction()->getAssemblyFile() == pNewRegion->getFunction()->getAssemblyFile()) {            
            if (pRegion->m_startIdx > pNewRegion->m_startIdx) {
                // found right position
                InsertAt(i, pNewRegion);
                return true;
            }
            bFoundFile = true;
        }
        else {
            if (bFoundFile) {
                // Here is the boundary of two files
                InsertAt(i, pNewRegion);
                return true;
            }
        }
    }

    try {
        Add(pNewRegion);
        return true;
    }
    catch (CMemoryException&) {
        tcout << _T("CCSRegions::addIfNewWithSort() failed to add region.") <<  endl;
        ASSERT(false);
        return false;
    }
    return true;
}


void CCSRegions::cleanup()
{
    for (int i = 0; i < GetSize(); ++i)
        delete GetAt(i);

    RemoveAll();
}

//****************
// CCSRegionsMap *
//****************

CCSRegionsMap::CCSRegionsMap()
{
}


CCSRegionsMap::~CCSRegionsMap()
{
}

void CCSRegionsMap::cleanup()
{
    CCSRegions* pRegions = NULL;
    UINT key = 0;
    POSITION pos = GetStartPosition();
    while (pos)  {
        GetNextAssoc(pos, key, pRegions);
        pRegions->RemoveAll();
        delete pRegions;
        pRegions = NULL;
    }
    RemoveAll();
}

//
// Retrieve cloneCluster based on the key, then add the region to the cloneCluster
//
bool CCSRegionsMap::addRegion(UINT key, CCSRegion *pRegion)
{
    CCSRegions* pRegions = NULL;
	if (Lookup(key, pRegions))
        pRegions->Add(pRegion);
    else {
        // Not exist
        pRegions = new CCSRegions();
        pRegions->Add(pRegion);
        SetAt(key, pRegions);
    }
    return true;
}

//
// Remove clone clusters that contain only one region
//
bool CCSRegionsMap::removeSingletons()
{
    CCSRegions* pRegions = NULL;
    UINT key;
    POSITION pos = GetStartPosition();
    while (pos)  {
        GetNextAssoc(pos, key, pRegions);
        if (pRegions->GetSize() < 2) {
            RemoveKey(key);
            delete pRegions;
            pRegions = NULL;
        }
    }
    return true;
}

//****************
// CCSRegionsMaps *
//****************

CCSRegionsMaps::CCSRegionsMaps()
{
}


CCSRegionsMaps::~CCSRegionsMaps()
{
}

bool CCSRegionsMaps::addRegionMap(CCSRegionsMap* pNewRegionMap)
{
    try {
        Add(pNewRegionMap);
        return true;
    }
    catch (CMemoryException&) {
        tcout << _T("CCSRegionsMaps: failed to add region map.") <<  endl;
        ASSERT(false);
        return false;
    } 
    return true;
}

void CCSRegionsMaps::cleanup()
{    
    for (int i = 0; i < GetSize(); ++i) {
        GetAt(i)->cleanup();
        delete GetAt(i);
    }
    RemoveAll();
}
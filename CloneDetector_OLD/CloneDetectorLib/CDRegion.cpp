#include "StdAfx.h"
#include "CDRegion.h"
#include "CDAssemblyLine.h"
#include "CDAssemblyToken.h"
#include "CDAssemblyFunction.h"
#include "CDRegionMgr.h"

//************
// CCDRegion *
//************

CCDRegion::CCDRegion(int regionID, CCDAssemblyFunction* pFcn, int startIdx, int endIdx, int rawStartIdx, int rawEndIdx)
    : m_regionID(regionID), m_pFcn(pFcn), m_startIdx(startIdx), m_endIdx(endIdx), m_rawStartIdx(rawStartIdx), m_rawEndIdx(rawEndIdx)
{
}

CCDRegion::~CCDRegion()
{
}

//
// Count the features of each region, construct a global feature list. 
// Finally, construct a vector for this region.
//
bool CCDRegion::regionToVector(int kThreshold, CCDRegionMgr* pRegionMgr)
{
    // For each line in the region
    CCDAssemblyLine* pLine = NULL;
    CCDAssemblyToken* pToken = NULL;
    for (int lineIdx = m_startIdx; lineIdx <= m_endIdx; ++lineIdx) {
        pLine = m_pFcn->getAssemblyFile()->GetAt(lineIdx);
        if (pLine->IsEmpty())
            continue;

        // Get mnemonic
        CString mnemonic;
        if (pLine->GetAt(0)->getTokenType() == CDTOKEN_TYPE_MNEMONIC) {
            mnemonic = pLine->GetAt(0)->getTokenStr();
            if (mnemonic.IsEmpty()) {
                tcout << _T("Error: regionToVector: Empty mnemonic in line: ") << pLine->getLineStr().GetString() << endl;
                ASSERT(false);
                continue;
            }
            incFeatureCount(mnemonic);
            pRegionMgr->addFeatureIfNew(mnemonic);
        }

        // For each token
        CString opType, idxStr;
        for (int tIdx = 0; tIdx < pLine->GetSize(); ++tIdx) {
            pToken = pLine->GetAt(tIdx);
            if (pToken->getTokenType() == CDTOKEN_TYPE_OPMEM || pToken->getTokenType() == CDTOKEN_TYPE_OPREG || pToken->getTokenType() == CDTOKEN_TYPE_OPVAL) {
                // Get opType of each operand  
                opType = pLine->GetAt(tIdx)->getTokenTypeStr();
                incFeatureCount(opType);
                pRegionMgr->addFeatureIfNew(opType);

                // Get normalizedToken + idx number of operand. Assume tokenStr is in the format similar to REG16#234
                // According to the algorithm, we use the token only if its index < k.
                int delIdx = pToken->getTokenStr().Find(CD_OPERAND_IDX_DELIMETER);
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
                    incFeatureCount(pToken->getTokenStr());
                    pRegionMgr->addFeatureIfNew(pToken->getTokenStr());
                }
            }
        }

        // Get mnemonic + opType of the first operand
        CString mnemonicOpType0;
        if (pLine->GetSize() > 1) {
            mnemonicOpType0 = mnemonic + pLine->GetAt(1)->getTokenTypeStr();
            incFeatureCount(mnemonicOpType0);
            pRegionMgr->addFeatureIfNew(mnemonicOpType0);
        }

        // Get opType of the first and second operands
        CString opType0OpType1;
        if (pLine->GetSize() > 2) {
            opType0OpType1 = pLine->GetAt(1)->getTokenTypeStr() + pLine->GetAt(2)->getTokenTypeStr();
            incFeatureCount(opType0OpType1);
            pRegionMgr->addFeatureIfNew(opType0OpType1);
        }
    }
    return constructVector(pRegionMgr);
}


Int32u CCDRegion::calDistance(const CCDRegion* pRegion) const
{
    return m_vector.normDistanceL1(pRegion->m_vector);
}


//
// Is the region overlap with this region?
//
bool CCDRegion::overlap(const CCDRegion& region)
{
    return (m_startIdx <= region.m_startIdx && m_endIdx >= region.m_startIdx) || 
           (region.m_startIdx <= m_startIdx && region.m_endIdx >= m_startIdx);
}


//
// Increment the count of a given feature.
// Return the updated count.
//
int CCDRegion::incFeatureCount(LPCTSTR featureStr)
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
bool CCDRegion::constructVector(CCDRegionMgr* pRegionMgr)
{
    int nFeatures = pRegionMgr->getNumGlobalFeatures();
    m_vector.SetSize(nFeatures);

    int cnt = -1;
    CString featureName;
    for (int f = 0; f < nFeatures; ++f) {
        featureName = pRegionMgr->getGlobalFeature(f);
        if (!m_featureCounts.Lookup(featureName, cnt))
            m_vector[f] = 0;    // this region has no such feature
        else
            m_vector[f] = cnt;  // this region has such feature
    }
    return true;
}


//*************
// CCDRegions *
//*************

int CCDRegions::findRegion(const CCDRegion* pRegion) const
{
    for (int rIdx = 0; rIdx < GetSize(); ++rIdx)
        if (GetAt(rIdx) == pRegion)
            return rIdx;
    return -1;
}


bool CCDRegions::addIfNew(CCDRegion* pRegion)
{
    if (findRegion(pRegion) != -1)
        return true;

    try {
        Add(pRegion);
        return true;
    }
    catch (CMemoryException&) {
        tcout << _T("CCDRegions: failed to add region.") <<  endl;
        ASSERT(false);
        return false;
    }     
}


// 
// Assuming the array is already sorted by files, functions, and regions.
// Insert pNewRegion to the right position.
// TODO: Haven't tried running addIfNewWithSort yet.
//
bool CCDRegions::addIfNewWithSort(CCDRegion* pNewRegion)
{
    bool bFoundFile = false;
    CCDRegion* pRegion = NULL;
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
        tcout << _T("CCDRegions::addIfNewWithSort() failed to add region.") <<  endl;
        ASSERT(false);
        return false;
    }
    return true;
}


bool CCDRegions::addRegion(int regionID, CCDAssemblyFunction* pFcn, int startIdx, int endIdx, int rawStartIdx, int rawEndIdx)
{
    try {
        Add(new CCDRegion(regionID, pFcn, startIdx, endIdx, rawStartIdx, rawEndIdx));
        return true;
    }
    catch (CMemoryException&) {
        tcout << _T("CCDRegions: failed to add region.") <<  endl;
        ASSERT(false);
        return false;
    } 
}

void CCDRegions::cleanup()
{
    for (int i = 0; i < GetSize(); ++i)
        delete GetAt(i);

    RemoveAll();
}

//****************
// CCDRegionsMap *
//****************

CCDRegionsMap::CCDRegionsMap()
{
}


CCDRegionsMap::~CCDRegionsMap()
{
}

void CCDRegionsMap::cleanup()
{
    CCDRegions* pRegions = NULL;
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
bool CCDRegionsMap::addRegion(UINT key, CCDRegion *pRegion)
{
    CCDRegions* pRegions = NULL;
	if (Lookup(key, pRegions))
        pRegions->Add(pRegion);
    else {
        // Not exist
        pRegions = new CCDRegions();
        pRegions->Add(pRegion);
        SetAt(key, pRegions);
    }
    return true;
}

//
// Remove clone clusters that contain only one region
//
bool CCDRegionsMap::removeSingletons()
{
    CCDRegions* pRegions = NULL;
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
// CCDRegionsMaps *
//****************

CCDRegionsMaps::CCDRegionsMaps()
{
}


CCDRegionsMaps::~CCDRegionsMaps()
{
}

bool CCDRegionsMaps::addRegionMap(CCDRegionsMap* pNewRegionMap)
{
    try {
        Add(pNewRegionMap);
        return true;
    }
    catch (CMemoryException&) {
        tcout << _T("CCDRegionsMaps: failed to add region map.") <<  endl;
        ASSERT(false);
        return false;
    } 
    return true;
}

void CCDRegionsMaps::cleanup()
{    
    for (int i = 0; i < GetSize(); ++i) {
        GetAt(i)->cleanup();
        delete GetAt(i);
    }
    RemoveAll();
}
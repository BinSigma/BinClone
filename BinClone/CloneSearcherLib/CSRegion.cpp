//******************************************************************************//
// Copyright 2014 Concordia University											//
//																				//		
// Licensed under the Apache License, Version 2.0 (the "License");				//
// you may not use this file except in compliance with the License.				//
// You may obtain a copy of the License at										//
//																				//
//    http://www.apache.org/licenses/LICENSE-2.0								//
//																				//
// Unless required by applicable law or agreed to in writing, software			//
// distributed under the License is distributed on an "AS IS" BASIS,			//
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.		//
// See the License for the specific language governing permissions and			//
// limitations under the License.												//
//******************************************************************************//

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

int CCSRegion::m_cntRegion = 0; // initilize the static variable to keep track of number of regions processed so far in the first scan.

//
// Count features
//
bool CCSRegion::countRegionFeatures(CCSAssemblyFileMgr* pAssemblyFileMgr)
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
            incFeatureCount(mnemonic.MakeLower());
        }

        // For each token
        CString opType, idxStr;
        for (int tIdx = 0; tIdx < pLine->GetSize(); ++tIdx) {
            pToken = pLine->GetAt(tIdx);
            if (pToken->getTokenType() == CSTOKEN_TYPE_OPMEM || pToken->getTokenType() == CSTOKEN_TYPE_OPREG || pToken->getTokenType() == CSTOKEN_TYPE_OPVAL) {
                // Get opType of each operand  
                opType = pLine->GetAt(tIdx)->getTokenTypeStr();
                incFeatureCount(opType.MakeLower());        
            }
        }

        // Get mnemonic + opType of the first operand
        CString mnemonicOpType0;
        if (pLine->GetSize() > 1) {
            mnemonicOpType0 = mnemonic + pLine->GetAt(1)->getTokenTypeStr();
            incFeatureCount(mnemonicOpType0.MakeLower());
        }

        // Get opType of the first and second operands
        CString opType0OpType1;
        if (pLine->GetSize() > 2) {
            opType0OpType1 = pLine->GetAt(1)->getTokenTypeStr() + pLine->GetAt(2)->getTokenTypeStr();
            incFeatureCount(opType0OpType1.MakeLower());
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
bool CCSRegion::constructVector(CStringArray& globalFeatures)
{   
    int cnt = -1;
	CString pFeatureStr;
    m_vector.SetSize(globalFeatures.GetSize());
    for (int f = 0; f < globalFeatures.GetSize(); ++f) {
		pFeatureStr = globalFeatures.GetAt(f).GetString();
		if (!m_featureCounts.Lookup(pFeatureStr, cnt))    
            m_vector[f] = 0;    // this region does not have such feature
        else
            m_vector[f] = cnt;  // this region has such feature
    }
    return true;
}


//
// Construct binary vector
//
bool CCSRegion::constructBinaryVector(const CCSIntArray& filteredFeatures, const CCSIntArray& globalMedians)
{    
#if 0
    if (m_vector.GetSize() != globalMedians.GetSize()) {
        tcout << _T("CCSRegion:constructBinaryVector: incorrect vector size.") <<  endl;
        ASSERT(false);
        return false;
    }
#endif
    m_binaryVector.SetSize(filteredFeatures.GetSize());
    for (int f = 0; f < filteredFeatures.GetSize(); ++f)
		m_binaryVector[f] = m_vector[filteredFeatures.GetAt(f)] >= globalMedians.GetAt(filteredFeatures.GetAt(f)); // filteredFeatures stores the index of the medians which are more than 0 in globalMedian Vector
    
    return true;
}


// 
// update the m_globalMedian vector when each region is created
//
bool CCSRegion::updateGlobalMedians(CCSIntArray& globalMedians, CCSAssemblyFileMgr* pAssemblyFileMgr)
{
    int cnt = -1;
	CString pFeatureStr;
    for (int i  = 0; i < globalMedians.GetSize(); ++i) {  
		pFeatureStr = pAssemblyFileMgr->m_globalFeatures.GetAt(i).GetString();
		if (!m_featureCounts.Lookup(pFeatureStr, cnt))  // if a region has a feature with value X, incremet X in redundancyVetor                      
            ++ pAssemblyFileMgr->m_redundancyVector[i][0]; //  this region does not have such feature, so just increase 0 (X = 0)
        else {    
                if (cnt > 100 ) {
                    tcout << _T("CCSAssemblyFileMgr::constructRedundancyVector: incorrect vector size. increment the size to more than 100") <<  endl;
                    ASSERT(false);
                    return false;
                }
            ++ pAssemblyFileMgr->m_redundancyVector[i][cnt];  // this region has such feature, so incerase X (X = cnt)                          
        }         
        globalMedians.GetAt(i) = findMedian(pAssemblyFileMgr->m_redundancyVector[i]);   // find the median of this feature and update m_globalMedians     
	}
    return true;
}

// 
// find the median of an integer vector (this vector is sorted). Median is the middle nuymber when the numbers are sorted
//
int CCSRegion::findMedian(const vector<int>& redundancyVector)
{
    int featureValue = 0;
    int sum = 0;  
    for (featureValue = 0; featureValue < redundancyVector.size(); ++featureValue) {
		sum += redundancyVector[featureValue];
        if (sum > m_cntRegion/2 + 1)
            break;
    }
    return featureValue;
}

//
// djb2 hash generator
//
UINT CCSRegion::hashGenerator(const char *str)
{
	unsigned long hash = 5381;
    int c;

    while (c = *str++)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
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
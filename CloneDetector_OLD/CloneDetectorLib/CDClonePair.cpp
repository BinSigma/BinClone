#include "StdAfx.h"
#include "CDClonePair.h"

//***************
// CCDClonePair *
//***************

//
// It is necessary to do a deep copy of the regions because
// every clone pair has different start and end indexes.
//
CCDClonePair::CCDClonePair(CCDRegion* pRegionA, CCDRegion* pRegionB)
    : m_regionA(pRegionA->m_regionID, pRegionA->getFunction(), pRegionA->m_startIdx, pRegionA->m_endIdx, pRegionA->m_rawStartIdx, pRegionA->m_rawEndIdx),
      m_regionB(pRegionB->m_regionID, pRegionB->getFunction(), pRegionB->m_startIdx, pRegionB->m_endIdx, pRegionB->m_rawStartIdx, pRegionB->m_rawEndIdx)
{
}

CCDClonePair::~CCDClonePair()
{
}


//****************
// CCDClonePairs *
//****************

void CCDClonePairs::cleanup()
{
    for (int i = 0; i < GetSize(); ++i)
        delete GetAt(i);

    RemoveAll();
}

CCDClonePair* CCDClonePairs::findClonePair(const CCDRegion* pRegionA, const CCDRegion* pRegionB) const
{
    for (int i = 0; i < GetSize(); ++i) {
        if (GetAt(i)->getRegionA()->m_startIdx == pRegionA->m_startIdx && GetAt(i)->getRegionA()->m_endIdx == pRegionA->m_endIdx && 
            GetAt(i)->getRegionB()->m_startIdx == pRegionB->m_startIdx && GetAt(i)->getRegionB()->m_endIdx == pRegionB->m_endIdx)
            return GetAt(i);
    }
    return NULL;
}


//
// Remove duplicates regions. Assume the array has been sorted by startIdx of RegionA.
//
bool CCDClonePairs::removeDuplicates()
{
    if (GetSize() < 2)
        return true;

    CCDClonePair* currPair = NULL;
    CCDClonePair* prevPair = NULL;
    for (int i = GetSize() - 1; i > 0; --i) {  
        currPair = GetAt(i);
        prevPair = GetAt(i - 1);
        if (currPair->getRegionA()->m_startIdx == prevPair->getRegionA()->m_startIdx && currPair->getRegionA()->m_endIdx == prevPair->getRegionA()->m_endIdx && 
            currPair->getRegionB()->m_startIdx == prevPair->getRegionB()->m_startIdx && currPair->getRegionB()->m_endIdx == prevPair->getRegionB()->m_endIdx) {
            RemoveAt(i);
            delete currPair;
            currPair = NULL;            
        }
    }
    return true;
}


//
// Add a new pair to the array
//
bool CCDClonePairs::addPair(CCDRegion* pRegionA, CCDRegion* pRegionB)
{
    CCDClonePair* pNewClonePair = new CCDClonePair(pRegionA, pRegionB);
    try {
        // Add it at the end.    
        Add(pNewClonePair);
        return true;
    }
    catch (CMemoryException&) {
        tcout << _T("CCDClonePairs: failed to add pair to clone pairs.") << endl;
        delete pNewClonePair;
        pNewClonePair = NULL;
        ASSERT(false);
        return false;
    }
}

//
// Sort the array of clones by the startIdx of regionA.
//
bool CCDClonePairs::sortByRegAStartIdx()
{
    return quickSort(0, GetSize() - 1);
}


//---------------------------------------------------------------------------
// Quick sort records by attrib index.
//---------------------------------------------------------------------------
bool CCDClonePairs::quickSort(int left, int right)
{
    int p = -1;
    int pivot = -1;
    if (findPivot(left, right, pivot)) {
        p = partition(left, right, pivot);
        if (!quickSort(left, p - 1)) {
            ASSERT(false);
            return false;
        }
        if (!quickSort(p, right)) {
            ASSERT(false);
            return false;
        }
    }
    return true;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
int CCDClonePairs::getNumericValue(int idx)
{
    return GetAt(idx)->getRegionA()->m_startIdx;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool CCDClonePairs::findPivot(int left, int right, int& pivot)
{
    pivot = -1;
    // left value
    int a = getNumericValue(left);
    // middle value
    int b = getNumericValue(left + (right - left) / 2);
    // right value
    int c = getNumericValue(right);
    // order these 3 values
    orderNumbers(a, b, c);

    if (a < b) {
        pivot = b;
        return true;
    }

    if (b < c) {
        pivot = c;
        return true;
    }

    int pValue = -1, leftValue = -1;
    int p = left + 1;
    while (p <= right) {
        pValue = getNumericValue(p);
        leftValue = getNumericValue(left);
        if (pValue != leftValue) {
            if (pValue < leftValue)
                pivot = leftValue;
            else
                pivot = pValue;
            return true;
        }
        ++p;
    }
    return false;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
int CCDClonePairs::partition(int left, int right, int pivot) 
{
    while (left <= right) {
        while (getNumericValue(left) < pivot)
            ++left;

        while (getNumericValue(right) >= pivot)
            --right;

        if (left < right) {
            swapEntry(left, right);
            ++left;
            --right;
        }
    }
    return left;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void CCDClonePairs::swapEntry(int idxA, int idxB)
{
    CCDClonePair* pTemp = GetAt(idxA);
    SetAt(idxA, GetAt(idxB));
    SetAt(idxB, pTemp);
}


//*******************
// CCDClonePairsMap *
//*******************

CCDClonePairsMap::CCDClonePairsMap()
{
}

CCDClonePairsMap::~CCDClonePairsMap()
{
}


//
// This functions add the pair to the map
//
bool CCDClonePairsMap::addPairToMap(const CString& filePathA, const CString& filePathB, CCDRegion* pRegionA, CCDRegion* pRegionB)
{
    // Ensure the hash key is consistent.
    // The "smaller" string (by alphabetical order) is placed at the front.
    // The regions are also arranged in the same order.
    CString key;
    if (filePathA.CompareNoCase(filePathB) < 0)
        key = filePathA + filePathB;
    else {
        key = filePathB + filePathA;
        CCDRegion* pTempRegion = pRegionA;
        pRegionA = pRegionB;
        pRegionB = pTempRegion;
    }

    CCDClonePairs* pClonePairs = NULL;
    if (Lookup(key, pClonePairs))
        return pClonePairs->addPair(pRegionA, pRegionB);
    
    // key not exist
    pClonePairs = new CCDClonePairs();        
    if (!pClonePairs->addPair(pRegionA, pRegionB)) {
        delete pClonePairs;
        pClonePairs = NULL;
        return false;
    }
    SetAt(key, pClonePairs);
    return true;
}

//
// Sort clone pairs by startIdx of RegionA and then remove duplicates
//
bool CCDClonePairsMap::sortAndRemoveDuplicates(int& nClonePairs)
{
    nClonePairs = 0;
    CCDClonePairs* pClonePairs = NULL;
    CString key;
    POSITION pos = GetStartPosition();
    while (pos)  {
        GetNextAssoc(pos, key, pClonePairs);
        if (!pClonePairs->sortByRegAStartIdx()) {
            nClonePairs = -1;
            return false;
        }
        if (!pClonePairs->removeDuplicates()) {
            nClonePairs = -1;
            return false;
        }
        nClonePairs += pClonePairs->GetSize();
    }
    return true;
}

void CCDClonePairsMap::cleanup()
{
    CCDClonePairs* pClonePairs = NULL;
    CString key;
    POSITION pos = GetStartPosition();
    while (pos)  {
        GetNextAssoc(pos, key, pClonePairs);
        pClonePairs->cleanup();
        pClonePairs->RemoveAll();
        delete pClonePairs;
        pClonePairs = NULL;
    }
    RemoveAll();
}
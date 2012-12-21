//-----------------------------------------------------
// Objective:   Model a clone pair.
//
// History:     2011/10/19 - B. Fung (Created)
// 
//-----------------------------------------------------

#pragma once

#include "CDRegion.h"

class CCDClonePair
{
public:
    CCDClonePair(CCDRegion* pRegionA, CCDRegion* pRegionB);
    virtual ~CCDClonePair();

    CCDRegion* getRegionA() { return &m_regionA; };
    CCDRegion* getRegionB() { return &m_regionB; };

private:
    CCDRegion m_regionA;
    CCDRegion m_regionB;
};

typedef CTypedPtrArray<CPtrArray, CCDClonePair*> CCDClonePairArray;
class CCDClonePairs : public CCDClonePairArray
{
public:
    CCDClonePairs() {};
    virtual ~CCDClonePairs() {};

    CCDClonePair* findClonePair(const CCDRegion* pRegionA, const CCDRegion* pRegionB) const;
    bool removeDuplicates();
    bool addPair(CCDRegion* pRegionA, CCDRegion* pRegionB);
    bool sortByRegAStartIdx();
    void cleanup();

protected:
    bool quickSort(int left, int right);
    int getNumericValue(int idx);
    bool findPivot(int left, int right, int& pivot);
    int partition(int left, int right, int pivot);
    void swapEntry(int idxA, int idxB);
};

typedef CTypedPtrMap<CMapStringToPtr, CString, CCDClonePairs*> CCDMapStringToClonePairs;
class CCDClonePairsMap : public CCDMapStringToClonePairs
{
public:
    CCDClonePairsMap();
    virtual ~CCDClonePairsMap();
    bool addPairToMap(const CString& filePathA, const CString& filePathB, CCDRegion* pRegionA, CCDRegion* pRegionB);
    bool sortAndRemoveDuplicates(int& nClonePairs);
    void cleanup();
};
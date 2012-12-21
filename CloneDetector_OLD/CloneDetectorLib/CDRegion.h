//-----------------------------------------------------
// Objective:   Model a region in an assembly file.
//
// History:     2011/10/19 - B. Fung (Created)
// 
//-----------------------------------------------------

#pragma once

#include "CDFeatureVector.h"

class CCDAssemblyFunction;
class CCDRegionMgr;

class CCDRegion
{
public:
    CCDRegion(int regionID, CCDAssemblyFunction* pFcn, int startIdx, int endIdx, int rawStartIdx, int rawEndIdx);
    virtual ~CCDRegion();
   
    bool regionToVector(int kThreshold, CCDRegionMgr* pRegionMgr);
    Int32u calDistance(const CCDRegion* pRegion) const;
    bool overlap(const CCDRegion& region);
    inline CCDAssemblyFunction* getFunction() const { return m_pFcn; };
    inline const CCDFeatureVector& getVector() const { return m_vector; };
    
    int m_startIdx;         // starting point of a region, inclusive
    int m_endIdx;           // ending point of a region, inclusive        
    int m_rawStartIdx;      // starting point of a region in raw file, inclusive
    int m_rawEndIdx;        // ending point of a region in raw file, inclusive         
    int m_regionID;         // region's ID

private:
    int incFeatureCount(LPCTSTR featureStr);
    bool constructVector(CCDRegionMgr* pRegionMgr);

    CCDAssemblyFunction* m_pFcn;        // function of this region
    CCDMapStringToInt m_featureCounts;  // keep track of the count of each feature. 
    CCDFeatureVector m_vector;          // feature vector
};

typedef CTypedPtrArray<CPtrArray, CCDRegion*> CCDRegionArray;
class CCDRegions : public CCDRegionArray
{
public:
    CCDRegions() {};
    virtual ~CCDRegions() {};
    int findRegion(const CCDRegion* pRegion) const; // return -1 if not found.
    bool addIfNew(CCDRegion* pRegion);
    bool addIfNewWithSort(CCDRegion* pNewRegion);
    bool addRegion(int regionID, CCDAssemblyFunction* pFcn, int startIdx, int endIdx, int rawStartIdx, int rawEndIdx);    
    void cleanup();
};

typedef CMap<UINT, UINT, CCDRegions*, CCDRegions*&> CCDMapUINTToRegions;
class CCDRegionsMap : public CCDMapUINTToRegions
{
public:
    CCDRegionsMap();
    virtual ~CCDRegionsMap();
   	bool addRegion(UINT key, CCDRegion* pRegion);
    bool removeSingletons();
    void cleanup();
};

typedef CTypedPtrArray<CPtrArray, CCDRegionsMap*> CCDRegionsMapArray;
class CCDRegionsMaps : public CCDRegionsMapArray
{
public:
    CCDRegionsMaps();
    virtual ~CCDRegionsMaps();
    bool addRegionMap(CCDRegionsMap* pNewRegionMap);
    void cleanup();
};
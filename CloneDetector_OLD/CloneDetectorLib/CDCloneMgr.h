//-----------------------------------------------------
// Objective:   Manage the clones of each file.
//
// History:     2011/10/19 - B. Fung (Created)
// 
//-----------------------------------------------------
#pragma once

#include "CDRegionMgr.h"
#include "CDCloneCluster.h"
#include "CDClonePair.h"

class CCDCloneMgr
{
public:
    CCDCloneMgr(CCDRegionMgr* pRegionMgr, int windowSize, double oThreshold, double minCoOccThreshold, bool bInFileClone);
    virtual ~CCDCloneMgr();
      
    inline CCDClonePairsMap* getClonePairsMap() { return &m_clonePairsMap; };

    bool findExactClones();
    bool removeTrivialClones();
    bool convertToClonePairs();
    bool unifyToLargestClones();
    bool findInexactClones();
    bool randomInexactCloneFinder(); 
    bool slidingInexactCloneFinder();
    bool twoCombinationInexactCloneFinder();
    
private:
    int median(int index);
    int bucketSort(const CCDIntArray& ary, int k);
    int bucketSortMedianFinder(const CCDIntArray& ary, int k);
    static int findLargestNumber(const CCDIntArray& ary);

    int m_nFeatures; 
    int m_medianNZCNT; 
    CCDIntArray m_medianNZ; 
    CCDRegions m_allRegions; 
    CCDIntArray m_scores; 
    CCDIntArray m_medianAmounts;
    int m_windowSize;           // window size
    double m_oThreshold;        // maximum allowed fraction of overlap  
    bool m_bInFileClone;        // do in file clone?
    double m_minCoOccThreshold; // minimum co-occurrence threshhold for inexact pairs       
    int m_nExactClonesPairs;
    CCDRegionMgr* m_pRegionMgr;
    CCDRegionsMap* m_pExactCloneClusters;
    CCDCloneClusters m_cloneClusters;   // an array of clone clusters, in which each clone cluster is an array of regions
    CCDClonePairsMap m_clonePairsMap;   // Map "<filePath1><filePath2>" to an array of clone pairs    
};


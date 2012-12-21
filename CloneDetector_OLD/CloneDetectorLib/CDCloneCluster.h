//-----------------------------------------------------
// Objective:   Model a clone cluster
//
// History:     2011/10/19 - B. Fung (Created)
// 
//-----------------------------------------------------

#pragma once

#include "CDRegion.h"

class CCDCloneCluster : public CCDRegions
{
public:
    CCDCloneCluster();
    virtual ~CCDCloneCluster();

    bool findRegionsInSameFunction(const CCDAssemblyFunction* pFcn, CCDRegions& regionsInSameFunction) const;
};


typedef CTypedPtrArray<CPtrArray, CCDCloneCluster*> CCDCloneClusterArray;
class CCDCloneClusters : public CCDCloneClusterArray
{
public:
    CCDCloneClusters() {};
    virtual ~CCDCloneClusters() {};
    
    void cleanup();
    bool removeSingletons();
};
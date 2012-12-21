#include "StdAfx.h"
#include "CDCloneCluster.h"


//******************
// CCDCloneCluster *
//******************

CCDCloneCluster::CCDCloneCluster()
{
}


CCDCloneCluster::~CCDCloneCluster()
{
}

//
// Find regions in this cluster that are in the same function.
//
bool CCDCloneCluster::findRegionsInSameFunction(const CCDAssemblyFunction* pFcn, CCDRegions& regionsInSameFunction) const
{
    CCDRegion* pRegion = NULL;
    for (int rIdx = 0; rIdx < GetSize(); ++rIdx) {
        pRegion = GetAt(rIdx);
        if (pRegion->getFunction() == pFcn)
            regionsInSameFunction.Add(pRegion);
    }
    return true;
}


//*******************
// CCDCloneClusters *
//*******************

void CCDCloneClusters::cleanup()
{
    for (int i = 0; i < GetSize(); ++i)
        delete GetAt(i);

    RemoveAll();
}


bool CCDCloneClusters::removeSingletons()
{
    CCDCloneCluster* pCloneCluster = NULL;
    for (int c = GetSize() - 1; c >= 0; --c) {
        pCloneCluster = GetAt(c);
        if (pCloneCluster->GetSize() < 2) {
            RemoveAt(c);
            delete pCloneCluster;
            pCloneCluster = NULL;
        }
    }
    return true;
}
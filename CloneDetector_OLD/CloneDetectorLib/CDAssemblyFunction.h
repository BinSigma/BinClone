#pragma once

#include "CDRegion.h"

class CCDAssemblyFile;

class CCDAssemblyFunction
{
public:
    CCDAssemblyFunction(CCDAssemblyFile* pAssemblyFile, int startIdx, int endIdx);
    virtual ~CCDAssemblyFunction();

    inline CCDAssemblyFile* getAssemblyFile() const { return m_pAssemblyFile; };
    inline bool addRegion(int regionID, CCDAssemblyFunction* pFcn, int startIdx, int endIdx, int rawStartIdx, int rawEndIdx) { return m_regions.addRegion(regionID, pFcn, startIdx, endIdx, rawStartIdx, rawEndIdx); };

    inline int getNumRegions() const { return m_regions.GetSize(); };
    inline CCDRegion* getRegion(int idx) const { return m_regions.GetAt(idx); };

    int m_startIdx;         // starting point of a function, inclusive
    int m_endIdx;           // ending point of a function, inclusive       

private:    
    CCDAssemblyFile* m_pAssemblyFile;     // file of this region
    CCDRegions m_regions;
};


typedef CTypedPtrArray<CPtrArray, CCDAssemblyFunction*> CCDFunctionArray;
class CCDAssemblyFunctions : public CCDFunctionArray
{
public:
    CCDAssemblyFunctions() {};
    virtual ~CCDAssemblyFunctions() {};
    bool addFunction(CCDAssemblyFile* pAssemblyFile, int startIdx, int endIdx);
    void cleanup();
};
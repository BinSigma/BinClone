#pragma once

#include "CSRegion.h"

class CCSAssemblyFile;

class CCSAssemblyFunction
{
public:
    CCSAssemblyFunction(CCSAssemblyFile* pAssemblyFile, int startIdx, int endIdx, int startRawIdx, int endRawIdx);
    virtual ~CCSAssemblyFunction();

    inline CCSAssemblyFile* getAssemblyFile() const { return m_pAssemblyFile; };
    inline void resetNumRegions() { m_nRegions = 0; };
    inline int getNumRegions() const { return m_nRegions; };
    inline void incNumRegions() { ++m_nRegions; };

    int m_dbFcnID;          // function ID in database
    int m_startIdx;         // starting point of a function, inclusive
    int m_endIdx;           // ending point of a function, inclusive  
    int m_startRawIdx;      // starting point of a function in raw file
    int m_endRawIdx;        // ending point of a function in raw file
    
    UINT m_hashValue;       // hash value of this funciton.

private:    
    CCSAssemblyFile* m_pAssemblyFile;     // file of this region    
    int m_nRegions;         // number of regions in this file.
};


typedef CTypedPtrArray<CPtrArray, CCSAssemblyFunction*> CCSFunctionArray;
class CCSAssemblyFunctions : public CCSFunctionArray
{
public:
    CCSAssemblyFunctions() {};
    virtual ~CCSAssemblyFunctions() {};
    bool addFunction(CCSAssemblyFile* pAssemblyFile, int startIdx, int endIdx, int startRawIdx, int endRawIdx);
    void cleanup();
};
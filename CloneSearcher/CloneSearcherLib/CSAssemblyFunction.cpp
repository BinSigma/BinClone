#include "StdAfx.h"
#include "CSAssemblyFunction.h"


//**********************
// CCSAssemblyFunction *
//**********************

CCSAssemblyFunction::CCSAssemblyFunction(CCSAssemblyFile* pAssemblyFile, int startIdx, int endIdx, int startRawIdx, int endRawIdx)
    : m_dbFcnID(-1), m_pAssemblyFile(pAssemblyFile), m_startIdx(startIdx), m_endIdx(endIdx), m_startRawIdx(startRawIdx), m_endRawIdx(endRawIdx), m_nRegions(0), m_hashValue(0)
{
}

CCSAssemblyFunction::~CCSAssemblyFunction()
{
}


//***********************
// CCSAssemblyFunctions *
//***********************

bool CCSAssemblyFunctions::addFunction(CCSAssemblyFile* pAssemblyFile, int startIdx, int endIdx, int startRawIdx, int endRawIdx)
{
    try {
        Add(new CCSAssemblyFunction(pAssemblyFile, startIdx, endIdx, startRawIdx, endRawIdx));
        return true;
    }
    catch (CMemoryException&) {
        tcout << _T("CCSAssemblyFunctions: failed to add function.") <<  endl;
        ASSERT(false);
        return false;
    } 
}

void CCSAssemblyFunctions::cleanup()
{
    for (int i = 0; i < GetSize(); ++i)
        delete GetAt(i);

    RemoveAll();
}
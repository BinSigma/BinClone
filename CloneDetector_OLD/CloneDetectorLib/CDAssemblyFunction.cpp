#include "StdAfx.h"
#include "CDAssemblyFunction.h"


//**********************
// CCDAssemblyFunction *
//**********************

CCDAssemblyFunction::CCDAssemblyFunction(CCDAssemblyFile* pAssemblyFile, int startIdx, int endIdx)
    : m_pAssemblyFile(pAssemblyFile), m_startIdx(startIdx), m_endIdx(endIdx)
{
}

CCDAssemblyFunction::~CCDAssemblyFunction()
{
    m_regions.cleanup();
}


//***********************
// CCDAssemblyFunctions *
//***********************

bool CCDAssemblyFunctions::addFunction(CCDAssemblyFile* pAssemblyFile, int startIdx, int endIdx)
{
    try {
        Add(new CCDAssemblyFunction(pAssemblyFile, startIdx, endIdx));
        return true;
    }
    catch (CMemoryException&) {
        tcout << _T("CCDAssemblyFunctions: failed to add function.") <<  endl;
        ASSERT(false);
        return false;
    } 
}

void CCDAssemblyFunctions::cleanup()
{
    for (int i = 0; i < GetSize(); ++i)
        delete GetAt(i);

    RemoveAll();
}
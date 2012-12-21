#include "StdAfx.h"
#include "CDAssemblyFile.h"

//******************
// CCDAssemblyFile *
//******************

CCDAssemblyFile::CCDAssemblyFile(LPCTSTR filePath)
    : m_fileIdx(-1), m_filePath(filePath)
{
}


CCDAssemblyFile::~CCDAssemblyFile()
{
    cleanup();
    m_functions.cleanup();
}

bool CCDAssemblyFile::addLine(CCDAssemblyLine* pAssemblyLine)
{
    try {
        Add(pAssemblyLine);
        return true;
    }
    catch (CMemoryException&) {
        tcout << _T("CCDAssemblyFile: failed to add line to file: ") << m_filePath.GetString() << endl;
        ASSERT(false);
        return false;
    } 
}

bool CCDAssemblyFile::addFunction(CCDAssemblyFunction* pFcn)
{
    try {
        m_functions.Add(pFcn);
        return true;
    }
    catch (CMemoryException&) {
        tcout << _T("CCDAssemblyFile: failed to add function to file: ") << m_filePath.GetString() << endl;
        ASSERT(false);
        return false;
    } 
}


//
// Compose the hash key based on the content.
// 
CString CCDAssemblyFile::composeContentHashKey(int startIdx, int endIdx) const
{
    CString hashStr;
    for (int lineIdx = startIdx; lineIdx <= endIdx; ++lineIdx) {
        if (GetAt(lineIdx)->isMnemonicStatement())  // only if it is a mnemonic statement
            hashStr += GetAt(lineIdx)->composeLineHashKey();
    }
    return hashStr;
}


//
// Get number of regions in this file
//
int CCDAssemblyFile::getNumRegions() const
{
    int nRegions = 0;
    for (int fcnIdx = 0; fcnIdx < m_functions.GetSize(); ++fcnIdx)
        nRegions += m_functions.GetAt(fcnIdx)->getNumRegions();
    return nRegions;
}



//
// Get content string
//
bool CCDAssemblyFile::getContentStr(CString& str) const
{
    for (int i = 0; i < GetSize(); ++i) {
        str += GetAt(i)->getLineStr();
        if (i < GetSize() - 1)
            str += _T("\n");
    }
    return true;
}


//
// Get line string
//
bool CCDAssemblyFile::getLineStr(int lineIdx, CString& str) const
{
    if (lineIdx >= GetSize() || lineIdx < 0) {
        tcout << _T("Error: CCDAssemblyFile: invalid lineIdx: ") << lineIdx << endl;
        ASSERT(false);
        return false;
    }
    str = GetAt(lineIdx)->getLineStr();
    return true;
}


//*******************
// CCDAssemblyFiles *
//*******************

bool CCDAssemblyFiles::addFile(CCDAssemblyFile* pAssemblyFile)
{
    try {
        pAssemblyFile->m_fileIdx = Add(pAssemblyFile);
        return true;
    }
    catch (CMemoryException&) {
        tcout << _T("CCDAssemblyFiles: failed to add file: ") << pAssemblyFile->getFilePath().GetString() << endl;
        ASSERT(false);
        return false;
    }    
}

void CCDAssemblyFiles::cleanup()
{
    for (int i = 0; i < GetSize(); ++i)
        delete GetAt(i);

    RemoveAll();
}
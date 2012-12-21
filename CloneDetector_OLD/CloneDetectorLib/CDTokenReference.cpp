#include "StdAfx.h"
#include "CDTokenReference.h"

//********************
// CCDTokenReference *
//********************

CCDTokenReference::CCDTokenReference(const CCDAssemblyFile* pRefFile, int refRawLineNumber)
    : m_pRefFile(pRefFile), m_refRawLineNumber(refRawLineNumber)
{
}


CCDTokenReference::~CCDTokenReference()
{
}


//*********************
// CCDTokenReferences *
//*********************

//
// This functions add a token reference
//
bool CCDTokenReferences::addTokenRef(const CCDAssemblyFile* pRefFile, int refRawLineNumber)
{
    CCDTokenReference* pNewTokRef = new CCDTokenReference(pRefFile, refRawLineNumber);
    try {
        Add(pNewTokRef);
        return true;
    }
    catch (CMemoryException&) {
        tcout << _T("CCDTokenReferences: failed to add token reference.") << endl;
        delete pNewTokRef;
        pNewTokRef = NULL;
        ASSERT(false);
        return false;
    }
}


void CCDTokenReferences::cleanup()
{
    for (int i = 0; i < GetSize(); ++i)
        delete GetAt(i);

    RemoveAll();
}


//*****************
// CCDTokenRefMap *
//*****************

//
// This functions add a token reference
//
bool CCDTokenRefMap::addTokenRef(LPCTSTR tokenKey, const CCDAssemblyFile* pRefFile, int refRawLineNumber)
{
    CCDTokenReferences* pTokReferences = NULL;
    if (!Lookup(tokenKey, pTokReferences)) {
        // key not exist
        pTokReferences = new CCDTokenReferences();        
        if (!pTokReferences->addTokenRef(pRefFile, refRawLineNumber)) {
            delete pTokReferences;
            pTokReferences = NULL;
            return false;
        }
        SetAt(tokenKey, pTokReferences);
    }
    else {
        // key already exists
        if (!pTokReferences->addTokenRef(pRefFile, refRawLineNumber)) 
            return false;
    }    
    return true;
}


void CCDTokenRefMap::cleanup()
{
    CCDTokenReferences* pTokReferences = NULL;
    CString key;
    POSITION pos = GetStartPosition();
    while (pos)  {
        GetNextAssoc(pos, key, pTokReferences);
        pTokReferences->cleanup();
        pTokReferences->RemoveAll();
        delete pTokReferences;
        pTokReferences = NULL;
    }
    RemoveAll();
}
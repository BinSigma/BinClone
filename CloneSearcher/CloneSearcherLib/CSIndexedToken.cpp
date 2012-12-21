#include "StdAfx.h"
#include "CSIndexedToken.h"

//******************
// CCSIndexedToken *
//******************

CCSIndexedToken::CCSIndexedToken(LPCTSTR tokStr, int dbFileID, int rawLineNum)
    : m_tokStr(tokStr),
      m_dbFileID(dbFileID),
      m_rawLineNum(rawLineNum)
{
}


CCSIndexedToken::~CCSIndexedToken()
{
}


//*******************
// CCSIndexedTokens *
//*******************

bool CCSIndexedTokens::addIndexedToken(CCSIndexedToken* pIndexedToken)
{
    try {
        Add(pIndexedToken);
        return true;
    }
    catch (CMemoryException&) {
        tcout << _T("CCSIndexedTokens: failed to add indexed token: ") << pIndexedToken->getTokStr().GetString() << endl;
        ASSERT(false);
        return false;
    }    
}

void CCSIndexedTokens::cleanup()
{
    for (int i = 0; i < GetSize(); ++i)
        delete GetAt(i);

    RemoveAll();
}
#include "StdAfx.h"
#include "CDTokenRefMgr.h"


CCDTokenRefMgr::CCDTokenRefMgr()
{
}


CCDTokenRefMgr::~CCDTokenRefMgr()
{
    m_constantsIndex.cleanup();
    m_stringsIndex.cleanup();
    m_importsIndex.cleanup();
}


//
// Iterate through each token, identify VAL, and store it.
//
bool CCDTokenRefMgr::parseConstantRef(const CCDAssemblyLine* pLine, const CCDAssemblyFile* pRefFile, int refRawLineNumber)
{
    CCDAssemblyToken* pToken = NULL;
    for (int t = 0; t < pLine->GetSize(); ++t) {
        pToken = pLine->GetAt(t);
        if (pToken->getTokenType() == CDTOKEN_TYPE_OPVAL)
            if (!addConstantRef((LPCTSTR) pToken->getTokenStr(), pRefFile, refRawLineNumber))
                return false;
    }
    return true;
}


//
// Extract all strings in double quotes, 
// e.g., push	offset aCcommonclienti ; "CCommonClientInfo::GetVersion() : reg.O"...
//
bool CCDTokenRefMgr::parseStringRef(const CString& str, const CCDAssemblyFile* pRefFile, int refRawLineNumber)
{
    int firstDoubleQuotePos = str.Find(_T("\""));
    if (firstDoubleQuotePos == -1)
        return true; // no strings

    int secondDoubleQuotePos = str.Find(_T("\""), firstDoubleQuotePos + 1);
    if (secondDoubleQuotePos == -1)
        return true; // no strings

    CString tokenKey = str.Mid(firstDoubleQuotePos + 1, secondDoubleQuotePos - firstDoubleQuotePos - 1);
    if (tokenKey.IsEmpty())
        return true;
    if (!addStringRef((LPCTSTR) tokenKey, pRefFile, refRawLineNumber))
        return false;

    return parseStringRef(str.Mid(secondDoubleQuotePos + 1), pRefFile, refRawLineNumber);
}


//
// Identify import in the format of "call ds:InitializeCriticalSection", and store it.
//
bool CCDTokenRefMgr::parseImportRef(const CCDAssemblyLine* pLine, const CCDAssemblyFile* pRefFile, int refRawLineNumber)
{  
    if (pLine->GetSize() < 2)
        return true;
    if (pLine->GetAt(0)->getTokenStr().CompareNoCase(_T("CALL")) != 0)
        return true;

    // Make sure op1 starts with "ds:"
    CString op1 = pLine->GetAt(1)->getTokenStr();
    int dsPos = op1.Find(_T("ds:"));

    CString importStr = op1.Mid(dsPos + 3);
    if (importStr.IsEmpty())
        return true;

    return addImportRef((LPCTSTR) importStr, pRefFile, refRawLineNumber);
}


//
// Add a constant reference
//
bool CCDTokenRefMgr::addConstantRef(LPCTSTR tokenKey, const CCDAssemblyFile* pRefFile, int refRawLineNumber)
{
    return m_constantsIndex.addTokenRef(tokenKey, pRefFile, refRawLineNumber);
}


//
// Add a string reference
//
bool CCDTokenRefMgr::addStringRef(LPCTSTR tokenKey, const CCDAssemblyFile* pRefFile, int refRawLineNumber)
{
    return m_stringsIndex.addTokenRef(tokenKey, pRefFile, refRawLineNumber);
}


//
// Add an import reference
//
bool CCDTokenRefMgr::addImportRef(LPCTSTR tokenKey, const CCDAssemblyFile* pRefFile, int refRawLineNumber)
{
    return m_importsIndex.addTokenRef(tokenKey, pRefFile, refRawLineNumber);
}
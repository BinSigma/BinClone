//******************************************************************************//
// Copyright 2014 Concordia University											//
//																				//		
// Licensed under the Apache License, Version 2.0 (the "License");				//
// you may not use this file except in compliance with the License.				//
// You may obtain a copy of the License at										//
//																				//
//    http://www.apache.org/licenses/LICENSE-2.0								//
//																				//
// Unless required by applicable law or agreed to in writing, software			//
// distributed under the License is distributed on an "AS IS" BASIS,			//
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.		//
// See the License for the specific language governing permissions and			//
// limitations under the License.												//
//******************************************************************************//

#include "StdAfx.h"
#include "CSTokenRefMgr.h"


CCSTokenRefMgr::CCSTokenRefMgr(CCSDatabaseMgr* pDBMgr)
    : m_pDBMgr(pDBMgr)
{
    ASSERT(m_pDBMgr);
}


CCSTokenRefMgr::~CCSTokenRefMgr()
{
}


//
// Iterate through each token, identify VAL, and store it.
//
bool CCSTokenRefMgr::parseConstantRef(const CCSAssemblyLine* pLine, int dbFileID, int refRawLineNumber)
{
    CCSAssemblyToken* pToken = NULL;
    for (int t = 0; t < pLine->GetSize(); ++t) {
        pToken = pLine->GetAt(t);
        if (pToken->getTokenType() == CSTOKEN_TYPE_OPVAL)
            if (!addConstantRef((LPCTSTR) pToken->getTokenStr(), dbFileID, refRawLineNumber))
                return false;
    }
    return true;
}


//
// Extract all strings in double quotes, 
// e.g., push	offset aCcommonclienti ; "CCommonClientInfo::GetVersion() : reg.O"...
//
bool CCSTokenRefMgr::parseStringRef(const CString& str, int dbFileID, int refRawLineNumber)
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
    if (!addStringRef((LPCTSTR) tokenKey, dbFileID, refRawLineNumber))
        return false;

    return parseStringRef(str.Mid(secondDoubleQuotePos + 1), dbFileID, refRawLineNumber);
}


//
// Identify import in the format of "call ds:InitializeCriticalSection", and store it.
//
bool CCSTokenRefMgr::parseImportRef(const CCSAssemblyLine* pLine, int dbFileID, int refRawLineNumber)
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

    return addImportRef((LPCTSTR) importStr, dbFileID, refRawLineNumber);
}


//
// Add a constant reference to DB
//
bool CCSTokenRefMgr::addConstantRef(LPCTSTR tokenKey, int dbFileID, int refRawLineNumber)
{
    CCSIndexedToken indexedTok(tokenKey, dbFileID, refRawLineNumber);
    return m_pDBMgr->storeConstant(indexedTok);
}


//
// Add a string reference to DB
//
bool CCSTokenRefMgr::addStringRef(LPCTSTR tokenKey, int dbFileID, int refRawLineNumber)
{
    CCSIndexedToken indexedTok(tokenKey, dbFileID, refRawLineNumber);
    return m_pDBMgr->storeString(indexedTok);
}


//
// Add an import reference to DB
//
bool CCSTokenRefMgr::addImportRef(LPCTSTR tokenKey, int dbFileID, int refRawLineNumber)
{
    CCSIndexedToken indexedTok(tokenKey, dbFileID, refRawLineNumber);
    return m_pDBMgr->storeImport(indexedTok);
}
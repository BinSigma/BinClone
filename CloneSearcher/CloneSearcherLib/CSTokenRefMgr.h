//-----------------------------------------------------
// Objective:   Manage the references to tokens
//
// History:     2012/10/07 - B. Fung (Created)
// 
//-----------------------------------------------------

#pragma once

#include "CSDatabaseMgr.h"
#include "CSAssemblyLine.h"

class CCSTokenRefMgr
{
public:
    CCSTokenRefMgr(CCSDatabaseMgr* pDBMgr);
    virtual ~CCSTokenRefMgr();

    bool parseConstantRef(const CCSAssemblyLine* pLine, int dbFileID, int refRawLineNumber);
    bool parseStringRef(const CString& str, int dbFileID, int refRawLineNumber);
    bool parseImportRef(const CCSAssemblyLine* pLine, int dbFileID, int refRawLineNumber);
    
private:
    bool addConstantRef(LPCTSTR tokenKey, int dbFileID, int refRawLineNumber);
    bool addStringRef(LPCTSTR tokenKey, int dbFileID, int refRawLineNumber);
    bool addImportRef(LPCTSTR tokenKey, int dbFileID, int refRawLineNumber);

    CCSDatabaseMgr* m_pDBMgr;
};


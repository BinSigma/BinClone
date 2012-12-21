//-----------------------------------------------------
// Objective:   Manage the references to tokens
//
// History:     2012/02/26 - B. Fung (Created)
// 
//-----------------------------------------------------

#pragma once

#include "CDTokenReference.h"

class CCDTokenRefMgr
{
public:
    CCDTokenRefMgr();
    virtual ~CCDTokenRefMgr();

    bool parseConstantRef(const CCDAssemblyLine* pLine, const CCDAssemblyFile* pRefFile, int refRawLineNumber);
    bool parseStringRef(const CString& str, const CCDAssemblyFile* pRefFile, int refRawLineNumber);
    bool parseImportRef(const CCDAssemblyLine* pLine, const CCDAssemblyFile* pRefFile, int refRawLineNumber);

    int getNumConstants() const { return m_constantsIndex.GetSize(); };
    int getNumStrings() const { return m_stringsIndex.GetSize(); };
    int getNumImports() const { return m_importsIndex.GetSize(); };

    const CCDTokenRefMap* getConstantsIndex() const { return &m_constantsIndex; };
    const CCDTokenRefMap* getStringsIndex() const { return &m_stringsIndex; };
    const CCDTokenRefMap* getImportsIndex() const { return &m_importsIndex; };

private:
    bool addConstantRef(LPCTSTR tokenKey, const CCDAssemblyFile* pRefFile, int refRawLineNumber);
    bool addStringRef(LPCTSTR tokenKey, const CCDAssemblyFile* pRefFile, int refRawLineNumber);
    bool addImportRef(LPCTSTR tokenKey, const CCDAssemblyFile* pRefFile, int refRawLineNumber);

    CCDTokenRefMap m_constantsIndex;        // an index for constants
    CCDTokenRefMap m_stringsIndex;          // an index for strings
    CCDTokenRefMap m_importsIndex;          // an index for imports
};


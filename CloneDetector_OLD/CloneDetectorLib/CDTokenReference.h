//-----------------------------------------------------
// Objective:   Model a token reference to facilitate
//              direct access to tokens
//
// History:     2012/02/26 - B. Fung (Created)
// 
//-----------------------------------------------------

#pragma once

#include "CDAssemblyFile.h"

class CCDTokenReference
{
public:
    CCDTokenReference(const CCDAssemblyFile* pRefFile, int refRawLineNumber);
    virtual ~CCDTokenReference();
    inline const CCDAssemblyFile* getRefFile() { return m_pRefFile; };
    inline int getRefRawLineNumber() { return m_refRawLineNumber; };

private:
    const CCDAssemblyFile* m_pRefFile;
    int m_refRawLineNumber;
};

typedef CTypedPtrArray<CPtrArray, CCDTokenReference*> CCDTokenReferenceArray;
class CCDTokenReferences : public CCDTokenReferenceArray
{
public:
    CCDTokenReferences() {};
    virtual ~CCDTokenReferences() {};    

    bool addTokenRef(const CCDAssemblyFile* pRefFile, int refRawLineNumber);
    void cleanup();
};

typedef CTypedPtrMap<CMapStringToPtr, CString, CCDTokenReferences*> CCDMapStringToTokenReferences;
class CCDTokenRefMap : public CCDMapStringToTokenReferences
{
public:
    CCDTokenRefMap() {};
    virtual ~CCDTokenRefMap() {};

    bool addTokenRef(LPCTSTR tokenKey, const CCDAssemblyFile* pRefFile, int refRawLineNumber);
    void cleanup();
};
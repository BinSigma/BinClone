//-----------------------------------------------------
// Objective:   Model a line in an assembly file.
//
// History:     2011/10/19 - B. Fung (Created)
// 
//-----------------------------------------------------
#pragma once

#include "CDAssemblyToken.h"

class CCDAssemblyLine : public CCDAssemblyTokens
{
public:
    CCDAssemblyLine(int asmFileLineIdx, LPCTSTR lineStr);
    virtual ~CCDAssemblyLine();
    const CString& getLineStr() const { return m_lineStr; };
    bool addTokens();
    CString composeLineHashKey();
    inline int getAsmFileLineIdx() const { return m_asmFileLineIdx; };

    bool isBeginProcedure() const { return GetSize() >= 3 && GetAt(1)->getTokenStr().CompareNoCase(_T("proc")) == 0 && (GetAt(2)->getTokenStr().CompareNoCase(_T("near")) == 0 || GetAt(2)->getTokenStr().CompareNoCase(_T("far")) == 0); } 
    bool isEndProcedure() const { return GetSize() >= 2 && GetAt(1)->getTokenStr().CompareNoCase(_T("endp")) == 0; };
    bool isMnemonicStatement() const { return GetSize() >= 1 && GetAt(0)->getTokenType() == CDTOKEN_TYPE_MNEMONIC; };

private:
    CString m_lineStr;
    int m_asmFileLineIdx;
};


typedef CTypedPtrArray<CPtrArray, CCDAssemblyLine*> CCDAssemblyLineArray;
class CCDAssemblyLines : public CCDAssemblyLineArray
{
public:
    CCDAssemblyLines() {};
    virtual ~CCDAssemblyLines() {};    
    void cleanup();
};
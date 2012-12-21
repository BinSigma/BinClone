#include "StdAfx.h"
#include "CSAssemblyLine.h"

//******************
// CCSAssemblyLine *
//******************

CCSAssemblyLine::CCSAssemblyLine(int asmFileLineIdx, LPCTSTR lineStr)
    : m_lineStr(lineStr), m_asmFileLineIdx(asmFileLineIdx)
{
}


CCSAssemblyLine::~CCSAssemblyLine()
{
    cleanup();
}


// 
// Parse the line into tokens.
//
bool CCSAssemblyLine::addTokens()
{
    CBFStrParser tokenParser(m_lineStr, CS_ASSEMBLYFILE_DELIMETER);
    
    CString tokenStr;
    bool bFirstToken = true, bOffsetToken = false;
    while (tokenParser.getNext(tokenStr)) {
        CBFStrHelper::trim(tokenStr);
        if (tokenStr.IsEmpty())
            continue;

        // Trim comma
        if (tokenStr.Right(1).CompareNoCase(_T(",")) == 0)
            tokenStr.Truncate(tokenStr.GetLength() - 1);

        CCSAssemblyToken* pNewToken = NULL;
        if (bFirstToken) {
            // Mnemonic
            tokenStr = tokenStr.MakeLower();
            if (CCSAssemblyToken::isMnemonic(tokenStr.GetString())) 
                pNewToken = new CCSAssemblyToken(tokenStr, CSTOKEN_TYPE_MNEMONIC);
            else
                pNewToken = new CCSAssemblyToken(tokenStr, CSTOKEN_TYPE_UNKNOWN);
            bFirstToken = false;
        }
        else {
            // Operands
            if (CCSAssemblyToken::isMemoryReference(tokenStr))
                pNewToken = new CCSAssemblyToken(tokenStr, CSTOKEN_TYPE_OPMEM);
            else if (CCSAssemblyToken::isRegisterReference(tokenStr))
                pNewToken = new CCSAssemblyToken(tokenStr, CSTOKEN_TYPE_OPREG);
            else if (CCSAssemblyToken::isConstantValue(tokenStr) || bOffsetToken) {
                pNewToken = new CCSAssemblyToken(tokenStr, CSTOKEN_TYPE_OPVAL);
                bOffsetToken = false;
            }
            else {
                //tcout << _T("Unknown token string: ") << tokenStr.GetString() << endl;
                pNewToken = new CCSAssemblyToken(tokenStr, CSTOKEN_TYPE_UNKNOWN);
            }

            // Is offset string?
            bOffsetToken = tokenStr.CompareNoCase(CS_ASSEMBLYFILE_OFFSET) == 0;
        }

        try {
            Add(pNewToken);
        }
        catch (CMemoryException&) {
            tcout << _T("CCSAssemblyLine: failed to add token from line: ") << m_lineStr.GetString() << endl;
            ASSERT(false);
            return false;
        }
    }
    return true;
}


//
// Compose hash string of this line.
//
CString CCSAssemblyLine::composeLineHashKey()
{
    CString hashStr;
    for (int t = 0; t < GetSize(); ++t) {
        hashStr += GetAt(t)->getTokenStr();
        if (t < GetSize() - 1)
            hashStr += _T(" ");
    }
    hashStr += _T("\n");
    return hashStr;
}


//*******************
// CCSAssemblyLines *
//*******************

void CCSAssemblyLines::cleanup()
{
    for (int i = 0; i < GetSize(); ++i)
        delete GetAt(i);

    RemoveAll();
}
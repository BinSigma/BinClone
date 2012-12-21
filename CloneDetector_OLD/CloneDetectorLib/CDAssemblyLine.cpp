#include "StdAfx.h"
#include "CDAssemblyLine.h"

//******************
// CCDAssemblyLine *
//******************

CCDAssemblyLine::CCDAssemblyLine(int asmFileLineIdx, LPCTSTR lineStr)
    : m_lineStr(lineStr), m_asmFileLineIdx(asmFileLineIdx)
{
}


CCDAssemblyLine::~CCDAssemblyLine()
{
    cleanup();
}


// 
// Parse the line into tokens.
//
bool CCDAssemblyLine::addTokens()
{
    CBFStrParser tokenParser(m_lineStr, CD_ASSEMBLYFILE_DELIMETER);
    
    CString tokenStr;
    bool bFirstToken = true, bOffsetToken = false;
    while (tokenParser.getNext(tokenStr)) {
        CBFStrHelper::trim(tokenStr);
        if (tokenStr.IsEmpty())
            continue;

        // Trim comma
        if (tokenStr.Right(1).CompareNoCase(_T(",")) == 0)
            tokenStr.Truncate(tokenStr.GetLength() - 1);

        CCDAssemblyToken* pNewToken = NULL;
        if (bFirstToken) {
            // Mnemonic
            tokenStr = tokenStr.MakeLower();
            if (CCDAssemblyToken::isMnemonic(tokenStr)) 
                pNewToken = new CCDAssemblyToken(tokenStr, CDTOKEN_TYPE_MNEMONIC);
            else
                pNewToken = new CCDAssemblyToken(tokenStr, CDTOKEN_TYPE_UNKNOWN);
            bFirstToken = false;
        }
        else {
            // Operands
            if (CCDAssemblyToken::isMemoryReference(tokenStr))
                pNewToken = new CCDAssemblyToken(tokenStr, CDTOKEN_TYPE_OPMEM);
            else if (CCDAssemblyToken::isRegisterReference(tokenStr))
                pNewToken = new CCDAssemblyToken(tokenStr, CDTOKEN_TYPE_OPREG);
            else if (CCDAssemblyToken::isConstantValue(tokenStr) || bOffsetToken) {
                pNewToken = new CCDAssemblyToken(tokenStr, CDTOKEN_TYPE_OPVAL);
                bOffsetToken = false;
            }
            else {
                //tcout << _T("Unknown token string: ") << tokenStr.GetString() << endl;
                pNewToken = new CCDAssemblyToken(tokenStr, CDTOKEN_TYPE_UNKNOWN);
            }

            // Is offset string?
            bOffsetToken = tokenStr.CompareNoCase(CD_ASSEMBLYFILE_OFFSET) == 0;
        }

        try {
            Add(pNewToken);
        }
        catch (CMemoryException&) {
            tcout << _T("CCDAssemblyLine: failed to add token from line: ") << m_lineStr.GetString() << endl;
            ASSERT(false);
            return false;
        }
    }
    return true;
}


//
// Compose hash string of this line.
//
CString CCDAssemblyLine::composeLineHashKey()
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
// CCDAssemblyLines *
//*******************

void CCDAssemblyLines::cleanup()
{
    for (int i = 0; i < GetSize(); ++i)
        delete GetAt(i);

    RemoveAll();
}
//---------------------------------------------------------------------------
// File:
//      BFStrPser.cpp BFStrPser.hpp
//
// Module:
//      CBFStrParser
//
// History:
//		Jan. 15, 2004		Created by Benjamin Fung
//---------------------------------------------------------------------------

#include "BFPch.h"

#if !defined(BFSTRPSER_H)
	#include "BFStrPser.h"
#endif

//--------------------------------------------------------------------
//--------------------------------------------------------------------
CBFStrParser::CBFStrParser(LPCTSTR str, TCHAR delimeter)
    : m_str(str), m_delimeter(delimeter), m_idx(-1)
{
}

CBFStrParser::~CBFStrParser()
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
bool CBFStrParser::getNext(CString& s)
{
    s.Empty();
    if (m_idx > m_str.GetLength() - 1)
        return false;
    
    int delPos = m_str.Find(m_delimeter, m_idx + 1);
    if (delPos > -1) {        
        s = m_str.Mid(m_idx + 1, delPos - m_idx - 1);
        m_idx = delPos;
    }
    else {        
        s = m_str.Mid(m_idx + 1);
        m_idx = m_str.GetLength();
    }    
    return true;
}
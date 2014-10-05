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
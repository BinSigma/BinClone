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
#include "CSIndexedToken.h"

//******************
// CCSIndexedToken *
//******************

CCSIndexedToken::CCSIndexedToken(LPCTSTR tokStr, int dbFileID, int rawLineNum)
    : m_tokStr(tokStr),
      m_dbFileID(dbFileID),
      m_rawLineNum(rawLineNum)
{
}


CCSIndexedToken::~CCSIndexedToken()
{
}


//*******************
// CCSIndexedTokens *
//*******************

bool CCSIndexedTokens::addIndexedToken(CCSIndexedToken* pIndexedToken)
{
    try {
        Add(pIndexedToken);
        return true;
    }
    catch (CMemoryException&) {
        tcout << _T("CCSIndexedTokens: failed to add indexed token: ") << pIndexedToken->getTokStr().GetString() << endl;
        ASSERT(false);
        return false;
    }    
}

void CCSIndexedTokens::cleanup()
{
    for (int i = 0; i < GetSize(); ++i)
        delete GetAt(i);

    RemoveAll();
}
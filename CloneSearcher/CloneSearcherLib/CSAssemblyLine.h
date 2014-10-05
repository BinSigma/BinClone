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

//-----------------------------------------------------
// Objective:   Model a line in an assembly file.
//
// History:     2012/10/7 - B. Fung (Created)
// 
//-----------------------------------------------------
#pragma once

#include "CSAssemblyToken.h"

class CCSAssemblyLine : public CCSAssemblyTokens
{
public:
    CCSAssemblyLine(int asmFileLineIdx, LPCTSTR lineStr);
    virtual ~CCSAssemblyLine();
    const CString& getLineStr() const { return m_lineStr; };
    bool addTokens();
    CString composeLineHashKey();
    inline int getAsmFileLineIdx() const { return m_asmFileLineIdx; };

    bool isBeginProcedure() const { return GetSize() >= 3 && GetAt(1)->getTokenStr().CompareNoCase(_T("proc")) == 0 && (GetAt(2)->getTokenStr().CompareNoCase(_T("near")) == 0 || GetAt(2)->getTokenStr().CompareNoCase(_T("far")) == 0); } 
    bool isEndProcedure() const { return GetSize() >= 2 && GetAt(1)->getTokenStr().CompareNoCase(_T("endp")) == 0; };
    bool isMnemonicStatement() const { return GetSize() >= 1 && GetAt(0)->getTokenType() == CSTOKEN_TYPE_MNEMONIC; };

private:
    CString m_lineStr;
    int m_asmFileLineIdx;
};


typedef CTypedPtrArray<CPtrArray, CCSAssemblyLine*> CCSAssemblyLineArray;
class CCSAssemblyLines : public CCSAssemblyLineArray
{
public:
    CCSAssemblyLines() {};
    virtual ~CCSAssemblyLines() {};    
    void cleanup();
};
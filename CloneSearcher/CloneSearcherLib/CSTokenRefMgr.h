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


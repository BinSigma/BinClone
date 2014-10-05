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
#include "CSAssemblyFunction.h"


//**********************
// CCSAssemblyFunction *
//**********************

CCSAssemblyFunction::CCSAssemblyFunction(CCSAssemblyFile* pAssemblyFile, int startIdx, int endIdx, int startRawIdx, int endRawIdx)
    : m_dbFcnID(-1), m_pAssemblyFile(pAssemblyFile), m_startIdx(startIdx), m_endIdx(endIdx), m_startRawIdx(startRawIdx), m_endRawIdx(endRawIdx), m_nRegions(0), m_hashValue(0)
{
}

CCSAssemblyFunction::~CCSAssemblyFunction()
{
}


//***********************
// CCSAssemblyFunctions *
//***********************

bool CCSAssemblyFunctions::addFunction(CCSAssemblyFile* pAssemblyFile, int startIdx, int endIdx, int startRawIdx, int endRawIdx)
{
    try {
        Add(new CCSAssemblyFunction(pAssemblyFile, startIdx, endIdx, startRawIdx, endRawIdx));
        return true;
    }
    catch (CMemoryException&) {
        tcout << _T("CCSAssemblyFunctions: failed to add function.") <<  endl;
        ASSERT(false);
        return false;
    } 
}

void CCSAssemblyFunctions::cleanup()
{
    for (int i = 0; i < GetSize(); ++i)
        delete GetAt(i);

    RemoveAll();
}
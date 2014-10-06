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
#include "CSParam.h"

//***********
// CCSParam *
//***********

CCSParam::CCSParam(TCSRegNormalizeLevel regNormLevel, int windowSize, int stride)
    : m_dbParamID(-1), 
      m_regNormLevel(regNormLevel),
      m_windowSize(windowSize), 
      m_stride(stride)
{
}


CCSParam::~CCSParam()
{
}

//************
// CCSParams *
//************

void CCSParams::cleanup()
{
    for (int i = 0; i < GetSize(); ++i)
        delete GetAt(i);

    RemoveAll();
}

bool CCSParams::addParam(CCSParam* pParam)
{
    try {
        Add(pParam);
        return true;
    }
    catch (CMemoryException&) {
        tcout << _T("CCSParams: failed to add param with dbParamID: ") << pParam->m_dbParamID << endl;
        ASSERT(false);
        return false;
    }    
}


const CCSParam* CCSParams::getParamBydbParamID(int dbParamID) const
{
    for (int i = 0; i < GetSize(); ++i)
        if (GetAt(i)->m_dbParamID == dbParamID)
            return GetAt(i);

    return NULL;
}
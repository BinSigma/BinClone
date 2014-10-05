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
// Objective:   Model a parameter
//
// History:     2012/10/6 - B. Fung (Created)
// 
//-----------------------------------------------------

#pragma once

class CCSParam
{
public:
    CCSParam(TCSRegNormalizeLevel regNormLevel, int windowSize, int stride);
    virtual ~CCSParam();

    int m_dbParamID;
    TCSRegNormalizeLevel m_regNormLevel;   
    int m_windowSize;
    int m_stride;    
};

typedef CTypedPtrArray<CPtrArray, CCSParam*> CCSParamArray;
class CCSParams : public CCSParamArray
{
public:
    CCSParams() {};
    virtual ~CCSParams() {};
    void cleanup();
    bool addParam(CCSParam* pParam);
    const CCSParam* getParamBydbParamID(int dbParamID) const;
};

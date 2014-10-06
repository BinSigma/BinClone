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

#pragma once

#include "CSRegion.h"

class CCSAssemblyFile;

class CCSAssemblyFunction
{
public:
    CCSAssemblyFunction(CCSAssemblyFile* pAssemblyFile, int startIdx, int endIdx, int startRawIdx, int endRawIdx);
    virtual ~CCSAssemblyFunction();

    inline CCSAssemblyFile* getAssemblyFile() const { return m_pAssemblyFile; };
    inline void resetNumRegions() { m_nRegions = 0; };
    inline int getNumRegions() const { return m_nRegions; };
    inline void incNumRegions() { ++m_nRegions; };

    int m_dbFcnID;          // function ID in database
    int m_startIdx;         // starting point of a function, inclusive
    int m_endIdx;           // ending point of a function, inclusive  
    int m_startRawIdx;      // starting point of a function in raw file
    int m_endRawIdx;        // ending point of a function in raw file
    
    UINT m_hashValue;       // hash value of this funciton.

private:    
    CCSAssemblyFile* m_pAssemblyFile;     // file of this region    
    int m_nRegions;         // number of regions in this file.
};


typedef CTypedPtrArray<CPtrArray, CCSAssemblyFunction*> CCSFunctionArray;
class CCSAssemblyFunctions : public CCSFunctionArray
{
public:
    CCSAssemblyFunctions() {};
    virtual ~CCSAssemblyFunctions() {};
    bool addFunction(CCSAssemblyFile* pAssemblyFile, int startIdx, int endIdx, int startRawIdx, int endRawIdx);
    void cleanup();
};
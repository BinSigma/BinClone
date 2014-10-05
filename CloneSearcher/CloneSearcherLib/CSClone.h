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
// Objective:   Model a clone
//
// History:     2012/10/6 - B. Fung (Created)
// 
//-----------------------------------------------------

#pragma once

#include "CSRegion.h"

class CCSClone
{
public:
    CCSClone(int srcRegionID, int srcFcnID, int srcFileID, LPCTSTR srcFilePath, int srcStart, int srcEnd, int srcRawStart, int srcRawEnd, int tarStart, int tarEnd, int tarRawStart, int tarRawEnd);    // src is the file in DB. tar is the target comparison file.
    virtual ~CCSClone();

    bool overlap(const CCSClone& theClone, int windowSize, double minCoOccThreshold) const;
    void print() const;

    // Note: tar is the new assembly file. src is the file in the database.
    int m_srcRegionID;
    int m_srcFcnID;
    int m_srcFileID;
    CString m_srcFilePath;      // UI 
    int m_srcStart;
    int m_srcEnd;
    int m_srcRawStart;          // UI
    int m_srcRawEnd;            // UI
    int m_tarStart;
    int m_tarEnd;
    int m_tarRawStart;          // UI
    int m_tarRawEnd;            // UI
};

typedef CTypedPtrArray<CPtrArray, CCSClone*> CCSCloneArray;
class CCSClones : public CCSCloneArray
{
public:
    CCSClones() {};
    virtual ~CCSClones() {};

    bool addClone(CCSClone* pClone);
    bool appendClones(const CCSClones& clones);
    void cleanup();

    bool makeClones(const CCSRegion& tarRegion, const CCSRegions& srcRegion); 	
    void printClones() const;
    bool sortByTarRawStart();
    bool unifyToLargestClones(int windowSize, double minCoOccThreshold);

protected:
    bool quickSort(int left, int right);
    int getNumericValue(int idx);
    bool findPivot(int left, int right, int& pivot);
    int partition(int left, int right, int pivot);
    void swapEntry(int idxA, int idxB);    
};

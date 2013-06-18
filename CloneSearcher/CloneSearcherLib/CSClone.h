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

    bool overlap(const CCSClone& theClone) const;
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
	//mm
	bool mymakeClones(const CCSRegion& tarRegion, const CCSRegion* srcRegions);    
    void printClones() const;
    bool sortByTarRawStart();
    bool unifyToLargestClones();

protected:
    bool quickSort(int left, int right);
    int getNumericValue(int idx);
    bool findPivot(int left, int right, int& pivot);
    int partition(int left, int right, int pivot);
    void swapEntry(int idxA, int idxB);    
};

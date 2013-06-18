#include "StdAfx.h"
#include "CSClone.h"

//***********
// CCSClone *
//***********

CCSClone::CCSClone(int srcRegionID, int srcFcnID, int srcFileID, LPCTSTR srcFilePath, int srcStart, int srcEnd, int srcRawStart, int srcRawEnd, int tarStart, int tarEnd, int tarRawStart, int tarRawEnd)
    : m_srcRegionID(srcRegionID),
      m_srcFcnID(srcFcnID),
      m_srcFileID(srcFileID), 
      m_srcFilePath(srcFilePath),
      m_srcStart(srcStart),
      m_srcEnd(srcEnd),
      m_srcRawStart(srcRawStart),
      m_srcRawEnd(srcRawEnd),
      m_tarStart(tarStart),
      m_tarEnd(tarEnd),
      m_tarRawStart(tarRawStart),
      m_tarRawEnd(tarRawEnd)
{
}


CCSClone::~CCSClone()
{
}


//
// Return true if two clones overlap; otherwise, false.
//
bool CCSClone::overlap(const CCSClone& theClone) const
{
	int offset = 0; 
	if (m_srcFileID !=theClone.m_srcFileID) // check to see if the clones are in the same file.
        return false;
	
	offset = m_tarStart - theClone.m_tarStart; //check the overlap offset. The offset should be the same.
	if (abs(offset) != abs(m_srcStart - theClone.m_srcStart))
		return false;
     
    return ((m_tarStart <= theClone.m_tarStart && m_tarEnd >= theClone.m_tarStart) || (theClone.m_tarStart <= m_tarStart && theClone.m_tarEnd >= m_tarStart)) &&
               ((m_srcStart <= theClone.m_srcStart && m_srcEnd >= theClone.m_srcStart) || (theClone.m_srcStart <= m_srcStart && theClone.m_srcEnd >= m_srcStart));
}


//
// Print the clone on the screen
//
void CCSClone::print() const
{
    tcout << _T("tarRawStart: ") << m_tarRawStart << _T(" tarRawEnd: " ) << m_tarRawEnd << " | srcFileID: " << m_srcFileID << _T("srcRawStart: ") << m_srcRawStart << _T(" srcRawEnd: ") << m_srcRawEnd << endl;
}


//************
// CCSClones *
//************

bool CCSClones::addClone(CCSClone* pClone)
{
    try {
        Add(pClone);
        return true;
    }
    catch (CMemoryException&) {
        tcout << _T("CCSClones: failed to add clone.") << endl;
        ASSERT(false);
        return false;
    }    
}


bool CCSClones::appendClones(const CCSClones& clones)
{
    try {
        Append(clones);
        return true;
    }
    catch (CMemoryException&) {
        tcout << _T("CCSClones: failed to add clones.") << endl;
        ASSERT(false);
        return false;
    }    
}

void CCSClones::cleanup()
{
    for (int i = 0; i < GetSize(); ++i)
        delete GetAt(i);

    RemoveAll();
}

//
// Make clones
//
bool CCSClones::makeClones(const CCSRegion& tarRegion, const CCSRegions& srcRegions)
{
    CCSClone* pNewClone = NULL;
    CCSRegion* pSrcRegion = NULL;
    for (int c = 0; c < srcRegions.GetSize(); ++c) {
        pSrcRegion = srcRegions.GetAt(c);
        pNewClone = new CCSClone(pSrcRegion->m_dbRegionID, 
                                 pSrcRegion->m_dbFcnID, 
                                 pSrcRegion->m_dbFileID, 
                                 _T(""),                    // do not know the path yet.
                                 pSrcRegion->m_startIdx, 
                                 pSrcRegion->m_endIdx, 
                                 pSrcRegion->m_rawStartIdx, 
                                 pSrcRegion->m_rawEndIdx, 
                                 tarRegion.m_startIdx,
                                 tarRegion.m_endIdx,
                                 tarRegion.m_rawStartIdx,
                                 tarRegion.m_rawEndIdx);
        if (!addClone(pNewClone))
            return false;
    }
    return true;
}

//mm

bool CCSClones::mymakeClones(const CCSRegion& tarRegion, const CCSRegion* srcRegion)
{
    CCSClone* pNewClone = NULL;
   // CCSRegion* pSrcRegion = NULL;
    pNewClone = new CCSClone(srcRegion->m_dbRegionID, 
                            srcRegion->m_dbFcnID, 
                            srcRegion->m_dbFileID, 
                            _T(""),                    // do not know the path yet.
                            srcRegion->m_startIdx, 
                            srcRegion->m_endIdx, 
                            srcRegion->m_rawStartIdx, 
                            srcRegion->m_rawEndIdx, 
                            tarRegion.m_startIdx,
                            tarRegion.m_endIdx,
                            tarRegion.m_rawStartIdx,
                            tarRegion.m_rawEndIdx);
	if (!addClone(pNewClone))
		return false;
    
    return true;
}


//
// Print clones on the screen.
//
void CCSClones::printClones() const
{
    for (int c = 0; c < GetSize(); ++c)
        GetAt(c)->print();
}


//
// Sort the array of clones by the raw start of target.
//
bool CCSClones::sortByTarRawStart()
{
    if (IsEmpty())
        return true;
    
    return quickSort(0, GetSize() - 1);
}


//
// Quick sort records by attrib index.
//
bool CCSClones::quickSort(int left, int right)
{
    int p = -1;
    int pivot = -1;
    if (findPivot(left, right, pivot)) {
        p = partition(left, right, pivot);
        if (!quickSort(left, p - 1)) {
            ASSERT(false);
            return false;
        }
        if (!quickSort(p, right)) {
            ASSERT(false);
            return false;
        }
    }
    return true;
}

//
//
//
int CCSClones::getNumericValue(int idx)
{
    return GetAt(idx)->m_tarRawStart;
}

//
//
//
bool CCSClones::findPivot(int left, int right, int& pivot)
{
    pivot = -1;
    // left value
    int a = getNumericValue(left);
    // middle value
    int b = getNumericValue(left + (right - left) / 2);
    // right value
    int c = getNumericValue(right);
    // order these 3 values
    orderNumbers(a, b, c);

    if (a < b) {
        pivot = b;
        return true;
    }

    if (b < c) {
        pivot = c;
        return true;
    }

    int pValue = -1, leftValue = -1;
    int p = left + 1;
    while (p <= right) {
        pValue = getNumericValue(p);
        leftValue = getNumericValue(left);
        if (pValue != leftValue) {
            if (pValue < leftValue)
                pivot = leftValue;
            else
                pivot = pValue;
            return true;
        }
        ++p;
    }
    return false;
}

//
//
//
int CCSClones::partition(int left, int right, int pivot) 
{
    while (left <= right) {
        while (getNumericValue(left) < pivot)
            ++left;

        while (getNumericValue(right) >= pivot)
            --right;

        if (left < right) {
            swapEntry(left, right);
            ++left;
            --right;
        }
    }
    return left;
}

//
//
//
void CCSClones::swapEntry(int idxA, int idxB)
{
    CCSClone* pTemp = GetAt(idxA);
    SetAt(idxA, GetAt(idxB));
    SetAt(idxB, pTemp);
}


//
// Unify smaller clones to form larger clones
//
bool CCSClones::unifyToLargestClones()
{
    CCSClone* pCloneX = NULL;
    CCSClone* pCloneY = NULL;
    for (int p = 0; p < GetSize(); ++p) {
#ifdef _CS_SHOW_PROGRESS
        _tprintf(_T("%c: pair #%d of %d.\r"), runningChars[p % CS_NUM_RUNNING_CHARS], p, GetSize() - 1);
#endif
        pCloneX = GetAt(p);

        for (int q = p + 1; q < GetSize(); ++q) {
            pCloneY = GetAt(q);
            if (pCloneX->overlap(*pCloneY)) {
                // merge X and Y

                // join tarX and tarY
                if (pCloneX->m_tarStart > pCloneY->m_tarStart) {
                    pCloneX->m_tarStart = pCloneY->m_tarStart;
                    pCloneX->m_tarRawStart = pCloneY->m_tarRawStart;
                }
                if (pCloneX->m_tarEnd < pCloneY->m_tarEnd) {
                    pCloneX->m_tarEnd = pCloneY->m_tarEnd;
                    pCloneX->m_tarRawEnd = pCloneY->m_tarRawEnd;
                }

                // join srcX and srcY
                if (pCloneX->m_srcStart > pCloneY->m_srcStart) {
                    pCloneX->m_srcStart = pCloneY->m_srcStart;
                    pCloneX->m_srcRawStart = pCloneY->m_srcRawStart;
                }
                if (pCloneX->m_srcEnd < pCloneY->m_srcEnd) {
                    pCloneX->m_srcEnd = pCloneY->m_srcEnd;
                    pCloneX->m_srcRawEnd = pCloneY->m_srcRawEnd;
                }

                // Now delete clone pair CD
                RemoveAt(q--);     // decrement the index; otherwise, q will be off by one element.
                delete pCloneY;
                pCloneY = NULL;
            }
        }
    }
    return true;
}
#include "StdAfx.h"
#include "CDCloneMgr.h"


CCDCloneMgr::CCDCloneMgr(CCDRegionMgr* pRegionMgr, int windowSize, double oThreshold, double minCoOccThreshold, bool bInFileClone)
    : m_pRegionMgr(pRegionMgr), m_windowSize(windowSize), m_oThreshold(oThreshold), m_minCoOccThreshold(minCoOccThreshold), m_bInFileClone(bInFileClone), m_nExactClonesPairs(0)
{
    m_pExactCloneClusters = pRegionMgr->getExactCloneClusters();
}

CCDCloneMgr::~CCDCloneMgr()
{
    m_cloneClusters.cleanup();
    m_clonePairsMap.cleanup();
}


//
// Find exact clones.
//
bool CCDCloneMgr::findExactClones()
{
    // for each exact clone
    UINT hashKey = 0;
    CCDRegions* pRegions = NULL;
    CCDCloneCluster* pCloneCluster = NULL;
    POSITION pos = m_pExactCloneClusters->GetStartPosition();
    while (pos)  {
        m_pExactCloneClusters->GetNextAssoc(pos, hashKey, pRegions);
        pCloneCluster = new CCDCloneCluster();
        for (int r = 0; r < pRegions->GetSize(); ++r)
            pCloneCluster->Add(pRegions->GetAt(r)); // already sorted by files, functions, and regions

        m_cloneClusters.Add(pCloneCluster);
    }

    // free the memory.
    m_pExactCloneClusters->cleanup();
    tcout << _T("Number of exact clone clusters: ") << m_cloneClusters.GetSize() << endl;
    return true;
}


//
// Remove trivial clones. Two consecutive regions may be considered as 
// clones, so remove them.
//
bool CCDCloneMgr::removeTrivialClones()
{
    CCDCloneCluster* pCloneCluster = NULL;
    CCDAssemblyFunction* pFcn = NULL;
    CCDRegion* pRegion = NULL;

    // for each clone cluster
    for (int ccIdx = 0; ccIdx < m_cloneClusters.GetSize(); ++ccIdx) {
        pCloneCluster = m_cloneClusters.GetAt(ccIdx);

        // temporary array for storing updated clone cluster
        CCDRegions updatedClusters;
        
        // for each function containing the clone region
        for (int crIdx = 0; crIdx < pCloneCluster->GetSize(); ++crIdx) {
            pFcn = pCloneCluster->GetAt(crIdx)->getFunction(); // function containing the region

            // find regions from pFcn in pCloneCluster
            CCDRegions regionsInSameFunction;
            if (!pCloneCluster->findRegionsInSameFunction(pFcn, regionsInSameFunction))
                return false;

            // for each region in the function (assume already sorted by startIdx)
            int lastStartIdx = 0;
            for (int rIdx = 0; rIdx < regionsInSameFunction.GetSize(); ++rIdx) {
                //pRegion = pFcn->getRegion(rIdx);
                pRegion = regionsInSameFunction.GetAt(rIdx);
                if (rIdx == 0 || pRegion->m_startIdx >= lastStartIdx + m_windowSize * m_oThreshold)
                    if (!updatedClusters.addIfNew(pRegion))
                        return false;

                lastStartIdx = pRegion->m_startIdx;
            }
        }

        pCloneCluster->RemoveAll();
        if (updatedClusters.GetSize() >= 2)
            pCloneCluster->Append(updatedClusters);
    }

    if (!m_cloneClusters.removeSingletons())
        return false;

    tcout << _T("Number of clone clusters after removing trivial clones: ") << m_cloneClusters.GetSize() << endl;
    return true;
}


//
// Convert clone clusters to clone pairs
//
bool CCDCloneMgr::convertToClonePairs()
{
    CCDRegion* pRegionA = NULL;    
    CCDRegion* pRegionB = NULL;
    CString filePathA, filePathB;
    CCDCloneCluster* pCloneCluster = NULL;

    // For each clone cluster
    for (int ccIdx = 0; ccIdx < m_cloneClusters.GetSize(); ++ccIdx) {
        pCloneCluster = m_cloneClusters.GetAt(ccIdx);
#ifdef _CD_SHOW_PROGRESS
        _tprintf(_T("%c: clone cluster #%d of %d has %d regions.\r"), runningChars[ccIdx % CD_NUM_RUNNING_CHARS], ccIdx, m_cloneClusters.GetSize() - 1, pCloneCluster->GetSize());
#endif

        // for each region i
        for (int i = 0; i < pCloneCluster->GetSize(); ++i) {
            pRegionA = pCloneCluster->GetAt(i);
            filePathA = pRegionA->getFunction()->getAssemblyFile()->getFilePath();
                        
            // for each region after i
            for (int j = i + 1; j < pCloneCluster->GetSize(); ++j) {
                pRegionB = pCloneCluster->GetAt(j);
                filePathB = pRegionB->getFunction()->getAssemblyFile()->getFilePath();

                // skip infile clones
                if (!m_bInFileClone && pRegionA->getFunction()->getAssemblyFile() == pRegionB->getFunction()->getAssemblyFile())
                    continue;

                // This map associates the pairs of filePaths with an array of clone pairs
                if (!m_clonePairsMap.addPairToMap(filePathA, filePathB, pRegionA, pRegionB))
                    return false;
            }
        }
    }

    // Sort all clone pairs by startIdx of Region A and remove the duplicates
    if (!m_clonePairsMap.sortAndRemoveDuplicates(m_nExactClonesPairs))
        return false;
    tcout << _T("Number of exact clone pairs: ") << m_nExactClonesPairs << endl;

    // free some memory space
    m_cloneClusters.cleanup();
    return true;
}


//
// Find inexact clones
// 
bool CCDCloneMgr::findInexactClones()
{
    CCDAssemblyFile* pFile = NULL;
    CCDAssemblyFunction* pFcn = NULL;
    CCDRegion* pRegion = NULL;
    m_nFeatures = m_pRegionMgr->getNumGlobalFeatures();
    srand((unsigned)time(NULL));

    
    if (!m_pRegionMgr->getAllRegions(m_allRegions)) {
        ASSERT(false);
        return false;
    }

    int scoresSize = m_allRegions.GetSize();
   
    m_scores.SetSize(scoresSize);
     
    m_medianAmounts.SetSize(m_nFeatures); 

    m_medianNZCNT = 0;
    for (int i = 0; i < m_nFeatures; i++) {   // find out how many features has medians more than 0
        m_medianAmounts[i] = median(i);
        if (m_medianAmounts[i] != 0)
            ++m_medianNZCNT;
    }
   
    m_medianNZ.SetSize(m_medianNZCNT);

    int j= 0 ;
    for (int i = 0; i< m_nFeatures; i++) {  // store the features index with median more than 0
        if (m_medianAmounts[i] !=0) {
            m_medianNZ[j]= i;
            j++;
        }
    }

    if (m_pRegionMgr->getInexactMethod() == CD_INEXACT_METHOD_RANDOM) 
        randomInexactCloneFinder();
    
    else if (m_pRegionMgr->getInexactMethod() == CD_INEXACT_METHOD_SLIDING) 
        slidingInexactCloneFinder();
       
    else if (m_pRegionMgr->getInexactMethod() == CD_INEXACT_METHOD_TWOCOMBINATION) 
        twoCombinationInexactCloneFinder();

    else { 
    tcout << _T("Error: inexactMethod(): unknown inexactMethod ")  << endl;
            ASSERT(false);
            return _T("");
    }
     // free some memory space
    m_pRegionMgr->getInexactHashTables()->cleanup();

    // Sort all clone pairs by startIdx of Region A and remove the duplicates
    int nTotalClonesPairs = 0;
    if (!m_clonePairsMap.sortAndRemoveDuplicates(nTotalClonesPairs))
        return false;

    tcout << _T("Number of inexact clone pairs: ") << nTotalClonesPairs - m_nExactClonesPairs << endl;
    tcout << _T("Total number of clone pairs: ") << nTotalClonesPairs << endl;
    tcout.flush();
    return true;  
}
  
//
// First of all, the features with median equal to zero are filtered out. Then, with a random function 5 features are chosen
// these 5 features are compared with their medians, the key is created for each 5 features and the associated hash table is filled with all regions.
// The above algorithm is iterated to "DefinedHashTableRatio" of global features list size.
//
bool CCDCloneMgr::randomInexactCloneFinder()
{
    int nKeyVectors = (int) (m_nFeatures * m_pRegionMgr->getDefinedHashTableRatio()); // number of hash tables (number of key vectors) we want to use is "getDefinedHashTableRatio()" percent of the size of all of features
       
    int dims[] = {nKeyVectors, nKeyVectors};
    CCDMDIntArray med(sizeof(dims) / sizeof(int), dims);
    CCDMDIntArray randomVector(sizeof(dims) / sizeof(int), dims);
    for (int i = 0; i < nKeyVectors; ++i) {
        for (int j = 0; j < nKeyVectors; ++j) {
            med[i][j] = 0;
            randomVector[i][j] = 0;
        }
    }       

    CCDBoolArray alreadyChosenFlags;
    alreadyChosenFlags.SetSize(m_nFeatures);

    CCDIntArray medianMap;    
    medianMap.SetSize(m_nFeatures);
    for (int i = 0; i < m_nFeatures; i++)
    medianMap [i] = -1;
        
    int myRandIdx = 0; 
    int myRand = 0;
    for (int i = 0 ; i < nKeyVectors; i++) {
        for (int k = 0; k < m_nFeatures; k++)
            alreadyChosenFlags[k] = false;
        for (int j = 0; j < m_pRegionMgr->getKeyVectorsSize(); j++) {
            myRandIdx = rand() % m_medianNZCNT;
            myRand = m_medianNZ[myRandIdx];    // random function only chooses important features (features with median more than 0)
            if (!alreadyChosenFlags[myRand]) {
                alreadyChosenFlags[myRand] = true;
                randomVector[i][j] = myRand;
                if (medianMap[myRand] != -1)
                    med[i][j] = medianMap[myRand];  // if the median is already calculated, do not do the calculation again
                else {
                    med[i][j] = median(randomVector[i][j]);  // calculate the median of the chosen features
                    medianMap[myRand] = med[i][j];
                }
            }
            else 
                j--;
        }
    }            
    // add inexact hash tables 
    for (int i = 0; i < nKeyVectors; i++){         
        CCDRegionsMap* pNewHashTable = new CCDRegionsMap(); 
        if (!m_pRegionMgr->getInexactHashTables()->addRegionMap(pNewHashTable)) {
            delete pNewHashTable;
            pNewHashTable = NULL;
            return false;
        }
    }
              
        // For each file
    for (int fIdx = 0; fIdx < m_pRegionMgr->getAssemblyFileMgr()->getNumAssemblyFiles(); ++fIdx) {
        CCDAssemblyFile* pFile = m_pRegionMgr->getAssemblyFileMgr()->getAssemblyFile(fIdx);

            // For each function
        for (int fcnIdx = 0; fcnIdx < pFile->getNumFunctions(); ++fcnIdx) {
            CCDAssemblyFunction* pFcn = pFile->getFunctions()->GetAt(fcnIdx);   
        
                // For each region
            for (int rIdx = 0; rIdx < pFcn->getNumRegions(); ++rIdx) {
                CCDRegion* pRegion = pFcn->getRegion(rIdx);
                for (int k = 0; k < m_allRegions.GetSize(); k++)
                    m_scores[k]= 0;                  
                                                
                for (int i = 0; i < nKeyVectors; i++) {  // put each region to a table
                    UINT key = 0;
                    for ( int j = 0; j < m_pRegionMgr->getKeyVectorsSize(); j++) {    
                        if (pRegion->getVector().GetSize() > randomVector[i][j] &&  pRegion->getVector().GetAt(randomVector[i][j]) >= med[i][j])
                            key += (UINT) pow(2, (double)j);   // convert the key vector elements to decimal numbers
                    }                                                  
                    m_pRegionMgr->getInexactHashTable(i)->addRegion(key,pRegion);  // add the region to the table.
                         
                    CCDRegions* pRegions = NULL;
	                CCDRegionsMap::CPair *pCurVal;
                    pCurVal = NULL;
                    pCurVal = m_pRegionMgr->getInexactHashTable(i)->PLookup(key) ; 
                    pRegions = pCurVal->value;  // find all regions with the same key with present region
                                                   
                    for (int i = 0; i < pRegions->GetSize() - 1 ; i++)
                        ++m_scores[pRegions->GetAt(i)->m_regionID];    // add the scores of prsenet region with all other regions with the same key                      
                }

                CCDRegion* pRegionA = pRegion;    
                CCDRegion* pRegionB = NULL;
                CString filePathA, filePathB;
                // find clones on the fly !                                                     
                for (int i =0; i < m_allRegions.GetSize() ; i++){
                    if ( m_scores[i] >= nKeyVectors *  m_minCoOccThreshold) { // Pass threshold?
                        pRegionB = m_allRegions[i]; // passed the treshhold
                        if (m_bInFileClone || !(pRegionA->getFunction()->getAssemblyFile()->getFilePath() == pRegionB->getFunction()->getAssemblyFile()->getFilePath())  ){  // skip infile clones if m_bInFileClone is false
                            if (pRegionA->getFunction() != pRegionB->getFunction() || abs(pRegionA->m_startIdx - pRegionB->m_startIdx) >= m_windowSize* m_oThreshold) {
					            filePathA = pRegionA->getFunction()->getAssemblyFile()->getFilePath();
                                filePathB = pRegionB->getFunction()->getAssemblyFile()->getFilePath();
                                if ( !m_clonePairsMap.addPairToMap(filePathA, filePathB, pRegionA, pRegionB))
                                    return false;
                            }
                        }
                    }
                }
            }
        }       
    }         
return true;
}

//
// First of all, the features with median equal to zero are filtered out. Then, with  window size of 5, the first 5 features are chosen.
// These 5 features are compared with their medians, the key is created for each 5 features and the associated hash table is filled with all regions.
// Next, the window is slided  with step size 1 and the other 5 features are collected.
// The above steps are iterated to the last 5 features.
//
bool CCDCloneMgr::slidingInexactCloneFinder()
{
    int nKeyVectors = m_medianNZCNT- m_pRegionMgr->getKeyVectorsSize() + 1;  // number of all possible sliding windows

    int dims[] = {nKeyVectors, nKeyVectors};
    CCDMDIntArray med(sizeof(dims) / sizeof(int), dims);
    CCDMDIntArray randomVector(sizeof(dims) / sizeof(int), dims);
    for (int i = 0; i < nKeyVectors; ++i) {
        for (int j = 0; j < nKeyVectors; ++j) {
            med[i][j] = 0;
            randomVector[i][j] = 0;
        }
    }

    for (int i = 0 ; i < nKeyVectors ; i++){  // compute all sliding windows
        for (int j = 0; j <m_pRegionMgr->getKeyVectorsSize(); j++){
            med[i][j] = m_medianAmounts[m_medianNZ[i+j]];
        }
    }
    
    // add inexact hash tables 
    for (int i = 0; i < nKeyVectors; i++){         
        CCDRegionsMap* pNewHashTable = new CCDRegionsMap(); 
        if (!m_pRegionMgr->getInexactHashTables()->addRegionMap(pNewHashTable)) {
            delete pNewHashTable;
            pNewHashTable = NULL;
            return false;
        }
    }        

    // For each file
    for (int fIdx = 0; fIdx < m_pRegionMgr->getAssemblyFileMgr()->getNumAssemblyFiles(); ++fIdx) {
        CCDAssemblyFile* pFile = m_pRegionMgr->getAssemblyFileMgr()->getAssemblyFile(fIdx);

         // For each function
        for (int fcnIdx = 0; fcnIdx < pFile->getNumFunctions(); ++fcnIdx) {
            CCDAssemblyFunction* pFcn = pFile->getFunctions()->GetAt(fcnIdx);   
        
            // For each region
            for (int rIdx = 0; rIdx < pFcn->getNumRegions(); ++rIdx) {
                CCDRegion* pRegion = pFcn->getRegion(rIdx);
                for (int k = 0; k < m_allRegions.GetSize(); k++)
                    m_scores[k]= 0;                  
                       
                for (int i = 0; i < nKeyVectors; i++) {  // put each region to a table
                    UINT key = 0;
                    for ( int j = 0; j < m_pRegionMgr->getKeyVectorsSize(); j++) {
                        if (pRegion->getVector().GetSize() > m_medianNZ[i+j] && pRegion->getVector().GetAt(m_medianNZ[i+j]) >= med[i][j])
                            key += (UINT) pow(2, (double)j);                            
                    }      
                    m_pRegionMgr->getInexactHashTable(i)->addRegion(key,pRegion);  // add the region to the table.
                         
                    CCDRegions* pRegions = NULL;
	                CCDRegionsMap::CPair *pCurVal;
                    pCurVal = NULL;
                    pCurVal = m_pRegionMgr->getInexactHashTable(i)->PLookup(key) ; 
                    pRegions = pCurVal->value;  // find all regions with the same key with present region
                                                   
                    for (int i = 0; i < pRegions->GetSize() - 1 ; i++)
                        ++m_scores[pRegions->GetAt(i)->m_regionID];    // add the scores of prsenet region with all other regions with the same key                      
                }

                CCDRegion* pRegionA = pRegion;    
                CCDRegion* pRegionB = NULL;
                CString filePathA, filePathB;
                // find clones on fly !                                                     
                for (int i =0; i < m_allRegions.GetSize() ; i++){
                    if ( m_scores[i] >= nKeyVectors *  m_minCoOccThreshold) { // Pass threshold?
                        pRegionB = m_allRegions[i]; // passed the treshhold
                        if (m_bInFileClone || !(pRegionA->getFunction()->getAssemblyFile()->getFilePath() == pRegionB->getFunction()->getAssemblyFile()->getFilePath())  ){  // skip infile clones if m_bInFileClone is false
                            if (pRegionA->getFunction() != pRegionB->getFunction() || abs(pRegionA->m_startIdx - pRegionB->m_startIdx) >= m_windowSize* m_oThreshold) {
					            filePathA = pRegionA->getFunction()->getAssemblyFile()->getFilePath();
                                filePathB = pRegionB->getFunction()->getAssemblyFile()->getFilePath();
                                if ( !m_clonePairsMap.addPairToMap(filePathA, filePathB, pRegionA, pRegionB))
                                    return false;
                            }
                        }
                    }
                }
            }
        }       
    }  
return true;
}

//
// First of all, the features with median equal to zero are filtered out. Then, all possible 2- combination of these features are constructed 
// These combiantions are compared with their medians and the key is created for each combination. The associated hash tables are filled with all regions for each combination.
//
bool CCDCloneMgr::twoCombinationInexactCloneFinder()
{
    int nKeyVectors = (m_medianNZCNT) * (m_medianNZCNT-1) / 2;  // number of all possible 2-combination of important features 

    int dims[] = {nKeyVectors, nKeyVectors};
    CCDMDIntArray med(sizeof(dims) / sizeof(int), dims);
    CCDMDIntArray indexVector(sizeof(dims) / sizeof(int), dims);
    for (int i = 0; i < nKeyVectors; ++i) {
        for (int j = 0; j < 2; ++j) {
            med[i][j] = 0;
            indexVector[i][j] = 0;
        }
    }
  
    nKeyVectors = 0;
    for (int i = 0; i < m_medianNZCNT - 1; i++) {  // compute all 2-combination of important features
        for(int j = i+1 ; j < m_medianNZCNT ; j++) {
            med[nKeyVectors][0] = m_medianAmounts[m_medianNZ[i]];
            indexVector[nKeyVectors][0] = m_medianNZ[i];
            med[nKeyVectors][1] = m_medianAmounts[m_medianNZ[j]];
            indexVector[nKeyVectors][1] = m_medianNZ[j];
            ++nKeyVectors;
        }
    } 

    // add inexact hash tables 
    for (int i = 0; i < nKeyVectors; i++) {         
        CCDRegionsMap* pNewHashTable = new CCDRegionsMap(); 
        if (!m_pRegionMgr->getInexactHashTables()->addRegionMap(pNewHashTable)) {
            delete pNewHashTable;
            pNewHashTable = NULL;
            return false;
        }
    }         

    // For each file
    for (int fIdx = 0; fIdx < m_pRegionMgr->getAssemblyFileMgr()->getNumAssemblyFiles(); ++fIdx) {
        CCDAssemblyFile* pFile = m_pRegionMgr->getAssemblyFileMgr()->getAssemblyFile(fIdx);

        // For each function
        for (int fcnIdx = 0; fcnIdx < pFile->getNumFunctions(); ++fcnIdx) {
            CCDAssemblyFunction* pFcn = pFile->getFunctions()->GetAt(fcnIdx);   
        
            // For each region
            for (int rIdx = 0; rIdx < pFcn->getNumRegions(); ++rIdx) {
                CCDRegion* pRegion = pFcn->getRegion(rIdx);
                for (int k = 0; k < m_allRegions.GetSize(); k++)
                m_scores[k]= 0; 
                  
                for (int i = 0; i < nKeyVectors; i++) { 
                    UINT key = 0;
                    for ( int j = 0; j < 2; j++) {        
                        if (pRegion->getVector().GetSize() > indexVector[i][j] &&  pRegion->getVector().GetAt(indexVector[i][j]) >= med[i][j])
                            key += (UINT) pow(2, (double)j);                            
                    } 
                    m_pRegionMgr->getInexactHashTable(i)->addRegion(key,pRegion);  // add the region to the table.
                         
                    CCDRegions* pRegions = NULL;
	                CCDRegionsMap::CPair *pCurVal;
                    pCurVal = NULL;
                    pCurVal = m_pRegionMgr->getInexactHashTable(i)->PLookup(key) ; 
                    pRegions = pCurVal->value;  // find all regions with the same key with present region
                                                   
                    for (int i = 0; i < pRegions->GetSize() - 1 ; i++)
                        ++m_scores[pRegions->GetAt(i)->m_regionID];    // add the scores of prsenet region with all other regions with the same key                      
                }

                CCDRegion* pRegionA = pRegion;    
                CCDRegion* pRegionB = NULL;
                CString filePathA, filePathB;
                // find clones on the fly!                                                     
                for (int i =0; i < m_allRegions.GetSize(); i++){
                    if ( m_scores[i] >= nKeyVectors *  m_minCoOccThreshold) { // Pass threshold?
                        pRegionB = m_allRegions[i]; // passed the threshold
                        if (m_bInFileClone || !(pRegionA->getFunction()->getAssemblyFile()->getFilePath() == pRegionB->getFunction()->getAssemblyFile()->getFilePath())  ){  // skip infile clones if m_bInFileClone is false
                            if (pRegionA->getFunction() != pRegionB->getFunction() || abs(pRegionA->m_startIdx - pRegionB->m_startIdx) >= m_windowSize* m_oThreshold) {
					            filePathA = pRegionA->getFunction()->getAssemblyFile()->getFilePath();
                                filePathB = pRegionB->getFunction()->getAssemblyFile()->getFilePath();
                                if (!m_clonePairsMap.addPairToMap(filePathA, filePathB, pRegionA, pRegionB))
                                    return false;
                            }
                        }
                    }
                }
            }
        }       
    }
    return true;
}


//
// Unify smaller clones to form larger clones
//
bool CCDCloneMgr::unifyToLargestClones()
{
    int nClonePairs = 0;
    CString key;
    CCDRegion* pRegionA = NULL;
    CCDRegion* pRegionB = NULL;
    CCDRegion* pRegionC = NULL;
    CCDRegion* pRegionD = NULL;
    CCDClonePair* pClonePairCD = NULL;
    CCDClonePairs* pClonePairs = NULL;    

    // For each array of pairs
    int cfpIdx = -1;
    POSITION pos = m_clonePairsMap.GetStartPosition();
    while (pos)  {
        m_clonePairsMap.GetNextAssoc(pos, key, pClonePairs);    
        if (pClonePairs->IsEmpty()) {
            tcout << _T("Warning: CCDCloneMgr::unifyToLargestClones - empty clone pairs.") << endl;
            ASSERT(false);
            continue;
        }

        ++cfpIdx;
        for (int p = 0; p < pClonePairs->GetSize(); ++p) {
#ifdef _CD_SHOW_PROGRESS
            _tprintf(_T("%c: clone file pairs #%d of %d: pair #%d of %d.\r"), runningChars[p % CD_NUM_RUNNING_CHARS], cfpIdx, m_clonePairsMap.GetSize() - 1, p, pClonePairs->GetSize());
#endif
            pRegionA = pClonePairs->GetAt(p)->getRegionA();
            pRegionB = pClonePairs->GetAt(p)->getRegionB();

            for (int q = p + 1; q < pClonePairs->GetSize(); ++q) {
                pClonePairCD = pClonePairs->GetAt(q);
                pRegionC = pClonePairCD->getRegionA();
                pRegionD = pClonePairCD->getRegionB();

                if (pRegionA->getFunction()->getAssemblyFile() != pRegionC->getFunction()->getAssemblyFile() ||
                    pRegionB->getFunction()->getAssemblyFile() != pRegionD->getFunction()->getAssemblyFile()) {
                    tcout << _T("Error: CCDCloneMgr::unifyToLargestClones - inconsistent regions arrangement in pair in file.") << endl;
                    ASSERT(false);
                    return false;
                }

                if (pRegionA->overlap(*pRegionC) && pRegionB->overlap(*pRegionD)) {
                    // merge AB and CD

                    // join A and C
                    if (pRegionA->m_startIdx > pRegionC->m_startIdx) {                    
                        pRegionA->m_startIdx = pRegionC->m_startIdx;
                        pRegionA->m_rawStartIdx = pRegionC->m_rawStartIdx;
                    }
                    if (pRegionA->m_endIdx < pRegionC->m_endIdx) {
                        pRegionA->m_endIdx = pRegionC->m_endIdx;
                        pRegionA->m_rawEndIdx = pRegionC->m_rawEndIdx;
                    }

                    // join B and D
                    if (pRegionB->m_startIdx > pRegionD->m_startIdx) {
                        pRegionB->m_startIdx = pRegionD->m_startIdx;
                        pRegionB->m_rawStartIdx = pRegionD->m_rawStartIdx;
                    }
                    if (pRegionB->m_endIdx < pRegionD->m_endIdx) {
                        pRegionB->m_endIdx = pRegionD->m_endIdx;
                        pRegionB->m_rawEndIdx = pRegionD->m_rawEndIdx;
                    }

                    // Now delete clone pair CD
                    pClonePairs->RemoveAt(q--);     // decrement the index; otherwise, q will be off by one element.
                    delete pClonePairCD;
                    pClonePairCD = NULL;
                }
            }
        }
        nClonePairs += pClonePairs->GetSize();
    }
    tcout << _T("Number of clone pairs after unification: ") << nClonePairs << endl;
    return true;
}

//
// find median
//
int CCDCloneMgr::median(int index)
{
    CCDAssemblyFile* pFile = NULL;
    CCDAssemblyFunction* pFcn = NULL;
    CCDRegion* pRegion = NULL;    
    CCDIntArray vectorValues;

    // For each file
    for (int fIdx = 0; fIdx < m_pRegionMgr->getAssemblyFileMgr()->getNumAssemblyFiles(); ++fIdx) {
        pFile = m_pRegionMgr->getAssemblyFileMgr()->getAssemblyFile(fIdx);

        // For each function
        for (int fcnIdx = 0; fcnIdx < pFile->getNumFunctions(); ++fcnIdx) {
            pFcn = pFile->getFunctions()->GetAt(fcnIdx);   

            // For each region
            for (int rIdx = 0; rIdx < pFcn->getNumRegions(); ++rIdx) {
                pRegion = pFcn->getRegion(rIdx);

                if (pRegion->getVector().GetSize() > index + 1)
                    vectorValues.Add(pRegion->getVector().GetAt(index));
                else
                    vectorValues.Add(0);

                //  tcout << vectorValues[vectorValues.GetSize() - 1] << ", "; // for test !
            }
        }
    }
    int result = bucketSort(vectorValues, vectorValues.GetSize() / 2 + 1);   
    return result; 
}

//
// find k median : returns the kth largest number of the array A
//
int CCDCloneMgr::bucketSort(const CCDIntArray& ary, int k)
{
    int bucketSize = findLargestNumber(ary) + 1;
    CCDIntArray buckets;
    buckets.SetSize(bucketSize);
    for (int i = 0; i < buckets.GetSize(); ++i)
        buckets[i] = 0;
    for (int i = 0; i < ary.GetSize(); i ++)
        ++buckets[ary[i]];

    return bucketSortMedianFinder(buckets, k);        
}

int CCDCloneMgr::bucketSortMedianFinder(const CCDIntArray& ary, int k)
{
    int sum = 0, i = 0;
    for (i = 0; i < ary.GetSize(); i++) {
        sum += ary[i];
        if (sum > k)
            break;
    }
    return i;
}

//
// Find the larget number.
//
int CCDCloneMgr::findLargestNumber(const CCDIntArray& ary)
{
    int largest = INT_MIN;
    for (int i = 0; i < ary.GetSize(); ++i)
        if (ary[i] > largest)
            largest = ary[i];

    return largest;
}
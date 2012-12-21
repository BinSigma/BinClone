#include "StdAfx.h"
#include "CDController.h"
#include "CDRegionMgr.h"


CCDController::CCDController(LPCTSTR assemblyFolderPath,
                             LPCTSTR clonePairsFilePath,
                             int windowSize,
                             int stride,
                             int kThreshold,
                             double oThreshold,
                             bool bFindExactClones,
                             bool bFindInexactClones,
                             TCDRegNormalizeLevel regNormLevel,
                             bool bNormalizeToken,
                             int keyVectorSize,
                             double minCoOccThreshold,
                             CDInexactMethod inexactMethod) 
    : m_bFindExactClones(bFindExactClones),
      m_bFindInexactClones(bFindInexactClones),
      m_assemblyFileMgr(&m_tokRefMgr, assemblyFolderPath),
      m_regionMgr(&m_assemblyFileMgr, windowSize, stride, kThreshold, regNormLevel, bNormalizeToken, keyVectorSize, inexactMethod), 
      m_cloneMgr(&m_regionMgr, windowSize, oThreshold, minCoOccThreshold, false),
      m_outputMgr(&m_assemblyFileMgr, &m_cloneMgr, &m_tokRefMgr, clonePairsFilePath, windowSize, stride, kThreshold, oThreshold, bFindExactClones, bFindInexactClones, regNormLevel, bNormalizeToken, keyVectorSize, minCoOccThreshold, inexactMethod) 
{
}


CCDController::~CCDController()
{
}


//
// Parse the assembly files in assemblyFolderPath and 
// store the clone results to clonePairsFilePath
// Return false if error.
//
bool CCDController::detectClones()
{
    return run(NULL);
}


//
// Search the assembly files in assemblyFolderPath
// for targetFragement, and store the matched region 
// to clonePairsFilePath.
// Return false if error.
//
bool CCDController::searchFragments(LPCTSTR targetFragment, CString& tempAssemblyFilePath)
{
    if (!m_assemblyFileMgr.createAssemblyFile(targetFragment, tempAssemblyFilePath)) {
        tcout << _T("Error: create temporary assembly file.") << endl;
        tempAssemblyFilePath = _T("");
        return false;
    }    
    return run((LPCTSTR) tempAssemblyFilePath);
}


//
// Open the given assembly file and return its file index
// Return -1 if error.
//
bool CCDController::openRawAssemblyFile(LPCTSTR assemblyFilePath)
{
    return m_assemblyFileMgr.openRawAssemblyFile(assemblyFilePath);
}


//
// Retrieve one line from the raw assembly file. 
// Return false if error.
//
bool CCDController::getRawAssemblyFileLineStr(CString& str)
{
    return m_assemblyFileMgr.getRawAssemblyFileLineStr(str);
}


//
// Close the raw assembly file.
//
void CCDController::closeRawAssemblyFile()
{
    m_assemblyFileMgr.closeRawAssemblyFile();
}


//
// Run the program without GUI.
// If filterByFilePath is not NULL, then clone pairs in XML will be filtered by filterByFilePath.
//
bool CCDController::run(LPCTSTR filterByFilePath)
{    
    // Read the assembly files from the given folder path
    long startProgTime = getTime();
    if (!m_assemblyFileMgr.parseFolder()) {
        tcout << _T("Error: failed to parse folder: ") << m_assemblyFileMgr.getAssemblyFolderPath() << endl;
        return false;
    }

    // Construct functions
    if (!m_regionMgr.createFunctions()) {
        tcout << _T("Error: failed to create functions.") << endl;
        return false;
    }    

    // Construct regions.
    if (!m_regionMgr.createRegions()) {
        tcout << _T("Error: failed to create regions.") << endl;
        return false;
    }    

    // Normalize each region
    if (!m_regionMgr.normalizeRegions()) {
        tcout << _T("Error: failed to normalize regions.") << endl;
        return false;
    }    
    long endPreprocessTime = getTime();

    long startExactCloneTime = 0, startConvertExactClonesToPairTime = 0, endConvertExactClonesToPairTime = 0;
    if (m_bFindExactClones) {
        // Create hash tables for exact clones.
        startExactCloneTime = getTime();
        if (!m_regionMgr.createExactHashTables()) {
            tcout << _T("Error: failed to create exact hash tables.") << endl;
            return false;
        }

        // find exact clones   
        if (!m_cloneMgr.findExactClones()) {
            tcout << _T("Error: failed to find exact clones.") << endl;
            return false;
        }

        // Remove trivial clones        
        if (!m_cloneMgr.removeTrivialClones()) {
            tcout << _T("Error: failed to remove trivial clones.") << endl;
            return false;
        }    

        // Convert clone clusters to clone pairs
        startConvertExactClonesToPairTime = getTime();
        if (!m_cloneMgr.convertToClonePairs()) {
            tcout << _T("Error: failed to convert clone pairs.") << endl;
            return false;
        }
        endConvertExactClonesToPairTime = getTime();
    }    

    long startInexactCloneTime = 0, endInexactCloneTime = 0;
    if (m_bFindInexactClones) {
        // Extract features
        startInexactCloneTime = getTime();
        if (!m_regionMgr.regionsToVectors()) {
            tcout << _T("Error: failed to convert regions to vectors.") << endl;
            return false;
        }
#if 0
        // Create hash tables for inexact clones.
        if (!m_regionMgr.createInexactHashTables()) {
            tcout << _T("Error: failed to create inexact hash tables.") << endl;
            return false;
        }        
#endif 
        if (!m_cloneMgr.findInexactClones()) {
            tcout << _T("Error: failed to find inexact clones.") << endl;
            return false;
        }
        endInexactCloneTime = getTime();
    }

    // Unify the clones
    long startUnificationTime = getTime();
    if (!m_cloneMgr.unifyToLargestClones()) {
        tcout << _T("Error: failed to unify clones.") << endl;
        return false;
    }

    // Ouptput clone pairs to a file.
    long startWriteXMLTime = getTime();
    int filterByFileID = -1;
    if (filterByFilePath) {
        filterByFileID = m_assemblyFileMgr.getAssemblyFileID(filterByFilePath);
        if (filterByFileID == -1) {
            tcout << _T("Error: failed to identify assembly file ID: ") << filterByFilePath << endl;
            return false;
        }
    }
    if (!m_outputMgr.writeCloneInfoToXMLFile(filterByFileID)) {
        tcout << _T("Error: failed to output clones to file.") << endl;
        return false;
    }
    long endProgTime = getTime();

    tcout << _T("Number of indexed constants = ") << m_tokRefMgr.getNumConstants() << endl;
    tcout << _T("Number of indexed strings = ") << m_tokRefMgr.getNumStrings() << endl;
    tcout << _T("Number of indexed imports = ") << m_tokRefMgr.getNumImports() << endl;
    
    tcout << _T("Spent: ") << endPreprocessTime - startProgTime << _T("s on reading and preprocessing data.") << endl;    
    tcout << _T("Spent: ") << startConvertExactClonesToPairTime - startExactCloneTime << _T("s on extracting exact clones.") << endl;
    tcout << _T("Spent: ") << endConvertExactClonesToPairTime - startConvertExactClonesToPairTime << _T("s on converting exact clone clusters to clone pairs.") << endl;
    tcout << _T("Spent: ") << endInexactCloneTime - startInexactCloneTime << _T("s on extracting inexact clones.") << endl;
    tcout << _T("Spent: ") << startWriteXMLTime - startUnificationTime << _T("s on unifying clones.") << endl;
    tcout << _T("Spent: ") << endProgTime - startWriteXMLTime << _T("s on writing the XML file.") << endl;
    tcout << _T("Spent: ") << (endProgTime - startProgTime) - (endProgTime - startUnificationTime) << _T("s in total (excluding unifying clones).") << std::endl; 
    tcout << _T("Spent: ") << endProgTime - startProgTime << _T("s in total (including unifying clones).") << std::endl; 
    return true;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void CCDController::printTime()
{
    time_t ltime;
    time(&ltime);
    std::cout << _T("Current System Time = ") << (long) ltime << std::endl;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
long CCDController::getTime()
{
    time_t ltime;
    time(&ltime);
    return (long) ltime;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
long CCDController::get_runtime()
{
    clock_t start;
    start = clock();
    return((long)((double)start*100.0/(double)CLOCKS_PER_SEC));
}
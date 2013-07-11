#include "stdafx.h"
#include "CSController.h"
#include "CSParam.h"
#include <iostream>
#include <fstream>
//farhadi
CCSController::CCSController(LPCTSTR dbName, LPCTSTR dbUser, LPCTSTR dbPwd) 
    : m_dbMgr(dbName, dbUser, dbPwd),
      m_tokRefMgr(&m_dbMgr),
      m_assemblyFileMgr(&m_dbMgr, &m_tokRefMgr),
      m_pTargetAssemblyFile(NULL)
{
}


CCSController::~CCSController()
{
    delete m_pTargetAssemblyFile;
    m_pTargetAssemblyFile = NULL;
    m_allParams.cleanup();
}


//
// Parse the assembly files in assemblyFolderPath and
// convert them, and store them into the database.
//
bool CCSController::populateAssemblyToDB(LPCTSTR assemblyFolderPath,
                                         TCSRegNormalizeLevel regNormLevel,                                      
                                         int windowSize,
                                         int stride                                 
                                         )
{
    // construct a list of feature strings.
    if (!m_assemblyFileMgr.constructBasicFeatures())
        return false;

    // Store parameters to database
    CCSParam param(regNormLevel, windowSize, stride);
    if (!m_dbMgr.storeParam(param))
        return false;
    
    // Read the assembly files from the given folder path.
    // The first call is to construct the global features.
    // The second call is to do the actual feature countings.
    long startProgTime = getTime();
    if (!m_assemblyFileMgr.parseFolder(assemblyFolderPath, param, true )) { // first scan of the files to calculate the final median vector
        tcout << _T("Error: failed to parse folder: ") << assemblyFolderPath << endl;
        return false;
    }
    if (!m_assemblyFileMgr.parseFolder(assemblyFolderPath, param, false)) {  // Second Scan of the files to find clones
        tcout << _T("Error: failed to parse folder: ") << assemblyFolderPath << endl;
        return false;
    }

    long endProgTime = getTime();
    tcout << _T("Spent: ") << endProgTime - startProgTime << _T("s in total to populate the assembly files to the database.") << std::endl; 
    return true;
}


//
// Retrieve all parameters
//
bool CCSController::fetchAllParameters(CCSParams& params)
{
    m_allParams.cleanup();
    if (!m_dbMgr.fetchParams(m_allParams))
        return false;

    try {
        params.Append(m_allParams);
        return true;
    }
    catch (CMemoryException&) {
        tcout << _T("fetchAllParameters: failed to parameters.") << endl;
        ASSERT(false);
        return false;
    }
}


//
// Retrieve the filePath of a given dbFileID.
//
bool CCSController::fetchFilePath(int dbFileID, CString& assemblyFilePath)
{
    return m_dbMgr.fetchFilePath(dbFileID, assemblyFilePath);
}


//
// Open the given assembly file by file path
//
bool CCSController::openRawAssemblyFile(LPCTSTR assemblyFilePath)
{
    return m_assemblyFileMgr.openRawAssemblyFile(assemblyFilePath);
}


//
// Retrieve one line from the raw assembly file. 
// Return false if error.
//
bool CCSController::getRawAssemblyFileLineStr(CString& str)
{
    return m_assemblyFileMgr.getRawAssemblyFileLineStr(str);
}


//
// Close the raw assembly file.
//
void CCSController::closeRawAssemblyFile()
{
    m_assemblyFileMgr.closeRawAssemblyFile();
}


//
// Find all the clones from the DB
//
bool CCSController::findFileClones(LPCTSTR targetAssemblyFilePath, 
                                   const CCSParam& param,
                                   bool bFindExactClones, 
                                   bool bFindInexactClones, 
                                   double minCoOccThreshold, 
                                   const CCSClones*& pClones)
{
	
	// initialze the inexact detection minCoOccThreshold and score vector
	if (bFindInexactClones) {
		m_dbMgr.setMinCoOccThreshold(minCoOccThreshold);
		m_dbMgr.setInitialVariables(param);
	}
	// Parse the target assembly file.
    long startProgTime = getTime();

    pClones = NULL;
    delete m_pTargetAssemblyFile;
    m_pTargetAssemblyFile = new CCSAssemblyFile(targetAssemblyFilePath, &m_assemblyFileMgr);
    if (!m_pTargetAssemblyFile->parseFile(param, false))
        return false;

    // normalize code
    if (!m_pTargetAssemblyFile->normalizeCode(param))
        return false;
    long endPreprocessTime = getTime();

    // create functions in memory.
    long startCloneTime = getTime();
    if (!m_pTargetAssemblyFile->extractFunctions(&m_dbMgr, param, false))
        return false;
    tcout << _T("Number of functions: ") << m_pTargetAssemblyFile->getNumFunctions() << " in " << targetAssemblyFilePath << endl;

    // create regions.
    if (!m_pTargetAssemblyFile->extractRegions(&m_dbMgr, param, bFindExactClones, bFindInexactClones, false, false, NULL))
        return false;
    tcout << _T("Number of regions: ") << m_pTargetAssemblyFile->getNumRegions() << _T(" in ") << targetAssemblyFilePath << endl;
       
    // unify the clones
    long startUnificationTime = getTime();
    tcout << _T("Number of clones before unification: ") << m_pTargetAssemblyFile->getClones()->GetSize() << endl;
    if (!m_pTargetAssemblyFile->getClones()->unifyToLargestClones()) {
        tcout << _T("Error: failed to unify clones.") << endl;
        return false;
    }
    tcout << _T("Number of clones after unification: ") << m_pTargetAssemblyFile->getClones()->GetSize() << endl;

    // fetch the file paths    
    CCSClone* pClone = NULL;
    for (int c = 0; c < m_pTargetAssemblyFile->getClones()->GetSize(); ++c) {
        pClone = m_pTargetAssemblyFile->getClones()->GetAt(c);
        if (!m_dbMgr.fetchFilePath(pClone->m_srcFileID, pClone->m_srcFilePath))
            return false;
    }

    // Sort the clones by the target raw start number
    if (!m_pTargetAssemblyFile->getClones()->sortByTarRawStart()) {
        tcout << _T("Error: failed to sort clones.") << endl;
        return false;
    }

    // Get all the clones
    pClones = m_pTargetAssemblyFile->getClones();
    //pClones->printClones();

    long endProgTime = getTime();    
    tcout << _T("Spent: ") << endPreprocessTime - startProgTime << _T("s on reading and preprocessing the target assembly file.") << endl;    
    tcout << _T("Spent: ") << startUnificationTime - startCloneTime << _T("s on finding clones.") << endl;
    tcout << _T("Spent: ") << endProgTime - startUnificationTime << _T("s on unifying and sorting clones.") << endl;
    tcout << _T("Spent: ") << endProgTime - startProgTime << _T("s in total.") << std::endl; 
    
	
	
		//farhadi
	ofstream myfile;
	time_t now = time(0);
	char* dt = ctime(&now);
	myfile.open ("C:\\Users\\farhadi\\Desktop\\array.txt");
	myfile << "Time\n";
	myfile << "Spent: " << endPreprocessTime - startProgTime << "s on reading and preprocessing the target assembly file." << endl;    
    myfile << "Spent: " << startUnificationTime - startCloneTime << "s on finding clones." << endl;
    myfile << "Spent: " << endProgTime - startUnificationTime << "s on unifying and sorting clones." << endl;
    myfile << "Spent: " << endProgTime - startProgTime << "s in total." << std::endl; 
	myfile << "The local date and time is: " << dt << endl;

	myfile.close();

	
	return true;
}


//
// Search the DB for targetFragement.
// Return false if error.
//
bool CCSController::findFragmentClones(LPCTSTR targetFragment,
                                       const CCSParam& param,
                                       bool bFindExactClones, 
                                       bool bFindInexactClones, 
                                       double minCoOccThreshold, 
                                       const CCSClones*& pClones,
                                       CString& tempAssemblyFilePath)
{
    if (!m_assemblyFileMgr.createAssemblyFile(targetFragment, tempAssemblyFilePath)) {
        tcout << _T("Error: failed to create temporary assembly file.") << endl;
        CBFFileHelper::removeFile((LPCTSTR) tempAssemblyFilePath);
        return false;
    }  

    if (!findFileClones((LPCTSTR) tempAssemblyFilePath, param, bFindExactClones, bFindInexactClones, minCoOccThreshold, pClones)) {
        CBFFileHelper::removeFile((LPCTSTR) tempAssemblyFilePath);
        return false;
    }

    //CBFFileHelper::removeFile((LPCTSTR) tempAssemblyFilePath);
    return true;
}


//
// Search for the given string token from the DB.
//
bool CCSController::searchStrings(LPCTSTR tokStr, CCSIndexedTokens& indexedTokens)
{
    return m_dbMgr.fetchStrings(tokStr, indexedTokens);
}

//
// Search for the given constant token from the DB.
//
bool CCSController::searchConstants(LPCTSTR tokStr, CCSIndexedTokens& indexedTokens)
{
    return m_dbMgr.fetchConstants(tokStr, indexedTokens);
}

//
// Search for the given import token from the DB.
//
bool CCSController::searchImports(LPCTSTR tokStr, CCSIndexedTokens& indexedTokens)
{
    return m_dbMgr.fetchImports(tokStr, indexedTokens);
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void CCSController::printTime()
{
    time_t ltime;
    time(&ltime);
    std::cout << _T("Current System Time = ") << (long) ltime << std::endl;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
long CCSController::getTime()
{
    time_t ltime;
    time(&ltime);
    return (long) ltime;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
long CCSController::get_runtime()
{
    clock_t start;
    start = clock();
    return((long)((double)start*100.0/(double)CLOCKS_PER_SEC));
}
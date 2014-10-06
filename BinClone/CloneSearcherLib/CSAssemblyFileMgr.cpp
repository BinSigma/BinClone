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

#include "stdafx.h"
#include "CSDatabaseMgr.h"
#include "CSAssemblyFileMgr.h"
#include "CSTokenRefMgr.h"

//*****************
// CCSOperandsMap *
//*****************

//
// Retrieve index number based on the operand. Add if cannot found
//
int CCSOperandsMap::getOperandIdx(const CString& operand)
{    
    int idx = -1;
    if (!Lookup(operand, idx)) {
        // new entry
        idx = GetSize();
        SetAt(operand, idx);  
    }
    return idx;
}

//*********************
// CCSAssemblyFileMgr *
//*********************

CCSAssemblyFileMgr::CCSAssemblyFileMgr(CCSDatabaseMgr* pDBMgr, CCSTokenRefMgr* pTokenRefMgr)
    : m_nTotalFiles(0), 
      m_nTotalFunctions(0), 
      m_nTotalRegions(0), 
      m_pDBMgr(pDBMgr), 
      m_pTokenRefMgr(pTokenRefMgr),
      m_pRawAssemblyFile(NULL)
{
    ASSERT(m_pDBMgr);
    ASSERT(pTokenRefMgr);
}


CCSAssemblyFileMgr::~CCSAssemblyFileMgr()
{
}


//
// Open the raw assembly file.
// Return false if error.
//
bool CCSAssemblyFileMgr::openRawAssemblyFile(LPCTSTR assemblyFilePath)
{
    if (m_pRawAssemblyFile) {
        tcout << _T("Error: CCSAssemblyFileMgr: only one raw file can be opened at a time: ") << assemblyFilePath << endl;
        ASSERT(false);
        return false;
    }
    m_pRawAssemblyFile = new CStdioFile();
    
    try {        
        if (!m_pRawAssemblyFile->Open(assemblyFilePath, CFile::modeRead)) {
            tcout << _T("CCSAssemblyFileMgr: failed to open file: ") << assemblyFilePath << endl;
            delete m_pRawAssemblyFile;
            m_pRawAssemblyFile = NULL;
            ASSERT(false);
            return false;
        }
        return true;
    }
    catch (CFileException&) {
        tcout << _T("CCSAssemblyFileMgr: failed to read assembly file: ") << assemblyFilePath << endl;
        delete m_pRawAssemblyFile;
        m_pRawAssemblyFile = NULL;
        ASSERT(false);
        return false;
    }
}


//
// Get raw assembly file line string
//
bool CCSAssemblyFileMgr::getRawAssemblyFileLineStr(CString& str)
{
    if (!m_pRawAssemblyFile) {
        tcout << _T("Error: CCSAssemblyFileMgr: open the raw assembly file first.") << endl;
        ASSERT(false);
        return false;
    }

    try {        
        // Parse one line
        if (m_pRawAssemblyFile->ReadString(str))
            return true;
        else
            return false;
    }
    catch (CFileException&) {
        tcout << _T("CCSAssemblyFileMgr: failed to read aw assembly file") << endl;
        closeRawAssemblyFile();
        ASSERT(false);
        return false;
    }
}


//
// Close the raw assembly file
//
void CCSAssemblyFileMgr::closeRawAssemblyFile()
{
    if (m_pRawAssemblyFile) {
        m_pRawAssemblyFile->Close();
        delete m_pRawAssemblyFile;
        m_pRawAssemblyFile = NULL;
    }
}


//
// Parse the whole input folder, including subfolders
//
bool CCSAssemblyFileMgr::parseFolder(LPCTSTR folderPath, const CCSParam& param, bool bFirstScan)
{   
    tcout << _T("Parsing folder: ") << folderPath << endl;
    CString folderPathStr = folderPath;
    m_nTotalFiles = m_nTotalFunctions = m_nTotalRegions = 0;

    WIN32_FIND_DATA fileInfo;
    HANDLE hFile = ::FindFirstFile(folderPathStr + _T("\\*.*"), &fileInfo);
    do {
        CString fileName = fileInfo.cFileName;
        if (fileName.CompareNoCase(_T(".")) == 0 || fileName.CompareNoCase(_T("..")) == 0)
            continue;

        if (fileInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {        
            // It is a directory.
            CString subFolderPath = folderPathStr + _T("\\") + fileName;
            if (!parseFolder((LPCTSTR) subFolderPath, param, bFirstScan))
                return false;
        }
        else {
            // It is a file.
            CString ext = fileName.Mid(fileName.Find(_T(".")) + 1);
            if (ext.CompareNoCase(CS_ASSEMBLYFILE_EXT1) == 0 || ext.CompareNoCase(CS_ASSEMBLYFILE_EXT2) == 0 || ext.CompareNoCase(CS_ASSEMBLYFILE_EXT3) == 0) {
                // Matched extension

                // parse the assembly file
                CString assemblyFilePath = folderPathStr + _T("\\") + fileName;
                CCSAssemblyFile assemblyFile(assemblyFilePath, this);
                if (!assemblyFile.parseFile(param, !bFirstScan))
                    return false;                

                // normalize code
                if (!assemblyFile.normalizeCode(param))
                    return false;

                // create functions in memory and store into DB.
                if (!assemblyFile.extractFunctions(m_pDBMgr, param, !bFirstScan))
                    return false;
                if (!bFirstScan) {  
                    m_nTotalFunctions += assemblyFile.getNumFunctions();
					tcout << _T("Number of functions: ") << assemblyFile.getNumFunctions() << _T(" in ") << assemblyFilePath.GetString() << endl;
                }

                // create regions and store into DB.
                if (!assemblyFile.extractRegions(m_pDBMgr, param, false, false, !bFirstScan, bFirstScan, this))
                    return false;
                if (!bFirstScan) {  
                    m_nTotalRegions += assemblyFile.getNumRegions();
                    tcout << _T("Number of regions: ") << assemblyFile.getNumRegions() << _T(" in ") << assemblyFilePath.GetString() << endl;
                    ++m_nTotalFiles;
                }
                // assemblyFile will be deallocated.
            }
        }
    } while (::FindNextFile(hFile, &fileInfo));
  
	if (bFirstScan) {
        if (!m_pDBMgr->storeGlobalFeatures(m_globalFeatures, m_globalMedians ,param)) {
			tcout << _T("Error: failed to store features") << endl;
			return false;	
        }  
		if(!filterOutFeatures(m_globalMedians)) {
			tcout << _T("Error: failed to filtering features ") << endl;
			return false;	
		}
		if(!m_pDBMgr->storeFilteredFeatures(m_globalFeatures, m_mediansNZ, m_globalMedians, param)){
			tcout << _T("Error: failed to store filtered features ") << endl;
			return false;	
		}	
	   /* if (!m_pDBMgr->preInsertInexact2Comb(m_mediansNZ, m_globalMedians, param) ) { 
			//tcout << _T("Error: failed to pre-insert Inexact2Comb table") << endl;
			//return false;	
        }*/
	}

    if(!bFirstScan) {  
        tcout << _T("Total number of files: ") << m_nTotalFiles << endl;
        tcout << _T("Total number of functions: ") << m_nTotalFunctions << endl;
        tcout << _T("Total number of regions: ") << m_nTotalRegions << endl;
    }
    return true;
}


//
// Create a temporary assembly file based on the given content.
//
bool CCSAssemblyFileMgr::createAssemblyFile(LPCTSTR content, CString& newAssemblyFilePath)
{
    CString contentStr = content;
    if (contentStr.IsEmpty()) {
        tcout << _T("CCSAssemblyFileMgr: empty content. Failed to create assembly file.") << endl;
        ASSERT(false);
        return false;
    }

    // create a new filename    
    static int tempFileID = 0;      // ensure every file has a different name
    CString tempFileIDstr, tempFolderPath;
    CBFStrHelper::intToStr(tempFileID++, tempFileIDstr);
    if (!CBFFileHelper::getTempFolderPath(tempFolderPath)) {
        tcout << _T("CCSAssemblyFileMgr: failed to get temporary folder path.") << endl;
        ASSERT(false);
        return false;
    }
    CString tempAssemblyFilePath = tempFolderPath + _T("\\_");
    tempAssemblyFilePath += CS_ASSMEBLYFILE_TMP_FILENAME;
    tempAssemblyFilePath += tempFileIDstr;
    tempAssemblyFilePath += _T(".");
    tempAssemblyFilePath += CS_ASSEMBLYFILE_EXT3;

    // make sure the content contains some at least one function.
    if (contentStr.Find(_T("proc near")) == -1 && contentStr.Find(_T("proc far")) == -1)
        contentStr = _T("sub_fake proc near\n") + contentStr + _T("\nsub_fake endp\n");
    
    try {
        // create a new file
        CStdioFile newAssemblyFile;
        if (!newAssemblyFile.Open(tempAssemblyFilePath, CFile::modeCreate | CFile::modeWrite)) {
            tcout << _T("CCDAssemblyFileMgr: Failed to create new assembly file ") << tempAssemblyFilePath << endl;
            return false;
        }        
        newAssemblyFile.WriteString(contentStr);
        newAssemblyFile.Close();
    }
    catch (CFileException&) {
        tcout << _T("Failed to create new assembly file: ") << tempAssemblyFilePath << endl;
        ASSERT(false);
        CBFFileHelper::removeFile((LPCTSTR) tempAssemblyFilePath);
        return false;
    }
    newAssemblyFilePath = tempAssemblyFilePath;
    return true;
}

//
// Construct a list of features (strings)
//
bool CCSAssemblyFileMgr::constructBasicFeatures()
{
    // construct only a single instance of feature list.
    if (!m_globalFeatures.IsEmpty())
        return true;

    if (!constructMnemonicFeatures())
        return false;

    if (!constructOpTypeFeatures())
        return false;

    if (!constructMnemonicOpType0Features())
        return false;

    if (!constructOpType0Type1Features())
        return false;
	
	if (!constructGlobalMedians())
        return false;

    if (!constructRedundancyVector()) 
        return false;
    
    return true;
}

//
// Construct mnemonic features
//
bool CCSAssemblyFileMgr::constructMnemonicFeatures()
{
    for (int i = 0; i < CS_NUM_MNEMONICS; ++i) {
		if (!addGlobalFeature(gMnemonics[i].MakeLower())) 
            return false;
    }
    return true;
}


//
// Construct operand type features
//
bool CCSAssemblyFileMgr::constructOpTypeFeatures()
{
    for (int i = 0; i < CS_NUM_OPTYPES; ++i) {
        if (!addGlobalFeature(gOpTypes[i].MakeLower())) 
            return false;
    }
    return true;
}

//
// Construct mnemonic and operand type features
//
bool CCSAssemblyFileMgr::constructMnemonicOpType0Features()  
{
    for (int i = 0; i < CS_NUM_MNEMONICS; ++i) {
		for (int j = 0; j < CS_NUM_OPTYPES; ++j) {
			if (!addGlobalFeature((gMnemonics[i] + gOpTypes[j]).MakeLower())) 
				 return false;			   
		}
	}
    return true;
}


//
// Construct operand type0 and type1 features
//
bool CCSAssemblyFileMgr::constructOpType0Type1Features()
{
    for (int i = 0; i < CS_NUM_OPTYPES; ++i) {
        for (int j = 0; j < CS_NUM_OPTYPES; ++j) {
            if (!addGlobalFeature((gOpTypes[i] + gOpTypes[j]).MakeLower())) 
                return false;
        }
    }
    return true;
}


//
// add a global feature
//
bool CCSAssemblyFileMgr::addGlobalFeature(LPCTSTR featureName)
{
    try {
        m_globalFeatures.Add(featureName);
        return true;
    }
    catch (CMemoryException&) {
        tcout << _T("CCSAssemblyFileMgr::addGlobalFeature: failed to add feature: ") << featureName << endl;
        ASSERT(false);
        return false;
    } 
}


//
// add a global feature only if it is a new one
//
bool CCSAssemblyFileMgr::addGlobalFeatureIfNew(LPCTSTR featureName)
{
    for (int f = 0; f < m_globalFeatures.GetSize(); ++f) {
        if (m_globalFeatures.GetAt(f).CompareNoCase(featureName) == 0)
            return true;
    }
    return addGlobalFeature(featureName);
}

//
// set the size of m_globalMedians vector equal to number of of features
//
bool CCSAssemblyFileMgr::constructGlobalMedians()
{
      m_globalMedians.SetSize(m_globalFeatures.GetSize());
      return true;     
}


//
// construct a two dimensial vector, the first dimension shows each feature and the second dimension keeps track of "number of occurance of each feature's value" 
// It will be used to find median of each feature

bool CCSAssemblyFileMgr::constructRedundancyVector()
{
    m_redundancyVector.resize(m_globalMedians.GetSize()); // set a size for the redundancy vector 
    for (int i = 0; i < m_globalMedians.GetSize(); ++i)
    m_redundancyVector[i].resize(100);
    return true;
}

bool CCSAssemblyFileMgr::filterOutFeatures(CCSIntArray& globalMedians) {

	for (int i = 0; i < globalMedians.GetSize(); ++i) {
		if (globalMedians.GetAt(i) != 0)
			if (i != 91 && i!= 92 && i!= 93 && i!= 367 && i!=368 && i!=369 && i!=370 && i!=371 && i!=372 && i!=373 && i!=374 && i!=375) // These are operand's ids, which we do not need them for combination (we already consider them in feature extraction)!
			m_mediansNZ.Add(i);  // store the index of the features with median more than 0
	}

	if (m_mediansNZ.GetSize() == 0) {
        tcout << _T("CSAssemblyFileMgr:filterOutFeatures: All feature have 0 median.") << endl;
        ASSERT(false);
        return false;
    }
	return true;
}



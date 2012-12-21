#include "StdAfx.h"
#include "CDAssemblyFileMgr.h"

//*********************
// CCDAssemblyFileMgr *
//*********************

CCDAssemblyFileMgr::CCDAssemblyFileMgr(CCDTokenRefMgr* pTokenRefMgr, LPCTSTR assemblyFolderPath)
    : m_pTokenRefMgr(pTokenRefMgr), m_assemblyFolderPath(assemblyFolderPath), m_pRawAssemblyFile(NULL)
{
}


CCDAssemblyFileMgr::~CCDAssemblyFileMgr()
{
    m_assemblyFiles.cleanup();
}


//
// Parse the whole input folder, including subfolders
//
bool CCDAssemblyFileMgr::parseFolder()
{
    return parseFolder(m_assemblyFolderPath);
}


//
// Parse the whole input folder, including subfolders
//
bool CCDAssemblyFileMgr::parseFolder(LPCTSTR folderPath)
{
    tcout << _T("Parsing folder: ") << folderPath << endl;
    CString folderPathStr = folderPath;

    WIN32_FIND_DATA fileInfo;
    HANDLE hFile = ::FindFirstFile(folderPathStr + _T("\\*.*"), &fileInfo);
    do {
        CString fileName = fileInfo.cFileName;
        if (fileName.CompareNoCase(_T(".")) == 0 || fileName.CompareNoCase(_T("..")) == 0)
            continue;

        if (fileInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {        
            // It is a directory.
            CString subFolderPath = folderPathStr + _T("\\") + fileName;
            if (!parseFolder((LPCTSTR) subFolderPath))
                return false;
        }
        else {
            // It is a file.
            CString ext = fileName.Mid(fileName.Find(_T(".")) + 1);
            if (ext.CompareNoCase(CD_ASSEMBLYFILE_EXT1) == 0 || ext.CompareNoCase(CD_ASSEMBLYFILE_EXT2) == 0 || ext.CompareNoCase(CD_ASSEMBLYFILE_EXT3) == 0) {
                // Matched extension
                if (!parseFile(folderPathStr + _T("\\") + fileName))
                    return false;
            }
        }
    } while (::FindNextFile(hFile, &fileInfo));
    return true;
}

//
// Parse one file.
//
bool CCDAssemblyFileMgr::parseFile(LPCTSTR assemblyFilePath)
{
    CCDAssemblyFile* pNewAssemblyFile = new CCDAssemblyFile(assemblyFilePath);
    try {
        CStdioFile assemblyFile;
        if (!assemblyFile.Open(assemblyFilePath, CFile::modeRead)) {
            tcout << _T("CCDAssemblyFileMgr: failed to open file: ") << assemblyFilePath << endl;
            delete pNewAssemblyFile;
            pNewAssemblyFile = NULL;
            ASSERT(false);
            return false;
        }        
        
        int asmFileLineIdx = -1;    // line index in the raw assembly file.        

        // Parse one line
        CString lineStr;
        while (assemblyFile.ReadString(lineStr)) {
            ++asmFileLineIdx;
            CBFStrHelper::trim(lineStr);
            if (lineStr.IsEmpty())
                continue;

            // Replace tab space by space
            lineStr.Replace(TCHAR('\t'), TCHAR(' '));

            // Parse and add strings
            if (!m_pTokenRefMgr->parseStringRef(lineStr, pNewAssemblyFile, asmFileLineIdx)) {
                delete pNewAssemblyFile;
                pNewAssemblyFile = NULL;
                return false;
            }

            // Skip comments
            int commentPos = lineStr.Find(CD_ASSEMBLYFILE_COMMENT);
            if (commentPos != -1) {
                lineStr = lineStr.Left(commentPos - 1);
                CBFStrHelper::trim(lineStr);
                if (lineStr.IsEmpty())
                    continue;
            }

            // Create a new line
            CCDAssemblyLine* pNewLine = new CCDAssemblyLine(asmFileLineIdx, lineStr);
            if (!pNewLine->addTokens()) {
                delete pNewLine;
                pNewLine = NULL;
                delete pNewAssemblyFile;
                pNewAssemblyFile = NULL;
                return false;
            }

            // Parse and add constants
            if (!m_pTokenRefMgr->parseConstantRef(pNewLine, pNewAssemblyFile, asmFileLineIdx)) {
                delete pNewLine;
                pNewLine = NULL;
                delete pNewAssemblyFile;
                pNewAssemblyFile = NULL;
                return false;
            }
            
            // Parse and add imports
            if (!m_pTokenRefMgr->parseImportRef(pNewLine, pNewAssemblyFile, asmFileLineIdx)) {
                delete pNewLine;
                pNewLine = NULL;
                delete pNewAssemblyFile;
                pNewAssemblyFile = NULL;
                return false;
            }

            // Add the new line only if it is a begin procedure, an end procedure, or a mnemonic statement
            //if (true) {
            if (pNewLine->isBeginProcedure() || pNewLine->isEndProcedure() || pNewLine->isMnemonicStatement()) {            
                if (!pNewAssemblyFile->addLine(pNewLine)) {
                    delete pNewLine;
                    pNewLine = NULL;
                    delete pNewAssemblyFile;
                    pNewAssemblyFile = NULL;
                    return false;
                }
            }
            else {
                delete pNewLine;
                pNewLine = NULL;
            }
        }
        
        assemblyFile.Close();
        if (pNewAssemblyFile->GetSize() == 0) {
            tcout << _T("CCDAssemblyFileMgr: empty file: " ) << assemblyFilePath << endl;
            delete pNewAssemblyFile;
            pNewAssemblyFile = NULL;
            return true;    // skip this file.
        }
        else {
            tcout << assemblyFilePath << _T(" has ") << pNewAssemblyFile->GetSize() << _T(" lines.") << endl;
        }
    }
    catch (CFileException&) {
        tcout << _T("CCDAssemblyFileMgr: failed to read assembly file: ") << assemblyFilePath << endl;
        delete pNewAssemblyFile;
        pNewAssemblyFile = NULL;
        ASSERT(false);
        return false;
    }

    // add the file object.
    return m_assemblyFiles.addFile(pNewAssemblyFile);
}


//
// Get assembly file.
//
CCDAssemblyFile* CCDAssemblyFileMgr::getAssemblyFile(int fIdx) const 
{ 
    if (fIdx < 0 || fIdx >= getNumAssemblyFiles()) {
        tcout << _T("Error: CCDAssemblyFileMgr: invalid assemblyFileIdx: ") << fIdx << endl;
        ASSERT(false);
        return NULL;
    }
    return m_assemblyFiles.GetAt(fIdx); 
}


//
// Get assembly file ID.
//
int CCDAssemblyFileMgr::getAssemblyFileID(LPCTSTR assemblyFilePath) const
{
    CCDAssemblyFile* pFile = NULL;
    for (int f = 0; f < m_assemblyFiles.GetSize(); ++f) {
        if (m_assemblyFiles.GetAt(f)->getFilePath().CompareNoCase(assemblyFilePath) == 0)
            return f;
    }
    return -1;
}


//
// Open the raw assembly file and return its index
// Return false if error.
//
bool CCDAssemblyFileMgr::openRawAssemblyFile(LPCTSTR assemblyFilePath)
{
    if (m_pRawAssemblyFile) {
        tcout << _T("Error: CCDAssemblyFileMgr: only one raw file can be opened at a time: ") << assemblyFilePath << endl;
        ASSERT(false);
        return false;
    }
    m_pRawAssemblyFile = new CStdioFile();
    
    try {        
        if (!m_pRawAssemblyFile->Open(assemblyFilePath, CFile::modeRead)) {
            tcout << _T("CCDAssemblyFileMgr: failed to open file: ") << assemblyFilePath << endl;
            delete m_pRawAssemblyFile;
            m_pRawAssemblyFile = NULL;
            ASSERT(false);
            return false;
        }
        return true;
    }
    catch (CFileException&) {
        tcout << _T("CCDAssemblyFileMgr: failed to read assembly file: ") << assemblyFilePath << endl;
        delete m_pRawAssemblyFile;
        m_pRawAssemblyFile = NULL;
        ASSERT(false);
        return false;
    }
}


//
// Get raw assembly file line string
//
bool CCDAssemblyFileMgr::getRawAssemblyFileLineStr(CString& str)
{
    if (!m_pRawAssemblyFile) {
        tcout << _T("Error: CCDAssemblyFileMgr: open the raw assembly file first.") << endl;
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
        tcout << _T("CCDAssemblyFileMgr: failed to read aw assembly file") << endl;
        closeRawAssemblyFile();
        ASSERT(false);
        return false;
    }
}


//
// Close the raw assembly file
//
void CCDAssemblyFileMgr::closeRawAssemblyFile()
{
    if (m_pRawAssemblyFile) {
        m_pRawAssemblyFile->Close();
        delete m_pRawAssemblyFile;
        m_pRawAssemblyFile = NULL;
    }
}


//
// Create a temporary assembly file based on the given content.
//
bool CCDAssemblyFileMgr::createAssemblyFile(LPCTSTR content, CString& newAssemblyFilePath)
{
    CString contentStr = content;
    if (contentStr.IsEmpty()) {
        tcout << _T("CCDAssemblyFileMgr: empty content. Failed to create assembly file.") << endl;
        ASSERT(false);
        return false;
    }

    // create a new filename    
    static int tempFileID = 0;      // ensure every file has a different name
    CString tempFileIDstr;
    CBFStrHelper::intToStr(tempFileID++, tempFileIDstr);
    CString tempAssemblyFilePath = m_assemblyFolderPath + _T("\\_");
    tempAssemblyFilePath += CD_ASSMEBLYFILE_TMP_FILENAME;
    tempAssemblyFilePath += tempFileIDstr;
    tempAssemblyFilePath += _T(".");
    tempAssemblyFilePath += CD_ASSEMBLYFILE_EXT3;

    // make sure the content contains at least one function.
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

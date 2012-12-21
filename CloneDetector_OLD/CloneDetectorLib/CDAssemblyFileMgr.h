//-----------------------------------------------------
// Objective:   Assembly File Manager parses assembly files in a folder
//
// History:     2011/10/19 - B. Fung (Created)
// 
//-----------------------------------------------------

#pragma once

#include "CDAssemblyFile.h"
#include "CDTokenRefMgr.h"


class CCDAssemblyFileMgr
{
public:
    CCDAssemblyFileMgr(CCDTokenRefMgr* pTokenRefMgr, LPCTSTR assemblyFolderPath);
    virtual ~CCDAssemblyFileMgr();
    bool parseFolder();    
    
    inline const CString& getAssemblyFolderPath() { return m_assemblyFolderPath; };
    inline int getNumAssemblyFiles() const { return m_assemblyFiles.GetSize(); }
    CCDAssemblyFile* getAssemblyFile(int fIdx) const;
    int getAssemblyFileID(LPCTSTR assemblyFilePath) const;

    // functions for manipulating raw assembly files
    bool openRawAssemblyFile(LPCTSTR assemblyFilePath);
    bool getRawAssemblyFileLineStr(CString& str);
    void closeRawAssemblyFile();

    // create a temporary assembly file based on the given content.
    bool createAssemblyFile(LPCTSTR content, CString& newAssemblyFilePath);

private:
    bool parseFolder(LPCTSTR folderPath);
    bool parseFile(LPCTSTR assemblyFilePath);    

    CCDTokenRefMgr* m_pTokenRefMgr;
    CCDAssemblyFiles m_assemblyFiles;   // assembly files with removed comments and empty lines
    CString m_assemblyFolderPath;       // path to the assembly code folder
    CStdioFile* m_pRawAssemblyFile;     // raw assembly file. Only one raw file can be opened at a time.
};




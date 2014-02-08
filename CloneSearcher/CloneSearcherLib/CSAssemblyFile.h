//-----------------------------------------------------
// Objective:   Model an assembly file.
//
// History:     2012/10/7 - B. Fung (Created)
// 
//-----------------------------------------------------

#pragma once

#include "CSAssemblyLine.h"
#include "CSAssemblyFunction.h"
#include "CSAssemblyFileMgr.h"
#include "CSParam.h"
#include "CSClone.h"

class CCSDatabaseMgr;

class CCSAssemblyFile : public CCSAssemblyLines
{
public:
    CCSAssemblyFile(LPCTSTR filePath, CCSAssemblyFileMgr* pAssemblyFileMgr);
    virtual ~CCSAssemblyFile();

    bool addLine(CCSAssemblyLine* pAssemblyLine);
    bool addFunction(CCSAssemblyFunction* pFcn);
    
    bool parseFile(const CCSParam& param, bool bStoreDB);
    bool normalizeCode(const CCSParam& param);
    bool extractFunctions(CCSDatabaseMgr* pDBMgr, const CCSParam& param, bool bStoreDB);
    bool extractRegions(CCSDatabaseMgr* pDBMgr, const CCSParam& param, bool bFindExactClones, bool bFindInexactClones, bool bStoreDB, bool bFirstScan, CCSAssemblyFileMgr* pAssemblyFileMgr);

    CString composeContentHashKey(int startIdx, int endIdx) const;
    int getNumRegions() const;

    bool getContentStr(CString& str) const;
    bool getLineStr(int lineIdx, CString& str) const;    
    
    inline const CString& getFilePath() const { return m_filePath; };

    inline CCSAssemblyFunctions* getFunctions() { return &m_functions; };
    inline int getNumFunctions() const { return m_functions.GetSize(); };

    inline CCSClones* getClones() { return &m_clones; };
    
    int m_dbFileID;    

private:    
    CCSAssemblyFileMgr* m_pAssemblyFileMgr;
	CCSAssemblyToken* m_pAssemblyToken;
    CCSAssemblyFunctions m_functions;  
    CString m_filePath;
    CCSClones m_clones;
    CMapStringToOb m_hashObject;
};

typedef CTypedPtrArray<CPtrArray, CCSAssemblyFile*> CCSAssemblyFileArray;
class CCSAssemblyFiles : public CCSAssemblyFileArray
{
public:
    CCSAssemblyFiles() {};
    virtual ~CCSAssemblyFiles() {};
    bool addFile(CCSAssemblyFile* pAssemblyFile);
    void cleanup();
};

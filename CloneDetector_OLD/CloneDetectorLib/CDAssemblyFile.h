//-----------------------------------------------------
// Objective:   Model an assembly file.
//
// History:     2011/10/19 - B. Fung (Created)
// 
//-----------------------------------------------------

#pragma once

#include "CDAssemblyLine.h"
#include "CDAssemblyFunction.h"

class CCDAssemblyFile : public CCDAssemblyLines
{
public:
    CCDAssemblyFile(LPCTSTR filePath);
    virtual ~CCDAssemblyFile();

    bool addLine(CCDAssemblyLine* pAssemblyLine);
    bool addFunction(CCDAssemblyFunction* pFcn);
    CString composeContentHashKey(int startIdx, int endIdx) const;
    int getNumRegions() const;

    bool getContentStr(CString& str) const;
    bool getLineStr(int lineIdx, CString& str) const;    
    
    inline const CString& getFilePath() const { return m_filePath; };
    inline CCDAssemblyFunctions* getFunctions() { return &m_functions; };
    inline int getNumFunctions() const { return m_functions.GetSize(); };

    int m_fileIdx;

private:    
    CString m_filePath;
    CCDAssemblyFunctions m_functions;  
};

typedef CTypedPtrArray<CPtrArray, CCDAssemblyFile*> CCDAssemblyFileArray;
class CCDAssemblyFiles : public CCDAssemblyFileArray
{
public:
    CCDAssemblyFiles() {};
    virtual ~CCDAssemblyFiles() {};
    bool addFile(CCDAssemblyFile* pAssemblyFile);
    void cleanup();
};

// BFFileHelper.h: interface for the File Helper classes
//
//////////////////////////////////////////////////////////////////////

#if !defined(BFFILEHELPER_H)
#define BFFILEHELPER_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//--------------------------------------------------------------------
//--------------------------------------------------------------------
class CBFFileHelper
{
public:
    CBFFileHelper();
    virtual ~CBFFileHelper();

// operations
    static bool removeFile(LPCTSTR filename);
    static bool getTempFolderPath(CString& tempFolderPath);
    static void splitPath(LPCTSTR fullPath, CString& drive, CString& dir, CString& fname, CString& ext);
    static bool replaceExtension(LPCTSTR fname, LPCTSTR ext, CString& res);
};

#endif // !defined(BFFILEHELPER_H)
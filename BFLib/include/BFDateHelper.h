// BFFileHelper.h: interface for the File Helper classes
//
//////////////////////////////////////////////////////////////////////

#if !defined(BFDATEHELPER_H)
#define BFDATEHELPER_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//--------------------------------------------------------------------
//--------------------------------------------------------------------

class CBFDateHelper
{
public:
    CBFDateHelper(void);
    virtual ~CBFDateHelper(void);

    static bool strToTime(LPCTSTR str, COleDateTime& res);
    static bool strToTimeSpan(LPCTSTR str, COleDateTimeSpan& res);
};

#endif // !defined(BFDATEHELPER_H)
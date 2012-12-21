// BFStrHelper.h: interface for the String Helper classes
//
//////////////////////////////////////////////////////////////////////

#if !defined(BFSTRHELPER_H)
#define BFSTRHELPER_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//--------------------------------------------------------------------
//--------------------------------------------------------------------
class CBFStrHelper
{
public:
    CBFStrHelper();
    virtual ~CBFStrHelper();

// operations    
    static void trim(CString& str);
    static bool isNumeric(const CString& str);
    static bool isHex(LPCTSTR str);
    static void intToStr(int i, CString& str);
    static void floatToStr(double d, int nDecimals, CString& str);
    static int strToInt(const TCHAR* string);
	static double strToFloat(const TCHAR* string);
    static bool strAToBool(const CStringA& ansiStr, bool& boolVal);
    static bool boolToStr(bool boolVal, CString& str);
    static bool convertCStringToCStringA(const CString& cstr, CStringA& ansiStr);
    static bool convertCStringAToCString(const CStringA& ansiStr, CString& str);
};

#endif
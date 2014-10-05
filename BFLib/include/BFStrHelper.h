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
//******************************************************************************//
// Copyright 2014 Benjamin Fung      											//
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

//---------------------------------------------------------------------------
// File:
//      BFStrHelper.cpp BFStrHelper.hpp
//
// Module:
//      CBFStrHelper
//
// History:
//		Jan. 15, 2004		Created by Benjamin Fung
//---------------------------------------------------------------------------

#include "BFPch.h"

#if !defined(BFSTRHELPER_H)
	#include "BFStrHelper.h"
#endif

//--------------------------------------------------------------------
//--------------------------------------------------------------------
CBFStrHelper::CBFStrHelper()
{
}

CBFStrHelper::~CBFStrHelper()
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void CBFStrHelper::trim(CString& str)
{
    str.TrimLeft();
    str.TrimRight();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
bool CBFStrHelper::isNumeric(const CString& str)
{
    int len = str.GetLength();
    for (int i = 0; i < len; ++i) {
        switch (str[i])
        {
        case '0': case '1': case '2': case '3': case '4': case '5': 
        case '6': case '7': case '8': case '9': case '.':
          //do nothing
          break;
        default:
          return false;
        }
    }
    return true;
}

//--------------------------------------------------------------------
// 0x45F3D is a hex
// 0x45F3Dwarf is not a hex
// x45F3D is not a hex
// 45F3D is a hex
//--------------------------------------------------------------------
bool CBFStrHelper::isHex(LPCTSTR str)
{
	DWORD_PTR dw = NULL;
	TCHAR ch;
	return (1 == _stscanf_s(str, _T("%x%c"), &dw, &ch));
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void CBFStrHelper::intToStr(int i, CString& str)
{
	char buffer[64];
	_itoa_s(i, buffer, 10);
    if (!buffer)
        str = _T("");
    else
        str = buffer;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void CBFStrHelper::floatToStr(double d, int nDecimals, CString& str)
{
    char buffer[100];
    sprintf_s (buffer, "%.*f", nDecimals, d);
    if (!buffer)
        str = _T("");
    else
        str = buffer;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
int CBFStrHelper::strToInt(const TCHAR* string)
{
    return _tstoi(string);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
double CBFStrHelper::strToFloat(const TCHAR* string)
{
    return _tstof(string);
}

//--------------------------------------------------------------------
// Convert CStringA to bool
//--------------------------------------------------------------------
bool CBFStrHelper::strAToBool(const CStringA& ansiStr, bool& boolVal)
{
    if (ansiStr.CompareNoCase("true") == 0 ||  ansiStr.CompareNoCase("t") == 0) {
        boolVal = true;
        return true;
    }
    else if (ansiStr.CompareNoCase("false") == 0 ||  ansiStr.CompareNoCase("f") == 0) {
        boolVal = false;
        return true;
    }
    else {  
        ASSERT(false);           
        return false;
    }
}

//--------------------------------------------------------------------
// Convert bool to CString
//--------------------------------------------------------------------
bool CBFStrHelper::boolToStr(bool boolVal, CString& str) 
{
    boolVal ? str = CString(_T("true")) : str = CStringA(_T("false"));
    return true;
}

//--------------------------------------------------------------------
// Convert CString to CStringA
//--------------------------------------------------------------------
bool CBFStrHelper::convertCStringToCStringA(const CString& str, CStringA& ansiStr)
{
#if defined(UNICODE) || defined(_UNICODE)
    const size_t newsizew = (str.GetLength() + 1) * 2;
    char* aStr = new char[newsizew];
    size_t convertedCharsw = 0;
    if (!aStr || wcstombs_s(&convertedCharsw, aStr, newsizew, str.GetString(), _TRUNCATE ) > 0) {        
        ASSERT(false);
        return false;
    }
    ansiStr = aStr;
    delete [] aStr;
    aStr = NULL;
    return true;
#else
    ansiStr = str;
    return true;
#endif
}

//--------------------------------------------------------------------
// Convert CStringA to CString
//--------------------------------------------------------------------
bool CBFStrHelper::convertCStringAToCString(const CStringA& ansiStr, CString& str)
{
#if defined(UNICODE) || defined(_UNICODE)
    const size_t newsizew = (ansiStr.GetLength() + 1);
    wchar_t * wStr = new wchar_t[newsizew];
    size_t convertedChars = 0;
    //mbstowcs_s(&convertedChars, wcstring, newsize, orig, _TRUNCATE);
    if (!wStr || mbstowcs_s(&convertedChars, wStr, newsizew, ansiStr.GetString(), _TRUNCATE ) > 0) {        
        ASSERT(false);
        return false;
    }
    str = wStr;
    delete [] wStr;
    wStr = NULL;
    return true;
#else
    str =ansiStr;
    return true;
#endif
}


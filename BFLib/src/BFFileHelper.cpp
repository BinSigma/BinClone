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

//---------------------------------------------------------------------------
// File:
//      BFFileHelper.cpp BFFileHelper.hpp
//
// Module:
//      CBFFileHelper
//
// History:
//		May. 7, 2002		Created by Benjamin Fung
//---------------------------------------------------------------------------

#include "BFPch.h"

#if !defined(BFFILEHELPER_H)
	#include "BFFileHelper.h"
#endif

//--------------------------------------------------------------------
//--------------------------------------------------------------------
CBFFileHelper::CBFFileHelper()
{
}

CBFFileHelper::~CBFFileHelper()
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
bool CBFFileHelper::removeFile(LPCTSTR filename)
{
    return _tremove(filename) == 0;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
bool CBFFileHelper::getTempFolderPath(CString& tempFolderPath)
{
    TCHAR tPath[_MAX_PATH];
    if (GetTempPath(_MAX_PATH, tPath) == 0)
        return false;

    tempFolderPath = tPath;
    return true;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void CBFFileHelper::splitPath(LPCTSTR fullPath, CString& drive, CString& dir, CString& fname, CString& ext)
{
    TCHAR tDrive[_MAX_DRIVE];
    TCHAR tDir[_MAX_DIR];
    TCHAR tFname[_MAX_FNAME];
    TCHAR tExt[_MAX_EXT];
    _tsplitpath_s(fullPath, tDrive, tDir, tFname, tExt);

    drive = tDrive;
    dir = tDir;
    fname = tFname;
    ext = tExt;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
bool CBFFileHelper::replaceExtension(LPCTSTR fname, LPCTSTR ext, CString& res)
{
    res = fname;
    int dotPos = res.ReverseFind(TCHAR('.'));
    if (dotPos == -1) {
        res.Empty();
        return false;
    }
    res = res.Left(dotPos + 1);
    res += ext;
    return true;
}
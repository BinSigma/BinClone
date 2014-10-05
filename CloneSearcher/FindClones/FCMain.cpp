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

// FCMain.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "FCMain.h"
#include "CSController.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// The one and only application object

CWinApp theApp;

using namespace std;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool parseArgs(int                   nArgs, 
               TCHAR*                argv[], 
               CString&              dbName,
               CString&              dbUser,
               CString&              dbPwd,
               CString&              targetAssemblyFilePath,
               bool&                 bFindExactClones,
               bool&                 bFindInexactClones,
               double&               minCoOccThreshold,
               int&                  dbParamID)
{
    if (nArgs != 9 || !argv) {
        tcout << _T("Usage: AssemblyToDB <dbName> <dbUser> <dbPwd> <targetAssemblyFilePath> <bFindExactClones> <bFindInexactClones> <minCoOccThreshold> <dbParamID>") << endl;
        tcout << _T("Format: <dbName:string>") << endl;
        tcout << _T("        <dbUser:string>") << endl;
        tcout << _T("        <dbPwd:string>") << endl;
        tcout << _T("        <targetAssemblyFilePath:string>:target assembly file path, e.g., C:\\temp\\newcode\\abc.asm") << endl;
        tcout << _T("        <bFindExactClones:boolean>:find exact clones with 2 possible values: TRUE or FALSE") << endl;
        tcout << _T("        <bFindInexactClones:boolean>:find inexact clones with 2 possible values: TRUE or FALSE") << endl;
        tcout << _T("        <minCoOccThreshold:real>:minimum co-occurrence threshold, e.g, 0.5") << endl;
        tcout << _T("        <dbParamID:integer>:DB parameter ID, e.g, 5") << endl;
        return false;
    }

    dbName = argv[1];
    dbUser = argv[2];
    dbPwd = argv[3];
    targetAssemblyFilePath = argv[4];

	if (_tcsicmp(argv[5], CS_TRUE_STR) == 0)
	    bFindExactClones = true;
	else if (_tcsicmp(argv[5], CS_FALSE_STR) == 0)
		bFindExactClones = false;
	else {
        tcout << _T("Input Error: invalid arguments: bFindExactClones") << endl;
        ASSERT(false);
        return false;
    }

	if (_tcsicmp(argv[6], CS_TRUE_STR) == 0)
	    bFindInexactClones = true;
	else if (_tcsicmp(argv[6], CS_FALSE_STR) == 0)
		bFindInexactClones = false;
	else {
        tcout << _T("Input Error: invalid arguments: bFindInexactClones") << endl;
        ASSERT(false);
        return false;
    }

    minCoOccThreshold = CBFStrHelper::strToFloat(argv[7]);
    dbParamID = StrToInt(argv[8]);
    return true;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	HMODULE hModule = ::GetModuleHandle(NULL);

	if (hModule != NULL)
	{
		// initialize MFC and print and error on failure
		if (!AfxWinInit(hModule, NULL, ::GetCommandLine(), 0))
		{
			// TODO: change error code to suit your needs
			_tprintf(_T("Fatal Error: MFC initialization failed\n"));
			nRetCode = 1;
		}
		else
		{
			//CString strHello;
			//strHello.LoadString(IDS_HELLO);
			//tcout << (LPCTSTR)strHello << endl;
            
			CString dbName, dbUser, dbPwd, targetAssemblyFilePath;
            int dbParamID = -1;
            double minCoOccThreshold = 0.0;
            bool bFindExactClones = false, bFindInexactClones = false;
			if (!parseArgs(argc, argv, dbName, dbUser, dbPwd, targetAssemblyFilePath, bFindExactClones, bFindInexactClones, minCoOccThreshold, dbParamID)) { 
				tcout << _T("Input Error: invalid arguments") << endl;
				return 1;
			}
           
            // test fetching parameters
            CCSParams params;
            CCSController controller(dbName, dbUser, dbPwd);
            if (!controller.fetchAllParameters(params)) {
                tcout << _T("Failed to fetech parameters") << endl;
#ifdef _DEBUG
                _getch();
#endif
				return 1;
            }                      

            const CCSParam* pParam = params.getParamBydbParamID(dbParamID);
            if (!pParam) {
                tcout << _T("Failed to fetech parameter with dbParamID: ") << dbParamID << endl;
#ifdef _DEBUG
                _getch();
#endif
				return 1;
            }

            // test searching for assembly code to DB.
            const CCSClones* pClones = NULL;
            if (!controller.findFileClones(targetAssemblyFilePath, *pParam, bFindExactClones, bFindInexactClones, minCoOccThreshold, pClones)) {
				tcout << _T("Error occured.") << endl;
#ifdef _DEBUG
                _getch();
#endif
				return 1;
			}
		}
	}
	else {
		// TODO: change error code to suit your needs
		_tprintf(_T("Fatal Error: GetModuleHandle failed\n"));
		nRetCode = 1;
	}

	tcout << _T("Bye!") << endl;
#ifdef _DEBUG
    _getch();
#endif
	return nRetCode;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void printTime()
{
    //CTime t = CTime::GetCurrentTime();
    //CString s = t.Format(_T("%H:%M:%S"));
    //DEBUGPrint(_T("Current System Time = %s\n"), s);

    time_t ltime;
    time(&ltime);
    tcout << _T("Current System Time = ") << ltime << endl;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
long getTime()
{
    time_t ltime;
    time(&ltime);
    return (long) ltime;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
long get_runtime()
{
    clock_t start;
    start = clock();
    return((long)((double)start*100.0/(double)CLOCKS_PER_SEC));
}

// CSMain.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "CSMain.h"
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
               CString&              assemblyFolder,
               TCSRegNormalizeLevel& regNormLevel, 
               int&                  windowSize,
               int&                  stride         
               )
{
    if (nArgs != 8 || !argv) {
        tcout << _T("Usage: AssemblyToDB <dbName> <dbUser> <dbPwd> <assemblyFolder> <regNormLevel>  <window size> <stride>") << endl;
        tcout << _T("Format: <dbName:string>") << endl;
        tcout << _T("        <dbUser:string>") << endl;
        tcout << _T("        <dbPwd:string>") << endl;
        tcout << _T("        <assemblyFolder:string>:input folder of assembly files, e.g., C:\\temp\\sample") << endl;
        tcout << _T("        <regNormLevel:boolean>:register normalization level with 4 possible values: REG, TYPE, IDXPTR, or BITS") << endl;   
        tcout << _T("        <windowSize:integer>:window size, e.g., 5") << endl;
        tcout << _T("        <stride:integer>:step size, e.g., 1") << endl;
        return false;
    }

    dbName = argv[1];
    dbUser = argv[2];
    dbPwd = argv[3];
    assemblyFolder = argv[4];

	if (_tcsicmp(argv[5], CS_PARAM_NORM_REG_ROOT_STR) == 0)
	    regNormLevel = CS_NORM_REG_ROOT;
	else if (_tcsicmp(argv[5], CS_PARAM_NORM_REG_TYPE_STR) == 0)
		regNormLevel = CS_NORM_REG_TYPE;
	else if (_tcsicmp(argv[5], CS_PARAM_NORM_REG_IDXPTR_STR) == 0)
		regNormLevel = CS_NORM_REG_IDXPTR;
	else if (_tcsicmp(argv[5], CS_PARAM_NORM_REG_BITS_STR) == 0)
		regNormLevel = CS_NORM_REG_BITS;
	else {
        tcout << _T("Input Error: invalid argument for register normalization level.") << endl;
        ASSERT(false);
        return false;
    }
	windowSize = StrToInt(argv[6]);
	stride = StrToInt(argv[7]);
    return true;
}

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

			CString dbName, dbUser, dbPwd, assemblyFolderPath;
            int windSize = 0, stride = 0;
            TCSRegNormalizeLevel regNormLevel = CS_NORM_REG_ROOT;
			if (!parseArgs(argc, argv, dbName, dbUser, dbPwd, assemblyFolderPath, regNormLevel, windSize, stride)) { 
				tcout << _T("Input Error: invalid arguments") << endl;
				return 1;
			}

            // test population of assembly code to DB.
            CCSController controller(dbName, dbUser, dbPwd);
            if (!controller.populateAssemblyToDB(assemblyFolderPath, regNormLevel, windSize, stride)) {
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
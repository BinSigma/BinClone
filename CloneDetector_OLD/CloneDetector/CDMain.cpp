// CDMain.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "CDMain.h"
#include "CDController.h"

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
               CString&              assemblyFolder,
               CString&              clonePairsFile,
               int&                  windowSize,
               int&                  stride,
               int&                  kThreshold,
               double&               oThreshold,
               bool&                 bFindExactClones,
               bool&                 bFindInexactClones,
               TCDRegNormalizeLevel& regNormLevel,
               bool&                 bNormalizeToken,
               int&                  keyVectorSize,
               double&               minCoOccThreshold,
               CDInexactMethod&      inexactMethod)
{
    if (nArgs != 14 || !argv) {
        tcout << _T("Usage: CloneDetector <assemblyFolder> <clonePairsFilePath> <window size> <stride> <maxKOperands> <maxDeltaDistance> <maxOverlapFraction> <bFindExactClones> <bFindInexactClones> <regNormLevel> <bNormalizeToken> <nImportantFeatures> <fracImportantFeatures> <keyVectorSize> <hashTableRatio> <minCoOccThreshold>") << endl;
        tcout << _T("Format: <assemblyFolder:string>:input folder with assembly files, e.g., C:\\temp\\sample") << endl;
        tcout << _T("        <clonePairsFile:string>:file path of output XML file containing clone pairs, e.g., C:\\temp\\sample\\clonepairs.xml") << endl;
        tcout << _T("        <windowSize:integer>:window size, e.g., 5") << endl;
        tcout << _T("        <stride:integer>:step size, e.g., 1") << endl;
        tcout << _T("        <maxKOperands:integer>:maximum k threshold, e.g., 5") << endl;
        tcout << _T("        <maxOverlapFraction:real>:maximum overlap threshold, e.g., 0.5") << endl;
        tcout << _T("        <bFindExactClones:boolean>:find exact clones with 2 possible values: TRUE or FALSE") << endl;
        tcout << _T("        <bFindInexactClones:boolean>:find inexact clones with 2 possible values: TRUE or FALSE") << endl;
        tcout << _T("        <regNormLevel:boolean>:register normalization level with 4 possible values: REG, TYPE, IDXPTR, or BITS") << endl;
        tcout << _T("        <bNormalizeToken:boolean>:normalize token: TRUE or FALSE") << endl;
        tcout << _T("        <keyVectorSize>:size of key vectors for inexact clone detection, e.g., 5") << endl;
        tcout << _T("        <minCoOccThreshold>:minimum co-occurrence threshold, e.g, 0.5 ") << endl;
        tcout << _T("        <inexactMethodLevel>: inexact method level: Method1, Method2, Method3") << endl;
        return false;
    }

    assemblyFolder = argv[1];
    clonePairsFile = argv[2];
	windowSize = StrToInt(argv[3]);
	stride = StrToInt(argv[4]);
    kThreshold = StrToInt(argv[5]);
    oThreshold = CBFStrHelper::strToFloat(argv[6]);

	if (_tcsicmp(argv[7], CD_TRUE_STR) == 0)
	    bFindExactClones = true;
	else if (_tcsicmp(argv[7], CD_FALSE_STR) == 0)
		bFindExactClones = false;
	else
        return false;

	if (_tcsicmp(argv[8], CD_TRUE_STR) == 0)
	    bFindInexactClones = true;
	else if (_tcsicmp(argv[8], CD_FALSE_STR) == 0)
		bFindInexactClones = false;
	else
        return false;

	if (_tcsicmp(argv[9], CD_PARAM_NORM_REG_ROOT_STR) == 0)
	    regNormLevel = CD_NORM_REG_ROOT;
	else if (_tcsicmp(argv[9], CD_PARAM_NORM_REG_TYPE_STR) == 0)
		regNormLevel = CD_NORM_REG_TYPE;
	else if (_tcsicmp(argv[9], CD_PARAM_NORM_REG_IDXPTR_STR) == 0)
		regNormLevel = CD_NORM_REG_IDXPTR;
	else if (_tcsicmp(argv[9], CD_PARAM_NORM_REG_BITS_STR) == 0)
		regNormLevel = CD_NORM_REG_BITS;
	else
        return false;

	if (_tcsicmp(argv[10], CD_TRUE_STR) == 0)
	    bNormalizeToken = true;
	else if (_tcsicmp(argv[10], CD_FALSE_STR) == 0)
		bNormalizeToken = false;
	else
        return false;

    keyVectorSize = StrToInt(argv[11]);
    minCoOccThreshold = CBFStrHelper::strToFloat(argv[12]);    
    return true;
          
    if (_tcsicmp(argv[13], CD_PARAM_CD_INEXACT_METHOD_RANDOM_STR ) == 0)
        inexactMethod = CD_INEXACT_METHOD_RANDOM;
    else if (_tcsicmp(argv[13], CD_PARAM_CD_INEXACT_METHOD_SLIDING_STR ) == 0)
        inexactMethod = CD_INEXACT_METHOD_SLIDING;
    else if (_tcsicmp(argv[13], CD_PARAM_CD_INEXACT_METHOD_TWOCOMBINATION_STR ) == 0)
        inexactMethod = CD_INEXACT_METHOD_TWOCOMBINATION;
    else
        return false;
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

			CString assemblyFolderPath, clonePairsFilePath;
            int windSize = 0, stride = 0, kThreshold = 0, keyVectorSize = 0;
            double oThreshold = 0.0, minCoOccThreshold = 0.0;
            bool bFindExactClones = false, bFindInexactClones = false, bNormalizeToken = false;
            TCDRegNormalizeLevel regNormLevel = CD_NORM_REG_ROOT;
            CDInexactMethod inexactMethod = CD_INEXACT_METHOD_RANDOM; 
			if (!parseArgs(argc, argv, assemblyFolderPath, clonePairsFilePath, windSize, stride, kThreshold, oThreshold, bFindExactClones, bFindInexactClones, regNormLevel, bNormalizeToken, keyVectorSize, minCoOccThreshold, inexactMethod)) { 
				tcout << _T("Input Error: invalid arguments") << endl;
				return 1;
			}

            if (true) {
                // test clone detection
			    CCDController controller(assemblyFolderPath, clonePairsFilePath, windSize, stride, kThreshold, oThreshold, bFindExactClones, bFindInexactClones, regNormLevel, bNormalizeToken, keyVectorSize, minCoOccThreshold, inexactMethod); 
                if (!controller.detectClones()) {
				    tcout << _T("Error occured.") << endl;
#ifdef _DEBUG
                    _getch();
#endif
				    return 1;
			    }
            }
            
            if (false) {
                // test fragment search
			    CCDController controller(assemblyFolderPath, clonePairsFilePath, windSize, stride, kThreshold, oThreshold, bFindExactClones, bFindInexactClones, regNormLevel, bNormalizeToken, keyVectorSize, minCoOccThreshold, inexactMethod); 
                CString codeFragment, tempAssemblyFilePath;

                // code fragement from Data02
                codeFragment = _T("loc_600012DD:				; CODE XREF: BSUSymInitialize(x,x,x,x)+54j\n");
                codeFragment += _T("codeFralea	eax, [ebp+var_4]\n");
                codeFragment += _T("push	eax		; lp\n");
                codeFragment += _T("push	0		; int\n");
                codeFragment += _T("push	0		; int\n");
                codeFragment += _T("push	0		; int\n");
                codeFragment += _T("mov	ecx, [ebp+dwProcessId]\n");
                codeFragment += _T("push	ecx		; dwProcessId\n");
                codeFragment += _T("call	_GetLoadedModules@16 ; GetLoadedModules(x,x,x,x)\n");
                codeFragment += _T("test	eax, eax\n");
                codeFragment += _T("jnz	short loc_60001303\n");
                codeFragment += _T("mov	edx, [ebp+hProcess]\n");
                codeFragment += _T("push	edx		; hProcess\n");
                codeFragment += _T("call	ds:SymCleanup\n");
                codeFragment += _T("xor	eax, eax");
                codeFragment += _T("jmp	loc_600014AA\n");

                if (!controller.searchFragments(codeFragment, tempAssemblyFilePath)) {
				    tcout << _T("Error occured.") << endl;
#ifdef _DEBUG
                    _getch();
#endif
				    return 1;
			    }

                if (!CBFFileHelper::removeFile((LPCTSTR) tempAssemblyFilePath)) {
                    tcout << _T("Warning: Failed to remove temporary file: ") << tempAssemblyFilePath << endl;
                    return 1;
                }
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
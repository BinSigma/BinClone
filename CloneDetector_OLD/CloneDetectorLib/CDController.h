//-----------------------------------------------------
// Objective:   Control the entire clone dectection process.
//              Entry point to the program.
//
// History:     2011/10/19 - B. Fung (Created)
// 
//-----------------------------------------------------

#pragma once

#include "CDAssemblyFileMgr.h"
#include "CDRegionMgr.h"
#include "CDCloneMgr.h"
#include "CDTokenRefMgr.h"
#include "CDOutputMgr.h"

class CCDController
{
public:
	CCDController(LPCTSTR assemblyFolderPath,
                  LPCTSTR clonePairsFilePath,
                  int windowSize,
                  int stride,
                  int kThreshold,
                  double oThreshold,
                  bool bFindExactClones,
                  bool bFindInexactClones,
                  TCDRegNormalizeLevel regNormLevel,
                  bool bNormalizeToken,
                  int keyVectorSize,
                  double minCoOccThreshold,
                  CDInexactMethod inexactMethod);
	virtual ~CCDController();

    //******************************************************
    // The UI should include two header files: CDController.h and CDDef.hpp
    // The UI should support two modes:
    // Mode #1: Generate clone pairs from assembly code.
    // - Create a CCDController object.
    // - Then call detectClones(), which will produce an XML file.
    // - Then parse the XML file to get all clone files
    // - Then let user to choose a pair of clone files
    // - Then call openAssemblyFile() twice to open the two files.
    // - Then call getRawAssemblyFileLineStr() to get the assembly file content.
    // - And highlight the cloned region in sequence.
    // Mode #2: Open an existing XML file.
    // - Create a CCDController object (use empty strings and -1 in parameters)
    // - Open/parse an existing XML file.
    // - Then let user to choose a pair of clone files
    // - Then call openAssemblyFile() to open the two files.
    // - Then call getRawAssemblyFileLineStr() to get the assembly file content.
    // - And highlight the cloned region in sequence.
    // Mode #3: Search code fragment.
    // - Prompt a textbox to get code fragement from user.
    // - Create a CCDController object
    // - Then call searchFragments(), which will produce an XML file.
    // - Then parse the XML file to get all clone files
    // - Then let user to choose a matched assembly file
    // - Then call openAssemblyFile() to open the selected file.
    // - Then call getRawAssemblyFileLineStr() to get the assembly file content.
    // - And highlight the matched in sequence.
    //******************************************************

    //
    // Parse the assembly files in assemblyFolderPath and 
    // store the clone results to clonePairsFilePath.
    // Return false if error.
    //
    bool detectClones();

    //
    // Search the assembly files in assemblyFolderPath
    // for targetFragement, and store the matched region 
    // to clonePairsFilePath. 
    //
    // (out) tempAssemblyFilePath contains the path to the
    // temporary file. The GUI should delete this 
    // temporary file. The one may use CBFFileHelper::removeFile()
    // to delete the file.
    //
    // Return false if error.
    //
    bool searchFragments(LPCTSTR targetFragment, CString& tempAssemblyFilePath);

    //
    // Open the given raw assembly file. Open only one file at a time.
    // Return false if error.
    //
    bool openRawAssemblyFile(LPCTSTR assemblyFilePath);

    //
    // Retrieve one line from the assembly file.
    // Return false if reached EOF.
    //
    bool getRawAssemblyFileLineStr(CString& str);

    //
    // Close the raw assembly file
    //
    void closeRawAssemblyFile();

private:
    bool run(LPCTSTR filterByFilePath);
    void printTime();
    long getTime();
    long get_runtime();

    CCDAssemblyFileMgr m_assemblyFileMgr;
    CCDRegionMgr m_regionMgr;
    CCDCloneMgr m_cloneMgr;
    CCDTokenRefMgr m_tokRefMgr;
    CCDOutputMgr m_outputMgr;

    bool m_bFindExactClones;
    bool m_bFindInexactClones;
};

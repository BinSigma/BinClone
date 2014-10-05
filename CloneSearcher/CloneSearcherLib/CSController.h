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

//-----------------------------------------------------
// Objective:   Control the entire clone search process.
//              Entry point to the program.
//
// History:     2012/10/6 - B. Fung (Created)
// 
//-----------------------------------------------------

#pragma once

#include "CSDatabaseMgr.h"
#include "CSTokenRefMgr.h"
#include "CSAssemblyFileMgr.h"
#include "CSClone.h"

class CCSController
{
public:
	CCSController(LPCTSTR dbName, LPCTSTR dbUser, LPCTSTR dbPwd);
	virtual ~CCSController();

    // 
    // The GUI should have 3 modes:
    //
    // Mode 1:
    // 1/ Ask user to open a target assembly file.
    // 2/ Call fetchAllParameters() to fetech all possible combinations of parameters.
    // 3/ Get additional parameters: bFindExactClones, bFindInexactClones, minCoOccThreshold.
    // 4/ Call findFileClones() to find all the clones
    // 5/ Display the clones retrieved from pClones
    //
    // Mode 2:
    // Same as mode 1 except that the program should ask the user to provide code fragment rather than an assembly file.
    //
    // Mode 3:
    // Search for specific tokens using searchStrings(), searchConstants(), and searchImports().
    //



    //
    // Parse the assembly files in assemblyFolderPath and
    // convert them, and store them into the database.
    //
    bool populateAssemblyToDB(LPCTSTR assemblyFolderPath,
                              TCSRegNormalizeLevel regNormLevel,
                              int windowSize,
                              int stride);

    //
    // Retrieve all combination of parameters from DB.
    //
    bool fetchAllParameters(CCSParams& params);

    //
    // Retrieve the filePath of a given dbFileID.
    //
    bool fetchFilePath(int dbFileID, CString& assemblyFilePath);

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

    //
    // Find all the clones in DB of the given file.
    // This library will deallocate pClones. Caller does not have to do it.
    // However, the caller can only call findFileClones or findFragmentClones
    // one at a time. The caller has to first finish parsing the pClones, 
    // and then make another call, if necessary.
    // Return false if error.
    //
    bool findFileClones(LPCTSTR targetAssemblyFilePath, 
                        const CCSParam& param,
                        bool bFindExactClones, 
                        bool bFindInexactClones, 
                        double minCoOccThreshold, 
                        const CCSClones*& pClones);

    //
    // Find all the clones in DB of the given fragment.
    // This library will deallocate pClones. Caller does not have to do it.
    // BUT caller should remove the temp file: CBFFileHelper::removeFile((LPCTSTR) tempAssemblyFilePath);
    // Return false if error.
    //
    bool findFragmentClones(LPCTSTR targetFragment, 
                            const CCSParam& param,
                            bool bFindExactClones, 
                            bool bFindInexactClones, 
                            double minCoOccThreshold, 
                            const CCSClones*& pClones,
                            CString& tempAssemblyFilePath);

    //
    // Search for the given string token from the DB.
    //
    bool searchStrings(LPCTSTR tokStr, CCSIndexedTokens& indexedTokens);

    //
    // Search for the given constant token from the DB.
    //
    bool searchConstants(LPCTSTR tokStr, CCSIndexedTokens& indexedTokens);

    //
    // Search for the given import token from the DB.
    //
    bool searchImports(LPCTSTR tokStr, CCSIndexedTokens& indexedTokens);

private:
    bool populateAssemblyToDB();
    void printTime();
    long getTime();
    long get_runtime();

    CCSDatabaseMgr m_dbMgr;
    CCSTokenRefMgr m_tokRefMgr;
    CCSAssemblyFileMgr m_assemblyFileMgr;

    CCSAssemblyFile* m_pTargetAssemblyFile;
    CCSParams m_allParams;
};

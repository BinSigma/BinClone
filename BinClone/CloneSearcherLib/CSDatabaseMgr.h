
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
// Objective:   Entry point to the database
//
// History:     2012/10/6 - B. Fung (Created)
//              2012/10/23 - S. Preda (Implemented Postgre SQL)
// 
//-----------------------------------------------------
#pragma once

#include "CSAssemblyFile.h"
#include "CSIndexedToken.h"
#include "libpq-fe.h"

class CCSDatabaseMgr
{
	PGconn *m_pPGDBconnection;

public:
    CCSDatabaseMgr(LPCTSTR dbName, LPCTSTR dbUser, LPCTSTR dbPwd);
    virtual ~CCSDatabaseMgr();

	inline PGconn* getPGDBConnection() {return m_pPGDBconnection;}

    bool storeParam(CCSParam& param);
    bool fetchParams(CCSParams& params);
    
    bool storeFile(CCSAssemblyFile& assemblyFile, const CCSParam& param);
    bool fetchFilePath(int dbFileID, CString& assemblyFilePath);

    bool storeFunction(CCSAssemblyFunction& assemblyFcn, const CCSParam& param);

    bool storeRegion(CCSRegion& region, const CCSParam& param);
    bool fetchRegions(int dbParamID, UINT hashValue, CCSRegions& cloneRegions);

    bool storeGlobalFeatures(const CStringArray& globalFeatures, const CCSIntArray& globalMedians, const CCSParam& param);
	bool storeFilteredFeatures(const CStringArray& globalFeatures, const CCSIntArray& filteredFeatures, const CCSIntArray& globalMedians, const CCSParam& param);
	bool preInsertInexact2Comb(const CCSIntArray& filteredFeatures, const CCSIntArray& globalMedians, const CCSParam& param);
	bool fetchFeatureVector(const CCSRegion& region, const CCSParam& param);
	bool createTargetRegionBinaryVector(const CCSRegion& region, const CCSParam& param);
	bool constructScoreVector(const CCSParam& param); 	
    bool storeInexact2CombRegion(const CCSRegion& region, const CCSIntArray& filteredFeatures, const CCSParam& param);
    bool fetchInexactScore(const CCSRegion& region, const CCSParam& param);
	bool fetchInexactRegions(const CCSRegion& region, const CCSParam& param, CCSRegions& cloneRegions);
   
    bool storeConstant(CCSIndexedToken& indexedToken);
    bool fetchConstants(LPCTSTR tokStr, CCSIndexedTokens& indexedTokens);

    bool storeString(CCSIndexedToken& indexedToken);
    bool fetchStrings(LPCTSTR tokStr, CCSIndexedTokens& indexedTokens);

    bool storeImport(CCSIndexedToken& indexedToken);
    bool fetchImports(LPCTSTR tokStr, CCSIndexedTokens& indexedTokens);

	inline void setMinCoOccThreshold (double minCoOccThreshold) {m_minCoOccThreshold = minCoOccThreshold;};
	inline Int64u getNDBTotalRegions() {return m_nDBTotalRegions; }; 
	bool setInitialVariables(const CCSParam& param); 

	bool setTotalNCombination(const CCSParam& param);

private:
	std::vector<std::vector<int>> m_scoreVector;  
	CCSFeatureVector m_targetRegionVector;	// Target region feature vector determined from DB
	CCSBoolArray m_targetpRegionBinaryVector;
	CCSIntArray m_filteredFeatures;
	double m_minCoOccThreshold;
	double m_actualminCoOccThreshold;
	
	int m_firstDBRegionIDInt;
	Int64u m_nDBTotalRegions;	
	CCSClones m_clones;


	CCSAssemblyFile* m_assemblyFile;	
};

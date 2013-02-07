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
    
    bool storeFile(CCSAssemblyFile& assemblyFile);
    bool fetchFilePath(int dbFileID, CString& assemblyFilePath);

    bool storeFunction(CCSAssemblyFunction& assemblyFcn, const CCSParam& param);

    bool storeRegion(CCSRegion& region, const CCSParam& param);
    bool storeGlobalFeatures(const CStringArray& globalFeatures, const CCSIntArray& globalMedians, const CCSParam& param); 
    bool storeInexact2CombRegion(const CCSRegion& region, const CCSIntArray& medianNZ, const CCSParam& param);
    bool fetchRegions(int dbParamID, UINT hashValue, CCSRegions& cloneRegions);

    bool storeVectorFeatureNames(int dbParamID, const CStringArray& vectorFeatureNames);
    bool fetchVectorFeatureNames(int dbParamID, CStringArray& vectorFeatureNames);

    bool storeRegionVector(int dbParamID, int dbRegionID, const CStringArray& vectorFeatureNames, const CCSFeatureVector& vector);
    bool computeMedianVector(int dbParamID, CCSFeatureVector& vector);
    
    bool storeConstant(CCSIndexedToken& indexedToken);
    bool fetchConstants(LPCTSTR tokStr, CCSIndexedTokens& indexedTokens);

    bool storeString(CCSIndexedToken& indexedToken);
    bool fetchStrings(LPCTSTR tokStr, CCSIndexedTokens& indexedTokens);

    bool storeImport(CCSIndexedToken& indexedToken);
    bool fetchImports(LPCTSTR tokStr, CCSIndexedTokens& indexedTokens);
};

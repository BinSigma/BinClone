#include "StdAfx.h"
#include "CSDatabaseMgr.h"


CCSDatabaseMgr::CCSDatabaseMgr(LPCTSTR dbName, LPCTSTR dbUser, LPCTSTR dbPwd)
{
    // initiate a connection object
    // better dynamically allocate a connection object with a pointer.
    // If the object is null, then it means the connection fails.
    
    CStringA dbNameANSI, dbUserANSI, dbPwdANSI;
	if (!CBFStrHelper::convertCStringToCStringA(dbName, dbNameANSI)) {
        tcout << _T("CSDataBaseMgr: failed to convert UNICODE string: ") << dbName << endl;
        ASSERT(false);
        return;
    }
	if (!CBFStrHelper::convertCStringToCStringA(dbUser, dbUserANSI)) {
        tcout << _T("CSDataBaseMgr: failed to convert UNICODE string: ") << dbUser << endl;
        ASSERT(false);
        return;
    }
	if (!CBFStrHelper::convertCStringToCStringA(dbPwd, dbPwdANSI)) {
        tcout << _T("CSDataBaseMgr: failed to convert UNICODE password.") << endl;
        ASSERT(false);
        return;
    }
    
    CStringA connectStr = "dbname = \'";
    connectStr += dbNameANSI + "\'";
    connectStr += " user = \'";
    connectStr += dbUserANSI + "\'";
    connectStr += " password = \'";
    connectStr += dbPwdANSI + "\'";
    connectStr += " connect_timeout = \'3\'";
	m_pPGDBconnection = PQconnectdb((LPCSTR) connectStr);	
	if (!m_pPGDBconnection) {
		tcout << _T("CSDataBaseMgr: failed to initiate a connection to the Postgre Database.") <<  endl;
        ASSERT(false);
        return;
    }
    PQexec(getPGDBConnection(), "SET client_encoding = \'WIN1252\'");
}


CCSDatabaseMgr::~CCSDatabaseMgr()
{
    // close and deallocate the connection object
	PQfinish(m_pPGDBconnection);
}

//
// Store one combination of oparameters. Return existing paramID if this combination of parameters already exists.
// bNormalizeToken is always true, right ?
//
bool CCSDatabaseMgr::storeParam(CCSParam& param)
{
	//windowSize | stride | kThreshold | regNormLevel | bNormalizeToken | dbParamID
	//-----------+--------+------------+--------------+-----------------+-----------
	//Any other character following a backslash is taken literally. Thus, to include a backslash character, write two backslashes (\\). (http://www.postgresql.org/docs/8.4/static/sql-syntax-lexical.html#SQL-SYNTAX-DOLLAR-QUOTING)
	if (PQstatus(m_pPGDBconnection) == CONNECTION_BAD) {
		tcout << _T("CSDataBaseMgr (storeParam): Bad Connection with the Server") <<  endl; // we could use PQreset, for the moment we go with this. 
		ASSERT(false);
		return false;
	}

	PGresult* pgresult; // result of the last query
	CString csSqlCheckParam = _T("SELECT \"dbParamID\" from \"Parameter\" WHERE ");
	CString csSqlVals;
	csSqlVals.Format(_T("\"windowSize\"=%d AND \"stride\"=%d AND \"regNormLevel\"=%d"), param.m_windowSize, param.m_stride, param.m_regNormLevel);  
	csSqlCheckParam += csSqlVals;
    CStringA csSqlCheckParamANSI;
	if (!CBFStrHelper::convertCStringToCStringA(csSqlCheckParam, csSqlCheckParamANSI)) {
        tcout << _T("Failed to convert UNICODE string: ") << csSqlCheckParam.GetString() << endl;
        ASSERT(false);
        return false;
    }

	pgresult = PQexec(getPGDBConnection(), (LPCSTR) csSqlCheckParamANSI);  
	if(PQresultStatus(pgresult) != PGRES_TUPLES_OK) { // PGRES_TUPLES_OK even if 0 tuples returned
	    tcout << _T("CSDataBaseMgr (storeParam): Bad query to check if the set of params exists in the database") <<  endl; 
		ASSERT(false);
		return false;
	}
    if (PQntuples(pgresult)) { //the params are in the database
	    CStringA csParamIDAnsi(PQgetvalue(pgresult, 0, 0));
        CString csParamID;
        // Delete the features from the table FeatureTable with ParamIDFkey = csParamIDAnsi.GetString();
        PQexec(getPGDBConnection(), (LPCSTR) (""));


        if(!CBFStrHelper::convertCStringAToCString(csParamIDAnsi, csParamID)) {
	        tcout << _T("Failed to convert ANSI to Unicode string:") << csParamIDAnsi.GetString() << endl; 
		    ASSERT(false);
		    return false;                     
         }
		 param.m_dbParamID = CBFStrHelper::strToInt((LPCTSTR) csParamID);
		 PQclear(pgresult);
	}
	else {
        // bfung: Add a boolean variable bNormalizeToken to the database table Parameter

		CString csSqlInsertParam = _T("INSERT into \"Parameter\" VALUES(");
		CString csSqlValues;
        //CString csNormalizeToken;
        //CBFStrHelper::convertBoolToCString(param.m_bNormalizeToken, csNormalizeToken);
		csSqlValues.Format(_T("%d, %d, %d, DEFAULT)"), param.m_windowSize, param.m_stride, param.m_regNormLevel);
		csSqlInsertParam += csSqlValues;

        CStringA csSqlInsertParamANSI;
	    if (!CBFStrHelper::convertCStringToCStringA(csSqlInsertParam, csSqlInsertParamANSI)) {
            tcout << _T("Failed to convert UNICODE string: ") << csSqlInsertParam.GetString() << endl;
            ASSERT(false);
            return false;
        }
		pgresult = PQexec(getPGDBConnection(), (LPCSTR) csSqlInsertParamANSI);
		if(PQresultStatus(pgresult) != PGRES_COMMAND_OK) { // PGRES_COMMAND_OK for a command which does not return tuples
			tcout << _T("CSDataBaseMgr (storeParam): Failing to insert the parameters in the database") <<  endl; 
			ASSERT(false);
			return false;
		} 
		PQclear(pgresult);
		pgresult = PQexec(getPGDBConnection(), (LPCSTR) csSqlCheckParamANSI);
		if(PQresultStatus(pgresult) != PGRES_TUPLES_OK) { // PGRES_COMMAND_OK for a command which does not return tuples
			tcout << _T("CSDataBaseMgr (storeParam): Failing to retrieve dbParamID parameter from the database") <<  endl; 
			ASSERT(false);
			return false;
		}
		CStringA csParamDbIDAnsi(PQgetvalue(pgresult, 0, 0));
        CString csParamDbID;
        if(!CBFStrHelper::convertCStringAToCString(csParamDbIDAnsi, csParamDbID)) {
	        tcout << _T("Failed to convert ANSI to Unicode string:") << csParamDbIDAnsi.GetString() << endl; 
	        ASSERT(false);
	        return false;                     
        }
        param.m_dbParamID = CBFStrHelper::strToInt((LPCTSTR) csParamDbID); 
		PQclear(pgresult);
	}
    return param.m_dbParamID > 0;
}

//
// Retreive all combinations of oparameters.
//
bool CCSDatabaseMgr::fetchParams(CCSParams& params)
{
    // Return an array of params by reference. You can use something like:
    // windowSize | stride | kThreshold | regNormLevel | bNormalizeToken | dbParamID
	//------------+--------+------------+--------------+-----------------+-----------
    //CCSParam* pNewParam = new CCSParam(<put parameters here>);
    //if (!params.addParam(pNewParam))
    //    return false;
    PGresult* pgresult; // result of the last query	
    if (PQstatus(m_pPGDBconnection) == CONNECTION_BAD) {
		tcout << _T("CSDataBaseMgr (fetchParams): Bad Connection with the Server") <<  endl; // we could use PQreset, for the moment we go with this. 
		ASSERT(false);
		return false;
	}
    CString csSqlFetchAllParams = _T("SELECT * from \"Parameter\"");
    CStringA csSqlFetchAllParamsANSI;
	if (!CBFStrHelper::convertCStringToCStringA(csSqlFetchAllParams, csSqlFetchAllParamsANSI)) {
        tcout << _T("Failed to convert UNICODE string: ") << csSqlFetchAllParams.GetString() << endl;
        ASSERT(false);
        return false;
    }
    pgresult = PQexec(getPGDBConnection(), (LPCSTR) csSqlFetchAllParamsANSI);
	if(PQresultStatus(pgresult) != PGRES_TUPLES_OK) { // PGRES_TUPLES_OK even if 0 tuples returned
		tcout << _T("CSDataBaseMgr (fetchParams): Bad query to fetch the Parameter from the database") <<  endl; 
		ASSERT(false);
		return false;
	}
    CCSParam* pNewParam = NULL;
    CStringA regNormLevelAnsi, windowSizeAnsi, strideAnsi, dbParamIDAnsi;
    bool bNormalizeToken = false;
    CString regNormLevel, windowSize, stride, kThreshold, dbParamID;
    for (int i = 0; i < PQntuples(pgresult); i++) {
        regNormLevelAnsi = PQgetvalue(pgresult, i, 3);
        windowSizeAnsi = PQgetvalue(pgresult, i, 0);
        strideAnsi = PQgetvalue(pgresult, i, 1);        
        dbParamIDAnsi = PQgetvalue(pgresult, i, 5);
        if(!(CBFStrHelper::convertCStringAToCString(regNormLevelAnsi, regNormLevel) && CBFStrHelper::convertCStringAToCString(windowSizeAnsi, windowSize) && CBFStrHelper::convertCStringAToCString(strideAnsi, stride) && CBFStrHelper::convertCStringAToCString(dbParamIDAnsi, dbParamID))) {
	        tcout << _T("Failed to convert ANSI to Unicode strings: ") << regNormLevelAnsi.GetString() << _T(", ") << windowSizeAnsi.GetString() << _T(", ") << strideAnsi.GetString() << _T(", ") << _T(", ") << dbParamIDAnsi.GetString() << endl; 
	        ASSERT(false);
	        return false;                     
        }
        // bfung: Add a boolean variable bNormalizeToken to the database table Parameter
        pNewParam = new CCSParam((TCSRegNormalizeLevel) CBFStrHelper::strToInt((LPCTSTR) regNormLevel), 
                                  CBFStrHelper::strToInt((LPCTSTR) windowSize), 
                                  CBFStrHelper::strToInt((LPCTSTR) stride));
        pNewParam->m_dbParamID = CBFStrHelper::strToInt((LPCTSTR) dbParamID);
        if (!params.addParam(pNewParam)) {
            ASSERT(false);
            return false;
        }
    }
	PQclear(pgresult);
    return true;
}

//
// Store file. Put the fileID in m_dbFileID. What if file already exists?
// 
bool CCSDatabaseMgr::storeFile(CCSAssemblyFile& assemblyFile)
{
    // if m_filePath already exists in DB, then delete all functions and regions assoicated with this fileID.
    // then store filePath and assign a unique ID to m_dbFileID,
	// if m_filePath already exists in DB, then retrieve fileID and put in m_dbFileID, delete all functions and regions assoicated with this fileID. This is automatically done by CASCADE on DELETE with Foreign Keys
	
	// dbFileID |  filePath
	//----------+-------------
	int piv = 0;
	PGresult* pgresult; // result of the last query
	if (PQstatus(m_pPGDBconnection) == CONNECTION_BAD) {
		tcout << _T("CSDataBaseMgr (storeFile): Bad Connection with the Server") <<  endl; // we could use PQreset, for the moment we go with this. 
		ASSERT(false);
		return false;
	}
	CString csSqlCheckFile = _T("SELECT \"dbFileID\" from \"File\" WHERE \"filePath\"=E'");
	csSqlCheckFile += assemblyFile.getFilePath();
	csSqlCheckFile += _T("'");

    CStringA csSqlCheckFileANSI;
	if (!CBFStrHelper::convertCStringToCStringA(csSqlCheckFile, csSqlCheckFileANSI)) {
        tcout << _T("Failed to convert UNICODE string: ") << csSqlCheckFile.GetString() << endl;
        ASSERT(false);
        return false;
    }
	csSqlCheckFileANSI.Replace((LPCSTR)"\\", (LPCSTR)"\\\\");
	pgresult = PQexec(getPGDBConnection(), (LPCSTR) csSqlCheckFileANSI);  
	if(PQresultStatus(pgresult) != PGRES_TUPLES_OK) { // PGRES_TUPLES_OK even if 0 tuples returned
		tcout << _T("CSDataBaseMgr (storeFile): Bad query to check if ") <<  assemblyFile.getFilePath().GetString() << _T(" exists in the data base") <<  endl; 
		ASSERT(false);
		return false;
	}
	if(PQntuples(pgresult)) { // there is at least one record in File : we reuse the dbFileID
		// getting the existing fileID from the DB : char -> int
		CStringA csFileIDAnsi(PQgetvalue(pgresult, 0, 0));
        CString csFileID;
        if(!CBFStrHelper::convertCStringAToCString(csFileIDAnsi, csFileID)) {
	        tcout << _T("Failed to convert ANSI to Unicode string:") << csFileIDAnsi.GetString() << endl; 
	        ASSERT(false);
	        return false;                     
        }
		assemblyFile.m_dbFileID = CBFStrHelper::strToInt(csFileID.GetString());
		piv = assemblyFile.m_dbFileID;
		PQclear(pgresult);
		CString csSqlDeleteExistingFile = _T("DELETE from \"File\" WHERE \"filePath\" = E'");
		csSqlDeleteExistingFile += assemblyFile.getFilePath();
		csSqlDeleteExistingFile += _T("'");

        CStringA ccSqlDeleteExistingFileANSI;
	    if (!CBFStrHelper::convertCStringToCStringA(csSqlDeleteExistingFile, ccSqlDeleteExistingFileANSI)) {
            tcout << _T("Failed to convert UNICODE string: ") << csSqlDeleteExistingFile.GetString() << endl;
            ASSERT(false);
            return false;
        }
		ccSqlDeleteExistingFileANSI.Replace((LPCSTR)"\\", (LPCSTR)"\\\\");
		pgresult = PQexec(getPGDBConnection(), (LPCSTR) ccSqlDeleteExistingFileANSI);
		if(PQresultStatus(pgresult) != PGRES_COMMAND_OK) { // PGRES_COMMAND_OK for a command which does not return tuples
			tcout << _T("CSDataBaseMgr (storeFile): Failing to delete ") <<  assemblyFile.getFilePath().GetString() << _T(" from the data base") <<  endl; 
			ASSERT(false);
			return false;
		} 
        tcout << _T("Inserting an already analysed file in the database. First, delete the corresponding record with : ") << ccSqlDeleteExistingFileANSI.GetString() << _T(";   Done !") << endl;
		PQclear(pgresult);
	}
	else // store filePath
		PQclear(pgresult);

	CString csSqlStoreFile = _T("INSERT INTO \"File\" VALUES (");
	if (piv) {
		CString dbFileID;
		dbFileID.Format(_T("%d"), piv);	
		csSqlStoreFile += dbFileID;
		//csSqlCheckFile += _T("'"); // I think it also works without ''
	}
	else 
		csSqlStoreFile += _T("DEFAULT");

	csSqlStoreFile += _T(", E'");
	csSqlStoreFile += assemblyFile.getFilePath();
	csSqlStoreFile += _T("')");
	CStringA ccSqlStoreFileANSI;
	if (!CBFStrHelper::convertCStringToCStringA(csSqlStoreFile, ccSqlStoreFileANSI)) {
        tcout << _T("Failed to convert UNICODE string: ") << csSqlStoreFile.GetString() << endl;
        ASSERT(false);
        return false;
    }
	// replace "\" with "\\"
	ccSqlStoreFileANSI.Replace((LPCSTR)"\\", (LPCSTR)"\\\\");
	pgresult = PQexec(getPGDBConnection(), (LPCSTR) ccSqlStoreFileANSI);
	if(PQresultStatus(pgresult) != PGRES_COMMAND_OK) { // PGRES_COMMAND_OK for a command which does not return tuples
		tcout << _T("CSDataBaseMgr (storeFile): Failing to insert ") <<  assemblyFile.getFilePath().GetString() << _T(" in the data base") <<  endl;
		ASSERT(false);
		return false;
	} 	
	PQclear(pgresult);
	// delete existing file OK and store OK
	// the file was insterted anyway, we need to retrieve m_dbFileID for the case DEFAULT
	if (!piv) {
		CString csSqlFetchFileID = _T("SELECT \"dbFileID\" from \"File\" WHERE \"filePath\"=E'");
		csSqlFetchFileID += assemblyFile.getFilePath();
		csSqlFetchFileID += _T("'");

        CStringA csSqlFetchFileIDANSI;
	    if (!CBFStrHelper::convertCStringToCStringA(csSqlFetchFileID, csSqlFetchFileIDANSI)) {
            tcout << _T("Failed to convert UNICODE string: ") << csSqlFetchFileID.GetString() << endl;
            ASSERT(false);
            return false;
        }
		csSqlFetchFileIDANSI.Replace((LPCSTR)"\\", (LPCSTR)"\\\\");
		pgresult = PQexec(getPGDBConnection(), (LPCSTR) csSqlFetchFileIDANSI);
		if(PQresultStatus(pgresult) != PGRES_TUPLES_OK) { // PGRES_TUPLES_OK even if 0 tuples returned
			tcout << _T("CSDataBaseMgr (storeFile): failed to fetch the dbFileID for the file ") <<  assemblyFile.getFilePath().GetString() << _T(" from the data base") <<  endl; 
			ASSERT(false);
			return false;
		}
		CStringA csFileIDAnsi(PQgetvalue(pgresult, 0, 0));
        CString csFileID;
        if(!CBFStrHelper::convertCStringAToCString(csFileIDAnsi, csFileID)) {
	        tcout << _T("Failed to convert ANSI to Unicode string:") << csFileIDAnsi.GetString() << endl; 
	        ASSERT(false);
	        return false;                     
        }
        assemblyFile.m_dbFileID = CBFStrHelper::strToInt(csFileID.GetString());
		PQclear(pgresult);
	}
    return assemblyFile.m_dbFileID > 0;
}

//
// Fetch file path of the given dbFileID.
//
bool CCSDatabaseMgr::fetchFilePath(int dbFileID, CString& assemblyFilePath)
{
    PGresult* pgresult; // result of the last query	
    if (PQstatus(m_pPGDBconnection) == CONNECTION_BAD) {
		tcout << _T("CSDataBaseMgr (fetchFilePath): Bad Connection with the Server") <<  endl; // we could use PQreset, for the moment we go with this. 
		ASSERT(false);
		return false;
	}
    CString csSqlFetchFilePath; 
    csSqlFetchFilePath.Format(_T("SELECT \"filePath\" from \"File\" where \"dbFileID\"=%d"), dbFileID);
    CStringA csSqlFetchFilePathANSI;
	if (!CBFStrHelper::convertCStringToCStringA(csSqlFetchFilePath, csSqlFetchFilePathANSI)) {
        tcout << _T("Failed to convert UNICODE string: ") << csSqlFetchFilePath.GetString() << endl;
        ASSERT(false);
        return false;
    }    
	pgresult = PQexec(getPGDBConnection(), (LPCSTR) csSqlFetchFilePathANSI);  
	if(PQresultStatus(pgresult) != PGRES_TUPLES_OK) { // PGRES_TUPLES_OK even if 0 tuples returned. Unique for a pathfile
		tcout << _T("CSDataBaseMgr (fetchFilePath): Bad query to fetch the file path corresponding to dbFileID = ") << dbFileID << _T(" in the database;") <<  endl; 
		ASSERT(false);
		return false;
	}
    CStringA assemblyFilePathAnsi(PQgetvalue(pgresult, 0, 0));
    if(!CBFStrHelper::convertCStringAToCString(assemblyFilePathAnsi, assemblyFilePath)) { // if the filePath returned by the SQL query is NULL (i.e., there is no file path corresponding to the dbFileID) then PQgetvalue returns an empty string. There is an API to test if the field is null : PQgetisnull 
	    tcout << _T("Failed to convert ANSI to Unicode string:") << assemblyFilePathAnsi.GetString() << endl; 
	    ASSERT(false);
	    return false;                     
    }    
    PQclear(pgresult);
    return true;
}

//
// Store function. Put the functionID in m_dbFcnID.
// 
bool CCSDatabaseMgr::storeFunction(CCSAssemblyFunction& assemblyFcn, const CCSParam& param)
{
    // Store the fileID from assemblyFcn.getAssemblyFile()->m_dbFileID;
    // Store the hash value from assemblyFcn.m_hashValue;
    // Store the start, end, startRaw, and endRaw.
    // Put the functionID in m_dbFcnID.
	//  dbFcnID | startIdx | endIdx | dbHashValueFcn | FileIdFKey | startRawIdx | endRawIdx
	// ---------+----------+--------+----------------+------------+-------------+-----------

	PGresult *pgresult;
	if (PQstatus(m_pPGDBconnection) == CONNECTION_BAD) {
		tcout << _T("CSDataBaseMgr (storeFunction): Bad Connection with the Server") <<  endl; // we could use PQreset, for the moment we go with this. 
		ASSERT(false);
		return false;
	}
	CString csSqlInsertFunction = _T("INSERT into \"Function\" VALUES (DEFAULT, ");
	CString csValues;
	csValues.Format(_T("%d, %d, %d, %d, %d, %d)"), assemblyFcn.m_startIdx, assemblyFcn.m_endIdx, (int)assemblyFcn.m_hashValue, assemblyFcn.getAssemblyFile()->m_dbFileID, assemblyFcn.m_startRawIdx, assemblyFcn.m_endRawIdx);
	csSqlInsertFunction += csValues;	
    CStringA csSqlInsertFunctionANSI;
	if (!CBFStrHelper::convertCStringToCStringA(csSqlInsertFunction, csSqlInsertFunctionANSI)) {
        tcout << _T("Failed to convert UNICODE string: ") << csSqlInsertFunction.GetString() << endl;
        ASSERT(false);
        return false;
    }
	pgresult = PQexec(getPGDBConnection(), (LPCSTR) csSqlInsertFunctionANSI);
	if(PQresultStatus(pgresult) != PGRES_COMMAND_OK) { // PGRES_COMMAND_OK for a command which does not return tuples
		tcout << _T("CSDataBaseMgr (storeFunction): Failing to insert the function from ") << (assemblyFcn.getAssemblyFile()->getFilePath()).GetString() << _T(", beginning at line no. ") << assemblyFcn.m_startIdx << _T(" in the data base") <<  endl;
		ASSERT(false);
		return false;
	}
	PQclear(pgresult);
	CString csSqlFetchFcnId = _T("SELECT \"dbFcnID\" from \"Function\" WHERE \"FileIdFKey\"=");
	CString csVals;
	csVals.Format(_T("%d AND \"startRawIdx\"=%d"), assemblyFcn.getAssemblyFile()->m_dbFileID, assemblyFcn.m_startRawIdx);
	csSqlFetchFcnId += csVals;

    CStringA csSqlFetchFcnIdANSI;
	if (!CBFStrHelper::convertCStringToCStringA(csSqlFetchFcnId, csSqlFetchFcnIdANSI)) {
        tcout << _T("Failed to convert UNICODE string: ") << csSqlFetchFcnId.GetString() << endl;
        ASSERT(false);
        return false;
    }
	pgresult = PQexec(getPGDBConnection(), (LPCSTR) csSqlFetchFcnIdANSI);
	if(PQresultStatus(pgresult) != PGRES_TUPLES_OK) { // PGRES_TUPLES_OK even if 0 tuples returned
		tcout << _T("CSDataBaseMgr (storeFunction): failed to fetch the dbFcnID from ") <<  (assemblyFcn.getAssemblyFile()->getFilePath()).GetString() << _T(", beginning at line no. ") << assemblyFcn.m_startIdx << _T(" in the data base") <<  endl;
		ASSERT(false);
		return false;
	}
	CStringA csFncIDAnsi(PQgetvalue(pgresult, 0, 0));
    CString csFncID;
    if(!CBFStrHelper::convertCStringAToCString(csFncIDAnsi, csFncID)) {
	    tcout << _T("Failed to convert ANSI to Unicode string:") << csFncIDAnsi.GetString() << endl; 
	    ASSERT(false);
	    return false;                     
    }
	assemblyFcn.m_dbFcnID = CBFStrHelper::strToInt(csFncID.GetString());
	PQclear(pgresult);
    return assemblyFcn.m_dbFcnID > 0;
}

//
// Store region. Put the regionID in m_dbRegionID.
// 
bool CCSDatabaseMgr::storeRegion(CCSRegion& region, const CCSParam& param)
{    
    // Store the fcnID from region.m_dbFcnID;
    // Store the hash value from region.m_hashValue;
    // Store the start, end, startRaw, and endRaw.
    // Put the regionID in m_dbRegionID.

	// dbRegionID | startIdx | endIdx | rawStartIdx | rawEndIdx | hashValue | FcnIDFkey | ParamIDFKey
	//------------+----------+--------+-------------+-----------+-----------+-----------+-------------
	PGresult *pgresult;
	if (PQstatus(m_pPGDBconnection) == CONNECTION_BAD) {
		tcout << _T("CSDataBaseMgr (storeRegion): Bad Connection with the Server") <<  endl; // we could use PQreset, for the moment we go with this. 
		ASSERT(false);
		return false;
	}
	CString csSqlInsertRegion = _T("INSERT into \"Region\" VALUES (DEFAULT, ");
	CString csValues;
	csValues.Format(_T("%d, %d, %d, %d, %d, %d, %d)"), region.m_startIdx, region.m_endIdx, region.m_rawStartIdx, region.m_rawEndIdx, (int)region.m_hashValue, region.getFunction()->m_dbFcnID, param.m_dbParamID);
	csSqlInsertRegion += csValues;

    CStringA csSqlInsertRegionANSI;
	if (!CBFStrHelper::convertCStringToCStringA(csSqlInsertRegion, csSqlInsertRegionANSI)) {
        tcout << _T("Failed to convert UNICODE string: ") << csSqlInsertRegion.GetString() << endl;
        ASSERT(false);
        return false;
    }
	pgresult = PQexec(getPGDBConnection(), (LPCSTR) csSqlInsertRegionANSI);
	if(PQresultStatus(pgresult) != PGRES_COMMAND_OK) { // PGRES_COMMAND_OK for a command which does not return tuples
		tcout << _T("CSDataBaseMgr (storeRegion): Failing to insert the region from ") << (region.getFunction()->getAssemblyFile()->getFilePath()).GetString() << _T(", beginning at line no. ") << region.m_rawStartIdx << _T(" in the data base") <<  endl;
		ASSERT(false);
		return false;
	}
	PQclear(pgresult);
	CString csSqlFetchRegId = _T("SELECT \"dbRegionID\" from \"Region\" WHERE \"FcnIDFkey\"=");
	CString csVals;
	csVals.Format(_T("%d AND \"rawStartIdx\"=%d"), region.getFunction()->m_dbFcnID, region.m_rawStartIdx);
	csSqlFetchRegId += csVals;

    CStringA csSqlFetchRegIdANSI;
	if (!CBFStrHelper::convertCStringToCStringA(csSqlFetchRegId, csSqlFetchRegIdANSI)) {
        tcout << _T("Failed to convert UNICODE string: ") << csSqlFetchRegId.GetString() << endl;
        ASSERT(false);
        return false;
    }
	pgresult = PQexec(getPGDBConnection(), (LPCSTR) csSqlFetchRegIdANSI);
	if(PQresultStatus(pgresult) != PGRES_TUPLES_OK) { // PGRES_TUPLES_OK even if 0 tuples returned
		tcout << _T("CSDataBaseMgr (storeRegion): failed to fetch the dbRegionID from ") <<  (region.getFunction()->getAssemblyFile()->getFilePath()).GetString() << _T(", beginning at line no. ") << region.m_rawStartIdx << _T(" in the data base") <<  endl;
		ASSERT(false);
		return false;
	}
	CStringA csRegIDAnsi(PQgetvalue(pgresult, 0, 0));
    CString csRegID;
    if(!CBFStrHelper::convertCStringAToCString(csRegIDAnsi, csRegID)) {
	    tcout << _T("Failed to convert ANSI to Unicode string:") << csRegIDAnsi.GetString() << endl; 
	    ASSERT(false);
	    return false;                     
    }
    region.m_dbRegionID = CBFStrHelper::strToInt(csRegID.GetString());
	PQclear(pgresult);
    return region.m_dbRegionID > 0;
}

//
// Store global features.
// 
bool CCSDatabaseMgr::storeGlobalFeatures(const CStringArray& globalFeatures, const CCSIntArray& globalMedians, const CCSParam& param)
{
	// paramIDFKey | featID | featName | featMedian
    //-------------+--------+----------------------
    
    PGresult *pgresult;
	if (PQstatus(m_pPGDBconnection) == CONNECTION_BAD) {
		tcout << _T("CSDataBaseMgr (storeFeature): Bad Connection with the Server") <<  endl; // we could use PQreset, for the moment we go with this. 
		ASSERT(false);
		return false;
	}        
    CStringA parameterId; 
    CStringA csSqlCheckFeatureANSI = "Select * from \"Feature\" where \"featName\"=\'regreg\' and \"paramIDFKey\"=";
    parameterId.Format("%d", param.m_dbParamID);
    csSqlCheckFeatureANSI += parameterId;
    pgresult = PQexec(getPGDBConnection(), csSqlCheckFeatureANSI);

    if(PQntuples(pgresult))
        return true; 
    else {
        for (int i = 0; i < globalFeatures.GetSize() - 1; ++i) {
            CString csSqlInsertFeatures = _T("INSERT into \"Feature\" VALUES(");   
            CString csValues;
            csValues.Format(_T("%d, %d, \'%s\', %d)"), param.m_dbParamID, i, globalFeatures.GetAt(i).GetString(), globalMedians.GetAt(i));
	        csSqlInsertFeatures =csSqlInsertFeatures + csValues;	
            CStringA csSqlInsertFeatureANSI;
            if (!CBFStrHelper::convertCStringToCStringA(csSqlInsertFeatures, csSqlInsertFeatureANSI)) {
                tcout << _T("Failed to convert UNICODE string: ") << csSqlInsertFeatures.GetString() << endl;
                ASSERT(false);
                return false;
            }
            pgresult = PQexec(getPGDBConnection(), (LPCSTR) csSqlInsertFeatureANSI);
	        if(PQresultStatus(pgresult) != PGRES_COMMAND_OK) { // PGRES_COMMAND_OK for a command which does not return tuples
                tcout << _T("CSDataBaseMgr (storeFeature): Failing to insert the feature name ") << globalFeatures.GetAt(i).GetString() << _T(" in the data base") <<  endl;
		        ASSERT(false);
		        return false;
            }	   

        }
    PQclear(pgresult);    
    }    
    return true;
}

//
// Pre-insert Inexact2Com table
//
bool CCSDatabaseMgr::preInsertInexact2Comb (const CCSIntArray& filteredFeatures, const CCSIntArray& globalMedians, const CCSParam& param)
{
	PGresult *pgresult;
    if (PQstatus(m_pPGDBconnection) == CONNECTION_BAD) {
        tcout << _T("CSDataBaseMgr (storeInexactRegion): Bad Connection with the Server") <<  endl; // we could use PQreset, for the moment we go with this. 
        ASSERT(false);
        return false;
	} 
	CStringA parameterID; 
    CStringA csSqlCheckInexactANSI = "Select * from \"Inexact2Comb\" where \"paramIDFKey\"=";
    parameterID.Format("%d", param.m_dbParamID);
    csSqlCheckInexactANSI += parameterID;
    pgresult = PQexec(getPGDBConnection(), csSqlCheckInexactANSI);

    if(PQntuples(pgresult))
        return true; 
	else {
		for (int i = 0; i < filteredFeatures.GetSize()-1; ++i) {
			for (int j = i + 1; j < filteredFeatures.GetSize(); ++j) {
			CString parameterIDString; 
				CString featAIDFKeyString;
				CString featBIDFKeyString;
				CString featAPresentString;
				CString featBPresentString;
				parameterIDString.Format(_T("%d"), param.m_dbParamID);
				featAIDFKeyString.Format(_T("%d"), filteredFeatures.GetAt(i));
				featBIDFKeyString.Format(_T("%d"), filteredFeatures.GetAt(j));
				for (int k = 0; k < 4; k++) {
					CString csSqlPreInsertInexact2Comb = _T("INSERT into \"Inexact2Comb\" VALUES(");   
					if (k == 0) { featAPresentString = _T("T"); featBPresentString = _T("T"); }
					if (k == 1) { featAPresentString = _T("T"); featBPresentString = _T("F"); }
					if (k == 2) { featAPresentString = _T("F"); featBPresentString = _T("T"); }
					if (k == 3) { featAPresentString = _T("F"); featBPresentString = _T("F"); }			
					CString csValues;
					csValues.Format(_T("%d, %d, %d, \'%s\', \'%s\', \'%s\')"), param.m_dbParamID, filteredFeatures.GetAt(i), filteredFeatures.GetAt(j), featAPresentString, featBPresentString, _T("-1"));
					csSqlPreInsertInexact2Comb += csValues;	
					CStringA csSqlPreInsertInexact2CombANSI;
					if (!CBFStrHelper::convertCStringToCStringA(csSqlPreInsertInexact2Comb, csSqlPreInsertInexact2CombANSI)) {
						tcout << _T("Failed to convert UNICODE string: ") << csSqlPreInsertInexact2Comb.GetString() << endl;
						ASSERT(false);
						return false;
					}
					pgresult = PQexec(getPGDBConnection(), (LPCSTR) csSqlPreInsertInexact2CombANSI);
					if(PQresultStatus(pgresult) != PGRES_COMMAND_OK) { // PGRES_COMMAND_OK for a command which does not return tuples
					   tcout << _T("CSDataBaseMgr (storeInexactRegion): Failing to Pre insert into Inexact2Comb ") << filteredFeatures.GetAt(i) << _T(" in the data base") <<  endl;
					   ASSERT(false);
					   return false;    
					}
				}		
			}		
		}
		CStringA createIndex; // create index to optimize searching
		CStringA csSqlcreateIndexANSI = "Create INDEX \"InexactIndex\" ON \"Inexact2Comb\" (\"paramIDFKey\", \"featAIDFKey\", \"featBIDFKey\", \"featAPresent\", \"featBPresent\")";
		pgresult = PQexec(getPGDBConnection(), csSqlcreateIndexANSI);
	}
	PQclear(pgresult); 
    return true;
}


//
// Store hash region.
// 
bool CCSDatabaseMgr::storeInexact2CombRegion(const CCSRegion& region, const CCSIntArray& medianNZ , const CCSParam& param)
{
	// paramIDFKey | featID | featName
    //-------------+--------+---------

    // paramIDFKey | featAIDFKey | featBIDFKey | featAPresent | featBPresent | dbRegionID
    // integer     | integer     | integer     | boolean      | boolean      | integer
	//-------------+-------------+-------------+--------------+--------------+-----------
    
           
    PGresult *pgresult;
    if (PQstatus(m_pPGDBconnection) == CONNECTION_BAD) {
        tcout << _T("CSDataBaseMgr (storeInexactRegion): Bad Connection with the Server") <<  endl; // we could use PQreset, for the moment we go with this. 
        ASSERT(false);
        return false;
	}    
    const CCSBoolArray& binaryVector = region.getBinaryVector(); 
    for (int i = 0; i < binaryVector.GetSize()-1; ++i) {
        for (int j = i + 1; j < binaryVector.GetSize(); ++j) {
            CString parameterIDString; 
            CString featAIDFKeyString;
            CString featBIDFKeyString;
            CString featAPresentString;
            CString featBPresentString;
			CString dbRegionIDString;
            parameterIDString.Format(_T("%d"), param.m_dbParamID);
            featAIDFKeyString.Format(_T("%d"), medianNZ.GetAt(i));
            featBIDFKeyString.Format(_T("%d"), medianNZ.GetAt(j));
			dbRegionIDString.Format(_T("%d"), region.m_dbRegionID);
            CBFStrHelper::boolToStr(region.getBinaryVector().GetAt(i), featAPresentString);
            CBFStrHelper::boolToStr(region.getBinaryVector().GetAt(j), featBPresentString);
			CString csSqlCheckInexact = _T("UPDATE \"Inexact2Comb\" SET \"dbRegionID\" = \"dbRegionID\" || '#' || \'") + dbRegionIDString + _T(" \' where \"paramIDFKey\" =") + parameterIDString + _T(" and \"featAIDFKey\" =") + featAIDFKeyString + _T(" and \"featBIDFKey\" =") + featBIDFKeyString + _T(" and \"featAPresent\" =") + featAPresentString + _T(" and \"featBPresent\" =") + featBPresentString;
            CStringA csSqlCheckInexactANSI;
            if (!CBFStrHelper::convertCStringToCStringA(csSqlCheckInexact, csSqlCheckInexactANSI)) {
                tcout << _T("Failed to convert UNICODE string: ") << region.m_dbRegionID << endl;
                ASSERT(false);
                return false;
            }
            pgresult = PQexec(getPGDBConnection(), csSqlCheckInexactANSI);
		}
    }
    PQclear(pgresult); 
    return true;
}

//
// Retrieve regions by dbParamID and hashValue
//
bool CCSDatabaseMgr::fetchRegions(int dbParamID, UINT hashValue, CCSRegions& cloneRegions)
{
    // Put the regionID in m_dbRegionID.
    // Put the functionID in m_dbFcnID.
    // Put the fileID in m_dbFileID.  
    // Put the hash value in m_hashValue.
    // Put the start, end, startRaw, and endRaw into the corresponding member variables
    // dbRegionID | startIdx | endIdx | rawStartIdx | rawEndIdx |  hashValue  | FcnIDFkey | ParamIDFKey
    //------------+----------+--------+-------------+-----------+-------------+-----------+-------------
    PGresult *pgresult;
	if (PQstatus(m_pPGDBconnection) == CONNECTION_BAD) {
		tcout << _T("CSDataBaseMgr (fetchRegions): Bad Connection with the Server") <<  endl; // we could use PQreset, for the moment we go with this. 
		ASSERT(false);
		return false;
	}
    CString csFetchAllRegs;
    // the following returns the tupples needed for the 2nd constructor of CCSRegion
    csFetchAllRegs.Format(_T("SELECT \"R\".\"dbRegionID\",  \"R\".\"FcnIDFkey\", \"F\".\"FileIdFKey\", \"R\".\"startIdx\", \"R\".\"endIdx\", \"R\".\"rawStartIdx\", \"R\".\"rawEndIdx\" from \"Region\" \"R\" JOIN \"Function\" \"F\" on (\"R\".\"FcnIDFkey\"=\"F\".\"dbFcnID\") where \"R\".\"hashValue\"=%d AND \"R\".\"ParamIDFKey\"=%d"), hashValue, dbParamID);
    // tuples from several tables. There are foreign keys in Function and Region tables, so I don't need the FileID from the File table (ie, this is the foreign key in Function table)
    // the query will give : 
    // dbRegionID | FcnIDFkey | FileIdFKey | startIdx | endIdx | rawStartIdx | rawEndIdx
    //------------+-----------+------------+----------+--------+-------------+-----------
    /////////
    CStringA csFetchAllRegsAnsi;
	if (!CBFStrHelper::convertCStringToCStringA(csFetchAllRegs, csFetchAllRegsAnsi)) {
        tcout << _T("Failed to convert UNICODE string: ") << csFetchAllRegs.GetString() << endl;
        ASSERT(false);
        return false;
    }
    pgresult = PQexec(getPGDBConnection(), (LPCSTR) csFetchAllRegsAnsi);
	if(PQresultStatus(pgresult) != PGRES_TUPLES_OK) { // PGRES_TUPLES_OK even if 0 tuples returned
		tcout << _T("CSDataBaseMgr (fetchRegions): Bad query to fetch the Regions from the database") <<  endl; 
		ASSERT(false);
		return false;
	}
    CCSRegion* pNewRegion = NULL;
    CStringA dbRegionIDAnsi, FcnIDFkeyAnsi, FileIdFKeyAnsi, startIdxAnsi, endIdxAnsi, rawStartIdxAnsi, rawEndIdxAnsi;
    CString dbRegionID, FcnIDFkey, FileIdFKey, startIdx, endIdx, rawStartIdx, rawEndIdx;
    for (int i = 0; i < PQntuples(pgresult); i++) {
        dbRegionIDAnsi = PQgetvalue(pgresult, i, 0);
        FcnIDFkeyAnsi = PQgetvalue(pgresult, i, 1);
        FileIdFKeyAnsi = PQgetvalue(pgresult, i, 2);
        startIdxAnsi = PQgetvalue(pgresult, i, 3);
        endIdxAnsi = PQgetvalue(pgresult, i, 4);
        rawStartIdxAnsi = PQgetvalue(pgresult, i, 5);
        rawEndIdxAnsi = PQgetvalue(pgresult, i, 6);
        if(!(CBFStrHelper::convertCStringAToCString(dbRegionIDAnsi, dbRegionID) && CBFStrHelper::convertCStringAToCString(FcnIDFkeyAnsi, FcnIDFkey) && CBFStrHelper::convertCStringAToCString(FileIdFKeyAnsi, FileIdFKey) && CBFStrHelper::convertCStringAToCString(startIdxAnsi, startIdx) && CBFStrHelper::convertCStringAToCString(endIdxAnsi, endIdx) && CBFStrHelper::convertCStringAToCString(rawStartIdxAnsi, rawStartIdx) && CBFStrHelper::convertCStringAToCString(rawEndIdxAnsi, rawEndIdx))) {
	        tcout << _T("Failed to convert ANSI to Unicode strings:") << dbRegionIDAnsi.GetString() << _T(", ") << FcnIDFkeyAnsi.GetString() << _T(", ") << FileIdFKeyAnsi.GetString() << _T(", ") << startIdxAnsi.GetString() << _T(", ") << endIdxAnsi.GetString() << rawStartIdxAnsi.GetString() << _T(", ") << rawEndIdxAnsi.GetString() << _T(", ") <<  endl; 
	        ASSERT(false);
	        return false;                     
        }
        pNewRegion = new CCSRegion(CBFStrHelper::strToInt(dbRegionID.GetString()), CBFStrHelper::strToInt(FcnIDFkey.GetString()), CBFStrHelper::strToInt(FileIdFKey.GetString()), CBFStrHelper::strToInt(startIdx.GetString()), CBFStrHelper::strToInt(endIdx.GetString()), CBFStrHelper::strToInt(rawStartIdx.GetString()), CBFStrHelper::strToInt(rawEndIdx.GetString()));
        pNewRegion->m_hashValue = hashValue;
        if (!cloneRegions.addRegion(pNewRegion)) {
            ASSERT(false);
            return false;
        }
    }
    PQclear(pgresult);
    return true;
}

//
// Store the feature vector names
//
bool CCSDatabaseMgr::storeVectorFeatureNames(int dbParamID, const CStringArray& vectorFeatureNames)
{
    return true;
}

//
// Fetch vector feature names
//
bool CCSDatabaseMgr::fetchVectorFeatureNames(int dbParamID, CStringArray& vectorFeatureNames)
{
    return true;
}

//
// Store the feature vector
//
bool CCSDatabaseMgr::storeRegionVector(int dbParamID, int dbRegionID, const CStringArray& vectorFeatureNames, const CCSFeatureVector& vector)
{
    return true;
}

//
// Compute the median vector
//
bool CCSDatabaseMgr::computeMedianVector(int dbParamID, CCSFeatureVector& vector)
{
    return true;
}

//
// Store constant for indexing.
//
bool CCSDatabaseMgr::storeConstant(CCSIndexedToken& indexedToken)
{
    //  constantToken | rawLineNum | dbFileID
    // ---------------+------------+----------
    // Watch out : constantToken is character varying 600. We could set it somehow differently... 
	if (PQstatus(m_pPGDBconnection) == CONNECTION_BAD) {
		tcout << _T("CSDataBaseMgr (storeConstant): Bad Connection with the Server") <<  endl; // we could use PQreset, for the moment we go with this. 
		ASSERT(false);
		return false;
	}
	PGresult* pgresult; // result of the last query
	CString csSqlInsertConstant = _T("INSERT into \"Constant\" VALUES(E'");
    CString token = indexedToken.getTokStr();
    //PQexec(getPGDBConnection(), "SET client_encoding = \'Latin9\'");
    token.Replace(_T("\\"), _T("\\\\"));
    token.Replace(_T("\'"), _T("\\'"));
    csSqlInsertConstant += token; // indexedToken.getTokStr();
    csSqlInsertConstant += _T("', ");
    CString csIntValues;
    csIntValues.Format(_T("%d, %d)"), indexedToken.getRawLineNum(), indexedToken.getDBfileID());
    csSqlInsertConstant += csIntValues;
    CStringA csSqlInsertConstantAnsi;
	if (!CBFStrHelper::convertCStringToCStringA(csSqlInsertConstant, csSqlInsertConstantAnsi)) {
        tcout << _T("Failed to convert UNICODE string: ") << csSqlInsertConstant.GetString() << endl;
        ASSERT(false);
        return false;
    }
    //are there any specific characters that SQL wants "\" prefixed ? for example, : 
    //csSqlInsertConstantAnsi.Replace((LPCSTR)"\\", (LPCSTR)"\\\\"); -- already done for the token
    //csSqlInsertConstantAnsi.Replace((LPCSTR)"E\'\\", (LPCSTR)"E\'"); -- already done for the token
	pgresult = PQexec(getPGDBConnection(), (LPCSTR) csSqlInsertConstantAnsi);
	if(PQresultStatus(pgresult) != PGRES_COMMAND_OK) { // PGRES_COMMAND_OK for a command which does not return tuples
		tcout << _T("CSDataBaseMgr (storeConstant): Failing to insert the Constant ") << indexedToken.getTokStr().GetString() << _T(" in the database") <<  endl;
        tcout << _T("If the Constant has more than 600 chars, then constantToken column type must be changed...") << endl; 
        cout << "INSERT COMMAND IS : " << csSqlInsertConstantAnsi.GetString() << " ;" << endl;
		ASSERT(false);
		return false;
	} 
	PQclear(pgresult);
    return true;
}

//
// Retreive indexed tokens having tokStr.
//
bool CCSDatabaseMgr::fetchConstants(LPCTSTR tokStr, CCSIndexedTokens& indexedTokens)
{
    // Return an array of indexed tokens by reference. You can use something like:    
    //CCSIndexedToken* pNewIndexedToken = new CCSIndexedToken(<put parameters here>);
    //if (!indexedTokens.addIndexedToken(pNewIndexedToken))
    //    return false;
    // constantToken | rawLineNum | dbFileID
    //---------------+------------+----------
    PGresult* pgresult; // result of the last query	
    if (PQstatus(m_pPGDBconnection) == CONNECTION_BAD) {
		tcout << _T("CSDataBaseMgr (fetchConstants): Bad Connection with the Server") <<  endl; // we could use PQreset, for the moment we go with this. 
		ASSERT(false);
		return false;
	}
    CString csSqlFetchConstants = _T("SELECT * from \"Constant\" where \"constantToken\"=E'");
    csSqlFetchConstants += tokStr;
    csSqlFetchConstants += _T("'");
    CStringA csSqlFetchConstantsAnsi;
	if (!CBFStrHelper::convertCStringToCStringA(csSqlFetchConstants, csSqlFetchConstantsAnsi)) {
        tcout << _T("Failed to convert UNICODE string: ") << csSqlFetchConstants.GetString() << endl;
        ASSERT(false);
        return false;
    }
    //are there any specific characters that SQL wants "\" prefixed ? for example, : 
    csSqlFetchConstantsAnsi.Replace((LPCSTR)"\\", (LPCSTR)"\\\\");
    pgresult = PQexec(getPGDBConnection(), (LPCSTR) csSqlFetchConstantsAnsi);
	if(PQresultStatus(pgresult) != PGRES_TUPLES_OK) { // PGRES_TUPLES_OK even if 0 tuples returned
		tcout << _T("CSDataBaseMgr (fetchConstants): Bad query to fetch the Constants from the database") <<  endl; 
		ASSERT(false);
		return false;
	}
    CCSIndexedToken* pNewIndexedToken = NULL;
    CStringA constantTokenAnsi, rawLineNumAnsi, dbFileIDAnsi;
    CString constantToken, rawLineNum, dbFileID;
    for (int i = 0; i < PQntuples(pgresult); i++) {
        constantTokenAnsi = PQgetvalue(pgresult, i, 0);
        rawLineNumAnsi = PQgetvalue(pgresult, i, 1);
        dbFileIDAnsi = PQgetvalue(pgresult, i, 2);
        if(!(CBFStrHelper::convertCStringAToCString(constantTokenAnsi, constantToken) && CBFStrHelper::convertCStringAToCString(rawLineNumAnsi, rawLineNum) && CBFStrHelper::convertCStringAToCString(dbFileIDAnsi, dbFileID))) {
	        tcout << _T("Failed to convert ANSI to Unicode strings:") << constantTokenAnsi.GetString() << _T(", ") << rawLineNumAnsi.GetString() << _T(", ") << dbFileIDAnsi.GetString() << endl; 
	        ASSERT(false);
	        return false;                     
        }
        pNewIndexedToken = new CCSIndexedToken((LPCTSTR) constantToken.GetString(), CBFStrHelper::strToInt(dbFileID.GetString()), CBFStrHelper::strToInt(rawLineNum.GetString()));
        if (!indexedTokens.addIndexedToken(pNewIndexedToken)) {
            ASSERT(false);
            return false;
        }
    }
	PQclear(pgresult);
    return true;
}

//
// Store string for indexing.
//
bool CCSDatabaseMgr::storeString(CCSIndexedToken& indexedToken)
{
    //  stringToken | rawLineNum | dbFileID
    // -------------+------------+--------
    // Watch out : stringToken is character varying 600. We could set it somehow differently... 
	if (PQstatus(m_pPGDBconnection) == CONNECTION_BAD) {
		tcout << _T("CSDataBaseMgr (storeString): Bad Connection with the Server") <<  endl; // we could use PQreset, for the moment we go with this. 
		ASSERT(false);
		return false;
	}
	PGresult* pgresult; // result of the last query
	CString csSqlInsertString = _T("INSERT into \"String\" VALUES(E'");
    csSqlInsertString += indexedToken.getTokStr();
    csSqlInsertString += _T("', ");
    CString csIntValues;
    csIntValues.Format(_T("%d, %d)"), indexedToken.getRawLineNum(), indexedToken.getDBfileID());
    csSqlInsertString += csIntValues;
    CStringA csSqlInsertStringAnsi;
	if (!CBFStrHelper::convertCStringToCStringA(csSqlInsertString, csSqlInsertStringAnsi)) {
        tcout << _T("Failed to convert UNICODE string: ") << csSqlInsertString.GetString() << endl;
        ASSERT(false);
        return false;
    }
    //are there any specific characters that SQL wants "\" prefixed ? for example, : 
    csSqlInsertStringAnsi.Replace((LPCSTR)"\\", (LPCSTR)"\\\\");
	pgresult = PQexec(getPGDBConnection(), (LPCSTR) csSqlInsertStringAnsi);
	if(PQresultStatus(pgresult) != PGRES_COMMAND_OK) { // PGRES_COMMAND_OK for a command which does not return tuples
		tcout << _T("CSDataBaseMgr (storeString): Failing to insert the String ") << indexedToken.getTokStr().GetString() << _T(" in the database") <<  endl;
        tcout << _T("If the String has more than 600 chars, then stringToken column type must be changed...") << endl; 
        cout << "INSERT COMMAND IS : " << csSqlInsertStringAnsi.GetString() << " ;" << endl;
		ASSERT(false);
		return false;
	} 
	PQclear(pgresult);
    return true;
}

//
// Retreive indexed tokens having tokStr.
//
bool CCSDatabaseMgr::fetchStrings(LPCTSTR tokStr, CCSIndexedTokens& indexedTokens)
{
    // Return an array of indexed tokens by reference. You can use something like:    
    //CCSIndexedToken* pNewIndexedToken = new CCSIndexedToken(<put parameters here>);
    //if (!indexedTokens.addIndexedToken(pNewIndexedToken))
    //    return false;
    // stringToken          | rawLineNum | dbFileID
    //----------------------+------------+----------
    PGresult* pgresult; // result of the last query	
    if (PQstatus(m_pPGDBconnection) == CONNECTION_BAD) {
		tcout << _T("CSDataBaseMgr (fetchStrings): Bad Connection with the Server") <<  endl; // we could use PQreset, for the moment we go with this. 
		ASSERT(false);
		return false;
	}
    CString csSqlFetchStrings = _T("SELECT * from \"String\" where \"stringToken\"=E'");
    csSqlFetchStrings += tokStr;
    csSqlFetchStrings += _T("'");
    CStringA csSqlFetchStringsAnsi;
	if (!CBFStrHelper::convertCStringToCStringA(csSqlFetchStrings, csSqlFetchStringsAnsi)) {
        tcout << _T("Failed to convert UNICODE string: ") << csSqlFetchStrings.GetString() << endl;
        ASSERT(false);
        return false;
    }
    //are there any specific characters that SQL wants "\" prefixed ? for example, : 
    csSqlFetchStringsAnsi.Replace((LPCSTR)"\\", (LPCSTR)"\\\\");
    pgresult = PQexec(getPGDBConnection(), (LPCSTR) csSqlFetchStringsAnsi);
	if(PQresultStatus(pgresult) != PGRES_TUPLES_OK) { // PGRES_TUPLES_OK even if 0 tuples returned
		tcout << _T("CSDataBaseMgr (fetchStrings): Bad query to fetch the Strings from the database") <<  endl; 
		ASSERT(false);
		return false;
	}
    CCSIndexedToken* pNewIndexedToken = NULL;
    CStringA stringTokenAnsi, rawLineNumAnsi, dbFileIDAnsi;
    CString stringToken, rawLineNum, dbFileID;
    for (int i = 0; i < PQntuples(pgresult); i++) {
        stringTokenAnsi = PQgetvalue(pgresult, i, 0);
        rawLineNumAnsi = PQgetvalue(pgresult, i, 1);
        dbFileIDAnsi = PQgetvalue(pgresult, i, 2);
        if(!(CBFStrHelper::convertCStringAToCString(stringTokenAnsi, stringToken) && CBFStrHelper::convertCStringAToCString(rawLineNumAnsi, rawLineNum) && CBFStrHelper::convertCStringAToCString(dbFileIDAnsi, dbFileID))) {
	        tcout << _T("Failed to convert ANSI to Unicode strings:") << stringTokenAnsi.GetString() << _T(", ") << rawLineNumAnsi.GetString() << _T(", ") << dbFileIDAnsi.GetString() << endl; 
	        ASSERT(false);
	        return false;                     
        }
        pNewIndexedToken = new CCSIndexedToken((LPCTSTR) stringToken.GetString(), CBFStrHelper::strToInt(dbFileID.GetString()), CBFStrHelper::strToInt(rawLineNum.GetString()));
        if (!indexedTokens.addIndexedToken(pNewIndexedToken)) {
            ASSERT(false);
            return false;
        }
    }
	PQclear(pgresult);
    return true;
}

//
// Store import for indexing.
//
bool CCSDatabaseMgr::storeImport(CCSIndexedToken& indexedToken)
{
    //  importToken | rawLineNum | dbFileID
    // -------------+------------+----------
    // Watch out : importToken is character varying 600. We could set it somehow differently... 
	if (PQstatus(m_pPGDBconnection) == CONNECTION_BAD) {
		tcout << _T("CSDataBaseMgr (storeImport): Bad Connection with the Server") <<  endl; // we could use PQreset, for the moment we go with this. 
		ASSERT(false);
		return false;
	}
	PGresult* pgresult; // result of the last query
	CString csSqlInsertImport = _T("INSERT into \"Import\" VALUES(E'");
    csSqlInsertImport += indexedToken.getTokStr();
    csSqlInsertImport += _T("', ");
    CString csIntValues;
    csIntValues.Format(_T("%d, %d)"), indexedToken.getRawLineNum(), indexedToken.getDBfileID());
    csSqlInsertImport += csIntValues;
    CStringA csSqlInsertImportAnsi;
	if (!CBFStrHelper::convertCStringToCStringA(csSqlInsertImport, csSqlInsertImportAnsi)) {
        tcout << _T("Failed to convert UNICODE string: ") << csSqlInsertImport.GetString() << endl;
        ASSERT(false);
        return false;
    }
    //are there any specific characters that SQL wants "\" prefixed ? for example, : 
    csSqlInsertImportAnsi.Replace((LPCSTR)"\\", (LPCSTR)"\\\\");
	pgresult = PQexec(getPGDBConnection(), (LPCSTR) csSqlInsertImportAnsi);
	if(PQresultStatus(pgresult) != PGRES_COMMAND_OK) { // PGRES_COMMAND_OK for a command which does not return tuples
		tcout << _T("CSDataBaseMgr (storeImport): Failing to insert the Import ") << indexedToken.getTokStr().GetString() << _T(" in the database") <<  endl;
        tcout << _T("If the Import has more than 600 chars, then importToken column type must be changed...") << endl; 
        cout << "INSERT COMMAND IS : " << csSqlInsertImportAnsi.GetString() << " ;" << endl;
		ASSERT(false);
		return false;
	} 
	PQclear(pgresult);
    return true;
}

//
// Retreive indexed tokens having tokStr.
//
bool CCSDatabaseMgr::fetchImports(LPCTSTR tokStr, CCSIndexedTokens& indexedTokens)
{
    // Return an array of indexed tokens by reference. You can use something like:    
    //CCSIndexedToken* pNewIndexedToken = new CCSIndexedToken(<put parameters here>);
    //if (!indexedTokens.addIndexedToken(pNewIndexedToken))
    //    return false;
    // importToken            | rawLineNum | dbFileID
    //------------------------+------------+----------
    PGresult* pgresult; // result of the last query	
    if (PQstatus(m_pPGDBconnection) == CONNECTION_BAD) {
		tcout << _T("CSDataBaseMgr (fetchImports): Bad Connection with the Server") <<  endl; // we could use PQreset, for the moment we go with this. 
		ASSERT(false);
		return false;
	}
    CString csSqlFetchImports = _T("SELECT * from \"Import\" where \"importToken\"=E'");
    csSqlFetchImports += tokStr;
    csSqlFetchImports += _T("'");
    CStringA csSqlFetchImportsAnsi;
	if (!CBFStrHelper::convertCStringToCStringA(csSqlFetchImports, csSqlFetchImportsAnsi)) {
        tcout << _T("Failed to convert UNICODE string: ") << csSqlFetchImports.GetString() << endl;
        ASSERT(false);
        return false;
    }
    //are there any specific characters that SQL wants "\" prefixed ? for example, : 
    csSqlFetchImportsAnsi.Replace((LPCSTR)"\\", (LPCSTR)"\\\\");
    pgresult = PQexec(getPGDBConnection(), (LPCSTR) csSqlFetchImportsAnsi);
	if(PQresultStatus(pgresult) != PGRES_TUPLES_OK) { // PGRES_TUPLES_OK even if 0 tuples returned
		tcout << _T("CSDataBaseMgr (fetchImports): Bad query to fetch the Imports from the database") <<  endl; 
		ASSERT(false);
		return false;
	}
    CCSIndexedToken* pNewIndexedToken = NULL;
    CStringA importTokenAnsi, rawLineNumAnsi, dbFileIDAnsi;
    CString importToken, rawLineNum, dbFileID;
    for (int i = 0; i < PQntuples(pgresult); i++) {
        importTokenAnsi = PQgetvalue(pgresult, i, 0);
        rawLineNumAnsi = PQgetvalue(pgresult, i, 1);
        dbFileIDAnsi = PQgetvalue(pgresult, i, 2);
        if(!(CBFStrHelper::convertCStringAToCString(importTokenAnsi, importToken) && CBFStrHelper::convertCStringAToCString(rawLineNumAnsi, rawLineNum) && CBFStrHelper::convertCStringAToCString(dbFileIDAnsi, dbFileID))) {
	        tcout << _T("Failed to convert ANSI to Unicode strings:") << importTokenAnsi.GetString() << _T(", ") << rawLineNumAnsi.GetString() << _T(", ") << dbFileIDAnsi.GetString() << endl; 
	        ASSERT(false);
	        return false;                     
        }
        pNewIndexedToken = new CCSIndexedToken(importToken.GetString(), CBFStrHelper::strToInt(dbFileID.GetString()), CBFStrHelper::strToInt(rawLineNum.GetString()));
        if (!indexedTokens.addIndexedToken(pNewIndexedToken)) {
            ASSERT(false);
            return false;
        }
    }
	PQclear(pgresult);
    return true;
}
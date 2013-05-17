#include "StdAfx.h"
#include "CSAssemblyFile.h"
#include "CSDatabaseMgr.h"
#include "CSTokenRefMgr.h"

//******************
// CCSAssemblyFile *
//******************

CCSAssemblyFile::CCSAssemblyFile(LPCTSTR filePath, CCSAssemblyFileMgr* pAssemblyFileMgr)
    : m_dbFileID(-1), m_filePath(filePath), m_pAssemblyFileMgr(pAssemblyFileMgr)
{
    ASSERT(m_pAssemblyFileMgr);
}


CCSAssemblyFile::~CCSAssemblyFile()
{
    cleanup();
    m_functions.cleanup();
    m_clones.cleanup();
}

bool CCSAssemblyFile::addLine(CCSAssemblyLine* pAssemblyLine)
{
    try {
        Add(pAssemblyLine);
        return true;
    }
    catch (CMemoryException&) {
        tcout << _T("CCSAssemblyFile: failed to add line to file: ") << m_filePath.GetString() << endl;
        ASSERT(false);
        return false;
    } 
}

bool CCSAssemblyFile::addFunction(CCSAssemblyFunction* pFcn)
{
    try {
        m_functions.Add(pFcn);
        return true;
    }
    catch (CMemoryException&) {
        tcout << _T("CCSAssemblyFile: failed to add function to file: ") << m_filePath.GetString() << endl;
        ASSERT(false);
        return false;
    } 
}


//
// Parse one file.
//
bool CCSAssemblyFile::parseFile(const CCSParam& param, bool bStoreDB)
{
    // Store all lines into assemblyFileObj.
    try {
        CStdioFile assemblyTextFile;
        if (!assemblyTextFile.Open((LPCTSTR) m_filePath, CFile::modeRead)) {
            tcout << _T("CCSAssemblyFileMgr: failed to open file: ") << m_filePath.GetString() << endl;
            ASSERT(false);
            return false;
        }        
        
		if (bStoreDB && !m_pAssemblyFileMgr->getDBMgr()->storeFile(*this, param))
            return false;

        int asmFileLineIdx = -1;    // line index in the raw assembly file.        

        // Parse one line
        CString lineStr;
        while (assemblyTextFile.ReadString(lineStr)) {
            ++asmFileLineIdx;
            CBFStrHelper::trim(lineStr);
            if (lineStr.IsEmpty())
                continue;

            // Replace tab space by space
            lineStr.Replace(TCHAR('\t'), TCHAR(' '));

            // Parse and add strings
            if (bStoreDB && !m_pAssemblyFileMgr->getTokenRefMgr()->parseStringRef(lineStr, m_dbFileID, asmFileLineIdx))
                return false;

            // Skip comments
            int commentPos = lineStr.Find(CS_ASSEMBLYFILE_COMMENT);
            if (commentPos != -1) {
                lineStr = lineStr.Left(commentPos - 1);
                CBFStrHelper::trim(lineStr);
                if (lineStr.IsEmpty())
                    continue;
            }

            // Create a new line
            CCSAssemblyLine* pNewLine = new CCSAssemblyLine(asmFileLineIdx, lineStr);
            if (!pNewLine->addTokens()) {
                delete pNewLine;
                pNewLine = NULL;
                return false;
            }

            // Parse and add constants
            if (bStoreDB && !m_pAssemblyFileMgr->getTokenRefMgr()->parseConstantRef(pNewLine, m_dbFileID, asmFileLineIdx)) {
                delete pNewLine;
                pNewLine = NULL;
                return false;
            }
            
            // Parse and add imports
            if (bStoreDB && !m_pAssemblyFileMgr->getTokenRefMgr()->parseImportRef(pNewLine, m_dbFileID, asmFileLineIdx)) {
                delete pNewLine;
                pNewLine = NULL;
                return false;
            }

            // Add the new line only if it is a begin procedure, an end procedure, or a mnemonic statement
            if (pNewLine->isBeginProcedure() || pNewLine->isEndProcedure() || pNewLine->isMnemonicStatement()) {            
                if (!addLine(pNewLine)) {
                    delete pNewLine;
                    pNewLine = NULL;
                    return false;
                }
            }
            else {
                delete pNewLine;
                pNewLine = NULL;
            }
        }
        assemblyTextFile.Close();
        if (GetSize() == 0) {
            tcout << _T("CCSAssemblyFile: empty file: " ) << m_filePath.GetString() << endl;
            return true;    // skip this file.
        }
        tcout << m_filePath.GetString() << _T(" has ") << GetSize() << _T(" lines.") << endl;        
    }
    catch (CFileException&) {
        tcout << _T("CCSAssemblyFileMgr: failed to read assembly file: ") << m_filePath.GetString() << endl;
        ASSERT(false);
        return false;
    }
    return true;
}


//
// Normalize each line of code based on
//
bool CCSAssemblyFile::normalizeCode(const CCSParam& param)
{
    CCSAssemblyFunction* pFcn = NULL;
    CCSAssemblyLine* pLine = NULL;
    CCSAssemblyToken* pToken = NULL;

    // For each line in the function
    for (int lineIdx = 0; lineIdx < GetSize(); ++lineIdx) {
        pLine = GetAt(lineIdx);

        // For each token
        for (int tIdx = 0; tIdx < pLine->GetSize(); ++tIdx) {
            pToken = pLine->GetAt(tIdx);

            // Get the index of the operand
            int opIdx = -1;
            CString newTokenStr;
            if (pToken->getTokenType() == CSTOKEN_TYPE_MNEMONIC)
                continue;   // do nothing for mnemonic
            else if (pToken->getTokenType() == CSTOKEN_TYPE_OPMEM) {
                opIdx = m_pAssemblyFileMgr->getOpMems()->getOperandIdx(pToken->getTokenStr());
                newTokenStr = CS_ASSEMBLYTOKEN_OPMEM;
            }
            else if (pToken->getTokenType() == CSTOKEN_TYPE_OPREG) {
                opIdx = m_pAssemblyFileMgr->getOpRegs()->getOperandIdx(pToken->getTokenStr());
                newTokenStr = CCSAssemblyToken::normalizeRegister(pToken->getTokenStr(), param.m_regNormLevel);
            }
            else if (pToken->getTokenType() == CSTOKEN_TYPE_OPVAL) {
                opIdx = m_pAssemblyFileMgr->getOpVals()->getOperandIdx(pToken->getTokenStr());
                newTokenStr = CS_ASSEMBLYTOKEN_OPVAL;
            }
            else if (pToken->getTokenType() == CSTOKEN_TYPE_UNKNOWN)
                continue;
            else {
                tcout << _T("Error: normalizeCode(): unknown token type for token string: ") << pToken->getTokenStr().GetString() << endl;
                ASSERT(false);
                return false;
            }

            if (opIdx == -1) {
                tcout << _T("Error: normalizeCode(): failed to get operand index.") << endl;
                ASSERT(false);
                return false;
            }
            pToken->setTokenStr(newTokenStr); // e.g., REG or REGGen32
        }
    }
    return true;
}


//
// Extract functions.
//
bool CCSAssemblyFile::extractFunctions(CCSDatabaseMgr* pDBMgr, const CCSParam& param, bool bStoreDB)
{
    // For each line
    int startIdx = -1, startRawIdx = -1;
    for (int lineIdx = 0; lineIdx < GetSize(); ++lineIdx) {
        CCSAssemblyLine* pLine = GetAt(lineIdx);
        if (pLine->isBeginProcedure()) {
            // beginning of a function
            if (startIdx != -1) {
                tcout << _T("Warning: CCSAssemblyFile::extractFunctions: file: ") << m_filePath.GetString() << endl;
                tcout << _T("Warning: function starts at ") << startIdx << _T(" without ending.") << endl;
                ASSERT(false);
            }
            startIdx = lineIdx;
            startRawIdx = pLine->getAsmFileLineIdx();
        }
        else if (pLine->isEndProcedure()) {
            // ending of a function
            if (startIdx == -1) {
                tcout << _T("Warning: CCSAssemblyFile::extractFunctions: file: ") << m_filePath.GetString() << endl;
                tcout << _T("Warning: function ends at ") << pLine->getAsmFileLineIdx() << _T(" without starting.") << endl;
                ASSERT(false);
            }
            else {
                // create a new function
                CCSAssemblyFunction* pNewFunction = new CCSAssemblyFunction(this, startIdx, lineIdx, startRawIdx, pLine->getAsmFileLineIdx());

                // create a hash of this function
                const CString contentHashKey = composeContentHashKey(startIdx, lineIdx);
                if (!contentHashKey.IsEmpty()) {
                    // the function contains at least one mnemonic statements.
                    pNewFunction->m_hashValue = m_hashObject.HashKey((LPCTSTR) contentHashKey);
                 
                    if (bStoreDB) {
                        // store function to DB
                        if (!pDBMgr->storeFunction(*pNewFunction, param)) {
                            delete pNewFunction;
                            pNewFunction = NULL;
                            return false;
                        }
                    }
                
                    // create a copy in memory
                    if (!addFunction(pNewFunction)) {
                        delete pNewFunction;
                        pNewFunction = NULL;
                        return false;
                    }
                    startIdx = -1;
                }
            }                
        }
    }
    return true;
}

//
// Extract regions.
// If bStoreDb == true, then store the regions into the database.
// If bFindExactClones == true or bFindExactClones == ture, then retrieve the clones from the database
//
bool CCSAssemblyFile::extractRegions(CCSDatabaseMgr* pDBMgr, const CCSParam& param, bool bFindExactClones, bool bFindInexactClones, bool bStoreDB, bool bFirstScan , CCSAssemblyFileMgr* pAssemblyFileMgr)
{
    // For each function
    int nRegionsInFile = 0;
	for (int fcnIdx = 0; fcnIdx < m_functions.GetSize(); ++fcnIdx) {
		CCSAssemblyFunction* pFcn = m_functions.GetAt(fcnIdx);
		pFcn->resetNumRegions();
		// skip this function if its size is smaller than the window size
		int fcnSize = (pFcn->m_endIdx -1) - (pFcn->m_startIdx +1) + 1;
		if (fcnSize < param.m_windowSize)
			continue;
		for (int sIdx = pFcn->m_startIdx + 1; sIdx < pFcn->m_endIdx; sIdx += param.m_stride) {
			// ensure the endIdx does not go beyond the limit
			int eIdx = sIdx + param.m_windowSize - 1;
			if (eIdx > pFcn->m_endIdx - 1)
				eIdx = pFcn->m_endIdx - 1;

			// skip this region if it is not the first region in the function and its size is smaller than the window size
			// because it is a region towards the end of a function, and it has already been covered by some previous regions.
			if (pFcn->getNumRegions() > 0 && eIdx - sIdx + 1 < param.m_windowSize)
				break;

			// create a new region
			int rawSidx = GetAt(sIdx)->getAsmFileLineIdx();
			int rawEidx = GetAt(eIdx)->getAsmFileLineIdx();
			CCSRegion region(pFcn, sIdx, eIdx, rawSidx, rawEidx);

				
            if (bFirstScan) {   // if bStoreDB is false, it means that the program is in first scan phase             
                ++ CCSRegion::m_cntRegion; // increament the total number of regions processes so far in the first scan
			    
                if (!region.countRegionFeatures(pAssemblyFileMgr))
				    return false;

			    //update the medians. 
			    if (!region.updateGlobalMedians(pAssemblyFileMgr->m_globalMedians, pAssemblyFileMgr))
					    return false;           
                continue;
            }

			// create a hash of this region
			const CString contentHashKey = composeContentHashKey(sIdx, eIdx);
			if (contentHashKey.IsEmpty())
				continue;

			// the function contains at least one mnemonic statements.
			region.m_hashValue = m_hashObject.HashKey((LPCTSTR) contentHashKey);

			if (bFindExactClones) {
				// find exact clones from DB
				CCSRegions cloneRegions;
				if (!pDBMgr->fetchRegions(param.m_dbParamID, region.m_hashValue, cloneRegions)) {
					cloneRegions.cleanup();
					return false;
				}

				if (!m_clones.makeClones(region, cloneRegions)) {
					cloneRegions.cleanup();
					return false;
				}
				cloneRegions.cleanup();
			}

			if (bFindInexactClones) {

				CCSRegions cloneRegions;
                if (!region.countRegionFeatures(pAssemblyFileMgr))
					return false;	

				if (!pDBMgr->fetchFeatureVector(region, param))
					return false;								
				
				if (!pDBMgr->createTargetRegionBinaryVector(region, param))
					return false;

				if(!pDBMgr->constructScoreVector(param))
					return false;

				if (!pDBMgr->fetchInexactScore(region, param))
			        return false;

				if (!pDBMgr->fetchInexactRegions(region, param, cloneRegions)) {
			        cloneRegions.cleanup();
					return false;
				}

				if (!m_clones.makeClones(region, cloneRegions)) {
					cloneRegions.cleanup();
					return false;
				}
				cloneRegions.cleanup();

			}

			if (bStoreDB) {                       
				// Store region into database (not storing the vector though)
				if (!pDBMgr->storeRegion(region, param))
					return false;

				// actually count the features.
				if (!region.countRegionFeatures(pAssemblyFileMgr))
					return false;
				if (!region.constructVector(pAssemblyFileMgr->m_globalFeatures))
					return false;
				ASSERT(pAssemblyFileMgr->m_globalFeatures.GetSize() == region.getVector().GetSize());

				// compute the binary vector, then store the hash into the database
				if (!region.constructBinaryVector(pAssemblyFileMgr->m_mediansNZ, pAssemblyFileMgr->m_globalMedians)) 
					return false;
				if (!pDBMgr->storeInexact2CombRegion(region, pAssemblyFileMgr->m_mediansNZ, param))
					return false;
			}

			//tcout << _T("Region: ") << region.m_dbRegionID << _T(" start: ") << sIdx << _T(" end: ") << eIdx << endl;
			//tcout << _T("Region: ") << region.m_dbRegionID << _T(" rawStart: ") << rawSidx << _T(" rawEnd: ") << rawEidx << endl;
			pFcn->incNumRegions();         
		}
		nRegionsInFile += pFcn->getNumRegions();
	}
    if (bStoreDB)
        tcout << _T("File: ") << m_filePath.GetString() << _T(" has ") << m_functions.GetSize() << _T(" functions and ") << nRegionsInFile << _T(" regions.") << endl;
    return true;
}


//
// Compose the hash key based on the content.
// 
CString CCSAssemblyFile::composeContentHashKey(int startIdx, int endIdx) const
{
    CString hashStr;
    for (int lineIdx = startIdx; lineIdx <= endIdx; ++lineIdx) {
        if (GetAt(lineIdx)->isMnemonicStatement())  // only if it is a mnemonic statement
            hashStr += GetAt(lineIdx)->composeLineHashKey();
    }
    return hashStr;
}


//
// Get number of regions in this file
//
int CCSAssemblyFile::getNumRegions() const
{
    int nRegions = 0;
    for (int fcnIdx = 0; fcnIdx < m_functions.GetSize(); ++fcnIdx)
        nRegions += m_functions.GetAt(fcnIdx)->getNumRegions();
    return nRegions;
}


//
// Get content string
//
bool CCSAssemblyFile::getContentStr(CString& str) const
{
    for (int i = 0; i < GetSize(); ++i) {
        str += GetAt(i)->getLineStr();
        if (i < GetSize() - 1)
            str += _T("\n");
    }
    return true;
}


//
// Get line string
//
bool CCSAssemblyFile::getLineStr(int lineIdx, CString& str) const
{
    if (lineIdx >= GetSize() || lineIdx < 0) {
        tcout << _T("Error: CCSAssemblyFile: invalid lineIdx: ") << lineIdx << endl;
        ASSERT(false);
        return false;
    }
    str = GetAt(lineIdx)->getLineStr();
    return true;
}


//*******************
// CCSAssemblyFiles *
//*******************

bool CCSAssemblyFiles::addFile(CCSAssemblyFile* pAssemblyFile)
{
    try {
        Add(pAssemblyFile);
        return true;
    }
    catch (CMemoryException&) {
        tcout << _T("CCSAssemblyFiles: failed to add file: ") << pAssemblyFile->getFilePath().GetString() << endl;
        ASSERT(false);
        return false;
    }    
}

void CCSAssemblyFiles::cleanup()
{
    for (int i = 0; i < GetSize(); ++i)
        delete GetAt(i);

    RemoveAll();
}
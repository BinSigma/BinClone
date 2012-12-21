#include "StdAfx.h"
#include "CDOutputMgr.h"


CCDOutputMgr::CCDOutputMgr(CCDAssemblyFileMgr*  pAssemblyFileMgr, 
                           CCDCloneMgr*         pCloneMgr, 
                           CCDTokenRefMgr*      pTokenRefMgr,
                           LPCTSTR              clonePairsFilePath,
                           int                  windowSize,
                           int                  stride,
                           int                  kThreshold,
                           double               oThreshold,
                           bool                 bFindExactClones,
                           bool                 bFindInexactClones,
                           TCDRegNormalizeLevel regNormLevel,
                           bool                 bNormalizeToken,
                           int                  keyVectorSize,
                           double               minCoOccThreshold,
                           CDInexactMethod      inexactMethod) 
    : m_pAssemblyFileMgr(pAssemblyFileMgr), 
      m_pCloneMgr(pCloneMgr), 
      m_pTokenRefMgr(pTokenRefMgr),
      m_clonePairsFilePath(clonePairsFilePath),
      m_windowSize(windowSize),
      m_stride(stride),
      m_kThreshold(kThreshold),
      m_oThreshold(oThreshold),
      m_bFindExactClones(bFindExactClones),
      m_bFindInexactClones(bFindInexactClones),
      m_regNormLevel(regNormLevel),
      m_bNormalizeToken(bNormalizeToken),
      m_keyVectorSize(keyVectorSize),
      m_minCoOccThreshold(minCoOccThreshold),
      m_inexactMethod(inexactMethod) 
{
}

CCDOutputMgr::~CCDOutputMgr()
{
}


bool CCDOutputMgr::writeCloneInfoToXMLFile(int filterByFileID)
{
    // Create XML file
    CBFXmlDoc cpFile;
    if (!cpFile.createXML(m_clonePairsFilePath, _T("clone_detection"))) {
        tcout << _T("Error: CCDOutputMgr::writeClonePairsTofile(): failed to create XML file: ") << m_clonePairsFilePath.GetString() << endl;
        ASSERT(false);
        return false;
    }

    // Create root node
    CBFXmlNode docElem;
    if (!cpFile.getDocumentNode(&docElem)) {
        tcout << _T("Error: CCDOutputMgr::writeClonePairsTofile(): failed to get document node from XML file.") << endl;
        ASSERT(false);
        return false;
    }

    // Write list of parameters.
    if (!writeParametersToXMLFile(cpFile, docElem))
        return false;

    // Write list of assembly files.
    if (!writeAssemblyFilesToXMLFile(cpFile, docElem))
        return false;

    // Write list of clone pairs.
    if (!writeClonePairsToXMLFile(cpFile, docElem, filterByFileID))
        return false;

    // Write list of token references.
    if (!writeTokenReferencesToXMLFile(cpFile, docElem))
        return false;

    // Save XML file
    if (!cpFile.saveXML(NULL)) {
        tcout << _T("Error: CCDOutputMgr::writeClonePairsTofile(): failed to save XML file: ") << m_clonePairsFilePath.GetString() << endl;
        ASSERT(false);
        return false;
    }
    return true;
}


bool CCDOutputMgr::writeParametersToXMLFile(CBFXmlDoc& cpFile, CBFXmlNode& parentNode) const
{
    CBFXmlNode paramsNode;
    if (!cpFile.createElement(_T("parameters"), &paramsNode)) {
        tcout << _T("Error: CCDOutputMgr::writeParametersToXMLFile(): failed to create element.") << endl;
        ASSERT(false);
        return false;
    }

    // Create nodes
    CBFXmlNode windowSizeNode;
    if (!cpFile.createElement(_T("windowSize"), &windowSizeNode)) {
        tcout << _T("Error: CCDOutputMgr::writeParametersToXMLFile(): failed to create element.") << endl;
        ASSERT(false);
        return false;
    }

    CBFXmlNode strideNode;
    if (!cpFile.createElement(_T("stepSize"), &strideNode)) {
        tcout << _T("Error: CCDOutputMgr::writeParametersToXMLFile(): failed to create element.") << endl;
        ASSERT(false);
        return false;
    }

    CBFXmlNode kThresholdNode;
    if (!cpFile.createElement(_T("maxKOperands"), &kThresholdNode)) {
        tcout << _T("Error: CCDOutputMgr::writeParametersToXMLFile(): failed to create element.") << endl;
        ASSERT(false);
        return false;
    }

    CBFXmlNode oThresholdNode;
    if (!cpFile.createElement(_T("maxOverlapFraction"), &oThresholdNode)) {
        tcout << _T("Error: CCDOutputMgr::writeParametersToXMLFile(): failed to create element.") << endl;
        ASSERT(false);
        return false;
    }

    CBFXmlNode bFindExactClonesNode;
    if (!cpFile.createElement(_T("bFindExactClones"), &bFindExactClonesNode)) {
        tcout << _T("Error: CCDOutputMgr::writeParametersToXMLFile(): failed to create element.") << endl;
        ASSERT(false);
        return false;
    }

    CBFXmlNode bFindInexactClonesNode;
    if (!cpFile.createElement(_T("bFindInexactClones"), &bFindInexactClonesNode)) {
        tcout << _T("Error: CCDOutputMgr::writeParametersToXMLFile(): failed to create element.") << endl;
        ASSERT(false);
        return false;
    }
    
    CBFXmlNode regNormLevelNode;
    if (!cpFile.createElement(_T("regNormLevel"), &regNormLevelNode)) {
        tcout << _T("Error: CCDOutputMgr::writeParametersToXMLFile(): failed to create element.") << endl;
        ASSERT(false);
        return false;
    }

    CBFXmlNode bNormalizeTokenNode;
    if (!cpFile.createElement(_T("bNormalizeToken"), &bNormalizeTokenNode)) {
        tcout << _T("Error: CCDOutputMgr::writeParametersToXMLFile(): failed to create element.") << endl;
        ASSERT(false);
        return false;
    }

    CBFXmlNode keyVectorSizeNode;
    if (!cpFile.createElement(_T("keyVectorSize"), &keyVectorSizeNode)) {
        tcout << _T("Error: CCDOutputMgr::writeParametersToXMLFile(): failed to create element.") << endl;
        ASSERT(false);
        return false;
    }

    CBFXmlNode minCoOccThresholdNode;
    if (!cpFile.createElement(_T("minCoOccThreshold"), &minCoOccThresholdNode)) {
        tcout << _T("Error: CCDOutputMgr::writeParametersToXMLFile(): failed to create element.") << endl;
        ASSERT(false);
        return false;
    }

    // Set text
    CString str;
    CBFStrHelper::intToStr(m_windowSize, str);
    if (!windowSizeNode.setText(str)) {
        tcout << _T("Error: CCDOutputMgr::writeParametersToXMLFile(): failed to set text.") << endl;
        ASSERT(false);
        return false;
    }

    CBFStrHelper::intToStr(m_stride, str);
    if (!strideNode.setText(str)) {
        tcout << _T("Error: CCDOutputMgr::writeParametersToXMLFile(): failed to set text.") << endl;
        ASSERT(false);
        return false;
    }

    CBFStrHelper::intToStr(m_kThreshold, str);
    if (!kThresholdNode.setText(str)) {
        tcout << _T("Error: CCDOutputMgr::writeParametersToXMLFile(): failed to set text.") << endl;
        ASSERT(false);
        return false;
    }

    CBFStrHelper::floatToStr(m_oThreshold, CD_NUM_DECIMALS_INFILE, str);
    if (!oThresholdNode.setText(str)) {
        tcout << _T("Error: CCDOutputMgr::writeParametersToXMLFile(): failed to set text.") << endl;
        ASSERT(false);
        return false;
    }

    if (m_bFindExactClones)
        str = CD_TRUE_STR;
    else 
        str = CD_FALSE_STR;
    if (!bFindExactClonesNode.setText(str)) {
        tcout << _T("Error: CCDOutputMgr::writeParametersToXMLFile(): failed to set text.") << endl;
        ASSERT(false);
        return false;
    }

    if (m_bFindInexactClones)
        str = CD_TRUE_STR;
    else 
        str = CD_FALSE_STR;
    if (!bFindInexactClonesNode.setText(str)) {
        tcout << _T("Error: CCDOutputMgr::writeParametersToXMLFile(): failed to set text.") << endl;
        ASSERT(false);
        return false;
    }

    switch (m_regNormLevel) {
    case CD_NORM_REG_ROOT:
        str = _T("CD_NORM_REG_ROOT");
        break;
    case CD_NORM_REG_TYPE:
        str = _T("CD_NORM_REG_ROOT");
        break;
    case CD_NORM_REG_IDXPTR:
        str = _T("CD_NORM_REG_IDXPTR");
        break;
    case CD_NORM_REG_BITS:
        str = _T("CD_NORM_REG_BITS");
        break;
    default:
        tcout << _T("Error: CCDOutputMgr::writeParametersToXMLFile(): unknow regNormLevel: ") << m_regNormLevel << endl;
        ASSERT(false);
        return false;
    }
    if (!regNormLevelNode.setText(str)) {
        tcout << _T("Error: CCDOutputMgr::writeParametersToXMLFile(): failed to set text.") << endl;
        ASSERT(false);
        return false;
    }

    if (m_bNormalizeToken)
        str = CD_TRUE_STR;
    else 
        str = CD_FALSE_STR;
    if (!bNormalizeTokenNode.setText(str)) {
        tcout << _T("Error: CCDOutputMgr::writeParametersToXMLFile(): failed to set text.") << endl;
        ASSERT(false);
        return false;
    }

    CBFStrHelper::intToStr(m_keyVectorSize, str);
    if (!keyVectorSizeNode.setText(str)) {
        tcout << _T("Error: CCDOutputMgr::writeParametersToXMLFile(): failed to set text.") << endl;
        ASSERT(false);
        return false;
    }

    CBFStrHelper::floatToStr(m_minCoOccThreshold, CD_NUM_DECIMALS_INFILE, str);
    if (!minCoOccThresholdNode.setText(str)) {
        tcout << _T("Error: CCDOutputMgr::writeParametersToXMLFile(): failed to set text.") << endl;
        ASSERT(false);
        return false;
    }

    // Append nodes
    if (!paramsNode.appendChild(&windowSizeNode)) {
        tcout << _T("Error: CCDOutputMgr::writeParametersToXMLFile(): failed to append child.") << endl;
        ASSERT(false);
        return false;
    }

    if (!paramsNode.appendChild(&strideNode)) {
        tcout << _T("Error: CCDOutputMgr::writeParametersToXMLFile(): failed to append child.") << endl;
        ASSERT(false);
        return false;
    }

    if (!paramsNode.appendChild(&kThresholdNode)) {
        tcout << _T("Error: CCDOutputMgr::writeParametersToXMLFile(): failed to append child.") << endl;
        ASSERT(false);
        return false;
    }

    if (!paramsNode.appendChild(&oThresholdNode)) {
        tcout << _T("Error: CCDOutputMgr::writeParametersToXMLFile(): failed to append child.") << endl;
        ASSERT(false);
        return false;
    }

    if (!paramsNode.appendChild(&bFindExactClonesNode)) {
        tcout << _T("Error: CCDOutputMgr::writeParametersToXMLFile(): failed to append child.") << endl;
        ASSERT(false);
        return false;
    }

    if (!paramsNode.appendChild(&bFindInexactClonesNode)) {
        tcout << _T("Error: CCDOutputMgr::writeParametersToXMLFile(): failed to append child.") << endl;
        ASSERT(false);
        return false;
    }

    if (!paramsNode.appendChild(&regNormLevelNode)) {
        tcout << _T("Error: CCDOutputMgr::writeParametersToXMLFile(): failed to append child.") << endl;
        ASSERT(false);
        return false;
    }

    if (!paramsNode.appendChild(&bNormalizeTokenNode)) {
        tcout << _T("Error: CCDOutputMgr::writeParametersToXMLFile(): failed to append child.") << endl;
        ASSERT(false);
        return false;
    }

    if (!paramsNode.appendChild(&keyVectorSizeNode)) {
        tcout << _T("Error: CCDOutputMgr::writeParametersToXMLFile(): failed to append child.") << endl;
        ASSERT(false);
        return false;
    }

    if (!paramsNode.appendChild(&minCoOccThresholdNode)) {
        tcout << _T("Error: CCDOutputMgr::writeParametersToXMLFile(): failed to append child.") << endl;
        ASSERT(false);
        return false;
    }

    if (!parentNode.appendChild(&paramsNode)) {
        tcout << _T("Error: CCDOutputMgr::writeParametersToXMLFile(): failed to append child.") << endl;
        ASSERT(false);
        return false;
    }
    return true;
}


bool CCDOutputMgr::writeAssemblyFilesToXMLFile(CBFXmlDoc& cpFile, CBFXmlNode& parentNode) const
{
    CBFXmlNode assemblyFilesNode;
    if (!cpFile.createElement(_T("assembly_files"), &assemblyFilesNode)) {
        tcout << _T("Error: CCDOutputMgr::writeClonePairsTofile(): failed to create element.") << endl;
        ASSERT(false);
        return false;
    }

    int nTotalFunctions = 0, nTotalRegions = 0;
    CCDAssemblyFile* pAssemblyFile = NULL;
    for (int f = 0; f < m_pAssemblyFileMgr->getNumAssemblyFiles(); ++f) {        
        pAssemblyFile = m_pAssemblyFileMgr->getAssemblyFile(f);

        CBFXmlNode assemblyFileNode;
        if (!cpFile.createElement(_T("file"), &assemblyFileNode)) {
            tcout << _T("Error: CCDOutputMgr::writeClonePairsTofile(): failed to create assembly file node: ") << pAssemblyFile->getFilePath().GetString() << endl;
            ASSERT(false);
            return false;
        }

        if (!assemblyFileNode.setAttribute(_T("fileID"), (LONG) pAssemblyFile->m_fileIdx)) {
            tcout << _T("Error: CCDOutputMgr::writeClonePairsTofile(): failed to set attribute in assembly file node: ") << pAssemblyFile->getFilePath().GetString() << endl;
            ASSERT(false);
            return false;
        }

        int nFunctions = pAssemblyFile->getNumFunctions();
        nTotalFunctions += nFunctions;
        if (!assemblyFileNode.setAttribute(_T("nFunctions"), (LONG) nTotalFunctions)) {
            tcout << _T("Error: CCDOutputMgr::writeClonePairsTofile(): failed to set attribute in assembly file node: ") << pAssemblyFile->getFilePath().GetString() << endl;
            ASSERT(false);
            return false;
        }

        int nRegions = pAssemblyFile->getNumRegions();
        nTotalRegions += nRegions;
        if (!assemblyFileNode.setAttribute(_T("nRegions"), (LONG) nRegions)) {
            tcout << _T("Error: CCDOutputMgr::writeClonePairsTofile(): failed to set attribute in assembly file node: ") << pAssemblyFile->getFilePath().GetString() << endl;
            ASSERT(false);
            return false;
        }

        if (!assemblyFileNode.setAttribute(_T("path"), (LPCTSTR) pAssemblyFile->getFilePath())) {
            tcout << _T("Error: CCDOutputMgr::writeClonePairsTofile(): failed to set attribute in assembly file node: ") << pAssemblyFile->getFilePath().GetString() << endl;
            ASSERT(false);
            return false;
        }

        if (!assemblyFilesNode.appendChild(&assemblyFileNode)) {
            tcout << _T("Error: CCDOutputMgr::writeClonePairsTofile(): failed to append assembly file node: ") << pAssemblyFile->getFilePath().GetString() << endl;
            ASSERT(false);
            return false;
        }
    }

    if (!assemblyFilesNode.setAttribute(_T("nFiles"), (LONG) m_pAssemblyFileMgr->getNumAssemblyFiles())) {
        tcout << _T("Error: CCDOutputMgr::writeClonePairsTofile(): failed to set attribute.") << endl;
        ASSERT(false);
        return false;
    }

    if (!assemblyFilesNode.setAttribute(_T("nTotalFunctions"), (LONG) nTotalFunctions)) {
        tcout << _T("Error: CCDOutputMgr::writeClonePairsTofile(): failed to set attribute.") << endl;
        ASSERT(false);
        return false;
    }
    
    if (!assemblyFilesNode.setAttribute(_T("nTotalRegions"), (LONG) nTotalRegions)) {
        tcout << _T("Error: CCDOutputMgr::writeClonePairsTofile(): failed to set attribute.") << endl;
        ASSERT(false);
        return false;
    }

    if (!parentNode.appendChild(&assemblyFilesNode)) {
        tcout << _T("Error: CCDOutputMgr::writeClonePairsTofile(): failed to append child.") << endl;
        ASSERT(false);
        return false;
    }
    return true;
}


bool CCDOutputMgr::writeClonePairsToXMLFile(CBFXmlDoc& cpFile, CBFXmlNode& parentNode, int filterByFileID) const
{
    CBFXmlNode cloneFilesNode;
    if (!cpFile.createElement(_T("clone_files"), &cloneFilesNode)) {
        tcout << _T("Error: CCDOutputMgr::writeClonePairsToXMLFile(): failed to create element.") << endl;
        ASSERT(false);
        return false;
    }

    int nCloneFilePairs = 0, nTotalClonePairs = 0;
    CString key;
    CCDClonePair* pClonePair = NULL;
    CCDClonePairs* pClonePairs = NULL;    

    // For each array of pairs
    CCDClonePairsMap* pClonePairsMap = m_pCloneMgr->getClonePairsMap();
    POSITION pos = pClonePairsMap->GetStartPosition();
    while (pos)  {
        pClonePairsMap->GetNextAssoc(pos, key, pClonePairs);
        if (pClonePairs->IsEmpty())
            continue;

        if (filterByFileID != -1 && 
            pClonePairs->GetAt(0)->getRegionA()->getFunction()->getAssemblyFile()->m_fileIdx != filterByFileID && 
            pClonePairs->GetAt(0)->getRegionB()->getFunction()->getAssemblyFile()->m_fileIdx != filterByFileID)
            continue;   // skip if filtering is required both files are not the target file ID.

        CBFXmlNode cloneFileNode;
        if (!cpFile.createElement(_T("clone_file"), &cloneFileNode)) {
            tcout << _T("Error: CCDOutputMgr::writeClonePairsToXMLFile(): failed to create clone file node.") << endl;
            ASSERT(false);
            return false;
        }

        // index of file1
        if (!cloneFileNode.setAttribute(_T("fileA_id"), (LONG) pClonePairs->GetAt(0)->getRegionA()->getFunction()->getAssemblyFile()->m_fileIdx)) {
            tcout << _T("Error: CCDOutputMgr::writeClonePairsToXMLFile(): failed to create clone file node.") << endl;
            ASSERT(false);
            return false;
        }

        // index of file2
        if (!cloneFileNode.setAttribute(_T("fileB_id"), (LONG) pClonePairs->GetAt(0)->getRegionB()->getFunction()->getAssemblyFile()->m_fileIdx)) {
            tcout << _T("Error: CCDOutputMgr::writeClonePairsToXMLFile(): failed to create clone file node.") << endl;
            ASSERT(false);
            return false;
        }

        nTotalClonePairs += pClonePairs->GetSize();
        if (!cloneFileNode.setAttribute(_T("nClonePairs"), (LONG) pClonePairs->GetSize())) {
            tcout << _T("Error: CCDOutputMgr::writeClonePairsToXMLFile(): failed to set attribute.") << endl;
            ASSERT(false);
            return false;
        }

        // For each pairs
        for (int p = 0; p < pClonePairs->GetSize(); ++p) {
            pClonePair = pClonePairs->GetAt(p);

            CBFXmlNode clonePairNode;
            if (!cpFile.createElement(_T("clone_pair"), &clonePairNode)) {
                tcout << _T("Error: CCDOutputMgr::writeClonePairsToXMLFile(): failed to create clone pair node.") << endl;
                ASSERT(false);
                return false;
            }

            if (!clonePairNode.setAttribute(_T("regionA_start"), (LONG) pClonePair->getRegionA()->m_rawStartIdx)) {
                tcout << _T("Error: CCDOutputMgr::writeClonePairsToXMLFile(): failed to create clone pair node.") << endl;
                ASSERT(false);
                return false;
            }

            if (!clonePairNode.setAttribute(_T("regionA_end"), (LONG) pClonePair->getRegionA()->m_rawEndIdx)) {
                tcout << _T("Error: CCDOutputMgr::writeClonePairsToXMLFile(): failed to create clone pair node.") << endl;
                ASSERT(false);
                return false;
            }

            if (!clonePairNode.setAttribute(_T("regionB_start"), (LONG) pClonePair->getRegionB()->m_rawStartIdx)) {
                tcout << _T("Error: CCDOutputMgr::writeClonePairsToXMLFile(): failed to create clone pair node.") << endl;
                ASSERT(false);
                return false;
            }

            if (!clonePairNode.setAttribute(_T("regionB_end"), (LONG) pClonePair->getRegionB()->m_rawEndIdx)) {
                tcout << _T("Error: CCDOutputMgr::writeClonePairsToXMLFile(): failed to create clone pair node.") << endl;
                ASSERT(false);
                return false;
            }

            if (!cloneFileNode.appendChild(&clonePairNode)) {
                tcout << _T("Error: CCDOutputMgr::writeClonePairsToXMLFile(): failed to append clone pair node.") << endl;
                ASSERT(false);
                return false;
            }
        }

        if (!cloneFilesNode.appendChild(&cloneFileNode)) {
            tcout << _T("Error: CCDOutputMgr::writeClonePairsToXMLFile(): failed to append clone file node.") << endl;
            ASSERT(false);
            return false;
        }
        ++nCloneFilePairs;
    }

    if (!cloneFilesNode.setAttribute(_T("nCloneFilePairs"), (LONG) nCloneFilePairs)) {
        tcout << _T("Error: CCDOutputMgr::writeClonePairsToXMLFile(): failed to set attribute.") << endl;
        ASSERT(false);
        return false;
    }

    if (!cloneFilesNode.setAttribute(_T("nTotalClonePairs"), (LONG) nTotalClonePairs)) {
        tcout << _T("Error: CCDOutputMgr::writeClonePairsToXMLFile(): failed to set attribute.") << endl;
        ASSERT(false);
        return false;
    }

    if (!parentNode.appendChild(&cloneFilesNode)) {
        tcout << _T("Error: CCDOutputMgr::writeClonePairsTofile(): failed to append child.") << endl;
        ASSERT(false);
        return false;
    }
    return true;
}


bool CCDOutputMgr::writeTokenReferencesToXMLFile(CBFXmlDoc& cpFile, CBFXmlNode& parentNode) const
{
    CBFXmlNode tokenReferencesNode;
    if (!cpFile.createElement(_T("token_references"), &tokenReferencesNode)) {
        tcout << _T("Error: CCDOutputMgr::writeTokenReferencesToXMLFile(): failed to create element.") << endl;
        ASSERT(false);
        return false;
    }

    if (!tokenReferencesNode.setAttribute(_T("nTotalTokens"), (LONG) (m_pTokenRefMgr->getNumConstants() + m_pTokenRefMgr->getNumStrings() + m_pTokenRefMgr->getNumImports()))) {
        tcout << _T("Error: CCDOutputMgr::writeTokenReferencesToXMLFile(): failed to set attribute in token references.") << endl;
        ASSERT(false);
        return false;
    }

    POSITION pos = NULL;
    CString tokenKey;
    CCDTokenReferences* pTokenRefs = NULL;

    //************
    // Constants *
    //************
    CBFXmlNode constantsIndexNode;
    if (!cpFile.createElement(_T("constants"), &constantsIndexNode)) {
        tcout << _T("Error: CCDOutputMgr::writeTokenReferencesToXMLFile(): failed to create element.") << endl;
        ASSERT(false);
        return false;
    }

    if (!constantsIndexNode.setAttribute(_T("nTokens"), (LONG) m_pTokenRefMgr->getNumConstants())) {
        tcout << _T("Error: CCDOutputMgr::writeTokenReferencesToXMLFile(): failed to set attribute in constant references.") << endl;
        ASSERT(false);
        return false;
    }
    
    const CCDTokenRefMap* pConstantsIdx = m_pTokenRefMgr->getConstantsIndex();
    pos = pConstantsIdx->GetStartPosition();
    while (pos)  {
        pConstantsIdx->GetNextAssoc(pos, tokenKey, pTokenRefs);

        CBFXmlNode indexNode;
        if (!cpFile.createElement(_T("index"), &indexNode)) {
            tcout << _T("Error: CCDOutputMgr::writeTokenReferencesToXMLFile(): failed to create element.") << endl;
            ASSERT(false);
            return false;
        }

        if (!indexNode.setAttribute(_T("token"), (LPCTSTR) tokenKey)) {
            tcout << _T("Error: CCDOutputMgr::writeTokenReferencesToXMLFile(): failed to set attribute in constant references.") << endl;
            ASSERT(false);
            return false;
        }

        for (int i = 0; i < pTokenRefs->GetSize(); ++i) {
            CBFXmlNode refNode;
            if (!cpFile.createElement(_T("reference"), &refNode)) {
                tcout << _T("Error: CCDOutputMgr::writeTokenReferencesToXMLFile(): failed to create element.") << endl;
                ASSERT(false);
                return false;
            }

            if (!refNode.setAttribute(_T("fileID"), (LONG) pTokenRefs->GetAt(i)->getRefFile()->m_fileIdx)) {
                tcout << _T("Error: CCDOutputMgr::writeTokenReferencesToXMLFile(): failed to set attribute in constant references.") << endl;
                ASSERT(false);
                return false;
            }

            if (!refNode.setAttribute(_T("line"), (LONG) pTokenRefs->GetAt(i)->getRefRawLineNumber())) {
                tcout << _T("Error: CCDOutputMgr::writeTokenReferencesToXMLFile(): failed to set attribute in constant references.") << endl;
                ASSERT(false);
                return false;
            }

            if (!indexNode.appendChild(&refNode)) {
                tcout << _T("Error: CCDOutputMgr::writeTokenReferencesToXMLFile(): failed to append reference node.") << endl;
                ASSERT(false);
                return false;
            }
        }

        if (!constantsIndexNode.appendChild(&indexNode)) {
            tcout << _T("Error: CCDOutputMgr::writeTokenReferencesToXMLFile(): failed to append reference node.") << endl;
            ASSERT(false);
            return false;
        }
    }
    if (!tokenReferencesNode.appendChild(&constantsIndexNode)) {
        tcout << _T("Error: CCDOutputMgr::writeTokenReferencesToXMLFile(): failed to append reference node.") << endl;
        ASSERT(false);
        return false;
    }

    //**********
    // Strings *
    //**********
    CBFXmlNode stringsIndexNode;
    if (!cpFile.createElement(_T("strings"), &stringsIndexNode)) {
        tcout << _T("Error: CCDOutputMgr::writeTokenReferencesToXMLFile(): failed to create element.") << endl;
        ASSERT(false);
        return false;
    }

    if (!stringsIndexNode.setAttribute(_T("nTokens"), (LONG) m_pTokenRefMgr->getNumStrings())) {
        tcout << _T("Error: CCDOutputMgr::writeTokenReferencesToXMLFile(): failed to set attribute in string references.") << endl;
        ASSERT(false);
        return false;
    }

    const CCDTokenRefMap* pStringsIdx = m_pTokenRefMgr->getStringsIndex();
    pos = pStringsIdx->GetStartPosition();
    while (pos)  {
        pStringsIdx->GetNextAssoc(pos, tokenKey, pTokenRefs);

        CBFXmlNode indexNode;
        if (!cpFile.createElement(_T("index"), &indexNode)) {
            tcout << _T("Error: CCDOutputMgr::writeTokenReferencesToXMLFile(): failed to create element.") << endl;
            ASSERT(false);
            return false;
        }

        if (!indexNode.setAttribute(_T("token"), (LPCTSTR) tokenKey)) {
            tcout << _T("Error: CCDOutputMgr::writeTokenReferencesToXMLFile(): failed to set attribute in string references.") << endl;
            ASSERT(false);
            return false;
        }

        for (int i = 0; i < pTokenRefs->GetSize(); ++i) {
            CBFXmlNode refNode;
            if (!cpFile.createElement(_T("reference"), &refNode)) {
                tcout << _T("Error: CCDOutputMgr::writeTokenReferencesToXMLFile(): failed to create element.") << endl;
                ASSERT(false);
                return false;
            }

            if (!refNode.setAttribute(_T("fileID"), (LONG) pTokenRefs->GetAt(i)->getRefFile()->m_fileIdx)) {
                tcout << _T("Error: CCDOutputMgr::writeTokenReferencesToXMLFile(): failed to set attribute in string references.") << endl;
                ASSERT(false);
                return false;
            }

            if (!refNode.setAttribute(_T("line"), (LONG) pTokenRefs->GetAt(i)->getRefRawLineNumber())) {
                tcout << _T("Error: CCDOutputMgr::writeTokenReferencesToXMLFile(): failed to set attribute in string references.") << endl;
                ASSERT(false);
                return false;
            }

            if (!indexNode.appendChild(&refNode)) {
                tcout << _T("Error: CCDOutputMgr::writeTokenReferencesToXMLFile(): failed to append reference node.") << endl;
                ASSERT(false);
                return false;
            }
        }

        if (!stringsIndexNode.appendChild(&indexNode)) {
            tcout << _T("Error: CCDOutputMgr::writeTokenReferencesToXMLFile(): failed to append reference node.") << endl;
            ASSERT(false);
            return false;
        }
    }
    if (!tokenReferencesNode.appendChild(&stringsIndexNode)) {
        tcout << _T("Error: CCDOutputMgr::writeTokenReferencesToXMLFile(): failed to append reference node.") << endl;
        ASSERT(false);
        return false;
    }

    //**********
    // Imports *
    //**********
    CBFXmlNode importsIndexNode;
    if (!cpFile.createElement(_T("imports"), &importsIndexNode)) {
        tcout << _T("Error: CCDOutputMgr::writeTokenReferencesToXMLFile(): failed to create element.") << endl;
        ASSERT(false);
        return false;
    }

    if (!importsIndexNode.setAttribute(_T("nTokens"), (LONG) m_pTokenRefMgr->getNumImports())) {
        tcout << _T("Error: CCDOutputMgr::writeTokenReferencesToXMLFile(): failed to set attribute in import references.") << endl;
        ASSERT(false);
        return false;
    }

    const CCDTokenRefMap* pImportsIdx = m_pTokenRefMgr->getImportsIndex();
    pos = pImportsIdx->GetStartPosition();
    while (pos)  {
        pImportsIdx->GetNextAssoc(pos, tokenKey, pTokenRefs);

        CBFXmlNode indexNode;
        if (!cpFile.createElement(_T("index"), &indexNode)) {
            tcout << _T("Error: CCDOutputMgr::writeTokenReferencesToXMLFile(): failed to create element.") << endl;
            ASSERT(false);
            return false;
        }

        if (!indexNode.setAttribute(_T("token"), (LPCTSTR) tokenKey)) {
            tcout << _T("Error: CCDOutputMgr::writeTokenReferencesToXMLFile(): failed to set attribute in import references.") << endl;
            ASSERT(false);
            return false;
        }

        for (int i = 0; i < pTokenRefs->GetSize(); ++i) {
            CBFXmlNode refNode;
            if (!cpFile.createElement(_T("reference"), &refNode)) {
                tcout << _T("Error: CCDOutputMgr::writeTokenReferencesToXMLFile(): failed to create element.") << endl;
                ASSERT(false);
                return false;
            }

            if (!refNode.setAttribute(_T("fileID"), (LONG) pTokenRefs->GetAt(i)->getRefFile()->m_fileIdx)) {
                tcout << _T("Error: CCDOutputMgr::writeTokenReferencesToXMLFile(): failed to set attribute in import references.") << endl;
                ASSERT(false);
                return false;
            }

            if (!refNode.setAttribute(_T("line"), (LONG) pTokenRefs->GetAt(i)->getRefRawLineNumber())) {
                tcout << _T("Error: CCDOutputMgr::writeTokenReferencesToXMLFile(): failed to set attribute in import references.") << endl;
                ASSERT(false);
                return false;
            }

            if (!indexNode.appendChild(&refNode)) {
                tcout << _T("Error: CCDOutputMgr::writeTokenReferencesToXMLFile(): failed to append reference node.") << endl;
                ASSERT(false);
                return false;
            }
        }

        if (!importsIndexNode.appendChild(&indexNode)) {
            tcout << _T("Error: CCDOutputMgr::writeTokenReferencesToXMLFile(): failed to append reference node.") << endl;
            ASSERT(false);
            return false;
        }
    }
    if (!tokenReferencesNode.appendChild(&importsIndexNode)) {
        tcout << _T("Error: CCDOutputMgr::writeTokenReferencesToXMLFile(): failed to append reference node.") << endl;
        ASSERT(false);
        return false;
    }

    // Append everything to the root node.
    if (!parentNode.appendChild(&tokenReferencesNode)) {
        tcout << _T("Error: CCDOutputMgr::writeClonePairsTofile(): failed to append child.") << endl;
        ASSERT(false);
        return false;
    }
    return true;
}

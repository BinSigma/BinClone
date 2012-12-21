
#include "StdAfx.h"
#include "CloneFiles.h"
#include "BFStrHelper.h"



CloneFiles::CloneFiles()
: m_windSize(0),
  m_stride(0),
  m_maxKOperands(0),
  m_maxOverlapFraction(0),
  m_bFindExactClones(false),
  m_FindInexactClones(false),
  m_normalLevel(_T("")),
  m_bNormalizeToken(false),
  m_keyVectorsSize(0),
  m_occurrenceThrs(0),
  m_inexactLevel(_T("")), 
  m_nTotalClonePairs(0),
  m_xmlFile(_T(""))
{
    m_funcMap[_T("parameters")]        = &CloneFiles::extractParameter; 
    m_funcMap[_T("assembly_files")]    = &CloneFiles::extractAsmFiles; 
    m_funcMap[_T("clone_files")]       = &CloneFiles::extractCloneFiles; 
	m_funcMap[_T("token_references")]  = &CloneFiles::extractTokenReferences; 
}

CloneFiles::~CloneFiles()
{
}

bool CloneFiles::init( const CString & p_xmlFile, CString & p_errorInfo)
{
	BOOL result(false);
	CBFXmlDoc xmlDoc;
	result = xmlDoc.loadXML(p_xmlFile);
	if( !result)
	{
		p_errorInfo = _T("Failed to load XML file");
		AfxMessageBox( p_errorInfo);
		return false;
	}

	CBFXmlNode pDocNode;
	result = xmlDoc.getDocumentNode(&pDocNode);
	if( !result)
	{
		p_errorInfo = _T("Failed to getDocumentNode.");
		AfxMessageBox( p_errorInfo);
		return false;
	}

	CString text(_T(""));
	pDocNode.getNodeName(text);
	if( StrCmp( text, _T("clone_detection")) != 0)
	{
		p_errorInfo = _T("Error to parse clone_detection.");
		AfxMessageBox(p_errorInfo);
	    return false;
	}

	//AfxMessageBox(text);

	CBFXmlNodeList pNodeList;
	result = pDocNode.getChildNodes(&pNodeList);
	if( !result || pNodeList.isEmpty() )
	{
		p_errorInfo = _T("Error to getChileNodes.");
	    return false;
	}

	CBFXmlNode node;
	result = pNodeList.getNextNode(&node);	
	if( !result )
	{
		p_errorInfo = _T("Error to getNextNode.");
		AfxMessageBox(p_errorInfo);
		return false;
	}

	while( !node.isEmpty())
	{
		CString nodeName;
		node.getNodeName(nodeName);

		FuncPtrMap_t::iterator itor = m_funcMap.find(nodeName);
		if( itor != m_funcMap.end())
		{
			pF pFunc = itor->second;
			((*this).*pFunc)(node);
		}

	    result = pNodeList.getNextNode(&node);	
	    if( !result )
	    {
		    p_errorInfo = _T("Error to getNextNode.");
		    AfxMessageBox(p_errorInfo);
		    return false;
	    }	
	} 

	m_xmlFile=p_xmlFile;
	
	return true;
}


bool CloneFiles::extractParameter(CBFXmlNode&  p_Node)
{
	CBFStrHelper strHelper;
	if( p_Node.isEmpty())
	{
		AfxMessageBox(_T("Error in parsing parameter, node is empty."));
		return false;
	}

	CBFXmlNodeList childNodeList;
	if( !p_Node.getChildNodes(&childNodeList))
	{
		AfxMessageBox(_T("Error in parsing parameter, getChildNodes returned failure."));
		return false;
	}

	CBFXmlNode childNode;

	if( !childNodeList.getNextNode(&childNode) )
	{
		AfxMessageBox(_T("Error in parsing parameter, getNextNode returned failure"));
		return false;
	}

    while( !childNode.isEmpty())
	{
		CString    childName;
		if( !childNode.getNodeName(childName))
		{
			AfxMessageBox(_T("Error in parsing child node in parameter, getNodeName returned failure."));
			continue;
		}

		if( StrCmp ( childName, _T("windowSize")) == 0 )
		{
            CString value;
            if( !childNode.getText(value))
            {
               AfxMessageBox(_T("Error in parsing windowSize, getText returned failure."));
               continue;
            }
            m_windSize = strHelper.strToInt(value);
		}
		else if( StrCmp ( childName, _T("stride")) == 0 )
		{
            CString value;
            if( !childNode.getText(value))
            {
               AfxMessageBox(_T("Error in parsing stride, getText returned failure."));
               continue;
            }
            m_stride = strHelper.strToInt(value);
		}
		else if( StrCmp ( childName, _T("maxKOperands")) == 0 )
		{
            CString value;
            if( !childNode.getText(value))
            {
               AfxMessageBox(_T("Error in parsing mazKOperands, getText returned failure."));
               continue;
            }
            m_maxKOperands = strHelper.strToInt(value);
		}
		else if( StrCmp ( childName, _T("maxOverlapFraction")) == 0 )
		{
            CString value;
            if( !childNode.getText(value))
            {
               AfxMessageBox(_T("Error in parsing maxOverlapFraction, getText returned failure."));
               continue;
            }
            m_maxOverlapFraction = strHelper.strToFloat(value);
		}
		else if( StrCmp ( childName, _T("bFindExactClones")) == 0 )
		{
            CString value;
            if( !childNode.getText(value))
            {
               AfxMessageBox(_T("Error in parsing maxOverlapFraction, getText returned failure."));
               continue;
            }
            m_bFindExactClones = (StrCmp(value,_T("TRUE"))==0?true:false);
		}
		else if( StrCmp ( childName, _T("bFindInexactClones")) == 0 )
		{
            CString value;
            if( !childNode.getText(value))
            {
               AfxMessageBox(_T("Error in parsing bFindInexactClones, getText returned failure."));
               continue;
            }
            m_FindInexactClones = (StrCmp(value,_T("TRUE"))==0?true:false);
		}
		else if( StrCmp ( childName, _T("regNormLevel")) == 0 )
		{
            if( !childNode.getText(m_normalLevel))
            {
               AfxMessageBox(_T("Error in parsing regNormLevel, getText returned failure."));
               continue;
            }
		}
		else if( StrCmp ( childName, _T("bNormalizeToken")) == 0 )
		{
            CString value;
            if( !childNode.getText(value))
            {
               AfxMessageBox(_T("Error in parsing bNormalizeToken, getText returned failure."));
               continue;
            }
            m_bNormalizeToken = (StrCmp(value,_T("TRUE"))==0?true:false);
		}
		else if( StrCmp ( childName, _T("keyVectorSize")) == 0 )
		{
            CString value;
            if( !childNode.getText(value))
            {
               AfxMessageBox(_T("Error in parsing keyVectorSize, getText returned failure."));
               continue;
            }
            m_keyVectorsSize = strHelper.strToInt(value);
		}
		else if( StrCmp ( childName, _T("occThreshold")) == 0 )
		{
            CString value;
            if( !childNode.getText(value))
            {
               AfxMessageBox(_T("Error in parsing occThreshold, getText returned failure."));
               continue;
            }
            m_occurrenceThrs = strHelper.strToFloat(value);
		}
		
        		else if( StrCmp ( childName, _T("inexactMethod")) == 0 )
		{
            if( !childNode.getText(m_inexactLevel))
            {
               AfxMessageBox(_T("Error in parsing inexactMethod, getText returned failure."));
               continue;
            }
		}

		/*
		else {
		    //AfxMessageBox(_T("Error in parsing parameter, unsupported XML tag encountered."));    // bfung: temporaily remove it due to new parameters added to the XML file.
		}
		*/

		if( !childNodeList.getNextNode(&childNode))
	    {
			AfxMessageBox(_T("Error in parsing parameter, getNextNode returned failure."));
		    return false;
	    }
	}

	return true;
}

bool CloneFiles::extractAsmFiles(CBFXmlNode&   p_Node)
{
	CBFStrHelper strHelper;
	
	m_asmFilesMap.clear(); // clear up 

	if( p_Node.isEmpty())
	{
		AfxMessageBox(_T("Error in parsing assembly_files, node is empty."));
		return false;
	}

	int nFile(0);
	CString tmp(_T(""));
	if( p_Node.getAttribute(_T("nFiles"),tmp))
	{
		nFile = strHelper.strToInt(tmp);
	    /*
		CString n;
		n.Format(_T("nFiles=%d"),nFile);
		AfxMessageBox(n);
		*/
	}

	CBFXmlNodeList childNodeList;
	if( !p_Node.getChildNodes(&childNodeList))
	{
		AfxMessageBox(_T("Error in parsing assembly_files, getChildNodes returned failure."));
		return false;
	}

	CBFXmlNode childNode;

	if( !childNodeList.getNextNode(&childNode) )
	{
		AfxMessageBox(_T("Error in parsing assembly_files, getNextNode returned failure"));
		return false;
	}

    while( !childNode.isEmpty())
	{
		CString    childName;
		if( !childNode.getNodeName(childName))
		{
			AfxMessageBox(_T("Error in parsing child node in assembly_files, getNodeName returned failure."));
			continue;
		}

		if( StrCmp ( childName, _T("file")) == 0 )
		{
            CString attrName(_T("fileID"));
			CString value;

			if( childNode.hasAttribute(attrName))
			{
			   if( !childNode.getAttribute(attrName,value))
               {
                   AfxMessageBox(_T("Error in parsing file, failed to get the fileID."));
                   continue;
			   } 
            }
			unsigned fileId=0;
			fileId= strHelper.strToInt(value);

			attrName = _T("path");
			if( childNode.hasAttribute(attrName))
			{
			   if( !childNode.getAttribute(attrName,value))
               {
                   AfxMessageBox(_T("Error in parsing file, failed to get the file path."));
                   continue;
			   }
			}
			/*
			CString tmp;
			tmp.Format(_T("fileID=%d, path=%s"),fileId,value);
			AfxMessageBox(tmp);
			*/
		    
			m_asmFilesMap[fileId] = value;
		}
            
		if( !childNodeList.getNextNode(&childNode))
	    {
			AfxMessageBox(_T("Error in parsing assembly_files, getNextNode returned failure."));
		    return false;
	    }

	} // end while

	if( nFile != m_asmFilesMap.size())
	{
		CString tmp;
		tmp.Format(_T("Warning, the number of parsed files IDs = %d is not the same as the XML attribute nFiles=%d"),m_asmFilesMap.size(),nFile);
		AfxMessageBox(tmp);
	}

    return true;
}

bool CloneFiles::extractCloneFiles(CBFXmlNode& p_Node)
{
	CBFStrHelper strHelper;
	
	m_cloneFileVector.clear(); // clear up 

	if( p_Node.isEmpty())
	{
		AfxMessageBox(_T("Error in parsing clone_files, node is empty."));
		return false;
	}

	int nCloneFilePairs(0);
	CString tmp(_T(""));
	if( p_Node.getAttribute(_T("nCloneFilePairs"),tmp))
	{
		nCloneFilePairs = strHelper.strToInt(tmp);
	    /*
		CString n;
		n.Format(_T("nCloneFilePairs=%d"),nCloneFilePairs);
		AfxMessageBox(n);
		*/
	}

	int nTotalClonePairs(0);
	int totalClonePairs(0);
	tmp = _T("");
	if( p_Node.getAttribute(_T("nTotalClonePairs"),tmp))
	{
		nTotalClonePairs = strHelper.strToInt(tmp);
	    /*
		CString n;
		n.Format(_T("nTotalClonePairs=%d"),nTotalClonePairs);
		AfxMessageBox(n);
		*/
	}

	CBFXmlNodeList cloneFileList;
	if( !p_Node.getChildNodes(&cloneFileList))
	{
		AfxMessageBox(_T("Error in parsing clone_files, getChildNodes returned failure."));
		return false;
	}

	CBFXmlNode cloneFileNode;

	if( !cloneFileList.getNextNode(&cloneFileNode) )
	{
		AfxMessageBox(_T("Error in parsing clone_files, getNextNode returned failure"));
		return false;
	}

    while( !cloneFileNode.isEmpty())
	{
		CString    cloneFileName;
		if( !cloneFileNode.getNodeName(cloneFileName))
		{
			AfxMessageBox(_T("Error in parsing child node in clone_files, getNodeName returned failure."));
			return false;
		}

		if( StrCmp ( cloneFileName, _T("clone_file")) == 0 )
		{
			CString attrName(_T("nClonePairs"));
			CString value;

			if( cloneFileNode.hasAttribute(attrName))
			{
			   if( !cloneFileNode.getAttribute(attrName,value))
               {
                   AfxMessageBox(_T("Error in parsing clone_file, failed to get the nClonePairs."));
                   return false;
			   } 
            }
			unsigned int nClonePairs=0;
			nClonePairs = strHelper.strToInt(value);

            attrName = (_T("fileA_id"));
			if( cloneFileNode.hasAttribute(attrName))
			{
			   if( !cloneFileNode.getAttribute(attrName,value))
               {
                   AfxMessageBox(_T("Error in parsing clone_file, failed to get the fileA_id."));
                   return false;
			   } 
            }
			unsigned int fileIdA=0;
			fileIdA= strHelper.strToInt(value);

			attrName = _T("fileB_id");
			if( cloneFileNode.hasAttribute(attrName))
			{
			   if( !cloneFileNode.getAttribute(attrName,value))
               {
                   AfxMessageBox(_T("Error in parsing clone_file, failed to get the fileB_id."));
                   return false;
			   }
			}
			unsigned int fileIdB=0;
			fileIdB= strHelper.strToInt(value);

			/*
			CString tmp;
			tmp.Format(_T("nClonePairs=%d, fileA_id=%d, fileB_id=%d"),nClonePairs,fileIdA,fileIdB);
			AfxMessageBox(tmp);
		    */

			AsmFilesMap_t::iterator itr = m_asmFilesMap.find(fileIdA);
			if( itr == m_asmFilesMap.end())
			{
			       AfxMessageBox(_T("Error in parsing clone_file, Can't find the corresding file for fileA_id."));
                   return false;
			}
			CString strFileA = itr->second;
			
			itr = m_asmFilesMap.find(fileIdB);
			if( itr == m_asmFilesMap.end())
			{
			       AfxMessageBox(_T("Error in parsing clone_file, Can't find the corresding file for fileB_id."));
                   return false;
			}
			CString strFileB = itr->second;

			/*
			CString tmp2;
			tmp.Format(_T("fileA_id=%s\nfileB_id=%s"),strFileA,strFileB);
			AfxMessageBox(tmp);
			*/

			// create the CloneFile object
			CloneFile cloneFileObj(strFileA,strFileB);

			// now get the clone_pair
			CBFXmlNodeList clonePairNodeList;
	        if( !cloneFileNode.getChildNodes(&clonePairNodeList))
	        {
		       AfxMessageBox(_T("Error in parsing clone_file, getChildNodes returned failure."));
		       return false;
	        }

			CBFXmlNode clonePairNode;

			if( !clonePairNodeList.getNextNode(&clonePairNode) )
			{	
				//p_errorInfo = _T("Error in parsing parameter, getNextNode returned failure");
				AfxMessageBox(_T("Error in parsing clone_pair, getNextNode returned failure"));
				return false;
			}

			while( !clonePairNode.isEmpty())
	        {
				CString  clonePairName;
		        if( !clonePairNode.getNodeName(clonePairName))
				{
					AfxMessageBox(_T("Error in parsing clone_pair, getNodeName returned failure."));
					return false;
				}

				if( StrCmp ( clonePairName, _T("clone_pair")) == 0 )
				{
					CString attrRegBstart(_T("regionB_start"));
					CString attrRegBend(_T("regionB_end"));
					CString attrRegAstart(_T("regionA_start"));
					CString attrRegAend(_T("regionA_end"));

					CString regBstart(_T(""));
					CString regBend(_T(""));
					CString regAstart(_T(""));
					CString regAend(_T(""));
					
			        if( !clonePairNode.getAttribute(attrRegBstart,regBstart) ||
						!clonePairNode.getAttribute(attrRegBend,regBend) ||
						!clonePairNode.getAttribute(attrRegAstart,regAstart) ||
						!clonePairNode.getAttribute(attrRegAend,regAend)
						)
					{
						 AfxMessageBox(_T("Error in parsing clone_pair, failed to get the attributes."));
						 return false;					
					}

					int regionBstart_int = strHelper.strToInt( regBstart);
					int regionBend_int   = strHelper.strToInt( regBend);
					int regionAstart_int = strHelper.strToInt( regAstart);
					int regionAend_int   = strHelper.strToInt( regAend);

					/*
					CString tmp3;
			        tmp3.Format(_T("regionB_end=%d regionB_start=%d regionA_end=%d regionA_start=%d"),
						        regionBend_int, regionBstart_int, regionAend_int, regionAstart_int);
			        AfxMessageBox(tmp3);
					*/
					// create the RegionA/B
					Region regA(regionAstart_int,regionAend_int);
					Region regB(regionBstart_int,regionBend_int);

					// insert them into cloneFileObj
					cloneFileObj.insertClonePair(regA,regB);

				} // if clone_pair
				else
				{
					AfxMessageBox(_T("Warning: unsupported XML tag encountered."));
				}

				if( !clonePairNodeList.getNextNode(&clonePairNode) )
				{	
				    AfxMessageBox(_T("Error in parsing clone_pair, getNextNode returned failure"));
				    return false;
				}

			} // while

			if( nClonePairs != cloneFileObj.getNumberOfClonePairs())
			{
		        CString tmp;
				tmp.Format(_T("Warning, the number of parsed clone pairs = %d is not the same as the XML attribute nClonePairs=%d"),
					       cloneFileObj.getNumberOfClonePairs(),nClonePairs);
				AfxMessageBox(tmp);
			}

			totalClonePairs+=nClonePairs;

			insertCloneFile(cloneFileObj);

		} // if clone_files
            
		if( !cloneFileList.getNextNode(&cloneFileNode))
	    {
			AfxMessageBox(_T("Error in parsing clone_files, getNextNode returned failure."));
		    return false;
	    }

	} // end while

	if( nTotalClonePairs != totalClonePairs)
	{
		CString tmp;
		tmp.Format(_T("Warning, the number of parsed nClonePairs = %d is not the same as the XML attribute nTotalClonePairs=%d"),
			       totalClonePairs,nTotalClonePairs);
		AfxMessageBox(tmp);
	}

	m_nTotalClonePairs = nTotalClonePairs;

	if( nCloneFilePairs != m_cloneFileVector.size())
	{
		CString tmp;
		tmp.Format(_T("Warning, the number of parsed clone file pairs = %d is not the same as the XML attribute nCloneFilePairs=%d"),
			       m_cloneFileVector.size(),nCloneFilePairs);
		AfxMessageBox(tmp);
	}

    return true;
}

bool CloneFiles::extractTokenReferences(CBFXmlNode& p_Node)
{
	CBFStrHelper strHelper;
	
	m_tokenRefsVector.clear(); // clear up 

	if( p_Node.isEmpty())
	{
		AfxMessageBox(_T("Error in parsing token_references, node is empty."));
		return false;
	}

	int nTotalTokens(0);
	CString tmp(_T(""));
	if( p_Node.getAttribute(_T("nTotalTokens"),tmp))
	{
		nTotalTokens = strHelper.strToInt(tmp);
	    /*
		CString n;
		n.Format(_T("nTotalTokens=%d"),nTotalTokens);
		AfxMessageBox(n);
		*/
	}

	CBFXmlNodeList tokenRefsList; 
	if( !p_Node.getChildNodes(&tokenRefsList))
	{
		AfxMessageBox(_T("Error in parsing token references, getChildNodes returned failure."));
		return false;
	}

	CBFXmlNode tokenRefsNode; // list of nodes <constants>,<strings>,<imports>,...
	if( !tokenRefsList.getNextNode(&tokenRefsNode) )
	{
		AfxMessageBox(_T("Error in parsing token references node, getNextNode returned failure"));
		return false;
	}

	// for each of the child nodes of <constants/> <strings/> or <imports/>
    while( !tokenRefsNode.isEmpty())
	{
		CString tokenType;
		if( !tokenRefsNode.getNodeName(tokenType))
		{
			AfxMessageBox(_T("Error in parsing child node in token_references, getNodeName returned failure."));
			return false;
		}

		int nTokens(0);
		CString value_nTokens;
		if( !tokenRefsNode.getAttribute(_T("nTokens"),value_nTokens))
		{
			AfxMessageBox(_T("Error in parsing token references node, failed to get the nTokens. Continue..."));
		} 
		nTokens = strHelper.strToInt(value_nTokens);

		// create the TokenReference record
		TokenReference tokenReference;
		tokenReference.m_type=tokenType;

		// now get the index token list <index>,<index>,...
		CBFXmlNodeList indexTokenList;
	    if( !tokenRefsNode.getChildNodes(&indexTokenList))
		{
			AfxMessageBox(_T("Error in parsing index token, getChildNodes returned failure."));	
			break;
	    }

		CBFXmlNode indexTokenNode;
		if( !indexTokenList.getNextNode(&indexTokenNode) )
		{
			AfxMessageBox(_T("Error in parsing index token node, getNextNode returned failure"));
			break;
		}

		// for each of the node of <index> 
		while( !indexTokenNode.isEmpty()) 
		{
			CString value_token;
			if( !indexTokenNode.getAttribute(_T("token"),value_token))
			{
				AfxMessageBox(_T("Error in parsing index token node, failed to get the attribute token, Continue..."));
			    break;
			} 

			// create an IndexToken record
			IndexToken indexToken;
			indexToken.m_token = value_token;

			// now get the reference list <reference>,...
			CBFXmlNodeList referenctList;
			if( !indexTokenNode.getChildNodes(&referenctList))
			{
				AfxMessageBox(_T("Error in parsing reference list, getChildNodes returned failure."));	
				break;
			}

			CBFXmlNode referenceNode;
			if( !referenctList.getNextNode(&referenceNode) )
			{	
				AfxMessageBox(_T("Error in parsing reference node, getNextNode returned failure, continue..."));
			    break;
			}

			// for each nodes of <reference>
			while( !referenceNode.isEmpty())
			{
				//CString attr_fileID(_T("fileID"));
				CString value_fileID(_T(""));
				CString value_line(_T(""));
				
				if( !referenceNode.getAttribute(_T("fileID"),value_fileID) ||
					!referenceNode.getAttribute(_T("line"),value_line))
				{
					AfxMessageBox(_T("Error in parsing reference node, failed to get the attribute fileID or line, Continue next node..."));
				} 
				else
				{
					int fileID(0);
					int line(0);
					fileID = strHelper.strToInt(value_fileID);
					line   = strHelper.strToInt(value_line);
					Reference ref = {fileID,line};

				    // create a reference record and store it to the vector
					indexToken.m_reference.push_back(ref);
				}

				if( !referenctList.getNextNode(&referenceNode))
				{	
					AfxMessageBox(_T("Error in parsing reference node, getNextNode returned failure."));
					break;
				}
			}// while <reference>
		
			//store the index token record 
			tokenReference.m_indexTokenVector.push_back( indexToken);

			// get the next index token node
			if( !indexTokenList.getNextNode(&indexTokenNode) )
			{
				AfxMessageBox(_T("Error in parsing index token node, getNextNode returned failure"));
				break;
			}
		} // while <index>,...

		// store token reference reord
	    m_tokenRefsVector.push_back(tokenReference);

		// get the next token type
		if( !tokenRefsList.getNextNode(&tokenRefsNode))
		{
			AfxMessageBox(_T("Error in parsing token references node, getNextNode returned failure"));	
		}		
		
	} // while toke type <constants>,<strings>,...	

	return true;
}


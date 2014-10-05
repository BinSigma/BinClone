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

#pragma once

#ifndef _CLONEFILES_H_
#define _CLONEFILES_H_

#include "BFXmlPser.h"
#include <vector>
#include <map>
#include <hash_map>


//-----------------------------------------------------------------------------
// CString helper functions for hash_map
//-----------------------------------------------------------------------------
template<>
inline size_t stdext::hash_value< CString >(const CString & s)
{
    return stdext::hash_value((LPCTSTR)s);
}

struct Region
{
    unsigned int m_start;
    unsigned int m_end;

	Region(){};

	Region( unsigned int p_start, unsigned int p_end)
	: m_start(p_start),m_end(p_end){};
	
	Region(const Region &rhs)
	{
		m_start = rhs.m_start;
		m_end = rhs.m_end;
	}

	Region& operator=(const Region &rhs)
	{
		if( this!=&rhs)
		{
			m_start=rhs.m_start;
			m_end=rhs.m_end;
		}
		return *this;
	}
};

struct ClonePair
{
    Region m_regionA;
    Region m_regionB;

	ClonePair(){};

	ClonePair(const Region &p_regA,const Region&p_regB)
		:m_regionA(p_regA),m_regionB(p_regB){};

	ClonePair( const ClonePair & rhs)
	{
		m_regionA=rhs.m_regionA;
		m_regionB=rhs.m_regionB;
	}

	ClonePair& operator=(const ClonePair &rhs)
	{
		if( this!=&rhs)
		{
			m_regionA=rhs.m_regionA;
			m_regionB=rhs.m_regionB;
		}
		return *this;
	}
};

class CloneFile
{
public:

    typedef std::vector<ClonePair> ClonePairsVector_t;

	CloneFile()
	{
		m_fileA=_T("");
		m_fileB=_T("");
		m_pairs.clear();
	};
	virtual ~CloneFile(){};

    CloneFile(const CString& fileA, const CString& fileB)
	: m_fileA(fileA), m_fileB(fileB){};

	CloneFile( const CloneFile& rhs)
	{
		m_fileA=rhs.m_fileA;
		m_fileB=rhs.m_fileB;
		m_pairs=rhs.m_pairs;
	}

	CloneFile & operator=(const CloneFile &rhs)
	{
		if( this!=&rhs)
		{
            m_fileA=rhs.m_fileA;
		    m_fileB=rhs.m_fileB;
		    m_pairs=rhs.m_pairs;
		}
		return *this;
	}

	void setfiles( const CString& fileA, const CString& fileB){};
	void insertClonePair(const Region regionA, const Region regionB)
	{
		m_pairs.push_back(ClonePair(regionA,regionB));
	}

	unsigned int getNumberOfClonePairs() const
	{
		return m_pairs.size();
	}

	void getFiles(CString & p_fileA, CString & p_fileB) const
	{
		p_fileA = m_fileA;
		p_fileB = m_fileB;
	}

	void getFilesWithoutPath(CString & p_fileA, CString & p_fileB) const
	{
		p_fileA = m_fileA.Mid(m_fileA.ReverseFind(_TCHAR('\\'))+1);
		p_fileB = m_fileB.Mid(m_fileB.ReverseFind(_TCHAR('\\'))+1);
	}

	CString & getFileA()
	{
		return m_fileA;
	}

	CString & getFileB()
	{
		return m_fileB;
	}

	bool getClonePair(int p_index, ClonePair & p_clonePair) const
	{
		if( p_index < (int) m_pairs.size())
		{
			p_clonePair = m_pairs[p_index];
			return true;
		}
		return false;
	}

	void getClonePairs(ClonePairsVector_t & p_pairs)
	{
		p_pairs = m_pairs;
	}

private:
    CString                m_fileA;
    CString                m_fileB;
    ClonePairsVector_t     m_pairs;

};

struct Reference
{
	int m_fileId;
	int m_line;
};

struct IndexToken
{
	CString m_token;
	std::vector<Reference> m_reference;
};


struct TokenReference
{
	CString m_type;
	std::vector<IndexToken> m_indexTokenVector;

	bool searchTokenRef(const CString p_searchStr,std::vector<IndexToken> & p_result) const
	{
		p_result.clear();
		std::vector<IndexToken>::const_iterator itr     = m_indexTokenVector.begin();
		std::vector<IndexToken>::const_iterator itr_end = m_indexTokenVector.end();
		for(;itr < itr_end; ++itr){
			if( itr->m_token.Find(p_searchStr) != -1){
				p_result.push_back(*itr);
			}
		}
		return (!p_result.empty()?true:false);
	}
};


class CloneFiles
{
public:

    typedef std::map<int,CString>  AsmFilesMap_t; 
    typedef	std::vector<CloneFile> CloneFileVector_t;
	typedef bool (CloneFiles::*pF)(CBFXmlNode &);
	typedef std::hash_map<CString, pF> FuncPtrMap_t;
	typedef std::vector<TokenReference> TokenRefsVector_t;

	CloneFiles();
	virtual ~CloneFiles();
	
	CloneFiles( const CloneFiles & rhs)
	{
	     m_windSize           = rhs.m_windSize;
         m_stride             = rhs.m_stride;
         m_maxKOperands       = rhs.m_maxKOperands;
         m_maxOverlapFraction = rhs.m_maxOverlapFraction;
         m_bFindExactClones   = rhs.m_bFindExactClones;
         m_FindInexactClones  = rhs.m_FindInexactClones;
		 m_normalLevel        = rhs.m_normalLevel;
         m_bNormalizeToken    = rhs.m_bNormalizeToken;
		 m_keyVectorsSize     = rhs.m_keyVectorsSize;
		 m_occurrenceThrs     = rhs.m_occurrenceThrs; 
         m_inexactLevel       = rhs.m_inexactLevel; 
		 m_asmFilesMap        = rhs.m_asmFilesMap; 
		 m_cloneFileVector    = rhs.m_cloneFileVector;
		 m_nTotalClonePairs   = rhs.m_nTotalClonePairs;		 
		 m_xmlFile            = rhs.m_xmlFile;
		 m_tokenRefsVector    = rhs.m_tokenRefsVector;
	}
	
	CloneFiles & operator=(const CloneFiles &rhs)
	{
		if( this!=&rhs)
		{
	        m_windSize           = rhs.m_windSize;
            m_stride             = rhs.m_stride;
            m_maxKOperands       = rhs.m_maxKOperands;
            m_maxOverlapFraction = rhs.m_maxOverlapFraction;
            m_bFindExactClones   = rhs.m_bFindExactClones;
            m_FindInexactClones  = rhs.m_FindInexactClones;
			m_normalLevel        = rhs.m_normalLevel;
            m_bNormalizeToken    = rhs.m_bNormalizeToken;
		    m_keyVectorsSize     = rhs.m_keyVectorsSize;
		    m_occurrenceThrs     = rhs.m_occurrenceThrs;  
            m_inexactLevel       = rhs.m_inexactLevel; 
 		    m_asmFilesMap        = rhs.m_asmFilesMap; 
		    m_cloneFileVector    = rhs.m_cloneFileVector;
		    m_nTotalClonePairs   = rhs.m_nTotalClonePairs;
			m_xmlFile            = rhs.m_xmlFile;
			m_tokenRefsVector    = rhs.m_tokenRefsVector;
		}
		return *this;
	}

	bool init( const CString & p_xmlFile, CString & p_errorInfo);

	bool extractParameter(CBFXmlNode&       p_pNode);
	bool extractAsmFiles(CBFXmlNode&        p_pNode);
	bool extractCloneFiles(CBFXmlNode&      p_pNode);
	bool extractTokenReferences(CBFXmlNode& p_pNode);

	void insertCloneFile(const CloneFile & p_cloneFile)
	{
		m_cloneFileVector.push_back(p_cloneFile);
	}

	unsigned int getWindSize() const
	{
		return m_windSize;
	}

	unsigned int getStride() const
	{
		return m_stride;
	}

	unsigned int getMaxKOperands() const
	{
		return m_maxKOperands;
	}

	double getMaxOverlapFraction() const
	{
		return m_maxOverlapFraction;
	}

	bool getFindExactClones() const
	{
		return m_bFindExactClones;
	}

	bool getFindInexactClones() const
	{
		return m_FindInexactClones;
	}

	int getTotalClonePairs() const
	{
		return m_nTotalClonePairs;
	}

	CString getNormalLevel() const
	{
		return m_normalLevel;
	}

    CString getInexactLevel() const 
    {
        return m_inexactLevel;
    }

	bool getNormalizetoken()
	{  
		return m_bNormalizeToken;
	}

	int getKeyVectorsSize()
	{
		return m_keyVectorsSize;
	}

	double getOccurrenceThrs()
	{
		return m_occurrenceThrs;
	}

	void getAsmFilesMap(AsmFilesMap_t & p_AsmFilesMap) const
	{
		p_AsmFilesMap = m_asmFilesMap;
	}

	void getCloneFileVector(CloneFileVector_t & p_cloneFileVector)
	{
		p_cloneFileVector = m_cloneFileVector;
	}

	void getTokeRefsVector(TokenRefsVector_t & p_tokenRefsVector)
	{
		p_tokenRefsVector = m_tokenRefsVector;
	}

	bool getCloneFile( int p_index, CloneFile & p_cloneFile)
	{
		if( p_index < 0 )
		{
			return false;
		}

		if(p_index < (int) m_cloneFileVector.size())
		{
			p_cloneFile = m_cloneFileVector[p_index];
			return true;
		}

		return false;
	}

	CString getXMLFile()
	{
		return m_xmlFile;
	}

	bool searchTokens(const CString & p_searchStr, TokenRefsVector_t & p_resultTokens) const
	{
		TokenRefsVector_t::const_iterator itr     = m_tokenRefsVector.begin();
		TokenRefsVector_t::const_iterator itr_end = m_tokenRefsVector.end();
		for(; itr!=itr_end; ++itr){
			TokenReference tokenRefs;
			if( itr->searchTokenRef(p_searchStr,tokenRefs.m_indexTokenVector)){	
				tokenRefs.m_type = itr->m_type;
				p_resultTokens.push_back(tokenRefs);
			}
		}
		return (!p_resultTokens.empty()?true:false);
	}

private:

    unsigned int         m_windSize;
    unsigned int         m_stride;
    unsigned int         m_maxKOperands;
    double               m_maxOverlapFraction;
    bool                 m_bFindExactClones;
    bool                 m_FindInexactClones;
	CString              m_normalLevel;
    bool                 m_bNormalizeToken;
	int                  m_borderInFeaturesize;
	double               m_firstSegmentPercentage;
	int                  m_keyVectorsSize;
	double               m_occurrenceThrs;   
    CString              m_inexactLevel; 
	CString              m_xmlFile;
  
	int                  m_nTotalClonePairs;

    AsmFilesMap_t        m_asmFilesMap; 
    CloneFileVector_t    m_cloneFileVector;
	FuncPtrMap_t         m_funcMap;
	TokenRefsVector_t    m_tokenRefsVector;

};

#endif

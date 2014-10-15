
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

#ifndef CLONEPAIR_H
#define CLONEPAIR_H

#include "stdafx.h"
#include <vector>
#include <map>

struct Region
{
    unsigned int m_start;
    unsigned int m_end;
};

struct ClonePair
{
    Region m_regionA;
    Region m_regionB;
};

class CloneFile
{
public:

    typedef std::vector<ClonePair> ClonePairsVector_t;

    CloneFile(){};
    virtual ~CloneFile(){};

private:
    CString                m_fileA;
    CString                m_fileB;
    ClonePairsVector_t     m_pairs;

};

class CloneFiles
{
public:

    typedef std::map<int,CString>  AsmFilesMap_t; 
    typedef	std::vector<CloneFile> CloneFileVector_t;

    CloneFiles(){};
    virtual ~CloneFiles(){};

	init( cont CString & p_xmlFile)();

private:

    unsigned int        m_windSize;
    unsigned int        m_stride;
    unsigned int        m_maxKOperands;
    double              m_maxDeltaDistance;
    double              m_maxOverlapFraction;
    bool                m_bFindExactClones;
    bool                m_FindInexactClones;
   
    AsmFilesMap_t       m_asmFilesMap; 
    CloneFileVector_t   m_cloneFileVector;

}

#endif


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
// Objective:   Clone manager is responsible to idenitfy the clones.
//
// History:     2012/10/15 - B. Fung (Created)
// 
//-----------------------------------------------------

#pragma once

#include "CSDatabaseMgr.h"

class CCSCloneMgr
{
public:
    CCSCloneMgr(CCSDatabaseMgr* pDBMgr);
    virtual ~CCSCloneMgr();

    inline const CCSClones* getAllClones() const { return &m_allClones; };
    inline bool sortByTarRawStart() { return m_allClones.sortByTarRawStart(); };

    bool transferClones(CCSClones& clones);
    bool unifyToLargestClones();

private:
    CCSClones m_allClones;
    CCSDatabaseMgr* m_pDBMgr;
};


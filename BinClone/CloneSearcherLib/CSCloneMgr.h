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


#include "stdafx.h"
#include "CSCloneMgr.h"


CCSCloneMgr::CCSCloneMgr(CCSDatabaseMgr* pDBMgr)
    : m_pDBMgr(pDBMgr)
{
    ASSERT(m_pDBMgr);
}


CCSCloneMgr::~CCSCloneMgr()
{
    m_allClones.cleanup();
}


//
// Transfer clones to this class
//
bool CCSCloneMgr::transferClones(CCSClones& clones)
{
    // transferred ownership of clones to m_allClones
    if (!m_allClones.appendClones(clones))
        return false;

    clones.RemoveAll();    
    return true;
}




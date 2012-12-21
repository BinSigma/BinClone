//-----------------------------------------------------
// Objective:   Manage the regions of each file.
//
// History:     2011/10/19 - B. Fung (Created)
// 
//-----------------------------------------------------

#pragma once

#include "CDAssemblyFileMgr.h"


class CCDOperandsMap : public CCDMapStringToInt
{
public:
    CCDOperandsMap() {};
    virtual ~CCDOperandsMap() {};
    int getOperandIdx(const CString& operand);
};


class CCDRegionMgr
{
public:
    CCDRegionMgr(CCDAssemblyFileMgr* pAssemblyFileMgr, 
                 int windowSize, 
                 int stride, 
                 int kThreshold,
                 TCDRegNormalizeLevel regNormLevel,
                 bool bNormalizeToken,
                 int keyVectorSize,
                 CDInexactMethod InexactMethod); 
    virtual ~CCDRegionMgr();
        
    inline int getNumGlobalFeatures() const { return m_globalFeatures.GetSize(); };
    inline LPCTSTR getGlobalFeature(int idx) const { return m_globalFeatures.GetAt(idx).GetString(); };
    inline CCDRegionsMap* getExactCloneClusters() { return &m_exactCloneClusters; };
    inline CCDRegionsMaps* getInexactHashTables() { return &m_inexactHashTables; };
    inline CCDRegionsMap* getInexactHashTable(int idx) { return m_inexactHashTables.GetAt(idx); };
    inline CCDAssemblyFileMgr* getAssemblyFileMgr()  { return m_pAssemblyFileMgr; };

    inline CDInexactMethod getInexactMethod() { return m_inexactMethod; };
    inline int getNumKeyVectors() const { return m_nKeyVectorsCopy; };
	inline double getDefinedHashTableRatio() const { return ((double)1 / m_keyVectorSize ); };

    inline int getKeyVectorsSize() const {return m_keyVectorSize;};

    bool createFunctions();
    bool createRegions();
    bool normalizeRegions();
    bool createExactHashTables();    
    bool regionsToVectors();
    int  addFeatureIfNew(LPCTSTR featureStr);
  //  bool createInexactHashTables(); 
    bool getAllRegions(CCDRegions& allRegions);  

private:
    bool hashRegions();    

    CCDAssemblyFileMgr* m_pAssemblyFileMgr;
    CCDRegionsMap       m_exactCloneClusters;       // store all exact clone clusters
    CCDRegionsMaps      m_inexactHashTables;        // store all inexact hash tables
    CStringArray        m_globalFeatures;           // maintain an array of features
      
    CCDOperandsMap m_opMems;        // map string to index
    CCDOperandsMap m_opRegs;        // map string to index, e.g, [eax,0][edx,1]
    CCDOperandsMap m_opVals;        // map string to index
   
    int m_nKeyVectorsCopy;
    int m_nKeyVectors;
    int m_windowSize;
    int m_stride;
    int m_kThreshold;
    TCDRegNormalizeLevel m_regNormLevel;
    bool m_bNormalizeToken;
    int m_keyVectorSize;   
    CDInexactMethod m_inexactMethod;
};


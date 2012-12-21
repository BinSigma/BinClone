//-----------------------------------------------------
// Objective:   Model an indexed token
//
// History:     2012/10/12 - B. Fung (Created)
// 
//-----------------------------------------------------

#pragma once

class CCSIndexedToken
{
public:
    CCSIndexedToken(LPCTSTR tokStr, int dbFileID, int rawLineNum);
    virtual ~CCSIndexedToken();

    inline const CString& getTokStr() const { return m_tokStr; };
    inline int getDBfileID() { return m_dbFileID; };
    inline int getRawLineNum() { return m_rawLineNum; };

private:
    CString m_tokStr;
    int m_dbFileID;
    int m_rawLineNum;
};

typedef CTypedPtrArray<CPtrArray, CCSIndexedToken*> CCSIndexedTokenArray;
class CCSIndexedTokens : public CCSIndexedTokenArray
{
public:
    CCSIndexedTokens() {};
    virtual ~CCSIndexedTokens() {};
    bool addIndexedToken(CCSIndexedToken* pIndexedToken);
    void cleanup();
};

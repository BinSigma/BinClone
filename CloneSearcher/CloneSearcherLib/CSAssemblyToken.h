//-----------------------------------------------------
// Objective:   Model a token in an assembly line.
//
// History:     2011/10/19 - B. Fung (Created)
// 
//-----------------------------------------------------

#pragma once

typedef enum {
    CSTOKEN_TYPE_MNEMONIC,    // mnemonic
    CSTOKEN_TYPE_OPMEM,       // operand: memory references, e.g., [0x805b634]
    CSTOKEN_TYPE_OPREG,       // operand: register references, e.g., eax
    CSTOKEN_TYPE_OPVAL,       // operand: constant values, e.g., 0x10
    CSTOKEN_TYPE_UNKNOWN
} CCSAssemblyTokenType;

class CCSAssemblyToken
{
public:
    CCSAssemblyToken(LPCTSTR tokenStr, CCSAssemblyTokenType tokenType);
    virtual ~CCSAssemblyToken();

    inline const CString& getTokenStr() const { return m_tokenStr; };
    void setTokenStr(const CString& tokenStr) { m_tokenStr = tokenStr; };
    inline CCSAssemblyTokenType getTokenType() { return m_tokenType; };

    CString getTokenTypeStr() const;

    static bool isMnemonic(LPCTSTR str);
    static bool isMemoryReference(const CString& str);    
    static bool isConstantValue(const CString& str);
    static bool isRegisterReference(const CString& str);    
    static LPCTSTR normalizeRegister(const CString& str, TCSRegNormalizeLevel regNormLevel);

private:    
    CString m_tokenStr;
    CCSAssemblyTokenType m_tokenType;
};

typedef CTypedPtrArray<CPtrArray, CCSAssemblyToken*> CCSAssemblyTokenArray;
class CCSAssemblyTokens : public CCSAssemblyTokenArray
{
public:
    CCSAssemblyTokens() {};
    virtual ~CCSAssemblyTokens() {};    
    void cleanup();
};

//-----------------------------------------------------
// Objective:   Model a token in an assembly line.
//
// History:     2011/10/19 - B. Fung (Created)
// 
//-----------------------------------------------------

#pragma once

typedef enum {
    CDTOKEN_TYPE_MNEMONIC,    // mnemonic
    CDTOKEN_TYPE_OPMEM,       // operand: memory references, e.g., [0x805b634]
    CDTOKEN_TYPE_OPREG,       // operand: register references, e.g., eax
    CDTOKEN_TYPE_OPVAL,       // operand: constant values, e.g., 0x10
    CDTOKEN_TYPE_UNKNOWN
} CCDAssemblyTokenType;

class CCDAssemblyToken
{
public:
    CCDAssemblyToken(LPCTSTR tokenStr, CCDAssemblyTokenType tokenType);
    virtual ~CCDAssemblyToken();

    inline const CString& getTokenStr() const { return m_tokenStr; };
    void setTokenStr(const CString& tokenStr) { m_tokenStr = tokenStr; };
    inline CCDAssemblyTokenType getTokenType() { return m_tokenType; };

    CString getTokenTypeStr() const;

    static bool isMnemonic(const CString& str);
    static bool isMemoryReference(const CString& str);    
    static bool isConstantValue(const CString& str);
    static bool isRegisterReference(const CString& str);    
    static LPCTSTR normalizeRegister(const CString& str, TCDRegNormalizeLevel regNormLevel);

private:    
    CString m_tokenStr;
    CCDAssemblyTokenType m_tokenType;
};

typedef CTypedPtrArray<CPtrArray, CCDAssemblyToken*> CCDAssemblyTokenArray;
class CCDAssemblyTokens : public CCDAssemblyTokenArray
{
public:
    CCDAssemblyTokens() {};
    virtual ~CCDAssemblyTokens() {};    
    void cleanup();
};

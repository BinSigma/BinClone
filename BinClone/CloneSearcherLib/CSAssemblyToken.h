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
	CSTOKEN_TYPE_CALLorJMP,  //CALLJMP
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
	static bool isCallorJMPReference(const CString& str); // call or jmp
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

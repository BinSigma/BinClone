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

#include "StdAfx.h"
#include <sstream> // for istringstream
#include "CSAssemblyToken.h"


//*******************
// CCSAssemblyToken *
//*******************


CCSAssemblyToken::CCSAssemblyToken(LPCTSTR tokenStr, CCSAssemblyTokenType tokenType)
    : m_tokenStr(tokenStr), m_tokenType(tokenType)
{
}


CCSAssemblyToken::~CCSAssemblyToken()
{
}


CString CCSAssemblyToken::getTokenTypeStr() const
{
    if (m_tokenType == CSTOKEN_TYPE_OPMEM)
        return CS_ASSEMBLYTOKEN_OPMEM;
    else if (m_tokenType == CSTOKEN_TYPE_OPREG)
        return CS_ASSEMBLYTOKEN_OPREG;
    else if (m_tokenType == CSTOKEN_TYPE_OPVAL)
        return CS_ASSEMBLYTOKEN_OPVAL;
    else
        return _T("");
}


//
// Is it a mnemonic?
//
bool CCSAssemblyToken::isMnemonic(LPCTSTR str)
{
    for (int i = 0; i < CS_NUM_MNEMONICS; ++i) {
        if (gMnemonics[i].CompareNoCase(str) == 0)
            return true;
    }
    return false;
}


//
// Is it a memory reference? Assume str is already trimmed.
//
bool CCSAssemblyToken::isMemoryReference(const CString& str)
{
    return str.Left(1).CompareNoCase(_T("[")) == 0 && str.Right(1).CompareNoCase(_T("]")) == 0;
}


//
// Is it a constant value? Assume str is not a memory reference. Assume str is not a register reference. Assume str is already trimmed.
// Currently assume _T("600E0023h") or _T("10A2h") or _T("123"). 
//
bool CCSAssemblyToken::isConstantValue(const CString& str)
{
    return CBFStrHelper::isHex((LPCTSTR) str) || str.Right(1).CompareNoCase(_T("h")) == 0;
}


//
// Is it a register reference? Assume str is not a memory reference. Assume str is already trimmed.
//
bool CCSAssemblyToken::isRegisterReference(const CString& str)
{
    // Refer Intel's instruction set reference for a complete list http://www.eecg.toronto.edu/~amza/www.mindsec.com/files/x86regs.html
    return str.CompareNoCase(_T("EAX")) == 0 || str.CompareNoCase(_T("EBX")) == 0 || str.CompareNoCase(_T("ECX")) == 0 || str.CompareNoCase(_T("EDX")) == 0 ||      // 32-bit general registers
           str.CompareNoCase(_T("AX")) == 0 || str.CompareNoCase(_T("BX")) == 0 || str.CompareNoCase(_T("CX")) == 0 || str.CompareNoCase(_T("DX")) == 0 ||          // 16-bit general registers
           str.CompareNoCase(_T("AH")) == 0 || str.CompareNoCase(_T("BH")) == 0 || str.CompareNoCase(_T("CH")) == 0 || str.CompareNoCase(_T("DH")) == 0 ||          // 8-bit general registers
           str.CompareNoCase(_T("AL")) == 0 || str.CompareNoCase(_T("BL")) == 0 || str.CompareNoCase(_T("CL")) == 0 || str.CompareNoCase(_T("DL")) == 0 ||          // 8-bit general registers
           str.CompareNoCase(_T("CS")) == 0 || str.CompareNoCase(_T("DS")) == 0 || str.CompareNoCase(_T("ES")) == 0 || str.CompareNoCase(_T("FS")) == 0 || str.CompareNoCase(_T("GS")) == 0 || str.CompareNoCase(_T("SS")) == 0 ||      // segment registers
           str.CompareNoCase(_T("EDI")) == 0 || str.CompareNoCase(_T("DI")) == 0 || str.CompareNoCase(_T("ESI")) == 0 || str.CompareNoCase(_T("SI")) == 0 ||        // index registers
           str.CompareNoCase(_T("EBP")) == 0 || str.CompareNoCase(_T("BP")) == 0 || str.CompareNoCase(_T("ESP")) == 0 || str.CompareNoCase(_T("SP")) == 0 || str.CompareNoCase(_T("EIP")) == 0 || str.CompareNoCase(_T("IP")) == 0 ||   // pointer registers
           str.CompareNoCase(_T("CF")) == 0 || str.CompareNoCase(_T("PF")) == 0 || str.CompareNoCase(_T("AF")) == 0 || str.CompareNoCase(_T("ZF")) == 0 ||          // EFLAGS registers
           str.CompareNoCase(_T("SF")) == 0 || str.CompareNoCase(_T("TF")) == 0 || str.CompareNoCase(_T("IF")) == 0 || str.CompareNoCase(_T("DF")) == 0 ||          // EFLAGS registers
           str.CompareNoCase(_T("OF")) == 0 || str.CompareNoCase(_T("IOPL")) == 0 || str.CompareNoCase(_T("NT")) == 0 || str.CompareNoCase(_T("RF")) == 0 ||        // EFLAGS registers
           str.CompareNoCase(_T("VM")) == 0 || str.CompareNoCase(_T("AC")) == 0 || str.CompareNoCase(_T("VIF")) == 0 || str.CompareNoCase(_T("VIP")) == 0 || str.CompareNoCase(_T("ID")) == 0 ||  // EFLAGS registers
		   str.CompareNoCase(_T("RAX")) == 0 || str.CompareNoCase(_T("RBX")) == 0 || str.CompareNoCase(_T("RCX")) == 0 || str.CompareNoCase(_T("RDX")) == 0 || str.CompareNoCase(_T("RSI")) == 0 || str.CompareNoCase(_T("RDI")) == 0 || str.CompareNoCase(_T("RBP")) == 0 || str.CompareNoCase(_T("RSP")) == 0 || 
		   str.CompareNoCase(_T("R8")) == 0 || str.CompareNoCase(_T("R9")) == 0 || str.CompareNoCase(_T("R10")) == 0 || str.CompareNoCase(_T("R11")) == 0 || str.CompareNoCase(_T("R12")) == 0 || str.CompareNoCase(_T("R13")) == 0 || str.CompareNoCase(_T("R14")) == 0 || str.CompareNoCase(_T("R15")) == 0 ||  // 64-bit registers
		   str.CompareNoCase(_T("R8D")) == 0 || str.CompareNoCase(_T("R9D")) == 0 || str.CompareNoCase(_T("R10D")) == 0 || str.CompareNoCase(_T("R11D")) == 0 || str.CompareNoCase(_T("R12D")) == 0 || str.CompareNoCase(_T("R13D")) == 0 || str.CompareNoCase(_T("R14D")) == 0 || str.CompareNoCase(_T("R15D")) == 0 ||
		   str.CompareNoCase(_T("R8W")) == 0 || str.CompareNoCase(_T("R9W")) == 0 || str.CompareNoCase(_T("R10W")) == 0 || str.CompareNoCase(_T("R11W")) == 0 || str.CompareNoCase(_T("R12W")) == 0 || str.CompareNoCase(_T("R13W")) == 0 || str.CompareNoCase(_T("R14W")) == 0 || str.CompareNoCase(_T("R15W")) == 0 ||
		   str.CompareNoCase(_T("R8B")) == 0 || str.CompareNoCase(_T("R9B")) == 0 || str.CompareNoCase(_T("R10B")) == 0 || str.CompareNoCase(_T("R11B")) == 0 || str.CompareNoCase(_T("R12B")) == 0 || str.CompareNoCase(_T("R13B")) == 0 || str.CompareNoCase(_T("R14B")) == 0 || str.CompareNoCase(_T("R15B")) == 0 ;

}


LPCTSTR CCSAssemblyToken::normalizeRegister(const CString& str, TCSRegNormalizeLevel regNormLevel)
{
    if (regNormLevel == CS_NORM_REG_ROOT) {
        if (isRegisterReference(str))
            return CS_ASSEMBLYTOKEN_OPREG;
        else {
            tcout << _T("Error: normalizeRegister(): unknown register str ") << str.GetString() << endl;
            ASSERT(false);
            return _T("");
        }
    }
    else if (regNormLevel == CS_NORM_REG_TYPE || regNormLevel == CS_NORM_REG_IDXPTR || regNormLevel == CS_NORM_REG_BITS) {
         if (str.CompareNoCase(_T("EAX")) == 0 || str.CompareNoCase(_T("EBX")) == 0 || str.CompareNoCase(_T("ECX")) == 0 || str.CompareNoCase(_T("EDX")) == 0) {
             if (regNormLevel == CS_NORM_REG_TYPE || regNormLevel == CS_NORM_REG_IDXPTR)
                 return CS_NORM_REG_GENERAL_STR;
             else if (regNormLevel == CS_NORM_REG_BITS)
                 return CS_NORM_REG_GENERAL32_STR;
             else {
                tcout << _T("Error: normalizeRegister(): unknown register str ") << str.GetString() << endl;
                ASSERT(false);
                return _T("");
             }
         }
         else if (str.CompareNoCase(_T("AX")) == 0 || str.CompareNoCase(_T("BX")) == 0 || str.CompareNoCase(_T("CX")) == 0 || str.CompareNoCase(_T("DX")) == 0) {
             if (regNormLevel == CS_NORM_REG_TYPE || regNormLevel == CS_NORM_REG_IDXPTR)
                 return CS_NORM_REG_GENERAL_STR;
             else if (regNormLevel == CS_NORM_REG_BITS)
                 return CS_NORM_REG_GENERAL16_STR;
             else {
                tcout << _T("Error: normalizeRegister(): unknown register str ") << str.GetString() << endl;
                ASSERT(false);
                return _T("");
             }
         }
         else if (str.CompareNoCase(_T("AH")) == 0 || str.CompareNoCase(_T("BH")) == 0 || str.CompareNoCase(_T("CH")) == 0 || str.CompareNoCase(_T("DH")) == 0 ||
             str.CompareNoCase(_T("AL")) == 0 || str.CompareNoCase(_T("BL")) == 0 || str.CompareNoCase(_T("CL")) == 0 || str.CompareNoCase(_T("DL")) == 0) {
             if (regNormLevel == CS_NORM_REG_TYPE || regNormLevel == CS_NORM_REG_IDXPTR)
                 return CS_NORM_REG_GENERAL_STR;
             else if (regNormLevel == CS_NORM_REG_BITS)
                 return CS_NORM_REG_GENERAL8_STR;
             else {
                tcout << _T("Error: normalizeRegister(): unknown register str ") << str.GetString() << endl;
                ASSERT(false);
                return _T("");
             }
         }
         else if (str.CompareNoCase(_T("CS")) == 0 || str.CompareNoCase(_T("DS")) == 0 || str.CompareNoCase(_T("ES")) == 0 || str.CompareNoCase(_T("FS")) == 0 || str.CompareNoCase(_T("GS")) == 0 || str.CompareNoCase(_T("SS")) == 0) {
            return CS_NORM_REG_SEGMENT_STR;
         }
         else if (str.CompareNoCase(_T("EDI")) == 0 || str.CompareNoCase(_T("DI")) == 0 || str.CompareNoCase(_T("ESI")) == 0 || str.CompareNoCase(_T("SI")) == 0) {
             if (regNormLevel == CS_NORM_REG_TYPE)
                 return CS_NORM_REG_IDXPTR_STR;
             else if (regNormLevel == CS_NORM_REG_IDXPTR || regNormLevel == CS_NORM_REG_BITS)
                 return CS_NORM_REG_IDX_STR;
             else {
                tcout << _T("Error: normalizeRegister(): unknown register str ") << str.GetString() << endl;
                ASSERT(false);
                return _T("");
             }
         }
         else if (str.CompareNoCase(_T("EBP")) == 0 || str.CompareNoCase(_T("BP")) == 0 || str.CompareNoCase(_T("ESP")) == 0 || str.CompareNoCase(_T("SP")) == 0 || str.CompareNoCase(_T("EIP")) == 0 || str.CompareNoCase(_T("IP")) == 0) {
             if (regNormLevel == CS_NORM_REG_TYPE)
                 return CS_NORM_REG_IDXPTR_STR;
             else if (regNormLevel == CS_NORM_REG_IDXPTR || regNormLevel == CS_NORM_REG_BITS)
                 return CS_NORM_REG_PTR_STR;
             else {
                tcout << _T("Error: normalizeRegister(): unknown register str ") << str.GetString() << endl;
                ASSERT(false);
                return _T("");
             }
         }
         else if (str.CompareNoCase(_T("CF")) == 0 || str.CompareNoCase(_T("PF")) == 0 || str.CompareNoCase(_T("AF")) == 0 || str.CompareNoCase(_T("ZF")) == 0 ||
             str.CompareNoCase(_T("SF")) == 0 || str.CompareNoCase(_T("TF")) == 0 || str.CompareNoCase(_T("IF")) == 0 || str.CompareNoCase(_T("DF")) == 0 ||
             str.CompareNoCase(_T("OF")) == 0 || str.CompareNoCase(_T("IOPL")) == 0 || str.CompareNoCase(_T("NT")) == 0 || str.CompareNoCase(_T("RF")) == 0 ||
             str.CompareNoCase(_T("VM")) == 0 || str.CompareNoCase(_T("AC")) == 0 || str.CompareNoCase(_T("VIF")) == 0 || str.CompareNoCase(_T("VIP")) == 0 || str.CompareNoCase(_T("ID")) == 0) {
            return CS_NORM_REG_EFLAGS_STR;
         }
         else {
            tcout << _T("Error: normalizeRegister(): unknown register str ") << str.GetString() << endl;
            ASSERT(false);
            return _T("");
         }
    }
    return _T("");
}

// the operand after these mnemonics are location
bool CCSAssemblyToken::isCallorJMPReference(const CString& str)
{
	return	str.CompareNoCase(_T("CALL")) == 0 || str.CompareNoCase(_T("JMP")) == 0 || str.CompareNoCase(_T("JE")) == 0 || str.CompareNoCase(_T("JZ")) == 0 
			|| str.CompareNoCase(_T("JCXZ")) == 0 || str.CompareNoCase(_T("JP")) == 0 || str.CompareNoCase(_T("JPE")) == 0 || str.CompareNoCase(_T("JNE")) == 0
			|| str.CompareNoCase(_T("JNZ")) == 0 || str.CompareNoCase(_T("JECXZ")) == 0 || str.CompareNoCase(_T("JNP")) == 0 || str.CompareNoCase(_T("JPO")) == 0;
			
}



//********************
// CCSAssemblyTokens *
//********************

void CCSAssemblyTokens::cleanup()
{
    for (int i = 0; i < GetSize(); ++i)
        delete GetAt(i);

    RemoveAll();
}
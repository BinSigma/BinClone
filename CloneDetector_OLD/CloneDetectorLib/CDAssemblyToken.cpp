#include "StdAfx.h"
#include <sstream> // for istringstream
#include "CDAssemblyToken.h"


//*******************
// CCDAssemblyToken *
//*******************


CCDAssemblyToken::CCDAssemblyToken(LPCTSTR tokenStr, CCDAssemblyTokenType tokenType)
    : m_tokenStr(tokenStr), m_tokenType(tokenType)
{
}


CCDAssemblyToken::~CCDAssemblyToken()
{
}


CString CCDAssemblyToken::getTokenTypeStr() const
{
    if (m_tokenType == CDTOKEN_TYPE_OPMEM)
        return CD_ASSEMBLYTOKEN_OPMEM;
    else if (m_tokenType == CDTOKEN_TYPE_OPREG)
        return CD_ASSEMBLYTOKEN_OPREG;
    else if (m_tokenType == CDTOKEN_TYPE_OPVAL)
        return CD_ASSEMBLYTOKEN_OPVAL;
    else
        return _T("");
}


//
// Is it a mnemonic?
//
bool CCDAssemblyToken::isMnemonic(const CString& str)
{
    return str.CompareNoCase(_T("MOV")) == 0 || str.CompareNoCase(_T("MOVZX")) == 0 || str.CompareNoCase(_T("MOVSX")) == 0 || str.CompareNoCase(_T("MOVSS")) == 0 ||
           str.CompareNoCase(_T("XCHG")) == 0 ||
           str.CompareNoCase(_T("STC")) == 0 || str.CompareNoCase(_T("CLC")) == 0 || str.CompareNoCase(_T("CMC")) == 0 || 
           str.CompareNoCase(_T("STD")) == 0 || str.CompareNoCase(_T("CLD")) == 0 || 
           str.CompareNoCase(_T("STI")) == 0 || str.CompareNoCase(_T("CLI")) == 0 ||
           str.CompareNoCase(_T("PUSH")) == 0 || str.CompareNoCase(_T("PUSHF")) == 0 || str.CompareNoCase(_T("PUSHA")) == 0 ||
           str.CompareNoCase(_T("POP")) == 0 || str.CompareNoCase(_T("POPF")) == 0 || str.CompareNoCase(_T("POPA")) == 0 ||
           str.CompareNoCase(_T("CBW")) == 0 || str.CompareNoCase(_T("CWD")) == 0 || str.CompareNoCase(_T("CWDE")) == 0 || str.CompareNoCase(_T("CDQ")) == 0 ||
           str.CompareNoCase(_T("IN")) == 0 || str.CompareNoCase(_T("OUT")) == 0 || 
           str.CompareNoCase(_T("ADD")) == 0 || str.CompareNoCase(_T("ADC")) == 0 ||
           str.CompareNoCase(_T("SUB")) == 0 || str.CompareNoCase(_T("SBB")) == 0 ||
           str.CompareNoCase(_T("DIV")) == 0 || str.CompareNoCase(_T("IDIV")) == 0 ||
           str.CompareNoCase(_T("MUL")) == 0 || str.CompareNoCase(_T("IMUL")) == 0 ||
           str.CompareNoCase(_T("INC")) == 0 || str.CompareNoCase(_T("DEC")) == 0 ||
           str.CompareNoCase(_T("CMP")) == 0 || 
           str.CompareNoCase(_T("SAL")) == 0 || str.CompareNoCase(_T("SAR")) == 0 ||
           str.CompareNoCase(_T("RCL")) == 0 || str.CompareNoCase(_T("RCR")) == 0 ||
           str.CompareNoCase(_T("ROL")) == 0 || str.CompareNoCase(_T("ROR")) == 0 ||
           str.CompareNoCase(_T("NEG")) == 0 || str.CompareNoCase(_T("NOT")) == 0 ||
           str.CompareNoCase(_T("AND")) == 0 || str.CompareNoCase(_T("OR")) == 0 || str.CompareNoCase(_T("XOR")) == 0 ||
           str.CompareNoCase(_T("SHL")) == 0 || str.CompareNoCase(_T("SHR")) == 0 ||
           str.CompareNoCase(_T("NOP")) == 0 || str.CompareNoCase(_T("LEA")) == 0 || str.CompareNoCase(_T("INT")) == 0 ||
           str.CompareNoCase(_T("CALL")) == 0 || str.CompareNoCase(_T("JMP")) == 0 || 
           str.CompareNoCase(_T("JE")) == 0 || str.CompareNoCase(_T("JZ")) == 0 || str.CompareNoCase(_T("JCXZ")) == 0 || str.CompareNoCase(_T("JP")) == 0 || str.CompareNoCase(_T("JPE")) == 0 ||
           str.CompareNoCase(_T("JNE")) == 0 || str.CompareNoCase(_T("JNZ")) == 0 || str.CompareNoCase(_T("JECXZ")) == 0 || str.CompareNoCase(_T("JNP")) == 0 || str.CompareNoCase(_T("JPO")) == 0 ||
           str.CompareNoCase(_T("RET")) == 0 || str.CompareNoCase(_T("RETN")) == 0 || str.CompareNoCase(_T("RETF")) == 0 ||
           str.CompareNoCase(_T("JA")) == 0 || str.CompareNoCase(_T("JAE")) == 0 ||
           str.CompareNoCase(_T("JB")) == 0 || str.CompareNoCase(_T("JBE")) == 0 ||
           str.CompareNoCase(_T("JNA")) == 0 || str.CompareNoCase(_T("JNAE")) == 0 ||
           str.CompareNoCase(_T("JNB")) == 0 || str.CompareNoCase(_T("JNBE")) == 0 ||
           str.CompareNoCase(_T("JC")) == 0 || str.CompareNoCase(_T("JNC")) == 0 ||
           str.CompareNoCase(_T("JG")) == 0 || str.CompareNoCase(_T("JGE")) == 0 ||
           str.CompareNoCase(_T("JL")) == 0 || str.CompareNoCase(_T("JLE")) == 0 ||
           str.CompareNoCase(_T("JNG")) == 0 || str.CompareNoCase(_T("JNGE")) == 0 ||
           str.CompareNoCase(_T("JNL")) == 0 || str.CompareNoCase(_T("JNLE")) == 0 ||
           str.CompareNoCase(_T("JO")) == 0 || str.CompareNoCase(_T("JNO")) == 0 ||
           str.CompareNoCase(_T("JS")) == 0 || str.CompareNoCase(_T("JNS")) == 0 ||
           str.CompareNoCase(_T("TEST")) == 0 || str.CompareNoCase(_T("FLD")) == 0 || 
           str.CompareNoCase(_T("FSTP")) == 0;
}


//
// Is it a memory reference? Assume str is already trimmed.
//
bool CCDAssemblyToken::isMemoryReference(const CString& str)
{
    // TODO: improve memory reference recognition.
    return str.Left(1).CompareNoCase(_T("[")) == 0 && str.Right(1).CompareNoCase(_T("]")) == 0;
}


//
// Is it a constant value? Assume str is not a memory reference. Assume str is not a register reference. Assume str is already trimmed.
// Currently assume _T("600E0023h") or _T("10A2h") or _T("123"). 
//
bool CCDAssemblyToken::isConstantValue(const CString& str)
{
    return CBFStrHelper::isHex((LPCTSTR) str) || str.Right(1).CompareNoCase(_T("h")) == 0;
}


//
// Is it a register reference? Assume str is not a memory reference. Assume str is already trimmed.
//
bool CCDAssemblyToken::isRegisterReference(const CString& str)
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
           str.CompareNoCase(_T("VM")) == 0 || str.CompareNoCase(_T("AC")) == 0 || str.CompareNoCase(_T("VIF")) == 0 || str.CompareNoCase(_T("VIP")) == 0 || str.CompareNoCase(_T("ID")) == 0;  // EFLAGS registers
}


LPCTSTR CCDAssemblyToken::normalizeRegister(const CString& str, TCDRegNormalizeLevel regNormLevel)
{
    if (regNormLevel == CD_NORM_REG_ROOT) {
        if (isRegisterReference(str))
            return CD_ASSEMBLYTOKEN_OPREG;
        else {
            tcout << _T("Error: normalizeRegister(): unknown register str ") << str.GetString() << endl;
            ASSERT(false);
            return _T("");
        }
    }
    else if (regNormLevel == CD_NORM_REG_TYPE || regNormLevel == CD_NORM_REG_IDXPTR || regNormLevel == CD_NORM_REG_BITS) {
         if (str.CompareNoCase(_T("EAX")) == 0 || str.CompareNoCase(_T("EBX")) == 0 || str.CompareNoCase(_T("ECX")) == 0 || str.CompareNoCase(_T("EDX")) == 0) {
             if (regNormLevel == CD_NORM_REG_TYPE || regNormLevel == CD_NORM_REG_IDXPTR)
                 return CD_NORM_REG_GENERAL_STR;
             else if (regNormLevel == CD_NORM_REG_BITS)
                 return CD_NORM_REG_GENERAL32_STR;
             else {
                tcout << _T("Error: normalizeRegister(): unknown register str ") << str.GetString() << endl;
                ASSERT(false);
                return _T("");
             }
         }
         else if (str.CompareNoCase(_T("AX")) == 0 || str.CompareNoCase(_T("BX")) == 0 || str.CompareNoCase(_T("CX")) == 0 || str.CompareNoCase(_T("DX")) == 0) {
             if (regNormLevel == CD_NORM_REG_TYPE || regNormLevel == CD_NORM_REG_IDXPTR)
                 return CD_NORM_REG_GENERAL_STR;
             else if (regNormLevel == CD_NORM_REG_BITS)
                 return CD_NORM_REG_GENERAL16_STR;
             else {
                tcout << _T("Error: normalizeRegister(): unknown register str ") << str.GetString() << endl;
                ASSERT(false);
                return _T("");
             }
         }
         else if (str.CompareNoCase(_T("AH")) == 0 || str.CompareNoCase(_T("BH")) == 0 || str.CompareNoCase(_T("CH")) == 0 || str.CompareNoCase(_T("DH")) == 0 ||
             str.CompareNoCase(_T("AL")) == 0 || str.CompareNoCase(_T("BL")) == 0 || str.CompareNoCase(_T("CL")) == 0 || str.CompareNoCase(_T("DL")) == 0) {
             if (regNormLevel == CD_NORM_REG_TYPE || regNormLevel == CD_NORM_REG_IDXPTR)
                 return CD_NORM_REG_GENERAL_STR;
             else if (regNormLevel == CD_NORM_REG_BITS)
                 return CD_NORM_REG_GENERAL8_STR;
             else {
                tcout << _T("Error: normalizeRegister(): unknown register str ") << str.GetString() << endl;
                ASSERT(false);
                return _T("");
             }
         }
         else if (str.CompareNoCase(_T("CS")) == 0 || str.CompareNoCase(_T("DS")) == 0 || str.CompareNoCase(_T("ES")) == 0 || str.CompareNoCase(_T("FS")) == 0 || str.CompareNoCase(_T("GS")) == 0 || str.CompareNoCase(_T("SS")) == 0) {
            return CD_NORM_REG_SEGMENT_STR;
         }
         else if (str.CompareNoCase(_T("EDI")) == 0 || str.CompareNoCase(_T("DI")) == 0 || str.CompareNoCase(_T("ESI")) == 0 || str.CompareNoCase(_T("SI")) == 0) {
             if (regNormLevel == CD_NORM_REG_TYPE)
                 return CD_NORM_REG_IDXPTR_STR;
             else if (regNormLevel == CD_NORM_REG_IDXPTR || regNormLevel == CD_NORM_REG_BITS)
                 return CD_NORM_REG_IDX_STR;
             else {
                tcout << _T("Error: normalizeRegister(): unknown register str ") << str.GetString() << endl;
                ASSERT(false);
                return _T("");
             }
         }
         else if (str.CompareNoCase(_T("EBP")) == 0 || str.CompareNoCase(_T("BP")) == 0 || str.CompareNoCase(_T("ESP")) == 0 || str.CompareNoCase(_T("SP")) == 0 || str.CompareNoCase(_T("EIP")) == 0 || str.CompareNoCase(_T("IP")) == 0) {
             if (regNormLevel == CD_NORM_REG_TYPE)
                 return CD_NORM_REG_IDXPTR_STR;
             else if (regNormLevel == CD_NORM_REG_IDXPTR || regNormLevel == CD_NORM_REG_BITS)
                 return CD_NORM_REG_PTR_STR;
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
            return CD_NORM_REG_EFLAGS_STR;
         }
         else {
            tcout << _T("Error: normalizeRegister(): unknown register str ") << str.GetString() << endl;
            ASSERT(false);
            return _T("");
         }
    }
    return _T("");
}

//********************
// CCDAssemblyTokens *
//********************

void CCDAssemblyTokens::cleanup()
{
    for (int i = 0; i < GetSize(); ++i)
        delete GetAt(i);

    RemoveAll();
}
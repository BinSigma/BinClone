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

#pragma once

// Modes
#define _CS_SHOW_PROGRESS

// Common types
typedef CArray<int, int>      CCSIntArray;
typedef CArray<bool, bool>    CCSBoolArray;
typedef CBFMultiDimArray<int> CCSMDIntArray;
//typedef CArray<double, double>  CCSDoubleArray;
//typedef CArray<float, float>  CCSFloatArray;
//typedef CArray<COleDateTime, COleDateTime>  CCSTimeArray;
typedef CMap<CString, LPCTSTR, int, int> CCSMapStringToInt;

enum TCSRegNormalizeLevel {
    CS_NORM_REG_ROOT,       // normlize all registers to REG
    CS_NORM_REG_TYPE,       // normalize registers to one of the four types: general, segment, indexes, and EFLAGS
    CS_NORM_REG_IDXPTR,     // normalize indexes and pointers registers to one of the two types: index, and pointer, e.g., EDI -> REGIDX, EBP -> REGPTR
    CS_NORM_REG_BITS        // normalize general registers to 32-bit, 16-bit, 8-bit, e.g., EAX -> REG32, AX -> AX16, AH -> REG8
};

#define CS_NUM_MNEMONICS                    91
static CString gMnemonics [] = {
    _T("MOV"),
    _T("MOVZX"),
    _T("MOVSX"),
    _T("MOVSS"),
    _T("XCHG"),
    _T("STC"),
    _T("CLC"),
    _T("CMC"),
    _T("STD"),
    _T("CLD"),
    _T("STI"),
    _T("CLI"),
    _T("PUSH"),
    _T("PUSHF"),
    _T("PUSHA"),
    _T("POP"),
    _T("POPF"),
    _T("POPA"),
    _T("CBW"),
    _T("CWD"),
    _T("CWDE"),
    _T("CSQ"),
    _T("IN"),
    _T("OUT"),
    _T("ADD"),
    _T("ADC"),
    _T("SUB"),
    _T("SBB"),
    _T("DIV"),
    _T("IDIV"),
    _T("MUL"),
    _T("IMUL"),
    _T("INC"),
    _T("DEC"),
    _T("CMP"),
    _T("SAL"),
    _T("SAR"),
    _T("RCL"),
    _T("RCR"),
    _T("ROL"),
    _T("ROR"),
    _T("NEG"),
    _T("NOT"),
    _T("AND"),
    _T("OR"),
    _T("XOR"),
    _T("SHL"),
    _T("SHR"),
    _T("NOP"),
    _T("LEA"),
    _T("INT"),
    _T("CALL"),
    _T("JMP"), 
    _T("JE"),
    _T("JZ"),
    _T("JCXZ"),
    _T("JP"),
    _T("JPE"),
    _T("JNE"),
    _T("JNZ"),
    _T("JECXZ"),
    _T("JNP"),
    _T("JPO"),
    _T("RET"),
    _T("RETN"),
    _T("RETF"),
    _T("JA"),
    _T("JAE"),
    _T("JB"),
    _T("JBE"),
    _T("JNA"),
    _T("JNAE"),
    _T("JNB"),
    _T("JNBE"),
    _T("JC"),
    _T("JNC"),
    _T("JG"),
    _T("JGE"),
    _T("JL"),
    _T("JLE"),
    _T("JNG"),
    _T("JNGE"),
    _T("JNL"),
    _T("JNLE"),
    _T("JO"),
    _T("JNO"),
    _T("JS"),
    _T("JNS"),
    _T("TEST"),
    _T("FLD"),
    _T("FSTP")
};

#define CS_ASSEMBLYTOKEN_OPMEM              _T("MEM")
#define CS_ASSEMBLYTOKEN_OPREG              _T("REG")
#define CS_ASSEMBLYTOKEN_OPVAL              _T("VAL")
#define CS_ASSEMBLYTOKEN_OPLOC				_T("LOC")

#define CS_NUM_OPTYPES                      3
static CString gOpTypes [] = {
    CS_ASSEMBLYTOKEN_OPMEM,
    CS_ASSEMBLYTOKEN_OPREG,
    CS_ASSEMBLYTOKEN_OPVAL			
};

#define CS_NUM_RUNNING_CHARS                4
static TCHAR runningChars [] = {'-', '\\', '|', '/'};

// Constants
#define CS_NUM_DECIMALS_INFILE              8

#define CS_ASSEMBLYFILE_EXT1                _T("txt")
#define CS_ASSEMBLYFILE_EXT2                _T("asm")
#define CS_ASSEMBLYFILE_EXT3                _T("tmp")
#define CS_ASSEMBLYFILE_DELIMETER           TCHAR(' ')
#define CS_ASSEMBLYFILE_COMMENT             _T(";")
#define CS_ASSEMBLYFILE_OFFSET              _T("OFFSET")
#define CS_ASSMEBLYFILE_TMP_FILENAME        _T("CCS");

#define CS_TRUE_STR                         _T("TRUE")
#define CS_FALSE_STR                        _T("FALSE")

#define CS_OPERAND_IDX_DELIMETER            _T("#")

#define CS_PARAM_NORM_REG_ROOT_STR           _T("REG")
#define CS_PARAM_NORM_REG_TYPE_STR           _T("TYPE")
#define CS_PARAM_NORM_REG_IDXPTR_STR         _T("IDXPTR")
#define CS_PARAM_NORM_REG_BITS_STR           _T("BITS")

#define CS_NORM_REG_GENERAL_STR              _T("REGGen")
#define CS_NORM_REG_GENERAL32_STR            _T("REGGen32")
#define CS_NORM_REG_GENERAL16_STR            _T("REGGen16")
#define CS_NORM_REG_GENERAL8_STR             _T("REGGen8")
#define CS_NORM_REG_SEGMENT_STR              _T("REGSeg")
#define CS_NORM_REG_IDXPTR_STR               _T("REGIdxPtr")
#define CS_NORM_REG_IDX_STR                  _T("REGIdx")
#define CS_NORM_REG_PTR_STR                  _T("REGPtr")
#define CS_NORM_REG_EFLAGS_STR               _T("REGEFlags")



//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
static void swapNumbers(int& a, int& b)
{
    int temp = a;
    b = a;
    a = temp;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
static void orderNumbers(int& a, int& b, int& c)
{
    if (a > b)
        swapNumbers(a, b);
    if (a > c)
        swapNumbers(a, c);
    if (b > c)
        swapNumbers(b, c);
}

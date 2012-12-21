#pragma once

// Modes
#define _CD_SHOW_PROGRESS

// Common types
typedef CArray<int, int>      CCDIntArray;
typedef CArray<bool, bool>    CCDBoolArray;
typedef CBFMultiDimArray<int> CCDMDIntArray;
//typedef CArray<double, double>  CCDDoubleArray;
//typedef CArray<float, float>  CCDFloatArray;
//typedef CArray<COleDateTime, COleDateTime>  CCDTimeArray;
typedef CMap<CString, LPCTSTR, int, int> CCDMapStringToInt;

enum TCDRegNormalizeLevel {
    CD_NORM_REG_ROOT,       // normlize all registers to REG
    CD_NORM_REG_TYPE,       // normalize registers to one of the four types: general, segment, indexes, and EFLAGS
    CD_NORM_REG_IDXPTR,     // normalize indexes and pointers registers to one of the two types: index, and pointer, e.g., EDI -> REGIDX, EBP -> REGPTR
    CD_NORM_REG_BITS        // normalize general registers to 32-bit, 16-bit, 8-bit, e.g., EAX -> REG32, AX -> AX16, AH -> REG8
};

enum CDInexactMethod {
    CD_INEXACT_METHOD_RANDOM,
    CD_INEXACT_METHOD_SLIDING,
    CD_INEXACT_METHOD_TWOCOMBINATION
};


static TCHAR runningChars [] = {'-', '\\', '|', '/'};

// Constants
#define CD_NUM_RUNNING_CHARS                4
#define CD_NUM_DECIMALS_INFILE              8

#define CD_ASSEMBLYFILE_EXT1                _T("txt")
#define CD_ASSEMBLYFILE_EXT2                _T("asm")
#define CD_ASSEMBLYFILE_EXT3                _T("tmp")
#define CD_ASSEMBLYFILE_DELIMETER           TCHAR(' ')
#define CD_ASSEMBLYFILE_COMMENT             _T(";")
#define CD_ASSEMBLYFILE_OFFSET              _T("OFFSET")
#define CD_ASSMEBLYFILE_TMP_FILENAME        _T("CCD");

#define CD_TRUE_STR                         _T("TRUE")
#define CD_FALSE_STR                        _T("FALSE")

#define CD_OPERAND_IDX_DELIMETER            _T("#")

#define CD_PARAM_NORM_REG_ROOT_STR           _T("REG")
#define CD_PARAM_NORM_REG_TYPE_STR           _T("TYPE")
#define CD_PARAM_NORM_REG_IDXPTR_STR         _T("IDXPTR")
#define CD_PARAM_NORM_REG_BITS_STR           _T("BITS")

#define CD_NORM_REG_GENERAL_STR              _T("REGGen")
#define CD_NORM_REG_GENERAL32_STR            _T("REGGen32")
#define CD_NORM_REG_GENERAL16_STR            _T("REGGen16")
#define CD_NORM_REG_GENERAL8_STR             _T("REGGen8")
#define CD_NORM_REG_SEGMENT_STR              _T("REGSeg")
#define CD_NORM_REG_IDXPTR_STR               _T("REGIdxPtr")
#define CD_NORM_REG_IDX_STR                  _T("REGIdx")
#define CD_NORM_REG_PTR_STR                  _T("REGPtr")
#define CD_NORM_REG_EFLAGS_STR               _T("REGEFlags")

#define CD_ASSEMBLYTOKEN_OPMEM              _T("MEM")
#define CD_ASSEMBLYTOKEN_OPREG              _T("REG")
#define CD_ASSEMBLYTOKEN_OPVAL              _T("VAL")


#define CD_PARAM_CD_INEXACT_METHOD_RANDOM_STR           _T("METHOD1") 
#define CD_PARAM_CD_INEXACT_METHOD_SLIDING_STR          _T("METHOD2") 
#define CD_PARAM_CD_INEXACT_METHOD_TWOCOMBINATION_STR   _T("METHOD3") 

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

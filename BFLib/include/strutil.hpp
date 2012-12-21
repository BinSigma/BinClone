// File:    StrUtil.h
//

#if !defined (STRUTIL_HPP)
#define STRUTIL_HPP

#if !defined (_SIZE_T)
    #define _SIZE_T
    typedef unsigned size_t;
#endif //(_SIZE_T)

const Int16u MaxIntLength = 7;

void lmemmove (void far       * destination, 
               const void far * source,
               size_t           nChars);

void lmemcpy (void far       * destination, 
              const void far * source,
              size_t           nChars);

char  * IntToStr (Int16s value, Int16s width = 1);

Int16s StrToInt (const char * string);

char  * FloatToStr (double value, Int16s nDecimals = 0);

double StrToFloat (const char * string);

const char * AnsiStrChr (const char * s, char c);

inline char * AnsiStrChr (char * s, char c)
{
    return ((char *)AnsiStrChr ((const char *)s, c));
}

const char * AnsiStrNChr (const char * s, char c, size_t nChars);

inline char * AnsiStrNChr (char * s, char c, size_t nChars)
{
    return ((char *)AnsiStrNChr ((const char *)s, c, nChars));
}

const char * AnsiStrBrk (const char * s, char c);

inline char * AnsiStrBrk (char * s, char c)
{
    return ((char *)AnsiStrBrk ((const char *)s, c));
}

const char * AnsiStrNBrk (const char * s, char c, size_t nChars);

inline char * AnsiStrNBrk (char * s, char c, size_t nChars)
{
    return ((char *)AnsiStrNBrk ((const char *)s, c, nChars));
}

Int16u TrimRight (char * string);

inline Boolean IsAnsiAlpha (char c) {return (IsCharAlpha (c));}

inline Boolean IsAnsiDigit (char c) {return (IsCharAlphaNumeric (c) && !IsCharAlpha (c));}

inline Boolean IsAnsiAlphaDigit (char c) {return (IsCharAlphaNumeric (c));}

Boolean IsAnsiPrintable (char c);

inline Int16u StrLen (const char *string) {return (Int16u) (string == 0 ? 0 : strlen (string));}

char * StrCpy  (char * str1, const char * str2);

// Note: StrCpyN does _not_ behave like strncpy;
//       rather, it behaves like the Windows API call lstrcpyn
//       Thus, it copies at most nChars - 1 characters, then null-terminates.
char * StrCpyN (char * str1, const char * str2, size_t nChars);

char * StrCat  (char * str1, const char * str2);

// The comparison functions return -1 if the first argument
// is less than the second, 0 if they are equal, and 1 otherwise
Int16s StrCmp  (const char * str1, const char * str2);

Int16s StrCmpI (const char * str1, const char * str2);

char * StrDup (const char * string);

Int16u GetWordLength (const char far * string);

Int16u GetWhiteSpaceLength (const char far * string);

Int16u GetNewLineLength (const char far * string);

char * MakeCanonicalFileName (const char * fileName);

#if defined (_MSC_VER) && !defined (DBCS)
    void 
#else
    Int16u 
#endif //defined (_MSC_VER) && !defined (DBCS)
            FNSplit (const char * pathP, 
                     char       * driveP,
                     char       * dirP, 
                     char       * nameP, 
                     char       * extP);

#if defined (WIN32)
    wchar_t * MakeUnicode (const char * ansi);
    
    char    * MakeAnsi    (const wchar_t * unicode);
#endif //(WIN32)

#if defined (DBCS)
    inline Boolean IsDBCSEnabled ()
    {
        return GetSystemMetrics (SM_DBCSENABLED) != 0;
    }
#endif //(DBCS)

#if defined (KEYWORD_LIKE)
    Boolean MatchStringPattern (const char * string, const char * pattern, Boolean CaseSensative  = TRUE);
#endif //(KEYWORD_LIKE)

#if defined (KEYWORD_STARTWITH)
    Boolean StringStartWith (const char * string, const char * startWithString, Boolean CaseSensative  = TRUE);
#endif //(KEYWORD_STARTWITH)

BOOL SafeStrInsert (char * & str1, const char * str2, Int16u pos, Int16u &physicalLen);

BOOL SafeStrCat (char * & str1, const char * str2, Int16u &physicalLen);

Boolean TrimMiddle(char* string);
BOOL TrimLeft(char* &string);
BOOL TrimMiddleLeaveOne(char* string);
#endif //!defined (STRUTIL_HPP)


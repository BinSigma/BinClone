//---------------------------------------------------------------------------
// File:
//      StrUtil.h, StrUtil.cpp
//
// Objective:
//      Useful string utilities functions
//
// History:
//      Mar. 12, 2002   Created by Benjamin Fung
//
//---------------------------------------------------------------------------

#pragma hdrstop

#include <windows.h>
#include "types.hpp"

#ifdef _MSC_VER
  #include <tchar.h>
#endif

#include <ctype.h>
#include <limits.h>

#if !defined (STRUTIL_HPP)
    #include "strutil.hpp"
#endif

#if defined (DBCS)
    #define LONG_FILE_NAME
//  #define DEBUG_FNSPLIT
#endif //(DBCS)

#if !defined (_MSC_VER)
    #include <dir.h>
#else
    #define WILDCARDS 0x01
    #define EXTENSION 0x02
    #define FILENAME  0x04
    #define DIRECTORY 0x08
    #define DRIVE     0x10
    #define MAXDRIVE  _MAX_DRIVE
    #define MAXDIR    _MAX_DIR
    #define MAXFILE   _MAX_FNAME
    #define MAXEXT    _MAX_EXT
    #define MAXPATH   _MAX_DRIVE + _MAX_DIR + _MAX_FNAME + _MAX_EXT
#endif //!defined (_MSC_VER)

#if !defined (PLAT_WINDU)
    #include <stdlib.h>
#else
    #include "windu_stdlib.h"
    #include "windu_string.h"
    #define memicmp _memicmp
#endif //!defined (PLAT_WINDU)

#include <stdio.h>

#if !defined (OIW_DLL)
    #if !defined (ROLL_OUR_OWN)
        #if defined (_MSC_VER)
            #include <string.h>
        #elif defined(PLAT_WINDU)
            #include "windu_string.h"
        #else
            #include "mem.h"	// Quote non system include files.
        #endif //(_MSC_VER)
    #endif //!defined (ROLL_OUR_OWN)
#endif //(OIW_DLL)

#if defined (DBCS)
    #if defined (COMPLEX_FNSPLIT)
        #include <string.h>

        #if defined (MFC)  // 03-07-96 ccw, Left this one, don't know what it did
            MFC_MEM_DEBUG ()
            #include "memdebug.h"
        #endif //(MFC)

        struct TokenIndexList
        {
            Int16u           index;
            TokenIndexList * next;
        };
    #endif //(COMPLEX_FNSPLIT)
#endif //(DBCS)

// Make up for the idiosyncrasy of Windows...
// Note that version 3.10 is returned as 0x030a
inline WORD GetWindowsVersion ()
{
    WORD version = LOWORD(GetVersion());   // 03-07-96 ccw, Added LOWORD call.  
                                           // LOWORD is Windows version, HIWORD is DOS version.
    return (LOBYTE (version) << 8) | HIBYTE (version);
}

void lmemcpy (void far       * destination, 
              const void far * source,
              size_t           nChars)
{
#if defined (ROLL_OUR_OWN)
    lmemmove (destination, source, nChars);
#else
    memcpy (destination, source, nChars);
#endif //(ROLL_OUR_OWN)
}

void lmemmove (void far       * destination, 
               const void far * source,
               size_t           nChars)
{
#if defined (ROLL_OUR_OWN)
    const char far * s = (const char far *) source;
    char far * d = (char far *) destination;

    if (s >= d)
    {
        while (nChars > 0)
        {
            *d++ = *s++;
            --nChars;
        }
    } else
    {
        s += nChars;
        d += nChars;

        while (nChars > 0)
        {
            *--d = *--s;
            --nChars;
        }
    }
#else
    memmove (destination, source, nChars);
#endif //(ROLL_OUR_OWN)
}

#if defined (LONG_FILE_NAME)
inline Boolean StartsWithAnsi (const char * string)
{
    return ((AnsiNext (string)) == (string + 1));
}

inline Boolean StartsWithAnsiChar (const char * string, char c)
{
    return StartsWithAnsi (string) && *string == c;
}
#endif //(LONG_FILE_NAME)

char * IntToStr (Int16s value,
                 Int16s width)
{
    char format [10];

    wsprintf (format, "%%%dd", width);

    char * result = new char [MaxIntLength + 1];

    if (result != 0)
        wsprintf (result, format, value);

    return (result);
}

#ifdef _MSC_VER
Int16s StrToInt (const char * string)
{
    return (_ttoi (string));
}

char * FloatToStr (double value,
                   Int16s nDecimals)
{
    char numericString [100];

    sprintf_s (numericString, "%.*f", nDecimals, value);

    char * string = new char [lstrlen (numericString) + 1];

    if (string != 0)
        lstrcpy (string, numericString);

    return string;
}
#endif//_MSC_VER
double StrToFloat (const char * string)
{
    return atof (string);
}

const char * AnsiStrChr (const char * s, char c)
{
    while (*s != '\0')
#if defined (LONG_FILE_NAME)
        if (StartsWithAnsiChar (s, c))
#else
        if (*s == c)
#endif //(LONG_FILE_NAME)
            return (s);
        else
            s = (const char *)AnsiNext ((LPSTR)s);

    return (0);
}

const char * AnsiStrNChr (const char * s, char c, size_t nChars)
{
#if defined (LONG_FILE_NAME)
    while (nChars > 0 && *s != '\0')
    {
        if (StartsWithAnsiChar (s, c))
            return (s);
        else
        {
            const char * ptr = (const char *)AnsiNext ((LPSTR) s);
            nChars -= ptr - s;  // It may be fussy; however, logically this is
                                // necessarily.
            s = ptr;
        }
    }
#else
    while (nChars-- > 0 && *s != '\0')
    {
        if (*s == c)
            return (s);
        else
            s = (const char *)AnsiNext ((LPSTR)s);
    }
#endif //(LONG_FILE_NAME)
    return (0);
}

const char * AnsiStrBrk (const char * s, char c)
{
    while (*s != '\0')
#if defined (LONG_FILE_NAME)
        if (!StartsWithAnsiChar (s, c))
#else
        if (*s != c)
#endif //(LONG_FILE_NAME)
            return (s);
        else
            s = (const char *)AnsiNext ((LPSTR)s);

    return (0);
}

const char * AnsiStrNBrk (const char * s, char c, size_t nChars)
{
#if defined (LONG_FILE_NAME)
    while (nChars > 0 && *s != '\0')
    {
        if (!StartsWithAnsiChar (s, c))
            return (s);
        else
        {
            const char * ptr = (const char *)AnsiNext ((LPSTR) s);
            nChars -= ptr - s;  // It may be fussy; however, logically this is
                                          // necessarily.
            s = ptr;
        }
    }
#else
    while (nChars-- > 0 && *s != '\0')
    {
        if (*s != c)
            return (s);
        else
            s = (const char *)AnsiNext ((LPSTR)s);
    }
#endif //(LONG_FILE_NAME)

    return (0);
}

Int16u TrimRight (char * string)
{
    Int16u stringLength;
    
    for (stringLength = lstrlen (string), string += stringLength;
         stringLength > 0 && string [-1] == ' ';
         --stringLength, --string)
        ;

    *string = '\0';

    return stringLength;
}

Boolean TrimMiddle(char* string)
{
	int length = StrLen(string);
	char *stringCopy = new char[length +1];
	if(!stringCopy)
		return FALSE;
	int  j = 0;
	for(int i = 0; i < length; i++)
		if(string[i] != ' ')
			stringCopy[j++] = string[i];
	stringCopy[j] = '\0';	
	StrCpy(string, stringCopy);
	delete stringCopy;
	return TRUE;
}

BOOL TrimLeft(char* &string)
{
//	while(lstrlen(string) > 0 && *string++ == ' ')
//		;
//	string--;
	for(; lstrlen(string) > 0 && *string == ' '; string++)
		;
	return TRUE;
}

BOOL TrimMiddleLeaveOne(char* string)
{
	int length = StrLen(string);
	char *stringCopy = new char[length +1];
	if(!stringCopy)
		return FALSE;
	int  j = 0;
	for(int i = 0; i < length; i++)
	{
		if(string[i] != ' ')
			stringCopy[j++] = string[i];
		if(string[i] == ' ' && string[i+1] != ' ')
			stringCopy[j++] = string[i];
	}
	stringCopy[j] = '\0';	
	StrCpy(string, stringCopy);
	delete stringCopy;
	return TRUE;
}

#ifdef _MSC_VER
Boolean IsAnsiPrintable (char c)
{
    // assume any character above 0x80 is printable
    return (_istprint (c) || (unsigned char)c >= 0x80);
}
#endif
// The following functions handle an argument of 0
// by treating it as if it were ""

char * StrCpy (char * str1, const char * str2)
{
    if (str1 == 0)
        return 0;

    if (str2 == 0)
        str2 = "";

    return (lstrcpy (str1, (LPSTR)str2));
}

char * StrCpyN (char * str1, const char * str2, size_t nChars)
{
    if (str1 == 0 || nChars <= 0)
        return 0;

    if (str2 == 0)
        str2 = "";

#if !defined (WIN32) && !defined (PLAT_UNIX)
    if (GetWindowsVersion () >= 0x30a)
        return (lstrcpyn (str1, (LPSTR)str2, nChars));
    else
#endif //!defined (WIN32) && !defined (PLAT_UNIX)
    {
        char * cursor = str1;

        while (*str2 != '\0')
        {
            size_t charSize = IsDBCSLeadByte (*str2) ? 2 : 1;

            if (nChars > charSize)
            {
                *cursor++ = *str2;
                --nChars;

                if (charSize == 2)
                {
                    *cursor++ = *(str2 + 1);
                    --nChars;
                }

                str2 = AnsiNext (str2);
            } else
                break;
        }
        *cursor = '\0';

        return str1;
    }
}

char * StrCat (char * str1, const char * str2)
{
    if (str1 == 0)
        return 0;

    if (str2 == 0)
        return str1;

    return (lstrcat (str1, (LPSTR)str2));
}

Int16s StrCmp (const char * str1, const char * str2)
{
    if (str1 == 0)
        str1 = "";

    if (str2 == 0)
        str2 = "";

    return (lstrcmp (str1, str2));
}

Int16s StrCmpI (const char * str1, const char * str2)
{
    if (str1 == 0)
        str1 = "";

    if (str2 == 0)
        str2 = "";

    return (lstrcmpi (str1, str2));
}

char * StrDup (const char * string)
{
    if (string == 0)
        return (0);

    char * result = new char [StrLen (string) + 1];

    if (result != 0)
        StrCpy (result, string);

    return (result);
}

static inline Boolean isWhite (char c)
{
    return (c == ' ' || c == '\t');
}

static inline Boolean isNewLine (const char * c)
{
    return (*c == '\r' || *c == '\n' || (c [0] == '\x8d' && c [1] == '\n'));
}

Int16u GetWordLength (const char far * string)
{
    Int16u result = 0;

#if defined (DBCS)
    if (IsDBCSEnabled () && !isascii (*string))
    {
        result = (AnsiNext (string)) - ((char *)string);
        string += result;
        return result;
    }
#endif //(DBCS)

    while (*string != '\0' && !isWhite (*string) && !isNewLine (string)
#if defined (DBCS)
           && (!IsDBCSEnabled () || isascii (*string))
#endif //(DBCS)
          )
    {
        ++result;
        ++string;
    }

    return result;
}

Int16u GetWhiteSpaceLength (const char far * string)
{
    Int16u result = 0;

    while (isWhite (*string))
    {
        ++result;
        ++string;
    }

    return result;
}

Int16u GetNewLineLength (const char far * string)
{
    Int16u result = 0;

    if (isNewLine (string))
    {
        if (string [1] != string [0] && isNewLine (&string [1]))
            ++result;

        ++result;
    }

    return result;
}

static void slashToBackSlash (char * string)
{
    for (; *string != '\0'; string = AnsiNext (string))
    {
        if (*string == '/')
            *string = '\\';
    }
}

#ifdef _MSC_VER
char * MakeCanonicalFileName (const char * fileName)
{
    char buffer [_MAX_PATH];
    char * result = StrDup (_tfullpath (buffer, fileName, _MAX_PATH));

    if (result != 0)
    {
        AnsiLower (result);
        slashToBackSlash (result);
    }

    return (result);
}
#endif//_MSC_VER

#if defined (DBCS)
    #if defined (COMPLEX_FNSPLIT)
    /* Helping functions for FNSplit */
        static void RegisterTokens (TokenIndexList * & infoList, 
                                    const char       * pathStr,
                                    char               token)
        {
            infoList = 0;
            Int16u index = 1;
            char curChar = pathStr [index];

            while (curChar)
            {
                if (curChar == token)
                {
                    TokenIndexList * aNode = new TokenIndexList;
                    aNode->index = index;
                    aNode->next = infoList;
                    infoList = aNode;
                }

                index = AnsiNext (pathStr + index) - (char *)pathStr;
                curChar = pathStr [index];
            }
        }

        static Boolean IsAsciiToken (TokenIndexList * infoList, Int16u index)
        {
            while (infoList)
            {
                if (infoList->index == index)
                    return TRUE;

                infoList = infoList->next;
            }

            return FALSE;
        }

        static Boolean IsDirDot(TokenIndexList * slashList,
                                TokenIndexList * bkSlashList,
                                char           * pB, 
                                Int16u           index)
        {
            if (pB [index - 1] == '.')
                index--;

            switch (pB [--index])
            {
                case ':'  :
                    if (index != 1)
                        break;

                case '/'  :
                    if (!IsAsciiToken (slashList, index))
                        break;

                case '\\' :
                    if (!IsAsciiToken (bkSlashList, index))
                        break;

                case '\0' :
                    return 1;
            }

            return 0;
        }

        static void CopyIt(char * dst, const char * src, unsigned maxlen)
        {
            if (dst)
            {
                if (strlen(src) >= maxlen)
                {
                    _tcsncpy(dst, src, maxlen);
                    dst[maxlen] = 0;
                } else
                    lstrcpy(dst, src);
            }
        }

        static void DeleteIndexList (TokenIndexList * aList)
        {
            TokenIndexList * curNode = aList;

            while (curNode != 0)
            {
                aList = curNode->next;
                delete curNode;
                curNode = aList;
            }
        }

        // FNSplit differs from fnsplit in that it can split the path without
        // splitting a DBCS character whose trail byte can have the value of
        // 0x5C that also represents '\\' in the ASCII set.
        Int16u FNSplit(const char * pathP, 
                       char       * driveP,
                       char       * dirP, 
                       char       * nameP, 
                       char       * extP)
        {
            register char   * pB;
            register Int16u Wrk;
            Int16u Ret;
            char buf[MAXPATH + 2];

            //Set all string to default value zero
            Ret = 0;
            if (driveP)
                *driveP = 0;
            if (dirP)
                *dirP = 0;
            if (nameP)
                *nameP = 0;
            if (extP)
                *extP = 0;

            // Copy filename into template up to MAXPATH characters
            pB = buf;
            while (*pathP == ' ')
                pathP++;

            if ((Wrk = lstrlen(pathP)) > MAXPATH)
                Wrk = MAXPATH;
            pB [0] = 0;  // put a stopper at string beginning
            _tcsnccpy(pB + 1, pathP, Wrk);
            pB [++Wrk] = '\0';

            // forward-scan pathname and record occurences of '/', '\\', or '.'
            TokenIndexList * slashList = 0,
                           * bkSlashList = 0,
                           * dotList = 0;

            RegisterTokens (slashList, pB, '/');
            RegisterTokens (bkSlashList, pB, '\\');
            RegisterTokens (dotList, pB, '.');
    
            //Split the filename and fill corresponding nonzero pointers
            Int16s index = Wrk;
            Wrk = 0; // from this point on, Wrk serves as a Boolean flag
                     // indicating whether a directory is detected
            for ( ; ; )
            {
                --index;
                switch (pB [index])
                {
                    case '.'  :
                        if (!IsAsciiToken (dotList, index))
                            continue;

                        if (!Wrk && (pB [index + 1] == '\0'))
                            Wrk = IsDirDot(slashList, bkSlashList, pB, index);

                        if ((!Wrk) && ((Ret & EXTENSION) == 0))
                        {
                            Ret |= EXTENSION;
                            CopyIt (extP, pB + index, MAXEXT - 1);
                            pB [index] = 0;
                        }
                        continue;

                    case ':'  :
                        if (index != 2) // i.e., if not second char
                            continue;

                    case '\0' :
                        if (Wrk) // directory exists
                        {
                            if (pB [index + 1])
                                Ret |= DIRECTORY;
                            CopyIt(dirP, pB + index + 1, MAXDIR - 1);
                            pB [index + 1] = 0;
    
                            Int16u dirL = lstrlen (dirP);
                            if (dirL == 0)
                                break;

                            switch (dirP [dirL - 1])
                            {
                                case '\\':
                                    if (IsAsciiToken (bkSlashList, index + dirL))
                                        break;

                                case '/':
                                    if (IsAsciiToken (slashList, index + dirL))
                                        break;

                                default: // add trailing slash
                                    dirP [dirL] = '\\';
                                    dirP [dirL + 1] = '\0';
                            }
                            break;
                        }

                    case '/'  :
                    case '\\' :
                        if (!Wrk && (pB [index] == '\0' ||
                            IsAsciiToken (pB [index] == '/' ? slashList : bkSlashList, index)))
                        {
                            Wrk++;
                            if (pB [index + 1])
                                Ret |= FILENAME;
                            CopyIt(nameP, pB + index + 1, MAXFILE - 1);
                            pB [index + 1] = 0;
                            if (pB [index] == 0 ||
                                (pB [index] == ':' && index == 2))
                            break;
                        }
                        continue;

                    case '*'  :
                    case '?'  :
                        if (!Wrk)
                            Ret |= WILDCARDS;
                    default :
                        continue;
                } // switch
                break;
            } // for

            if (pB [2] == ':')
            {
                if (pB [0])
                    Ret |= DRIVE;
                CopyIt(driveP, &pB [1], MAXDRIVE - 1);
            }   

            DeleteIndexList (slashList);
            DeleteIndexList (bkSlashList);
            DeleteIndexList (dotList);

            return (Ret);
        }
    #elif !defined (LONG_FILE_NAME)
        static Boolean isDriveLetter (const char * p)
        {
        #if defined (DRIVE_LETTER_A_TO_Z)
            return (*p >= 'a' && *p <= 'z') || (*p >= 'A' && *p <= 'Z');
        #else
            return *p != '\0';
        #endif //(DRIVE_LETTER_A_TO_Z)
        }

        Int16u FNSplit (const char * pathP, 
                        char       * driveP,
                        char       * dirP, 
                        char       * nameP, 
                        char       * extP)
        {
            Int16u     Ret;
 
            //Set all string to default value zero
            Ret = 0;
            if (driveP)
                *driveP = 0;
            if (dirP)
                *dirP = 0;
            if (nameP)
                *nameP = 0;
            if (extP)
                *extP = 0;

            if (pathP == 0)
                return 0;

            const char * cursor = pathP,
                       * dirStart = 0,
                       * lastSlash = 0;

            // check for a drive letter
            if (isDriveLetter (cursor) && *AnsiNext (cursor) == ':')
            {
                if (driveP != 0)
                    StrCpyN (driveP, cursor, MAXDRIVE);

                cursor = AnsiNext (AnsiNext (cursor));
                Ret |= DRIVE;
            }

            dirStart = cursor;

            // now find the last slash
            while (*cursor != '\0')
            {
                if (*cursor == '/' || *cursor == '\\')
                    lastSlash = cursor;

                cursor = AnsiNext (cursor);
            }

            if (lastSlash != 0)
            {
                // a slash was seen
                if (dirP != 0)
                {
                    Int16u bytesToCopy = lastSlash - dirStart
                                         + 1  // for '/'
                                         + 1; // for '\0'

                    if (bytesToCopy > MAXDIR)
                        bytesToCopy = MAXDIR;
    
                    StrCpyN (dirP, dirStart, bytesToCopy);
                }

                cursor = lastSlash + 1;
                Ret |= DIRECTORY;
            } else
                cursor = dirStart;

            // cursor now points to the start of the filename
            // check for wildcards
            if (AnsiStrChr (cursor, '*') != 0 ||
                AnsiStrChr (cursor, '?') != 0)
                Ret |= WILDCARDS;

            // now see if there's an extension
            const char * period = AnsiStrChr (cursor, '.');

            if (period != 0)
            {
                Int16u bytesToCopy = period - cursor + 1;  // for '\0'

                if (bytesToCopy > MAXFILE)
                    bytesToCopy = MAXFILE;

                if (bytesToCopy > 0)
                {
                    if (nameP != 0)
                        StrCpyN (nameP, cursor, bytesToCopy);

                    Ret |= FILENAME;
                }

                if (extP != 0)
                    StrCpyN (extP, period, MAXEXT);

                Ret |= EXTENSION;
            } else 
            {
                if (*cursor != '\0')
                {
                    if (nameP != 0)
                        StrCpyN (nameP, cursor, MAXFILE);

                    Ret |= FILENAME;
                }
            }

        #if defined (DEBUG_FNSPLIT)
            static char buffer [MAXPATH + MAXPATH + 32];
            
            wsprintf (buffer, 
                      "pathP:[%s]  driveP:[%s]  dirP:[%s]  nameP:[%s]  extP:[%s]",
                      pathP,
                      driveP != NULL ? driveP : "<NULL>",
                      dirP != NULL ? dirP : "<NULL>",
                      nameP != NULL ? nameP : "<NULL>",
                      extP != NULL ? extP : "<NULL>");
            MessageBox (GetActiveWindow (), buffer, "FNSplit", MB_OK);
        #endif //(DEBUG_FNSPLIT)

        return Ret;
    }
	 #else

		  static Boolean isAlpha(char l)
		  {
			  return l >= 'a' && l <= 'z' || l >= 'A' && l <= 'Z';
		  }
		  Int16u FNSplit (const char * pathP,
								char       * driveP,
								char       * dirP,
								char       * nameP,
								char       * extP)
		  {
				Int16u     Ret;

				//Set all string to default value zero
				Ret = 0;
				if (driveP)
					 *driveP = 0;
				if (dirP)
					 *dirP = 0;
				if (nameP)
					 *nameP = 0;
				if (extP)
					 *extP = 0;

				if (pathP == 0)
					 return 0;


				const char * cursor = pathP,
							  * dirStart = 0,
							  * lastSlash = 0;

//				if (StartsWithAnsi (cursor) && _istalpha (*cursor) &&
				if (StartsWithAnsi (cursor) && isAlpha (*cursor) &&
					 StartsWithAnsiChar (cursor + 1, ':'))
				{
					 // A drive letter presents.
					 if (driveP != 0)
						  StrCpyN (driveP, cursor, MAXDRIVE);

					 cursor += 2;
                Ret |= DRIVE;
            }

            dirStart = cursor;

            // now find the last slash
            while (*cursor != '\0')
            {
                if (StartsWithAnsiChar (cursor, '/') ||
                    StartsWithAnsiChar (cursor, '\\'))
                    lastSlash = cursor;

                cursor = AnsiNext (cursor);
            }

            if (lastSlash != 0)
            {
                // a slash was seen
                if (dirP != 0)
                {
                    Int16u bytesToCopy = lastSlash - dirStart
                                         + 1  // for '/'
                                         + 1; // for '\0'

                    if (bytesToCopy > MAXDIR)
                        bytesToCopy = MAXDIR;

                    StrCpyN (dirP, dirStart, bytesToCopy);
                }

                cursor = lastSlash + 1;

                Ret |= DIRECTORY;
            } else
                cursor = dirStart;

            // cursor now points to the start of the filename
            // check for wildcards
            if (AnsiStrChr (cursor, '*') != 0 ||
                AnsiStrChr (cursor, '?') != 0)
                Ret |= WILDCARDS;

            // now see if there's an extension
            const char * period = 0;

            for (const char * ptr = cursor; *ptr != '\0'; ptr = AnsiNext (ptr))
            {
                if (StartsWithAnsiChar (ptr, '.'))
                    period = ptr;
            }

            if (period != 0)
            {
                Int16u bytesToCopy = period - cursor + 1;  // for '\0'

                if (bytesToCopy > MAXFILE)
                    bytesToCopy = MAXFILE;

                if (bytesToCopy > 0)
                {
                    if (nameP != 0)
                        StrCpyN (nameP, cursor, bytesToCopy);

                    Ret |= FILENAME;
                }

                if (extP != 0)
                    StrCpyN (extP, period, MAXEXT);

                Ret |= EXTENSION;
            } else 
            {
                if (*cursor != '\0')
                {
                    if (nameP != 0)
                        StrCpyN (nameP, cursor, MAXFILE);

                    Ret |= FILENAME;
                }
            }

        #if defined (DEBUG_FNSPLIT)
            static char buffer [MAXPATH + MAXPATH + 32];

            wsprintf (buffer, 
                      "pathP:[%s]  driveP:[%s]  dirP:[%s]  nameP:[%s]  extP:[%s]",
                      pathP,
                      driveP != NULL ? driveP : "<NULL>",
                      dirP != NULL ? dirP : "<NULL>",
                      nameP != NULL ? nameP : "<NULL>",
                      extP != NULL ? extP : "<NULL>");
            MessageBox (GetActiveWindow (), buffer, "FNSplit", MB_OK);
        #endif //(DEBUG_FNSPLIT)

            return Ret;
        }
    #endif //(COMPLEX_FNSPLIT)
#else // use SBCS version
    #if defined (_MSC_VER)
        void 
    #else
        Int16u 
    #endif //(_MSC_VER)
               FNSplit (const char * pathP, 
                        char       * driveP,
						size_t       driveSz,
                        char       * dirP, 
						size_t       dirSz,
                        char       * nameP,
						size_t       nameSz,
                        char       * extP,
						size_t       extSz)
    {
    #if defined (_MSC_VER)
        _tsplitpath_s(pathP, driveP, driveSz, dirP, dirSz, nameP, nameSz, extP, nameSz);
    #elif defined(PLAT_WINDU)
        _tsplitpath (pathP, driveP, dirP, nameP, extP);
        Int16u result = TRUE;
    #else 
        Int16u result = (Int16u)fnsplit (pathP, driveP, dirP, nameP, extP);
    #endif //(_MSC_VER)

        if (dirP != 0)
        {
            Int16u dirL = lstrlen (dirP);
            if (dirL > 0 && dirP [dirL - 1] != '/' && dirP [dirL - 1] != '\\')
            {
                dirP [dirL] = '\\';
                dirP [dirL + 1] = '\0';
            }
        }

    #if !defined (_MSC_VER)
        return result;
    #endif //!defined (_MSC_VER)
    }
#endif //(DBCS)

#if defined (WIN32)
/* bfung: Disable in VS2005
    wchar_t * MakeUnicode (const char * ansi)
    {
        if (ansi == 0)
            return 0;

        Int16u length = StrLen (ansi) + 1;
        wchar_t * result = new wchar_t [length];
		size_t nCharConverted = 0;

        if (result != 0)
            mbstowcs_s (&nCharConverted, result, ansi, length);

        return result;
    }
*/
    static Int32u UniStrLen (const wchar_t * unicode)
    {
        Int32u result = 0;

        while (*unicode++ != 0)
            ++result;

        return result;
    }
/* bfung: Disable in VS2005
    char * MakeAnsi (const wchar_t * unicode)
    {
        if (unicode == 0)
            return 0;

        Int32u length = UniStrLen (unicode) + 1;
        char * result = new char [length];

        if (result != 0)
            wcstombs_s (result, unicode, length);

        return result;
    }
*/
#endif //(WIN32)

#if defined (KEYWORD_LIKE)
    const char PATTERN_ANYCHAR = '?';
    const char PATTERN_MANYCHAR = '*';
    const char PATTERN_ESCCHAR  = '\\';
#ifdef _MSC_VER
    Boolean MatchStringPattern (const char * string, const char * pattern, Boolean CaseSensative)
    {
        Boolean escaped = FALSE;

        for (; *pattern != '\0'; pattern++)
        {
            if (PATTERN_ESCCHAR == *pattern && !escaped)
            {
                escaped = TRUE;
                continue;
            }

            if (PATTERN_MANYCHAR == *pattern && !escaped)
            {
                for (;;)
                {
                    if ('\0' == *(pattern + 1))
                       return TRUE;  // We can have any number of trailing characters.

                    if (MatchStringPattern (string, pattern + 1))
                       return TRUE;

                    if ('\0' == *string)
                        return FALSE;  // We have unmatched pattern char/s.

                    string++;
                }
            }

            if ('\0' == *string)
                return FALSE;  // We have unmatched pattern char/s. Not a PATTERN_MANYCHAR.

            if (*pattern != PATTERN_ANYCHAR || escaped)
            {
                if(CaseSensative)
                {
                  if (*pattern != *string)
                     return FALSE;  // Something is not matched.
                } else
                {
                    if(_totupper(*pattern) != _totupper(*string))
                     return FALSE;  // Something is not matched.
                }

            }

            escaped = FALSE;
            string++;
        }

        return '\0' == *string;
    }
#endif //_MSC_VER
#endif  //(KEYWORD_LIKE)

#if defined (KEYWORD_STARTWITH)
    Boolean StringStartWith (const char * string, const char * startWithString,Boolean CaseSensative)
    {
        int stringLen = StrLen (string);
        int startWithStringLen = StrLen (startWithString);

        if(CaseSensative)
           return stringLen >= startWithStringLen && 0 == memcmp (string, startWithString, startWithStringLen);
        else
            return stringLen >= startWithStringLen && 0 == memicmp (string, startWithString, startWithStringLen);
    }
#endif //(KEYWORD_STARTWITH)

BOOL SafeStrInsert (char far * & str1, const char far * str2, Int16u pos, Int16u & physicalLen)
{
    if ((str1 == NULL) || (str2 == NULL))
    {
        return FALSE;
    }
    int i;
    int len1 = StrLen(str1);
    int len2 = StrLen(str2);
    char far * tempStr = NULL;
    Int16u tempLen = physicalLen;
    if (len1 + len2 > physicalLen)
    {
        physicalLen = 2 * (len1 + len2);
        tempStr = str1;
        str1 = new char[physicalLen + 1];
        if (str1 == NULL)
        {   
            str1 = tempStr;
            physicalLen = tempLen;
            return FALSE;
        }
        StrCpy(str1 , tempStr);
        delete [] tempStr;

    }
    if (len2 == 0)
        return TRUE;

    for (i = len1 - 1 ; i >= pos ; i--)
        str1[i + len2] = str1[i];
    for (i = 0 ; i < len2 ; i++)
        str1[pos + i] = str2[i];
    str1[len1 + len2] = '\0';

    return TRUE;
}

BOOL SafeStrCat (char far * & str1, const char far * str2, Int16u & physicalLen)
{
    Int16u pos = StrLen(str1);
    return SafeStrInsert(str1, str2, pos, physicalLen);
}

//******************************************************************************//
// Copyright 2014 Benjamin Fung      											//
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

//---------------------------------------------------------------------------
// File:
//      types.hpp
//
// Objective:
//      This file contains the declaration of basic types.
//
// History:
//      Mar. 12, 2002   Created by Benjamin Fung
//
//---------------------------------------------------------------------------

#if !defined (TYPES_HPP)
#define TYPES_HPP

#if defined (LARGE_TWIPS)
    #define TWIP_IS_5TWIPS
#else
    #define TWIP_IS_TWIP
#endif //(LARGE_TWIPS)
// #define TWIP_IS_POINT

/*
** Arithmetic types
**
** This section should define types IntXy, where X is one
** of 8, 16 or 32, and y is s or u. X indicates the number
** of bits, and y whether the type is signed or unsigned.
**
** The actual definition should be to a type which is at least
** as wide as required, and properly signed or unsigned.
**
** The LongDouble and Number types should have as many significant
** digits as possible.
*/

#if defined (__BORLANDC__) && (defined (__MSDOS__) || defined (__WIN32__)) || defined (_MSC_VER) || defined (PLAT_UNIX)
#if defined(__SUNPRO_CC_COMPAT) && (__SUNPRO_CC_COMPAT == 5)
    #include <limits>
#else
    #include <limits.h>
#endif

    typedef signed   char  Int8s;
    static  const          Int8s  MinInt8s  = SCHAR_MIN;
    static  const          Int8s  MaxInt8s  = SCHAR_MAX;

    typedef unsigned char  Int8u;
    static  const          Int8u  MinInt8u  = 0;
    static  const          Int8u  MaxInt8u  = UCHAR_MAX;

    typedef signed   short Int16s;
    static  const          Int16s MinInt16s = SHRT_MIN;
    static  const          Int16s MaxInt16s = SHRT_MAX;

    typedef unsigned short Int16u;
    static  const          Int16u MinInt16u = 0;
    static  const          Int16u MaxInt16u = USHRT_MAX;

    typedef signed   long  Int32s;
    static  const          Int32s MinInt32s = LONG_MIN;
    static  const          Int32s MaxInt32s = LONG_MAX;

    typedef unsigned long  Int32u;
    static  const          Int32u MinInt32u = 0;
    static  const          Int32u MaxInt32u = ULONG_MAX;

#ifdef _BFLIB_ENABLE_64BIT
    typedef long long           Int64s;
    typedef unsigned long long  Int64u;
#endif

    typedef long double    LongDouble;
    typedef LongDouble     Number;
#endif //defined (__BORLANDC__) && (defined (__MSDOS__) || defined (__WIN32__)) || defined (_MSC_VER) || defined (PLAT_UNIX)

/*
** class Boolean
**
** Used to represent TRUE or FALSE
*/

#if !defined (FALSE)
    const Int16s FALSE = 0, TRUE = 1;
#endif //!defined (FALSE)

#if defined (DEFINE_BOOLEAN)
    class Boolean
    {
        private:
            Int16s b;
            void value (const Int16s i) {b = (i) ? TRUE : FALSE;};

        public:
            Boolean ()                  {b = FALSE;};
            Boolean (const Int16s i)    {value (i);};
            Boolean (const Boolean &i)  {b = i.b;};

            operator Int16s () const    {return (b);};
    };
#else
    typedef Int16s Boolean;
#endif //(DEFINE_BOOLEAN)

#define lengthof(array) (sizeof (array) / sizeof (array [0]))

#if defined (WIN32) || defined (PLAT_UNIX)
    #define ENUM(t)   typedef Int16u t;\
                      enum _##t
#else
    #define ENUM(t) enum t
#endif //(WIN32) || (PLAT_UNIX)

typedef Int16s Twip;        // big enough for 22.75"
typedef Int32s LargeTwip;

#if defined (NEGATIVE_TWIPS_IN_DESIGNWIN)
    static const Twip MaxTwip       = (Twip) SHRT_MAX;
    static const Twip InvalidTwip   = (Twip) -MaxTwip,
#else
    static const Twip InvalidTwip   = (Twip) SHRT_MIN,
                 MaxTwip            = (Twip) SHRT_MAX,
#endif //(NEGATIVE_TWIPS_IN_DESIGNWIN)

#if defined (TWIP_IS_TWIP)
                 TwipsPerTwip  = (Twip) 1,
                 TwipsPerPoint = (Twip) 20,
                 TwipsPerInch  = (Twip) 1440,
                 TwipsPerCm    = (Twip) 567;
#elif defined (TWIP_IS_5TWIPS)
                 TwipsPerTwip  = (Twip) 5,
                 TwipsPerPoint = (Twip) 4,
                 TwipsPerInch  = (Twip) 288,
                 TwipsPerCm    = (Twip) 113;
#elif defined (TWIP_IS_POINT)
                 TwipsPerPoint = (Twip) 1,
                 TwipsPerInch  = (Twip) 72,
                 TwipsPerCm    = (Twip) 28;
#endif //(TWIP_IS_TWIP)

struct TwipPoint
{
    POINT getPOINT () const;

    TwipPoint & operator = (const TwipPoint & p);
    TwipPoint & operator = (const POINT & p);

    Twip x, y;
};

inline POINT TwipPoint::getPOINT () const
{
    POINT result;

    result.x = x;
    result.y = y;

    return result;
}

inline TwipPoint & TwipPoint::operator = (const TwipPoint & p)
{
    x = p.x;
    y = p.y;

    return *this;
}

inline TwipPoint & TwipPoint::operator = (const POINT & p)
{
    x = Twip (p.x);
    y = Twip (p.y);

    return *this;
}

struct TwipRect
{
    Twip getWidth  () const {return right - left;}
    Twip getHeight () const {return bottom - top;}

    RECT getRECT () const;

    TwipRect & operator = (const TwipRect & r);
    TwipRect & operator = (const RECT & r);

    Boolean isEmpty () const {return left >= right || top >= bottom;};
    void setEmpty () {left = top = right = bottom = 0;};

    void set (Twip l, Twip t, Twip r, Twip b) {left = l; top = t; right = r; bottom = b;};
    void offset (Twip dx, Twip dy)            {left += dx; top += dy; right += dx; bottom += dy;};
    void inflate (Twip dx, Twip dy)           {left -= dx; top -= dx; right += dx; bottom += dx;};

    Boolean contains (TwipPoint pt) const 
        {return pt.x >= left && pt.x < right && pt.y >= top && pt.y < bottom;};

    Boolean  intersects (const TwipRect &other) const
        {return !isEmpty ()        && 
                !other.isEmpty ()  && 
                left < other.right && 
                top < other.bottom && 
                right > other.left && 
                bottom > other.top;};

    TwipRect unite (const TwipRect & other) const;
    TwipRect intersect (const TwipRect & other) const;

    Twip left,
         top,
         right,
         bottom;
};

inline RECT TwipRect::getRECT () const
{
    RECT result;

    result.left   = left;
    result.right  = right;
    result.top    = top;
    result.bottom = bottom;

    return result;
}

inline TwipRect &TwipRect::operator = (const TwipRect & r)
{
    left   = r.left;
    top    = r.top;
    right  = r.right;
    bottom = r.bottom;

    return *this;
}

inline TwipRect & TwipRect::operator = (const RECT & r)
{
    left   = Twip (r.left);
    top    = Twip (r.top);
    right  = Twip (r.right);
    bottom = Twip (r.bottom);

    return *this;
}

inline TwipRect TwipRect::unite (const TwipRect & other) const
{
    if (this->isEmpty ())
        return other;
    else if (other.isEmpty ())
        return *this;

    TwipRect result;

    result.left   = left   < other.left   ? left   : other.left;
    result.top    = top    < other.top    ? top    : other.top;
    result.right  = right  > other.right  ? right  : other.right;
    result.bottom = bottom > other.bottom ? bottom : other.bottom;

    return result;
}

inline TwipRect TwipRect::intersect (const TwipRect & other) const
{
    TwipRect result;

    if (!intersects (other))
        result.setEmpty ();
    else
    {
        result.left   = left   > other.left   ? left   : other.left;
        result.top    = top    > other.top    ? top    : other.top;
        result.right  = right  < other.right  ? right  : other.right;
        result.bottom = bottom < other.bottom ? bottom : other.bottom;
    }

    return result;
}

#if defined (LARGE_TWIPS)
    Twip       ConvertTwipTo5Twips(Twip oldTwip);
    TwipPoint  ConvertTwipPointTo5Twips(TwipPoint oldTwipPoint);
    TwipRect   ConvertTwipRectTo5Twips(TwipRect oldTwipRect);
    Twip       Convert5TwipsToTwips(Twip oldTwip);
    TwipPoint  Convert5TwipsPointToTwips(TwipPoint oldTwipPoint);
    TwipRect   Convert5TwipsRectToTwips(TwipRect oldTwipRect);
#endif //(LARGE_TWIPS)

enum UnitsType
{
    inTwips, inPoints, inInches, inCentimeters
};

typedef Int16u PointSize;
typedef Number Currency;
typedef Int32s Date;
typedef Int32s Time;

static const Date NullDate = -1;
static const Time NullTime = -1;
static const Int32s NSecondsInDay = (Int32s)60 * 60 * 24;

#if !defined (_MSC_VER) || defined (MFC)
    static const COLORREF NoColour = (COLORREF) MaxInt32u;
#endif //!defined (_MSC_VER) || defined (MFC)

struct DateTimeBase
{
	Date date;
	Time time;
};

struct DateTime : public DateTimeBase
{
    DateTime (Date date_ = NullDate,
              Time time_ = NullTime)
    {
        date = date_;
        time = time_;
    }
    DateTime (const DateTime & dateTime)
    {
        date = dateTime.date;
        time = dateTime.time;
    }

    DateTime & operator = (const DateTime & dateTime)
    {
        date = dateTime.date;
        time = dateTime.time;

        return *this;
    }

    int operator == (const DateTime & dateTime) const
    {
        return date == dateTime.date &&
               time == dateTime.time;
    }                
    int operator != (const DateTime & dateTime) const
    {
        return !(*this == dateTime);
    }

    int operator < (const DateTime & dateTime) const
    {
        return date < dateTime.date ||
               (date == dateTime.date &&
                time < dateTime.time);
    }                
    int operator > (const DateTime & dateTime) const
    {
        return date > dateTime.date ||
               (date == dateTime.date &&
                time > dateTime.time);
    }
    
    int operator <= (const DateTime & dateTime) const
    {
        return !(*this > dateTime);
    }                
    int operator >= (const DateTime &dateTime) const
    {
        return !(*this < dateTime);
    }


    Boolean IsNullDateTime() {return (date == NullDate && time == NullTime );};

    void normalize ();

    static void NumberToNDaysNSeconds (double   fractionalNDays,
                                       Int32s & nDays,
                                       Int32s & nSeconds);
    
    static double NDaysNSecondsToNumber (Int32s nDays,
                                         Int32s nSeconds);
};

inline void DateTime::normalize ()
{
    if (time >= 0 && time < NSecondsInDay)
        return;

    Boolean negative = time < 0;
    if (negative)
        time = -time - 1;

    Int32s nDays = time / NSecondsInDay;
    Int32s nSeconds = time % NSecondsInDay;

    if (negative)
    {
        date -= nDays + 1;
        time = NSecondsInDay - nSeconds - 1;
    } else
    {
        date += nDays;
        time = nSeconds;
    }
}
 
inline void DateTime::NumberToNDaysNSeconds (double   fractionalNDays,
                                             Int32s & nDays,
                                             Int32s & nSeconds)
{
    double days = (double)((Int32s)fractionalNDays);
    double seconds = fractionalNDays - days;

    nDays = (Int32s) days;
    nSeconds = (Int32s) (seconds * NSecondsInDay);
}

inline double DateTime::NDaysNSecondsToNumber (Int32s nDays,
                                               Int32s nSeconds)
{
    return nDays + (double) nSeconds / NSecondsInDay;
}

inline DateTime operator + (const DateTime &dateTime, double fractionalNDays)
{
    Int32s nDays, nSeconds;
    DateTime::NumberToNDaysNSeconds (fractionalNDays, nDays, nSeconds);

    DateTime dt (dateTime.date + nDays, dateTime.time + nSeconds);
    dt.normalize ();
    
    return dt;
}

inline DateTime operator + (double nDays, const DateTime & dateTime)
{
    return dateTime + nDays;
}

inline DateTime operator - (const DateTime & dateTime, double fractionalNDays)
{
    Int32s nDays, nSeconds;
    DateTime::NumberToNDaysNSeconds (fractionalNDays, nDays, nSeconds);

    DateTime dt (dateTime.date - nDays, dateTime.time - nSeconds);
    dt.normalize ();
    
    return dt;
}

inline double operator - (const DateTime & dateTime1, const DateTime & dateTime2)
{
    double fractionalNDays1 = DateTime::NDaysNSecondsToNumber (dateTime1.date, dateTime1.time);
    double fractionalNDays2 = DateTime::NDaysNSecondsToNumber (dateTime2.date, dateTime2.time);  
        
    return fractionalNDays1 - fractionalNDays2;
}

struct VersionInfo
{
    Int16u major,
           minor;
    char   letter;
};

inline int operator == (const VersionInfo & version1, const VersionInfo & version2)
{
#if defined (ORIGINAL)
    return version1.major == version2.major &&
           version1.minor == version2.minor &&
           version1.letter == version2.letter;
#else
    return version1.major == version2.major &&
           version1.minor == version2.minor;
#endif //(ORIGINAL)
}
inline int operator < (const VersionInfo & version1, const VersionInfo & version2)
{
#if defined (ORIGINAL)
    return version1.major < version2.major ||
           (version1.major == version2.major &&
            (version1.minor < version2.minor ||
             (version1.minor == version2.minor &&
              version1.letter < version2.letter)));
#else
    return version1.major < version2.major ||
           (version1.major == version2.major &&
            version1.minor < version2.minor);
#endif //(ORIGINAL)
}
inline int operator > (const VersionInfo & version1, const VersionInfo & version2)
{
#if defined (ORIGINAL)
    return version1.major > version2.major ||
           (version1.major == version2.major &&
            (version1.minor > version2.minor ||
             (version1.minor == version2.minor &&
              version1.letter > version2.letter)));
#else
    return version2 < version1;
#endif //(ORIGINAL)
}
inline int operator <= (const VersionInfo & version1, const VersionInfo & version2)
{
    return version1 < version2 || version1 == version2;
}
inline int operator >= (const VersionInfo & version1, const VersionInfo & version2)
{
    return version1 > version2 || version1 == version2;
}

ENUM (FieldValueType)
{
    int8sField, 
    int8uField,
    int16sField, 
    int16uField,
    int32sField, 
    int32uField,
    numberField,
    currencyField,
    booleanField,
    dateField, 
    timeField,
    stringField,
    transientMemoField, 
    persistentMemoField,
    blobField,
	datetimeField,
#if defined (GRAPHIC_FIELDS)
    bitmapField = 20,
    iconField,
    pictureField,  // Windows metafile
    oleField,
    chartField,
#endif //(GRAPHIC_FIELDS)
#if defined (GROUP_BY)
	sameAsInputField = 250,
#endif //(GROUP_BY)
    unknownField = 255
};

ENUM (SortDirection)
{
    inAscendingOrder, 
    inDescendingOrder, 
    inOriginalOrder
#if defined (MORE_GROUP_OPTIONS)
    ,inSpecifiedOrder
#endif //(MORE_GROUP_OPTIONS)
};

typedef Boolean (* ActionFunction) (void * object, void * parameters);

#if !defined (_MSC_VER) && !defined (PLAT_UNIX)
    template<class T> T max (T a, T b)
    {
        return a > b ? a : b;
    }

    template<class T> T min (T a, T b)
    {
        return a < b ? a : b;
    }

    template<class T> T abs (T a)
    {
        return a < 0 ? -a : a;
    }

    Int8s  max (Int8s  a, Int8s  b);
    Int8u  max (Int8u  a, Int8u  b);
    Int16s max (Int16s a, Int16s b);
    Int16u max (Int16u a, Int16u b);
    Int32s max (Int32s a, Int32s b);
    Int32u max (Int32u a, Int32u b);
    Number max (Number a, Number b);
    Twip   max (Twip   a, Twip   b);
    Int8s  min (Int8s  a, Int8s  b);
    Int8u  min (Int8u  a, Int8u  b);
    Int16s min (Int16s a, Int16s b);
    Int16u min (Int16u a, Int16u b);
    Int32s min (Int32s a, Int32s b);
    Int32u min (Int32u a, Int32u b);
    Number min (Number a, Number b);
    Twip   min (Twip   a, Twip   b);

    #if defined (OWL2)
        unsigned min (unsigned a, unsigned b);
    #endif //(OWL2)
#endif //!defined (_MSC_VER) && !defined (PLAT_UNIX)

#if defined (DEPENDENCY)
    enum ChangeType
    {
        changeUnknown,
    #if defined (CHANGE_DELETED)
        changeDeleted,
    #endif //(CHANGE_DELETED)
        changeToBeDeleted,
        changeValueType,
        changeDataSize,
        changeDisplayWidth,
        changeSectionAppearance,
        changeData,
        changeFrameAppearance,
        changeFormulaText
    };
#endif //(DEPENDENCY)

#if defined (_MSC_VER) || defined (PLAT_UNIX)
    #define min(a, b)  (((a) < (b)) ? (a) : (b))
    #define max(a, b)  (((a) > (b)) ? (a) : (b))

    #if defined (WIN32) || defined (PLAT_UNIX)
        #define _far
        #define _export
        #define huge
        #define _loadds
      #if !defined (PLAT_UNIX_NO_F_DEFINES)
        #define _fstrstr strstr
        #define _fstrchr strchr
        #define _fstrcpy strcpy
        #define _fstrcmp strcmp
        #define _fstricmp stricmp
        #if !defined(PLAT_UNIX)
        	#define _fstrupr strupr
        	#define _fstrlwr strlwr
        #endif
      #endif
    #endif //(WIN32)

    #if !defined (PLAT_UNIX)
        #define _FAR _far
    #endif //(PLAT_UNIX)
        
        #define _FARFUNC
#endif //defined (_MSC_VER) || defined (PLAT_UNIX)

/*
 *  The following 2 macro-s are used often enough, so that I felt they should be here.
 *  Please, use them instead of casting in assignments!
 *
 *  The need for this comes from the hard alignment requirements of RISC processors running Unix.
 *  So, when PLAT_UNIX is defined it expands to memcpy, othrewise it is just a regular assignment.
 *  I could have used memcpy everywhere, but chose not to because of probable performance penalty on 
 *  Intel platforms.
 */

/*
 *  If your platform has strict  requirements for data alignment you should define 
 *  PLAT_STRICT_ALIGNMENT as well.
 */
#if defined (PLAT_UNIX)
    #define PLAT_STRICT_ALIGNMENT
#endif //(PLAT_UNIX)

/* Replaces:    to = *(type *) from; */
#if !defined (SAFE_ASSIGNMENT)
    #if !defined (PLAT_STRICT_ALIGNMENT)
        #define   SAFE_ASSIGNMENT(to, from, type)   *(type far*) &to = *(type far*) from
    #else
        #define   SAFE_ASSIGNMENT(to, from, type)   memcpy((char *) &to, (char *) from, sizeof(type))
    #endif //!defined (PLAT_STRICT_ALIGNMENT)
#endif //!defined (SAFE_ASSIGNMENT)

/* Replaces:    *(type *) to = *(type *) from; */
#if !defined (SAFE_ASSIGNMENT2)
    #if !defined (PLAT_STRICT_ALIGNMENT)
        #define   SAFE_ASSIGNMENT2(to, from, type)   *(type far*) to = *(type far*) from
    #else
        #define   SAFE_ASSIGNMENT2(to, from, type)   memcpy((char *) to, (char *) from, sizeof(type))
    #endif //!defined (PLAT_STRICT_ALIGNMENT)
#endif //(SAFE_ASSIGNMENT2)

/* Replaces:    *(type1 *) to = (type1) *(type2 *) from; */
#if !defined (SAFE_ASSIGNMENT3)
    #if !defined (PLAT_STRICT_ALIGNMENT)
        #define   SAFE_ASSIGNMENT3(to, from, type1, type2) *(type1 far*) to = (type1) *(type2 far*) from
    #else
        #define   SAFE_ASSIGNMENT3(to, from, type1, type2) {\
	                                                        type1 t1;\
												            type2 t2;\
											         	    memcpy((char *) &t2, (char *) from, sizeof(type2)); \
											   	            t1 = (type1) t2;\
												            memcpy((char *) to, (char *) &t1, sizeof(type1)); \
                                                           }
    #endif //!defined (PLAT_STRICT_ALIGNMENT)
#endif //(SAFE_ASSIGNMENT3)

#endif //!defined (TYPES_HPP)


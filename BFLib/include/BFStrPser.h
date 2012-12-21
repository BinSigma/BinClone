// BFStrPser.h: interface for the String Parser classes
//
//////////////////////////////////////////////////////////////////////

#if !defined(BFSTRPSER_H)
#define BFSTRPSER_H

//--------------------------------------------------------------------
//--------------------------------------------------------------------
class CBFStrParser
{
public:
    CBFStrParser(LPCTSTR str, TCHAR delimeter);
    virtual ~CBFStrParser();

    bool getNext(CString& s);
    void reset() { m_idx = -1; };

protected:
    CString m_str;
    TCHAR m_delimeter;
    int m_idx;
};

#endif
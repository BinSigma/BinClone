#if !defined(AFX_CLONEPAIRLISTCTRL_H__E7C610D7_C3C4_11D3_A6BE_88BBC02C944D__INCLUDED_)
#define AFX_CLONEPAIRLISTCTRL_H__E7C610D7_C3C4_11D3_A6BE_88BBC02C944D__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ClonePairListCtrl.h : header file
//

#include <map>


/////////////////////////////////////////////////////////////////////////////
// CClonePairListCtrl window
class CloneFiles;
class CloneFile;
class CClonePairListCtrl : public CListCtrl
{
// Construction
public:
	CClonePairListCtrl();

// Attributes
public:

// Operations
public:
	COLORREF m_colRow1;
	COLORREF m_colRow2;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CClonePairListCtrl)
public:
	virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CClonePairListCtrl();

	BOOL Init(CloneFiles * p_CloneFiles);
	BOOL InitCloneFile(CloneFile & p_cloneFile);

    friend class CCloneDetectorGUIView;
	// Generated message map functions
protected:
	//int curSelRow;
	bool                      m_init;
	std::map<CString,CString> m_toolTipTexts;

	//{{AFX_MSG(CClonePairListCtrl)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);	
	afx_msg void OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG


	DECLARE_MESSAGE_MAP()
	virtual void PreSubclassWindow();
public:
	virtual INT_PTR OnToolHitTest(CPoint point, TOOLINFO* pTI) const;
	afx_msg BOOL OnToolNeedText(UINT id, NMHDR* pNMHDR, LRESULT* pResult);

	void CellHitTest(const CPoint& pt, int& nRow, int& nCol) const;
	LPTSTR GetToolTipText(int nRow, int nCol);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ClonePairListCtrl_H__E7C610D7_C3C4_11D3_A6BE_88BBC02C944D__INCLUDED_)

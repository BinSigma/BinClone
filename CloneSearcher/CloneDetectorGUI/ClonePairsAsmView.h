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

#include "..\..\CloneSearcher\CloneSearcherLib\CSController.h"
#include "CloneFiles.h"
#include "ClonePairsAsmDoc.h"
#include "ClonePairsAsmFrame.h"


// ClonePairsAsmView view

class ClonePairsAsmView : public CRichEditView
{
	DECLARE_DYNCREATE(ClonePairsAsmView)

protected:
	ClonePairsAsmView();           // protected constructor used by dynamic creation
	virtual ~ClonePairsAsmView();

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif
	ClonePairsAsmDoc* GetDocument() const;

protected:
	bool    m_isListView;
	bool    m_binit;
	CString m_filename;
	int     m_beginLine;
	int     m_endLine;
	long    m_beginPos;
	long    m_endPos;
	int     m_fileId;
	int     m_viewId;
	bool    m_popSyncScroll;
	

	DECLARE_MESSAGE_MAP()


public:
	int getFileId() {return m_fileId; }
	virtual void OnInitialUpdate();
	void selectedLine(unsigned int p_line);
	void highLightLines(int p_begin, int p_end);
	void initView(const CString & p_filename, const CString & p_str, int viewId, int p_fileId=-1);
	void initCloneFile( CloneFile & p_cloneFile);
	void SetProtected(void);
	void SetWordWrap(const bool bOn=true, const int iLineWidth=0);
	void SetColour(COLORREF color);
	void SyncScroll(UINT nScrollCode, int pos);
	void SyncVWScroll(UINT nFlags, short zDelta, CPoint pt);
	void KeySynchro(UINT nChar);
	void KeySynchro(UINT nChar, UINT nRepCnt, UINT nFlags);

	
	LRESULT OnCopy(WPARAM, LPARAM);

	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnViewClose32780();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnTokenViewClose();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnMouseHWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	//afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	LRESULT OnMenuResync(WPARAM, LPARAM);
};

class ClonePairsListView :public ClonePairsAsmView
{
	DECLARE_DYNCREATE(ClonePairsListView)

public:
	ClonePairsListView(void);
	virtual ~ClonePairsListView(void);

	void initClonePairList();

	virtual void OnInitialUpdate();

	DECLARE_MESSAGE_MAP()
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

protected:
	CString tarFile;
	CString tarContent;
	CString srcFile;
	CString srcContent;
};




// ClonePairsTreeView view

class ClonePairsTreeView : public CTreeView
{
	DECLARE_DYNCREATE(ClonePairsTreeView)

	typedef struct groupClonePair{
		CString itemString;
		int     clonePairsPos;
	} groupClonePair;

	typedef struct prevNextPair{
		HTREEITEM pHitem;
		int       prev;
	    int       next;
	} prevNextPair_t;

	typedef std::map<HTREEITEM,int>  hItemClonePairsRefMap_t;
	typedef std::map<int, prevNextPair_t>  prevNextPairMap_t;

protected:
	ClonePairsTreeView();           // protected constructor used by dynamic creation
	virtual ~ClonePairsTreeView();

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

	ClonePairsAsmDoc* GetDocument() const;
	void ReSyncScroll(ClonePairsAsmFrame* pFrame);

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	void Next();
	void Previous();

private:
	hItemClonePairsRefMap_t   m_clonePairsLineMap;
	HTREEITEM                 m_prevHItem;
	prevNextPairMap_t         m_prevNextPairsMap;   

public:
	afx_msg void OnTvnItemChanged(NMHDR *pNMHDR, LRESULT *pResult);
	void selectedLine(unsigned int p_line);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};



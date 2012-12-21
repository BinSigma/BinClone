// NewDetectDialog.cpp : implementation file
//

#include "stdafx.h"
#include "CloneDetectorGUI.h"
#include "NewDetectDialog.h"
#include "afxdialogex.h"

static TCHAR BASED_CODE szFilter[] = _T("XML Files (*.xml)|*.XML|")
                                     _T("All Files (*.*)|*.*||");
// NewDetectDialog dialog

static CString g_asmFilePath(_T(""));
static CString g_xmlFileName(_T(""));
static CString g_windSize(_T("40"));
static CString g_stride(_T("1"));
static CString g_maxKOP(_T("40"));
static CString g_maxOVF(_T("0.5"));
static BOOL    g_bFindExactClonesChk(TRUE);
static BOOL    g_bFindInexactClonesChk(FALSE);
static int     g_regNormalizedLevel(0);
static BOOL    g_bNormalizeTokenChk(TRUE);
static CString g_keyVectorsSize(_T("5"));
static CString g_occurrenceThrs(_T("0.8"));
static int     g_inexactMethodLevel(0); 

IMPLEMENT_DYNAMIC(NewDetectDialog, CDialogEx)

NewDetectDialog::NewDetectDialog(bool p_searchCode, CWnd* pParent /*=NULL*/)
	: CDialogEx(NewDetectDialog::IDD, pParent), 
	  m_withSearchCode(p_searchCode),
      m_windSize(g_windSize), 
      m_stride(g_stride), 
      m_maxKOP(g_maxKOP), 
      m_maxOVF(g_maxOVF), 
      m_regNormalizedLevel(g_regNormalizedLevel), 
      m_bFindExactClonesChk(g_bFindExactClonesChk), 
      m_bFindInexactClonesChk(g_bFindInexactClonesChk), 
      m_bNormalizeTokenChk(g_bNormalizeTokenChk),
      m_xmlFileName(g_xmlFileName), 
      m_keyVectorsSize(g_keyVectorsSize), 
      m_occurrenceThrs(g_occurrenceThrs), 
      m_inexactMethodLevel(g_inexactMethodLevel), 
	  m_searchCodeFragString(_T("")),
	  m_keepTempFileChk(FALSE),
	  m_ok(false)
{
	if (g_asmFilePath.GetLength() <= 0) {
	    TCHAR temp[MAX_PATH]; 
        GetEnvironmentVariable(_T("USERPROFILE"), temp, MAX_PATH);
        m_asmFilePath = temp;
		g_asmFilePath = temp;
	}
}

NewDetectDialog::~NewDetectDialog()
{
}

void NewDetectDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_WINDSZIE_EDIT, m_windSize);
	DDX_Text(pDX, IDC_STRIDE_EDIT, m_stride);
	DDX_Text(pDX, IDC_MAXKOP_EDIT, m_maxKOP);
	DDX_Text(pDX, IDC_MAXOVF_EDIT, m_maxOVF);
	DDX_Check(pDX, IDC_CHKEXACT, m_bFindExactClonesChk);
	DDX_Check(pDX, IDC_CHKINEXACT, m_bFindInexactClonesChk);
	DDX_Check(pDX, IDC_CHKNORMTOK, m_bNormalizeTokenChk);
    DDX_Check(pDX, IDC_KEEP_TEMP_FILE_CHECK, m_keepTempFileChk);
	DDX_Text(pDX, IDC_EDIT_KEYVECTSIZE, m_keyVectorsSize);
	DDX_Text(pDX, IDC_EDIT_OCCTHRS, m_occurrenceThrs);

	DDX_Control(pDX, IDC_WINDSZIE_EDIT, m_winSizeCtrl);
	DDX_Control(pDX, IDC_STRIDE_EDIT, m_strideCtrl);
	DDX_Control(pDX, IDC_MAXKOP_EDIT, m_maxkCtrl);
	DDX_Control(pDX, IDC_MAXOVF_EDIT, m_maxovfCtrl);
	DDX_Control(pDX, IDC_REGNLVL_COMBO, m_regNormLvl);
    DDX_Control(pDX, IDC_INEXACTMTLVL_COMBO, m_inexactMTD); 
	DDX_Control(pDX, IDC_CHKEXACT, m_exactCtrl);
	DDX_Control(pDX, IDC_CHKINEXACT, m_inexactCtrl);
	DDX_Control(pDX, IDC_CHKNORMTOK, m_normTokCtrl);    
	DDX_Control(pDX, IDC_EDIT_KEYVECTSIZE, m_keyvectsizeCtrl);
	DDX_Control(pDX, IDC_EDIT_OCCTHRS, m_occthrsCtrl);
	DDX_Control(pDX, IDC_RICHEDIT_SEARCH_CODE, m_editSearchCodeFrag);
	DDX_Control(pDX, IDC_EDIT_PATH_SEL, m_asmFilePathCtrl);
	DDX_Control(pDX, IDC_EDIT_XML_FILE, m_clonePairXmlFileCtrl);
	DDX_Control(pDX, IDC_KEEP_TEMP_FILE_CHECK, m_keepTempFileChkCtrl);
	DDX_Check(pDX, IDC_KEEP_TEMP_FILE_CHECK, m_keepTempFileChk);
}


BEGIN_MESSAGE_MAP(NewDetectDialog, CDialogEx)
	ON_BN_CLICKED(IDOK, &NewDetectDialog::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_PATH_SEL, &NewDetectDialog::OnBnClickedButtonPathSel)
	ON_BN_CLICKED(IDC_BUTTON_XML_FILE, &NewDetectDialog::OnBnClickedButtonXmlFile)
	ON_BN_CLICKED(IDOK2, &NewDetectDialog::OnBnClickedOk2)
	ON_BN_CLICKED(IDCANCEL2, &NewDetectDialog::OnBnClickedCancel2)
	ON_MESSAGE(ID_MENU_COPY,OnCopy)
	ON_MESSAGE(ID_MENU_PASTE,OnPaste)
    ON_CBN_SELCHANGE(IDC_REGNLVL_COMBO, &NewDetectDialog::OnCbnSelchangeRegnlvlCombo)
    ON_STN_CLICKED(IDC_STATIC_KEY_VECT_SIZE, &NewDetectDialog::OnStnClickedStaticKeyVectSize)
    ON_EN_CHANGE(IDC_EDIT_KEYVECTSIZE, &NewDetectDialog::OnEnChangeEditKeyvectsize)
END_MESSAGE_MAP()


// NewDetectDialog message handlers

void NewDetectDialog::OnBnClickedBexactTrue()
{
	// TODO: Add your control notification handler code here
	if (!UpdateData())
		return;

}


void NewDetectDialog::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	// validate the inputs before returning

    UpdateData(TRUE);

	CBFStrHelper strHelp;
	m_asmFilePath.Empty();
	int len = m_asmFilePathCtrl.LineLength(m_asmFilePathCtrl.LineIndex(0));
	m_asmFilePathCtrl.GetLine(0,m_asmFilePath.GetBuffer(len),len);
	m_asmFilePath.ReleaseBuffer(len);
	if( m_asmFilePath.GetLength() == 0)
	{
		AfxMessageBox(_T("Assembly files path cannot be empty!"));
		return;
	}
	g_asmFilePath = m_asmFilePath;

	m_xmlFileName.Empty();
	len = m_clonePairXmlFileCtrl.LineLength(m_clonePairXmlFileCtrl.LineIndex(0));
	m_clonePairXmlFileCtrl.GetLine(0,m_xmlFileName.GetBuffer(len),len);
	m_xmlFileName.ReleaseBuffer(len);
	if( m_xmlFileName.GetLength() == 0)
	{
		AfxMessageBox(_T("The XML file name cannot be empty!"));
		return;
	}
	g_xmlFileName = m_xmlFileName;

	if(m_windSize.GetLength() == 0)
	{
		AfxMessageBox(_T("Window size cannot be empty."));
		return;
	}
	else
	{
		int tmp = strHelp.strToInt(m_windSize);
		if (tmp < 1) {
		    AfxMessageBox(_T("Window size must be positive integer."));
		    return;
		}
	}
	g_windSize = m_windSize;


	if( m_stride.GetLength() == 0)
	{
		AfxMessageBox(_T("Step size cannot be empty."));
		return;
	}
	else
	{
		int tmp = strHelp.strToInt(m_stride);
        int tmpWndSze = strHelp.strToInt(m_windSize);
		if (tmp < 1 || tmp > tmpWndSze) {
		    AfxMessageBox(_T("Step size must be positive integer and smaller than the window size."));
		    return;
		}
	}
	g_stride = m_stride;


	if( m_maxKOP.GetLength() == 0)
	{
		AfxMessageBox(_T("Feature Extraction Threshold cannot be empty!"));
		return;
	}
	else
	{
		int tmp = strHelp.strToInt(m_maxKOP);
		if (tmp < 1)
		{
		    AfxMessageBox(_T("The maximum number of features must be positive interger."));
		    return;
		}
	}
	g_maxKOP = m_maxKOP;

	if( (m_maxOVF.GetLength() == 0) ||
		!strHelp.isNumeric(m_maxOVF))
	{
		AfxMessageBox(_T("maxOverlapFraction is not valid!"));
		return;
	}
	else
	{
		double tmp = strHelp.strToFloat(m_maxOVF);
		if((tmp < 0.1) || (tmp > 1) )
		{
		    AfxMessageBox(_T("The 1-norm Distance Threshold must be 0.1 - 1.0 "));
		    return;
		}
	}
	g_maxOVF = m_maxOVF;

	m_regNormalizedLevel = m_regNormLvl.GetCurSel();
	if ((m_regNormalizedLevel < CD_NORM_REG_ROOT) || (m_regNormalizedLevel > CD_NORM_REG_BITS))
	{
		AfxMessageBox(_T("RegNormalizeLevel is invalid!"));
		return;
	}
	g_regNormalizedLevel = m_regNormalizedLevel;

   
    m_inexactMethodLevel = m_inexactMTD.GetCurSel();  
    if (m_bFindInexactClonesChk && (m_inexactMethodLevel < CD_INEXACT_METHOD_RANDOM) || (m_inexactMethodLevel > CD_INEXACT_METHOD_TWOCOMBINATION))
	{
		AfxMessageBox(_T("inexactMethodLevel is invalid!"));
		return;
	}
    g_inexactMethodLevel = m_inexactMethodLevel;

	g_bFindExactClonesChk = m_bFindExactClonesChk;
	g_bFindInexactClonesChk = m_bFindInexactClonesChk;
    g_bNormalizeTokenChk = m_bNormalizeTokenChk;

	if( m_keyVectorsSize.GetLength() <= 0) 
	{
		AfxMessageBox(_T("Key vectors size cannot be empty."));
		return;
	}
	else
	{
		int tmp = strHelp.strToInt(m_keyVectorsSize);
		if (tmp < 2 || tmp > 10)
		{
		    AfxMessageBox(_T("Key vectors size must be 2 - 10."));
		    return;
		}
	}	
	g_keyVectorsSize = m_keyVectorsSize;
	
	if (m_occurrenceThrs.GetLength() <= 0 || !strHelp.isNumeric(m_occurrenceThrs))
	{
		AfxMessageBox(_T("Minimum co-occurrence threshold is invalid."));
		return;
	}
	else
	{
		double tmp = strHelp.strToFloat(m_occurrenceThrs);
		if (tmp < 0.1 || tmp > 1)
		{
		    AfxMessageBox(_T("Minimum co-occurrence threshold must be between 0.1 and 1."));
		    return;
		}
	}
	g_occurrenceThrs = m_occurrenceThrs;	

	if( m_withSearchCode)
	{
		if( m_editSearchCodeFrag.GetTextLengthEx(GTL_DEFAULT) > 0)
		{
			// extract and append each line to the output CString
			CString strLine(_T(""));
			m_searchCodeFragString.Empty();
			int nLineLength, nLineIndex, nLineCount = m_editSearchCodeFrag.GetLineCount();
			for (int i = 0; i < nLineCount; i++)
			{
				CString strText(_T(""));
				nLineIndex = m_editSearchCodeFrag.LineIndex(i);
				nLineLength = m_editSearchCodeFrag.LineLength(nLineIndex);
				int tmp = nLineLength;
				if(nLineLength < 4) tmp = 4;  // need to allocate minimum 4 bytes for GetLine to work!!!
				                              // Microsoft claims that this is not a bug !!!
				m_editSearchCodeFrag.GetLine(i, strText.GetBufferSetLength(tmp+1),tmp);
				strText.SetAt(nLineLength, _T('\0')); // null terminate
				strText.ReleaseBuffer(tmp + 1);
                m_searchCodeFragString += strText;

				// do not add the new line if this is the last line
				//if((i+1) < nLineCount) 
					m_searchCodeFragString += _T("\n");
			}
		}
		else
		{
		    AfxMessageBox(_T("Please enter a code fragment."));
		    return;
		}
	}

	CDialogEx::OnOK();
	
	m_ok = true;
	
}


BOOL NewDetectDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	
	m_asmFilePathCtrl.SetWindowTextW(g_asmFilePath);
	UpdateData(FALSE);

	m_clonePairXmlFileCtrl.SetWindowTextW(g_xmlFileName);
	UpdateData(FALSE);

	CRect rcWnd, rcDefaultBox;
	CWnd * wndDefaultBox=NULL;
	if( !m_withSearchCode)
	{
	   	wndDefaultBox = GetDlgItem(IDC_DEFAULT_WIND);
		if (wndDefaultBox==NULL) return FALSE;

		// make the other ctrls invisible
		CWnd * wndStaticCodeFrag = GetDlgItem(IDC_STATIC_SREACH_CODE);
	    CWnd * wndOK2            = GetDlgItem(IDOK2);
		CWnd * wndCANCEL2        = GetDlgItem(IDCANCEL2);

		wndStaticCodeFrag->ShowWindow(SW_HIDE);
		m_editSearchCodeFrag.ShowWindow(SW_HIDE);
		wndOK2->ShowWindow(SW_HIDE);
		wndCANCEL2->ShowWindow(SW_HIDE);

		wndDefaultBox->GetWindowRect(&rcDefaultBox);

		GetWindowRect(&rcWnd);

		SetWindowPos(NULL,0,0,
			rcDefaultBox.right - rcWnd.left, 
			rcDefaultBox.bottom - rcWnd.top,
			SWP_NOZORDER|SWP_NOMOVE);
	}
	else
	{
		CString text; 
		text.LoadString(IDS_STRING_CODE_FRAGEMENT);
		SetDlgItemTextW(IDC_STATIC_SREACH_CODE,text);
	    CWnd * wndOK     = GetDlgItem(IDOK);
		CWnd * wndCANCEL = GetDlgItem(IDCANCEL);
		wndOK->ShowWindow(SW_HIDE);
		wndCANCEL->ShowWindow(SW_HIDE);
	}

	CString text; 
    text.LoadString( IDS_STRING_WIND_SIZE );
	SetDlgItemTextW( IDC_STATIC_WIND_SIZE,text);
	text.LoadString( IDS_STRING_STEP_SIZE );
	SetDlgItemTextW(IDC_STATIC_STEP_SIZE,text);

	if( !m_ToolTip.Create(this))
	{
	    AfxMessageBox(_T("Unable to create the ToolTip!"));
	}
	else
	{
	    m_ToolTip.AddTool(&m_winSizeCtrl, _T("Size of sliding window"));
		m_ToolTip.AddTool(&m_strideCtrl, _T("Step size of sliding window"));
		m_ToolTip.AddTool(&m_maxkCtrl, _T("Maximum number of features for inexact clone detection"));
		m_ToolTip.AddTool(&m_maxovfCtrl, _T("Maximum fraction of overlapping between two consecutive clones"));
		m_ToolTip.AddTool(&m_exactCtrl, _T("Identify exact clones?"));
		m_ToolTip.AddTool(&m_inexactCtrl, _T("Identify inexact clones?"));
        m_ToolTip.AddTool(&m_normTokCtrl, _T("Normalize all tokens to types?"));
		m_ToolTip.AddTool(&m_1stsegperCtrl, _T("Fraction of important features for inexct clone detection to generate key vectors"));
		m_ToolTip.AddTool(&m_keyvectsizeCtrl, _T("Maximum length of each key vector to be mapped to inexact hash tables"));
		m_ToolTip.AddTool(&m_occthrsCtrl, _T("Fraction of the number of hash tables which pairs are grouped together"));
	    m_ToolTip.Activate(TRUE);
	}

	// TODO:  Add extra initialization here
	m_regNormLvl.AddString(_T("CD_NORM_REG_ROOT"));
	m_regNormLvl.AddString(_T("CD_NORM_REG_TYPE"));
	m_regNormLvl.AddString(_T("CD_NORM_REG_IDXPTR"));
	m_regNormLvl.AddString(_T("CD_NORM_REG_BITS"));
	m_regNormLvl.SetCurSel(g_regNormalizedLevel);
	m_regNormalizedLevel = 0;

    m_inexactMTD.AddString(_T("RANDOM"));
	m_inexactMTD.AddString(_T("SLIDING"));
	m_inexactMTD.AddString(_T("TWO COMBINATION"));
    m_regNormLvl.SetCurSel(g_inexactMethodLevel);
	m_inexactMethodLevel = 0;


	if( m_withSearchCode)
	{
		m_editSearchCodeFrag.SetEventMask(ENM_MOUSEEVENTS);
		m_editSearchCodeFrag.SetSel(0,0);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void NewDetectDialog::OnBnClickedButtonPathSel()
{
	// TODO: Add your control notification handler code here
	//m_asmFilePath.Empty();
	CString tmpPath(m_asmFilePath);
	int len = m_asmFilePathCtrl.LineLength(m_asmFilePathCtrl.LineIndex(0));
	if( len > 0)
	{
		m_asmFilePathCtrl.GetLine(0,tmpPath.GetBuffer(len),len);
	    tmpPath.ReleaseBuffer(len);
	}

	CFolderPickerDialog dlg(tmpPath, 0, NULL, 0);
	if (dlg.DoModal())
	{
		m_asmFilePath = dlg.GetPathName();//  GetFolderPath();
		m_asmFilePathCtrl.SetWindowTextW(m_asmFilePath);
        UpdateData(FALSE);
	} 
}


void NewDetectDialog::OnBnClickedButtonXmlFile()
{
	// TODO: Add your control notification handler code here
	CFileDialog dlg( FALSE,NULL,NULL,
		             0, //OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
					 szFilter,
					 NULL);
	if (dlg.DoModal())
	{
		m_xmlFileName = dlg.GetPathName();//  GetFolderPath();
		m_clonePairXmlFileCtrl.SetWindowTextW(m_xmlFileName);
        UpdateData(FALSE);
	} 
}


BOOL NewDetectDialog::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
    m_ToolTip.RelayEvent(pMsg);
	return CDialogEx::PreTranslateMessage(pMsg);
}


void NewDetectDialog::OnBnClickedOk2()
{
	// TODO: Add your control notification handler code here
	OnBnClickedOk();
}


void NewDetectDialog::OnBnClickedCancel2()
{
	// TODO: Add your control notification handler code here
	CDialogEx::OnCancel();
}

BOOL NewDetectDialog::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	// TODO: Add your specialized code here and/or call the base class
	MSGFILTER * lpMsgFilter = (MSGFILTER *)lParam; 

	if ((wParam == IDC_RICHEDIT_SEARCH_CODE) && 
		(lpMsgFilter->nmhdr.code == EN_MSGFILTER) &&  
		(lpMsgFilter->msg == WM_RBUTTONDOWN))                                       
	{
		//if we get through here, we have trapped the right click event of the richeditctrl! 
		CPoint point;	                                        
  		::GetCursorPos(&point); //where is the mouse?
 		CMenu menu; //lets display out context menu :) 
 		UINT dwSelectionMade;                                       
		VERIFY(menu.LoadMenu(IDR_MOUSE_MENU) );  
  		CMenu *pmenuPopup = menu.GetSubMenu(0);
  		ASSERT(pmenuPopup != NULL);                                       
  		dwSelectionMade = pmenuPopup->TrackPopupMenu((TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_NONOTIFY|TPM_RETURNCMD),
  								                     point.x, 
													 point.y, 
													 this);                                
 		pmenuPopup->DestroyMenu(); 
		PostMessage(dwSelectionMade,0,0);
 	}

	return CDialogEx::OnNotify(wParam, lParam, pResult);
}

LRESULT NewDetectDialog::OnCopy(WPARAM, LPARAM)
{
	m_editSearchCodeFrag.Copy();
    return 1;
}

LRESULT NewDetectDialog::OnPaste(WPARAM, LPARAM)
{
	m_editSearchCodeFrag.Paste();
    return 1;
}



void NewDetectDialog::OnCbnSelchangeRegnlvlCombo()
{
    // TODO: Add your control notification handler code here
}


void NewDetectDialog::OnStnClickedStaticKeyVectSize()
{
    // TODO: Add your control notification handler code here
}


void NewDetectDialog::OnEnChangeEditKeyvectsize()
{
    // TODO:  If this is a RICHEDIT control, the control will not
    // send this notification unless you override the CDialogEx::OnInitDialog()
    // function and call CRichEditCtrl().SetEventMask()
    // with the ENM_CHANGE flag ORed into the mask.

    // TODO:  Add your control notification handler code here
}

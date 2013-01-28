// NewDetectDialog.cpp : implementation file
//

#include "stdafx.h"
#include "CloneDetectorGUI.h"
#include "NewDetectDialog.h"
#include "afxdialogex.h"
#include "CSController.h"
#include "ParamsListDialog.h"

static TCHAR BASED_CODE szFilter[] = _T("XML Files (*.asm)|*.ASM|")
                                     _T("All Files (*.*)|*.*||");
// NewDetectDialog dialog

static CString g_asmFilePath(_T(""));
static CString g_targetAsmFile(_T(""));
static BOOL    g_bFindExactClonesChk(TRUE);
static BOOL    g_bFindInexactClonesChk(FALSE);
static int     g_regNormalizedLevel(0);
static BOOL    g_bNormalizeTokenChk(TRUE);
static CString g_occurrenceThrs(_T("0.8"));
static int     g_inexactMethodLevel(0);
static int     g_dbParamID(-1);
static int     g_windSize(-1);
static int     g_stride(-1);
static int     g_regNormLvl(-1);

CString g_dbName(_T("CSDataBase"));
CString g_dbUser(_T("postgres"));
CString g_dbPwd(_T(""));

CString REG_NORM_LVL[4] = { _T("CD_NORM_REG_ROOT"), 
                            _T("CD_NORM_REG_TYPE"), 
							_T("CD_NORM_REG_IDXPTR"), 
							_T("CD_NORM_REG_BITS") };


IMPLEMENT_DYNAMIC(NewDetectDialog, CDialogEx)

NewDetectDialog::NewDetectDialog(bool p_searchCode, CWnd* pParent /*=NULL*/)
	 : CDialogEx(NewDetectDialog::IDD, pParent)	 
	 , m_withSearchCode(p_searchCode)
	 , m_ok(false)
     , m_bFindExactClonesChk(g_bFindExactClonesChk) 
     , m_bFindInexactClonesChk(g_bFindInexactClonesChk) 
     , m_targetAsmFile(g_targetAsmFile) 
     , m_occurrenceThrs(g_occurrenceThrs) 
     , m_inexactMethodLevel(g_inexactMethodLevel) 
	 , m_db_param_id(g_dbParamID)
	 , m_searchCodeFragString(_T(""))
	 , m_keepTempFileChk(FALSE)
	 , m_dbName(g_dbName)
	 , m_dbUser(g_dbUser)
	 , m_dbPwd(g_dbPwd)
	 , m_paramsId(-1)
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
	DDX_Check(pDX, IDC_CHKEXACT, m_bFindExactClonesChk);
	DDX_Check(pDX, IDC_CHKINEXACT, m_bFindInexactClonesChk);
	DDX_Check(pDX, IDC_KEEP_TEMP_FILE_CHECK, m_keepTempFileChk);
	DDX_Text(pDX, IDC_EDIT_OCCTHRS, m_occurrenceThrs); 
	DDX_Control(pDX, IDC_RICHEDIT_SEARCH_CODE, m_editSearchCodeFrag);
	DDX_Control(pDX, IDC_EDIT_ASM_FILE, m_targetAsmFileCtrl);
	DDX_Control(pDX, IDC_KEEP_TEMP_FILE_CHECK, m_keepTempFileChkCtrl);
	DDX_Text(pDX, IDC_DB_PWD, m_dbPwd);
	DDX_Text(pDX, IDC_DB_NAME, m_dbName);
	DDX_Text(pDX, IDC_DB_USER, m_dbUser);
}


BEGIN_MESSAGE_MAP(NewDetectDialog, CDialogEx)
	ON_BN_CLICKED(IDOK, &NewDetectDialog::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_XML_FILE, &NewDetectDialog::OnBnClickedButtonXmlFile)
	ON_BN_CLICKED(IDOK2, &NewDetectDialog::OnBnClickedOk2)
	ON_BN_CLICKED(IDCANCEL2, &NewDetectDialog::OnBnClickedCancel2)
	ON_MESSAGE(ID_MENU_COPY,OnCopy)
	ON_MESSAGE(ID_MENU_PASTE,OnPaste)
	ON_BN_CLICKED(IDCANCEL, &NewDetectDialog::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_CHKINEXACT, &NewDetectDialog::OnBnClickedChkinexact)
ON_BN_CLICKED(IDC_BUTTON_GET_PARAMS, &NewDetectDialog::OnBnClickedButtonGetParams)
END_MESSAGE_MAP()


// NewDetectDialog message handlers

void NewDetectDialog::updateParametersStaticText(int windSize, int stride, int regNormsLvl)
{
	CString WinSizeStr(_T("Window Size : "));
	WinSizeStr.Format(WinSizeStr + "%d", windSize);
	CString StrideStr(_T("Stride : "));
	StrideStr.Format(StrideStr + "%d", stride);
	CString RegNormLvlStr(_T("Register Normalization Level : "));
	if( regNormsLvl >= 0 && regNormsLvl < 4)
	{
		RegNormLvlStr.Format(RegNormLvlStr+ "%s", REG_NORM_LVL[regNormsLvl]);
	}
	CWnd * staticWinSizeTxt = GetDlgItem(IDC_WIND_SIZE);
	staticWinSizeTxt->SetWindowTextW(WinSizeStr);

	CWnd * staticStrideTxt = GetDlgItem(IDC_STRIDE);
	staticStrideTxt->SetWindowTextW(StrideStr);

    CWnd * staticRegNormLvlTxt = GetDlgItem(IDC_NORM_LEVEL);
	staticRegNormLvlTxt->SetWindowTextW(RegNormLvlStr);

}

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
	
	if( !m_withSearchCode)
	{
		m_targetAsmFile.Empty();
		int len = m_targetAsmFileCtrl.LineLength(m_targetAsmFileCtrl.LineIndex(0));
		m_targetAsmFileCtrl.GetLine(0,m_targetAsmFile.GetBuffer(len),len);
		m_targetAsmFile.ReleaseBuffer(len);
		if( m_targetAsmFile.GetLength() == 0)
		{
			AfxMessageBox(_T("The Target assembly file name cannot be empty!"));
			return;
		}
		g_targetAsmFile = m_targetAsmFile;
	}

    g_inexactMethodLevel = m_inexactMethodLevel;

	g_bFindExactClonesChk = m_bFindExactClonesChk;
	g_bFindInexactClonesChk = m_bFindInexactClonesChk;
	
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

	g_dbParamID = m_db_param_id;

	if (m_dbName.GetLength() <= 0)
	{
		AfxMessageBox(_T("Database name is invalid."));
		return;
	}
	g_dbName = m_dbName;

	if (m_dbUser.GetLength() <= 0)
	{
		AfxMessageBox(_T("Database user is invalid."));
		return;
	}
	g_dbUser = m_dbUser;

	if (m_dbPwd.GetLength() <= 0)
	{
		AfxMessageBox(_T("Database user password is invalid."));
		return;
	}
	g_dbPwd = m_dbPwd;

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
	
	UpdateData(FALSE);

	m_targetAsmFileCtrl.SetWindowTextW(m_targetAsmFile);
	UpdateData(FALSE);

	updateParametersStaticText(g_windSize,g_stride,g_regNormLvl);

	CWnd * staticFileTxt = GetDlgItem(IDC_STATIC_FILES);
	staticFileTxt->SetWindowTextW(_T("File"));

	CWnd * staticTarAsmFileTxt = GetDlgItem(IDC_STATIC_TAR);
	staticTarAsmFileTxt->SetWindowTextW(_T("Target assembly file"));

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

		CWnd * wndStaticFiles = GetDlgItem(IDC_STATIC_FILES);
		wndStaticFiles->ShowWindow(SW_HIDE);
		CWnd * wndStaticTar = GetDlgItem(IDC_STATIC_TAR);
		wndStaticTar->ShowWindow(SW_HIDE);

		CWnd * wndEditAsmFile = GetDlgItem(IDC_EDIT_ASM_FILE);
		wndEditAsmFile->ShowWindow(SW_HIDE);

		CWnd * wndButXMLFile = GetDlgItem(IDC_BUTTON_XML_FILE);
		wndButXMLFile->ShowWindow(SW_HIDE);
		 

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
		/*
	    m_ToolTip.AddTool(&m_winSizeCtrl, _T("Size of sliding window"));
		m_ToolTip.AddTool(&m_strideCtrl, _T("Step size of sliding window"));
		m_ToolTip.AddTool(&m_maxkCtrl, _T("Maximum number of features for inexact clone detection"));
		m_ToolTip.AddTool(&m_maxovfCtrl, _T("Maximum fraction of overlapping between two consecutive clones"));
        m_ToolTip.AddTool(&m_normTokCtrl, _T("Normalize all tokens to types?"));
		m_ToolTip.AddTool(&m_1stsegperCtrl, _T("Fraction of important features for inexct clone detection to generate key vectors"));
		m_ToolTip.AddTool(&m_keyvectsizeCtrl, _T("Maximum length of each key vector to be mapped to inexact hash tables"));
		m_ToolTip.AddTool(&m_occthrsCtrl, _T("Fraction of the number of hash tables which pairs are grouped together"));
		*/
	    m_ToolTip.Activate(TRUE);
	}

	// TODO:  Add extra initialization here
/*
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
*/
	m_inexactMethodLevel = 0;


	if( m_withSearchCode)
	{
		m_editSearchCodeFrag.SetEventMask(ENM_MOUSEEVENTS);
		m_editSearchCodeFrag.SetSel(0,0);
	}

	CWnd *  occThrs = GetDlgItem(IDC_EDIT_OCCTHRS);
	occThrs->EnableWindow(m_bFindInexactClonesChk);


	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void NewDetectDialog::OnBnClickedButtonXmlFile()
{
	// TODO: Add your control notification handler code here
	CFileDialog dlg( TRUE,NULL,NULL,
		             0, //OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
					 szFilter,
					 NULL);
	if (dlg.DoModal())
	{
		m_targetAsmFile = dlg.GetPathName();//  GetFolderPath();
		m_targetAsmFileCtrl.SetWindowTextW(m_targetAsmFile);
        UpdateData(TRUE);
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


void NewDetectDialog::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	CDialogEx::OnCancel();
}


void NewDetectDialog::OnBnClickedChkinexact()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	CWnd *  occThrs = GetDlgItem(IDC_EDIT_OCCTHRS);
	occThrs->EnableWindow(m_bFindInexactClonesChk);
}


void NewDetectDialog::OnBnClickedButtonGetParams()
{
	UpdateData(TRUE);

	// TODO: Add your control notification handler code here
	if (m_dbName.GetLength() <= 0)
	{
		AfxMessageBox(_T("Database name is invalid."));
		return;
	}

	if (m_dbUser.GetLength() <= 0)
	{
		AfxMessageBox(_T("Database user is invalid."));
		return;
	}

	if (m_dbPwd.GetLength() <= 0)
	{
		AfxMessageBox(_T("Database user password is invalid."));
		return;
	}

	ParamsListDialog paramsListDialog;
	paramsListDialog.init(m_dbName,m_dbUser,m_dbPwd);
	paramsListDialog.DoModal();

	if(paramsListDialog.m_result)
	{
		m_db_param_id = paramsListDialog.m_paramId;
		g_windSize = paramsListDialog.m_selWndSize;
		g_stride = paramsListDialog.m_selStride;
		g_regNormLvl = paramsListDialog.m_selRegNormLvl;

		updateParametersStaticText(paramsListDialog.m_selWndSize,
			                       paramsListDialog.m_selStride,
								   paramsListDialog.m_selRegNormLvl);
	}
	/*
	else
	{
		AfxMessageBox(_T("Failed to get the parameters."));
	}
	*/

}

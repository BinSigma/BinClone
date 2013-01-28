// ParamsListDialog.cpp : implementation file
//

#include "stdafx.h"
#include "CloneDetectorGUI.h"
#include "ParamsListDialog.h"
#include "afxdialogex.h"


// ParamsListDialog dialog

IMPLEMENT_DYNAMIC(ParamsListDialog, CDialogEx)

extern CString REG_NORM_LVL[4];


ParamsListDialog::ParamsListDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(ParamsListDialog::IDD, pParent)
	, m_paramId(-1)
	, m_selWndSize(-1)
	, m_selStride(-1)
	, m_selRegNormLvl(-1)
	, m_result(false)
{

}

ParamsListDialog::~ParamsListDialog()
{
	if(m_csController)
		delete m_csController;
}

void ParamsListDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_params_list_ctrl);
}


BEGIN_MESSAGE_MAP(ParamsListDialog, CDialogEx)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, &ParamsListDialog::OnLvnItemchangedList1)
	ON_NOTIFY(NM_CLICK, IDC_LIST1, &ParamsListDialog::OnClickList1)
	ON_BN_CLICKED(IDOK, &ParamsListDialog::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &ParamsListDialog::OnBnClickedCancel)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, &ParamsListDialog::OnNMDblclkList1)
END_MESSAGE_MAP()


// ParamsListDialog message handlers


BOOL ParamsListDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_params_list_ctrl.SetExtendedStyle( m_params_list_ctrl.GetExtendedStyle() | LVS_EX_TRACKSELECT | LVS_EX_ONECLICKACTIVATE | LVS_EX_FULLROWSELECT);

	// TODO:  Add extra initialization here
	m_csController = new CCSController( m_dbName, m_dbUser, m_dbPwd);
	if (!m_csController->fetchAllParameters(m_params)) 
	{
		AfxMessageBox(_T("Failed to fetech parameters"), MB_ICONSTOP, 0);					
		//ccsController->m_result = false;
		return FALSE;
    }

	// insert columns
	m_params_list_ctrl.InsertColumn(0,_T("Window Size"),LVCFMT_LEFT,100);
	m_params_list_ctrl.InsertColumn(1,_T("Stride"),LVCFMT_LEFT,100);
	m_params_list_ctrl.InsertColumn(2,_T("Register Normalization Level"),LVCFMT_LEFT,250);

	//return TRUE;

	int i = 0;
	for (; i < m_params.GetSize(); ++i)
	{
		int iRowNum;
		CString windSize;
		CBFStrHelper::intToStr(m_params.GetAt(i)->m_windowSize, windSize);
		iRowNum = m_params_list_ctrl.InsertItem(i,windSize);
		ASSERT(iRowNum == i);

		CString stride;
		CBFStrHelper::intToStr(m_params.GetAt(i)->m_stride, stride);
        m_params_list_ctrl.SetItemText(i, 1, stride );
		
		CString regNormLvl;
		int n = m_params.GetAt(i)->m_regNormLevel;
		if( (n < 4) && ( n >= 0))
		{
			 m_params_list_ctrl.SetItemText(i, 2, REG_NORM_LVL[n]);
		}
		else
		{
			AfxMessageBox(_T("RegNormalizeLevel is out of range."), MB_ICONSTOP, 0);
			return FALSE;
		}		
	}

	if( i==1)
	{
		m_params_list_ctrl.InsertItem(i, _T(""));
	}
	else if( i==0)
	{
		m_params_list_ctrl.InsertItem(0, _T(""));
		m_params_list_ctrl.InsertItem(1, _T(""));
	}

	UpdateData(TRUE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void ParamsListDialog::OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void ParamsListDialog::updateParameters()

{
    POSITION pos = m_params_list_ctrl.GetFirstSelectedItemPosition();
	int nItem = m_params_list_ctrl.GetNextSelectedItem(pos);
	if( nItem >= 0 )
	{
		CString windSize;
		CBFStrHelper::intToStr(m_params.GetAt(nItem)->m_windowSize, windSize);
		m_paramId = m_params.GetAt(nItem)->m_dbParamID;
		m_selWndSize = m_params.GetAt(nItem)->m_windowSize;
		m_selStride = m_params.GetAt(nItem)->m_stride; 
		m_selRegNormLvl = m_params.GetAt(nItem)->m_regNormLevel;
		m_result = true;
	}
}


void ParamsListDialog::OnClickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: Add your control notification handler code here
	updateParameters();

	*pResult = 0;
}


void ParamsListDialog::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	CDialogEx::OnOK();
}


void ParamsListDialog::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	CDialogEx::OnCancel();
}


void ParamsListDialog::OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: Add your control notification handler code here
	POSITION pos = m_params_list_ctrl.GetFirstSelectedItemPosition();
	int nItem = m_params_list_ctrl.GetNextSelectedItem(pos);
	if( nItem >= 0)
	{
		updateParameters();
		*pResult = 0;
		OnBnClickedOk();
		return;
	}

	*pResult = 0;
}

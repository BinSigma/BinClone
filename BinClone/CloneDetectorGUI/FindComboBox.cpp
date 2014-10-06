// FindComboBox.cpp : implementation file
//

#include "stdafx.h"
#include "CloneDetectorGUI.h"
#include "FindComboBox.h"


// CFindComboBox

IMPLEMENT_DYNAMIC(CFindComboBox, CComboBox)

CFindComboBox::CFindComboBox()
{

}

CFindComboBox::~CFindComboBox()
{
}


BEGIN_MESSAGE_MAP(CFindComboBox, CComboBox)
END_MESSAGE_MAP()



// CFindComboBox message handlers




BOOL CFindComboBox::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class

	return CComboBox::PreTranslateMessage(pMsg);
}


// CFindComboButton

CFindComboButton::CFindComboButton()
{
}

CFindComboButton::~CFindComboButton()
{
}


// CFindComboButton member functions

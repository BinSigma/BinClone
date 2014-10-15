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

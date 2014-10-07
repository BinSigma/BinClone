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

// BFPch.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(BFPCH_H)
#define BFPCH_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afx.h>
#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#include <afxtempl.h>

#define _BFLIB_ENABLE_64BIT

#if !defined (TYPES_HPP)
    #include "types.hpp"
#endif

#if !defined(BFFILEHELPER_H)
    #include "BFFileHelper.h"
#endif

#if !defined(BFSTRHELPER_H)
	#include "BFStrHelper.h"
#endif

#if !defined(BFDATEHELPER_H)
	#include "BFDateHelper.h"
#endif

#if !defined(BFSTRPSER_H)
	#include "BFStrPser.h"
#endif

#if !defined(BFMULTIDIMARY_H)
    #include "BFMultiDimAry.h"
#endif

#if !defined(BFXMLPSER_H)
    #include "BFXmlPser.h"
#endif

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(BFPCH_H)

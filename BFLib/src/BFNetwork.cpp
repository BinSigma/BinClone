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

//---------------------------------------------------------------------------
// Module:
//      BFNetwork
//
// History:
//		Aug. 7, 2004		Created by Benjamin Fung
//---------------------------------------------------------------------------

#include "BFPch.h"

#if !defined(BFNETWORK_H)
	#include "BFNetwork.h"
#endif

CString getServerIP()
{
	char szHostName[256];
	gethostname(szHostName, strlen(szHostName));
    LPHOSTENT pHost = gethostbyname(szHostName);
    if (!pHost)
        return _T("");

	struct in_addr* ptr = (struct in_addr*)pHost->h_addr_list[0];
    if (!ptr)
        return _T("");

	int a = ptr->S_un.S_un_b.s_b1;
	int b = ptr->S_un.S_un_b.s_b2;
	int c = ptr->S_un.S_un_b.s_b3;
	int d = ptr->S_un.S_un_b.s_b4;
	
	CString strTmp;
	strTmp.Format(_T("%d.%d.%d.%d"),a,b,c,d);
	return strTmp;
}


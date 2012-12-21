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


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

#include "BFPch.h"

#if !defined(BFDATEHELPER_H)
	#include "BFDateHelper.h"
#endif


//--------------------------------------------------------------------
//--------------------------------------------------------------------
CBFDateHelper::CBFDateHelper(void)
{
}

CBFDateHelper::~CBFDateHelper(void)
{
}

//--------------------------------------------------------------------
// Convert "9/28/1948 16:40:23" into CTime.
// nDST
// Indicates whether daylight savings time is in effect. Can have one of three values:
//
// nDST set to 0   Standard time is in effect.
//
// nDST set to a value greater than 0   Daylight savings time is in effect.
//
// nDST set to a value less than 0   The default. Automatically computes whether standard time or daylight savings time is in effect.
//--------------------------------------------------------------------
bool CBFDateHelper::strToTime(LPCTSTR str, COleDateTime& res)
{
    return res.ParseDateTime(str);
    /*
    COleDateTime oleDateTime;
    if (!oleDateTime.ParseDateTime(str))
        return false;

    CTime tempTime(oleDateTime.GetYear(), 
                   oleDateTime.GetMonth(), 
                   oleDateTime.GetDay(),
                   oleDateTime.GetHour(),
                   oleDateTime.GetMinute(),
                   oleDateTime.GetSecond(), 
                   nDST);
    res = tempTime;
    return true;
    */
}

//--------------------------------------------------------------------
// Convert "16:40:23" into CTimeSpan.
//--------------------------------------------------------------------
bool CBFDateHelper::strToTimeSpan(LPCTSTR str, COleDateTimeSpan& res)
{
    COleDateTime oleDateTime;
    if (!oleDateTime.ParseDateTime(str))
        return false;

    res.SetDateTimeSpan(0, oleDateTime.GetHour(), oleDateTime.GetMinute(), oleDateTime.GetSecond());
    return true;
}
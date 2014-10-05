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

//-----------------------------------------------------
// Objective:   Model an indexed token
//
// History:     2012/10/12 - B. Fung (Created)
// 
//-----------------------------------------------------

#pragma once

class CCSIndexedToken
{
public:
    CCSIndexedToken(LPCTSTR tokStr, int dbFileID, int rawLineNum);
    virtual ~CCSIndexedToken();

    inline const CString& getTokStr() const { return m_tokStr; };
    inline int getDBfileID() { return m_dbFileID; };
    inline int getRawLineNum() { return m_rawLineNum; };

private:
    CString m_tokStr;
    int m_dbFileID;
    int m_rawLineNum;
};

typedef CTypedPtrArray<CPtrArray, CCSIndexedToken*> CCSIndexedTokenArray;
class CCSIndexedTokens : public CCSIndexedTokenArray
{
public:
    CCSIndexedTokens() {};
    virtual ~CCSIndexedTokens() {};
    bool addIndexedToken(CCSIndexedToken* pIndexedToken);
    void cleanup();
};

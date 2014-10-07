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

// BFXmlPser.h: interface for the XML parser classes
//
//////////////////////////////////////////////////////////////////////

#if !defined(BFXMLPSER_H)
#define BFXMLPSER_H

#include <objbase.h>
#include <msxml6.h>

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//--------------------------------------------------------------------
// XML node
//--------------------------------------------------------------------
class CBFXmlNode
{
friend class CBFXmlDoc;
friend class CBFXmlNodeList;
public:
	CBFXmlNode();
	virtual ~CBFXmlNode();
	BOOL isEmpty() {return !m_pNode;};

// operations    
    BOOL hasAttribute(LPCTSTR attrName);
    BOOL setAttribute(LPCTSTR attrName, LPCTSTR attrValue);
    BOOL setAttribute(LPCTSTR attrName, LONG attrValue);
    BOOL setAttribute(LPCTSTR attrName, FLOAT attrValue);
	BOOL getAttribute(LPCTSTR attrName, CString& value, BOOL bTextField = FALSE);
	BOOL getElementsByTagName(LPCTSTR tagName, CBFXmlNodeList* pNodeList);

    BOOL appendChild(CBFXmlNode* pChildNode);
    BOOL getChildNodes(CBFXmlNodeList* pNodeList);

	BOOL getNodeName(CString& nodeName);
    BOOL getText(CString& text);
    BOOL setText(LPCTSTR text);

protected:
	CComPtr<IXMLDOMNode> m_pNode;
};

//--------------------------------------------------------------------
// XML node list
//--------------------------------------------------------------------
class CBFXmlNodeList
{
friend class CBFXmlNode;
public:
	CBFXmlNodeList();
	virtual ~CBFXmlNodeList();

// operations
	BOOL getNextNode(CBFXmlNode* pNode);
	BOOL isEmpty();
    LONG length() const;
    BOOL getItem(LONG itemID, CBFXmlNode* pNode) const;

protected:
	CComPtr<IXMLDOMNodeList> m_pNodeList;
};

//--------------------------------------------------------------------
// XML document
//--------------------------------------------------------------------
class CBFXmlDoc  
{
public:
	CBFXmlDoc();
	virtual ~CBFXmlDoc();

// operations    
    BOOL loadXML(LPCTSTR xmlFile);
    BOOL saveXML(LPCTSTR filename);
    BOOL createXML(LPCTSTR newFilename, LPCTSTR rootTag);

	BOOL getDocumentNode(CBFXmlNode* pDocElem);    
    BOOL createElement(LPCTSTR nodeName, CBFXmlNode* pNewNode);
    BOOL createText(LPCTSTR nodeName, CBFXmlNode* pNewNode);

protected:	
    BOOL setPreserveWhiteSpace(BOOL bPreserve);

	CComPtr<IXMLDOMDocument> m_pDoc;
	CString m_xmlFile;
};

#endif // !defined(BXMLPSER_H)

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

//---------------------------------------------------------------------------
// File:
//      BFXmlPser.cpp BFXmlPser.hpp
//
// Module:
//      CBFXmlDoc
//
// Description:
//      Wrapper for MSXML
//
// History:
//		Mar. 12, 2002		Created by Benjamin Fung
//---------------------------------------------------------------------------

#include "BFPch.h"

#if !defined(BFXMLPSER_H)
	#include "BFXmlPser.h"
#endif

#define BF_XML_HEADER   _T("<?xml version=\"1.0\" ?>\n")
//#define BF_XML_HEADER   _T("<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n")
//#define BF_XML_HEADER   _T("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n")
#define BF_XML_NUM_DECIMALS     6

//--------------------------------------------------------------------
// XML node
//--------------------------------------------------------------------
CBFXmlNode::CBFXmlNode()
{
}

CBFXmlNode::~CBFXmlNode()
{
}

BOOL CBFXmlNode::appendChild(CBFXmlNode* pChildNode)
{
    if (!m_pNode || !pChildNode) {
        ASSERT(FALSE);
        return FALSE;
    }

    CBFXmlNode newNode;
    HRESULT hr = m_pNode->appendChild(pChildNode->m_pNode, &newNode.m_pNode);
    if (FAILED(hr)) {
        ASSERT(FALSE);
        return FALSE;
    }
    return TRUE;
}

BOOL CBFXmlNode::hasAttribute(LPCTSTR attrName)
{
	if (!m_pNode || StrCmp(attrName, _T("")) == 0)
		return FALSE;

    CComPtr<IXMLDOMAttribute> pAttr = NULL;
    CComQIPtr<IXMLDOMElement> pElm = m_pNode;
    HRESULT hr = pElm->getAttributeNode(CComBSTR(attrName), &pAttr);
    if (FAILED(hr)) {
        ASSERT(FALSE);
        return FALSE;
    }
    return pAttr != NULL;
}

BOOL CBFXmlNode::setAttribute(LPCTSTR attrName, LPCTSTR attrValue)
{
    if (StrCmp(attrName, _T("")) == 0) {
        ASSERT(FALSE);
		return FALSE;
    }

    CComQIPtr<IXMLDOMElement> pElm = m_pNode;
    if (!pElm) {
        ASSERT(FALSE);
        return FALSE;
    }

    HRESULT hr = pElm->setAttribute(CComBSTR(attrName), CComVariant(attrValue));
    if (FAILED(hr)) {
        printf("Failed to set attribute\n");
        ASSERT(FALSE);
        return FALSE;
    }
    return TRUE;
}

BOOL CBFXmlNode::setAttribute(LPCTSTR attrName, LONG attrValue)
{
    CString str;
    CBFStrHelper::intToStr(attrValue, str);
    if (!setAttribute(attrName, str))
        return FALSE;
    return TRUE;
}

BOOL CBFXmlNode::setAttribute(LPCTSTR attrName, FLOAT attrValue)
{
    CString str;
    CBFStrHelper::floatToStr(attrValue, BF_XML_NUM_DECIMALS, str);
    if (!setAttribute(attrName, str))
        return FALSE;
    return TRUE;
}

BOOL CBFXmlNode::getAttribute(LPCTSTR attrName,
                              CString& value,
                              BOOL bTextField)
{
	if (!m_pNode || StrCmp(attrName, _T("")) == 0)
		return FALSE;

	CComQIPtr<IXMLDOMElement> pElm = m_pNode;
	CComVariant val;
	HRESULT hr;
	hr = pElm->getAttribute(CComBSTR(attrName), &val);
    if (FAILED(hr)) {
        ASSERT(FALSE);
        return FALSE;
    }

	USES_CONVERSION;
	value = OLE2T(val.bstrVal);
	return TRUE;
}

BOOL CBFXmlNode::getElementsByTagName(LPCTSTR tagName, CBFXmlNodeList* pNodeList)
{
	if (!m_pNode || !pNodeList || StrCmp(tagName, _T("")) == 0)
		return FALSE;

	CComQIPtr<IXMLDOMElement> pElm = m_pNode;
	HRESULT hr = pElm->getElementsByTagName(CComBSTR(tagName), &(pNodeList->m_pNodeList));
    if (FAILED(hr))
        return FALSE;

	return TRUE;
}

BOOL CBFXmlNode::getChildNodes(CBFXmlNodeList* pNodeList)
{
    if (!m_pNode || !pNodeList)
        return FALSE;

    HRESULT hr = m_pNode->get_childNodes(&(pNodeList->m_pNodeList));
    if (FAILED(hr))
        return FALSE;

    return TRUE;
}

BOOL CBFXmlNode::getNodeName(CString& nodeName)
{
	if (!m_pNode)
		return FALSE;

	CComBSTR nName;
	HRESULT hr;
    hr = m_pNode->get_nodeName(&nName);
    if (FAILED(hr))
        return FALSE;

	USES_CONVERSION;
	nodeName = OLE2T(nName);
	return TRUE;
}

BOOL CBFXmlNode::getText(CString& text)
{
	if (!m_pNode)
		return FALSE;

	CComBSTR strText;
	HRESULT hr;
    hr = m_pNode->get_text(&strText);
    if (FAILED(hr))
        return FALSE;

	USES_CONVERSION;
	text = OLE2T(strText);
	return TRUE;
}

BOOL CBFXmlNode::setText(LPCTSTR text)
{
	if (!m_pNode)
		return FALSE;

    HRESULT hr = m_pNode->put_text(CComBSTR(text));
    if (FAILED(hr))
        return FALSE;

    return TRUE;
}

//--------------------------------------------------------------------
// XML node list
//--------------------------------------------------------------------
CBFXmlNodeList::CBFXmlNodeList()
{
}

CBFXmlNodeList::~CBFXmlNodeList()
{
}

BOOL CBFXmlNodeList::getNextNode(CBFXmlNode* pNode)
{
	if (!m_pNodeList || !pNode)
		return FALSE;

	pNode->m_pNode.Release();
	if (SUCCEEDED(m_pNodeList->nextNode(&(pNode->m_pNode))))
		return TRUE;
	else
		return FALSE;
}

BOOL CBFXmlNodeList::isEmpty()
{
    if (!m_pNodeList)
        return TRUE;

    long length;
    HRESULT hr;
    hr = m_pNodeList->get_length(&length);
    if (FAILED(hr)) {
        ASSERT(FALSE);
        return TRUE;
    }

    return (length == 0);
}

LONG CBFXmlNodeList::length() const
{
    if (!m_pNodeList)
        return 0;

    LONG length;
    HRESULT hr;
    hr = m_pNodeList->get_length(&length);
    if (FAILED(hr)) {
        ASSERT(FALSE);
        return 0;
    }

    return length;
}

BOOL CBFXmlNodeList::getItem(LONG itemID, CBFXmlNode* pNode) const
{
	if (!m_pNodeList || !pNode)
		return FALSE;

	pNode->m_pNode.Release();
	if (SUCCEEDED(m_pNodeList->get_item(itemID, &(pNode->m_pNode))))
		return TRUE;
	else
		return FALSE;
}

//--------------------------------------------------------------------
// XML document
//--------------------------------------------------------------------
CBFXmlDoc::CBFXmlDoc()
{
    HRESULT hr;
	hr = CoInitialize(NULL);
    if(FAILED(hr)) {
        ASSERT(FALSE);
		return;
    }

    hr = m_pDoc.CoCreateInstance(CLSID_DOMDocument);
    if (FAILED(hr)) {
		ASSERT(FALSE);
		return;
	}
}

CBFXmlDoc::~CBFXmlDoc()
{
    m_pDoc.Release();
    m_pDoc = NULL;
	CoUninitialize();
}

BOOL CBFXmlDoc::loadXML(LPCTSTR xmlFile)
{
    if (StrCmp(xmlFile, _T("")) == 0)
        return FALSE;

    m_xmlFile = xmlFile;
    
    // In MSDN, search for "White Space Handling"
    // In order to have a "half-preserved" property.
    // Need to set it to FALSE before loading the doc.
    // Then set it back to TRUE after loading.
    if (!setPreserveWhiteSpace(FALSE))
        return FALSE;

    VARIANT_BOOL vb;
	HRESULT hr = m_pDoc->load(CComVariant(m_xmlFile), &vb);
    if (FAILED(hr) || vb == VARIANT_FALSE)
		return FALSE;

    //CComPtr<IXMLDOMParseError> pErrObj;
    //m_pDoc->get_parseError(&pErrObj);
    //BSTR reason;
    //pErrObj->get_reason(&reason);

    if (!setPreserveWhiteSpace(TRUE))
        return FALSE;

    return TRUE;
}

BOOL CBFXmlDoc::saveXML(LPCTSTR filename)
{
    CString fname;
    if (filename == NULL || StrCmp(filename, _T("")) == 0) {
        if (m_xmlFile.IsEmpty()) {
            ASSERT(FALSE);
            return FALSE;
        }
        fname = m_xmlFile;
    }
    else {
        fname = filename;
    }

    HRESULT hr = m_pDoc->save(CComVariant(fname));
    if (FAILED(hr))
        return FALSE;

    return TRUE;
}

BOOL CBFXmlDoc::createXML(LPCTSTR newFilename, LPCTSTR rootTag)
{
    if (StrCmp(newFilename, _T("")) == 0 ||
        StrCmp(rootTag, _T("")) == 0) {
        ASSERT(FALSE);
        return FALSE;
    }
    
    FILE *fStream;
    errno_t e = _tfopen_s(&fStream, newFilename, _T("wt,ccs=UNICODE"));
    if (e != 0) 
        return false; // failed..
    CStdioFile xmlFile(fStream);  // open the file from this stream

    //CFile  xmlFile(newFilename, CFile::modeCreate | CFile::modeReadWrite);
    CString str = BF_XML_HEADER;
    str += _T("<");
    str += rootTag;
    str += _T(">\n</");
    str += rootTag;
    str += _T(">\n");

    // write string, with null-terminator
    //xmlFile.Write(str, str.GetLength());
    xmlFile.WriteString(str);
    xmlFile.Close();

    if (!loadXML(newFilename)) {
        ASSERT(FALSE);
        return FALSE;
    }
    return TRUE;
}

BOOL CBFXmlDoc::getDocumentNode(CBFXmlNode* pDocNode)
{
	if (!pDocNode || !m_pDoc)
		return FALSE;

	CComPtr<IXMLDOMElement> pDocElm;
	HRESULT hr;
	hr = m_pDoc->get_documentElement(&pDocElm);
    if (FAILED(hr) || pDocElm == NULL)
        return FALSE;

	pDocNode->m_pNode = pDocElm;
	return TRUE;
}

BOOL CBFXmlDoc::createElement(LPCTSTR nodeName, CBFXmlNode* pNewNode)
{
	if (!pNewNode || !m_pDoc)
		return FALSE;

    HRESULT hr = m_pDoc->createNode(CComVariant(NODE_ELEMENT), CComBSTR(nodeName), NULL, &pNewNode->m_pNode);
    if (FAILED(hr))
        return FALSE;

    return TRUE;
}

BOOL CBFXmlDoc::createText(LPCTSTR nodeName, CBFXmlNode* pNewNode)
{
	if (!pNewNode || !m_pDoc)
		return FALSE;

    HRESULT hr = m_pDoc->createNode(CComVariant(NODE_TEXT), CComBSTR(nodeName), NULL, &pNewNode->m_pNode);
    if (FAILED(hr))
        return FALSE;

    return TRUE;
}

BOOL CBFXmlDoc::setPreserveWhiteSpace(BOOL bPreserve)
{
    HRESULT hr;
    if (bPreserve)
        hr = m_pDoc->put_preserveWhiteSpace(VARIANT_TRUE);
    else
        hr = m_pDoc->put_preserveWhiteSpace(VARIANT_FALSE);

    return !FAILED(hr);
}

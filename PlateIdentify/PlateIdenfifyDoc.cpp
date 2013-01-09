// PlateIdenfifyDoc.cpp : implementation of the CPlateIdenfifyDoc class
//

#include "stdafx.h"
#include "PlateIdenfify.h"
#include "PlateIdenfifyDoc.h"

#include "PlateIdenfifyView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPlateIdenfifyDoc

IMPLEMENT_DYNCREATE(CPlateIdenfifyDoc, CDocument)

BEGIN_MESSAGE_MAP(CPlateIdenfifyDoc, CDocument)
    //{{AFX_MSG_MAP(CPlateIdenfifyDoc)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlateIdenfifyDoc construction/destruction

CPlateIdenfifyDoc::CPlateIdenfifyDoc()
{
    // TODO: add one-time construction code here
    m_pView = NULL;
}

CPlateIdenfifyDoc::~CPlateIdenfifyDoc()
{
}

BOOL CPlateIdenfifyDoc::OnNewDocument()
{
    if (!CDocument::OnNewDocument())
        return FALSE;

    // TODO: add reinitialization code here
    // (SDI documents will reuse this document)
    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CPlateIdenfifyDoc serialization

void CPlateIdenfifyDoc::Serialize(CArchive& ar)
{
    if (ar.IsStoring()) {
        // TODO: add storing code here
    } else {
        // TODO: add loading code here
    }
}

/////////////////////////////////////////////////////////////////////////////
// CPlateIdenfifyDoc diagnostics

#ifdef _DEBUG
void CPlateIdenfifyDoc::AssertValid() const
{
    CDocument::AssertValid();
}

void CPlateIdenfifyDoc::Dump(CDumpContext& dc) const
{
    CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CPlateIdenfifyDoc commands

BOOL CPlateIdenfifyDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
    IplImage *img;
    if(NULL == (img = cvLoadImage(lpszPathName, CV_LOAD_IMAGE_ANYCOLOR))) {
        MessageBox(m_pView->m_hWnd,"不支持的文件格式","打开错误",MB_OK);
        return FALSE;
    }

    if(img->width < 600) {
        // 将大小放大一倍
        IplImage *pTemp = cvCreateImage(cvSize(img->width*2,img->height*2),img->depth,3);
        cvResize(img,pTemp,CV_INTER_LINEAR);
        cvReleaseImage(&img);
        img = pTemp;
    }
    ((CPlateIdenfifyView*)m_pView)->CleanUp();

    ((CPlateIdenfifyView*)m_pView)->m_strFileName = lpszPathName;
    ((CPlateIdenfifyView*)m_pView)->m_pCurrentImg = img;
    ((CPlateIdenfifyView*)m_pView)->m_pOpenImg = img;
    return TRUE;
}

BOOL CPlateIdenfifyDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
    IplImage *pTemp = ((CPlateIdenfifyView*)m_pView)->m_pCurrentImg;
    if(0 == cvSaveImage(lpszPathName,((CPlateIdenfifyView*)m_pView)->m_pCurrentImg)) {
        (CPlateIdenfifyView*)m_pView->MessageBox("文件保存失败");
        return false;
    }
    return true;
}
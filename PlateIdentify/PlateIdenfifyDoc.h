// PlateIdenfifyDoc.h : interface of the CPlateIdenfifyDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_PLATEIDENFIFYDOC_H__44D6E368_E46D_4F6B_8024_FF98420AD5AB__INCLUDED_)
#define AFX_PLATEIDENFIFYDOC_H__44D6E368_E46D_4F6B_8024_FF98420AD5AB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "highgui.h"
#include "cv.h"
#include "ShowImg.h"
class CPlateIdenfifyDoc : public CDocument
{
protected: // create from serialization only
    CPlateIdenfifyDoc();
    DECLARE_DYNCREATE(CPlateIdenfifyDoc)

// Attributes
public:

// Operations
public:

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CPlateIdenfifyDoc)
public:
    virtual BOOL OnNewDocument();
    virtual void Serialize(CArchive& ar);
    virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
    virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
    //}}AFX_VIRTUAL

// Implementation
public:
    virtual ~CPlateIdenfifyDoc();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
    //{{AFX_MSG(CPlateIdenfifyDoc)
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
public:
    CWnd *m_pView;

private:
    CString m_strCurrentFileName;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PLATEIDENFIFYDOC_H__44D6E368_E46D_4F6B_8024_FF98420AD5AB__INCLUDED_)
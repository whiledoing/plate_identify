// PlateIdenfify.h : main header file for the PLATEIDENFIFY application
//

#if !defined(AFX_PLATEIDENFIFY_H__8FD96A82_EF97_4AC9_8A41_236FF09CC1B9__INCLUDED_)
#define AFX_PLATEIDENFIFY_H__8FD96A82_EF97_4AC9_8A41_236FF09CC1B9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CPlateIdenfifyApp:
// See PlateIdenfify.cpp for the implementation of this class
//

class CPlateIdenfifyApp : public CWinApp
{
public:
    CPlateIdenfifyApp();

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CPlateIdenfifyApp)
public:
    virtual BOOL InitInstance();
    //}}AFX_VIRTUAL

// Implementation
    //{{AFX_MSG(CPlateIdenfifyApp)
    afx_msg void OnAppAbout();
    // NOTE - the ClassWizard will add and remove member functions here.
    //    DO NOT EDIT what you see in these blocks of generated code !
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PLATEIDENFIFY_H__8FD96A82_EF97_4AC9_8A41_236FF09CC1B9__INCLUDED_)
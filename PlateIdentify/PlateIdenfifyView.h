// PlateIdenfifyView.h : interface of the CPlateIdenfifyView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_PLATEIDENFIFYVIEW_H__6D4E6FF3_842D_4226_9ECF_55799342F261__INCLUDED_)
#define AFX_PLATEIDENFIFYVIEW_H__6D4E6FF3_842D_4226_9ECF_55799342F261__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "highgui.h"
#include "ShowImg.h"

#include <vector>
#include <list>
#include <math.h>
#include <fstream>
#include <iomanip>
#include <queue>

#include "ann.h"
#include "GetPicData.h"

#define VALUE(pImg,x,y) (((UCHAR*)(pImg->imageData + (y) * pImg->width))[(x)])
#define VALUEB(pImg,x,y) (((UCHAR*)(pImg->imageData + (y) * pImg->width))[((x) * pImg->nChannels + 0)])
#define VALUEG(pImg,x,y) (((UCHAR*)(pImg->imageData + (y) * pImg->width))[((x) * pImg->nChannels + 1)])
#define VALUER(pImg,x,y) (((UCHAR*)(pImg->imageData + (y) * pImg->width))[((x) * pImg->nChannels + 2)])

const unsigned MAX_BUFF_LENGTH = 200;

struct MyRect {
    double xMin, xMax, yMin ,yMax;
};

class CPlateIdenfifyView : public CView
{
protected: // create from serialization only
    CPlateIdenfifyView();
    DECLARE_DYNCREATE(CPlateIdenfifyView)

// Attributes
public:
    CPlateIdenfifyDoc* GetDocument();

// Operations
public:

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CPlateIdenfifyView)
public:
    virtual void OnDraw(CDC* pDC);  // overridden to draw this view
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
    //}}AFX_VIRTUAL

// Implementation
public:
    IplImage *m_pOpenImg;
    IplImage *m_pGrayImg;
    IplImage *m_pTwoValueImg;
    IplImage *m_pCurrentImg;
    IplImage *m_pCanndy;
    IplImage *m_pPlateArea;
    CShowImg m_Showimg;
    CvRect m_recCharacter[7];		//保存分割区域的x边界
    CvRect m_r;						//车牌的区域
    CvRect m_bestRec;
    CString m_strFileName;			//打开文件的名称
    std::list<CvRect> m_recArea;
    std::vector<CString> m_characterName;	//保存分割出来字符的位置
    virtual ~CPlateIdenfifyView();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
    DECLARE_MESSAGE_MAP()
    //{{AFX_MSG(CPlateIdenfifyView)
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnClose();
    afx_msg void OnCanny();
    afx_msg void OnPlateArea();
    afx_msg void OnSplite();
    afx_msg void OnFindPlate();
    afx_msg void OnSpliteChara();
    afx_msg void OnNetIdentify();
    afx_msg void OnNetTrain();
    afx_msg void OnTrainNum();
    afx_msg void OnTrainLet();
    afx_msg void OnTrainNumlet();
    afx_msg void OnTrainChinese();
    //}}AFX_MSG

private:
    bool TrainNumImpl();
    bool TrainLetImpl();
    bool TrainNumLetImpl();
    bool TrainChineseImpl();

public:
    void CleanUp();

private:
    void SlopeAdjust(IplImage *pImg );
    void GetGray();
    void AdaptiveThreshold();
    void GetHough();
    void OnPlatelocate();
    void GrayChange();

    void FindConnectArea(std::vector< std::vector<CPoint> >& ptVec);
    bool FindFirstPt(IplImage *pImg, CPoint &pt );
    void GotTheArea(MyRect &rec);
    bool SpliteCharacter();
    void Rotate();
    void Thining(IplImage *res,IplImage *dst);				//细化算法
    void CopyRect(IplImage *src, IplImage *dst, CvRect rec);
    void ReverseGray(IplImage *img);
    void drawSquares( IplImage* img, CvSeq* squares );
    CvSeq* findSquares4( IplImage* img, CvMemStorage* storage );
    void SaveCharacter();
    void TightCharacter(IplImage *&img);
    bool NetCalculate(CNeuralNet &net, char *fileName, int &nType, double &dConfidence);

    // finds a cosine of angle between vectors from pt0->pt1 and from pt0->pt2
    double angle( CvPoint* pt1, CvPoint* pt2, CvPoint* pt0 ) {
        double dx1 = pt1->x - pt0->x;
        double dy1 = pt1->y - pt0->y;
        double dx2 = pt2->x - pt0->x;
        double dy2 = pt2->y - pt0->y;
        return (dx1*dx2 + dy1*dy2)/sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);
    }

    bool CPlateIdenfifyView::BeWhite(IplImage *img,int x,int y) {
        int r,g,b;
        CvScalar s;
        s = cvGet2D(img,y,x);
        b = s.val[0];
        g = s.val[1];
        r = s.val[2];
        double sum = r + g + b;
        if(sum > 200 && r/sum < 0.4 && g/sum < 0.4 && b/sum < 0.4)
            return true;
        return false;
    }

    bool CPlateIdenfifyView::BeBlue(IplImage *img,int x,int y) {
        int r,g,b;
        CvScalar s;
        s = cvGet2D(img,y,x);
        b = s.val[0];
        g = s.val[1];
        r = s.val[2];
        if(b <= 20)
            return false;
        double f1 = b / (double) r;
        double f2 = b / (double) g;
        if(f1 > 1.4 && f2 > 1.4 )
            return true;
        return false;
    }
    void output(const char *name,IplImage *img) {
        using namespace std;
        std::fstream fout;
        fout.open(name,ios::out);
        CvScalar s;
        for(int i =0 ; i < img->height; ++i) {
            for(int j = 0; j < img->width; ++j) {
                s = cvGet2D(img,i,j);
                fout<<setw(4)<<(int)(s.val[0])<<" ";
            }
            fout<<endl;
        }
    }
};

#ifndef _DEBUG  // debug version in PlateIdenfifyView.cpp
inline CPlateIdenfifyDoc* CPlateIdenfifyView::GetDocument()
{
    return (CPlateIdenfifyDoc*)m_pDocument;
}
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PLATEIDENFIFYVIEW_H__6D4E6FF3_842D_4226_9ECF_55799342F261__INCLUDED_)
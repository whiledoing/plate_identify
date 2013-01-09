#ifndef __CSHOWIMG_H__
#define __CSHOWIMG_H__
#include "highgui.h"

class CShowImg : public CvvImage
{
public:
    CShowImg() {}
    virtual ~CShowImg() {}
    void SetImg(IplImage *pImg) {
        m_img = cvCloneImage(pImg);
    }
};
#endif
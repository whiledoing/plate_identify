#ifndef _GETPICDATA__H_
#define _GETPICDATA__H_

#include "CImg.h"
#include <vector>
const int RESAMPLE_LENTH = 4;
typedef std::vector<double> DVECTOR;
class CPicData
{
public:
    CPicData():m_pImg(NULL) {}
    bool Initialize(CString strFileName) {
        if(m_pImg != NULL)
            delete m_pImg;
        m_pImg = new CImg;
        if(m_pImg->AttachFromFile(strFileName))
            return true;
        return false;
    }
    virtual ~CPicData() {};

public:
    bool GetResampleData(DVECTOR &eachInput);		//从图像中获取采样后的输入数据

public:
    CImg *m_pImg;
};

class CTrainData
{
public:
    CTrainData(const char *lpszDir, unsigned int nTypeNum)
        :m_lpszDir(lpszDir),m_nTypeNum(nTypeNum) {}
    virtual ~CTrainData() {}

    bool GetTrainData(std::vector<DVECTOR> &input, std::vector<DVECTOR> &output);
private:
    const char *m_lpszDir;		//训练图像所在的目录
    unsigned int m_nTypeNum;	//类别的数目
};
#endif
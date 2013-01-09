#ifndef __CIMG_H__
#define __CIMG_H__
#include <afxwin.h>
#define WIDTHBYTES(x) ((((x)+31)>>5)<<2)	//make 4 as the factor of the width bytes
class CImg
{
public:
    //data member
    BITMAPINFOHEADER* m_pBMIH;
    LPBYTE* m_lpData;
    LPBYTE m_lpvColorTable;
    UINT m_nColorTableEntries;
    //member function
public:
    CImg():m_lpData(NULL),m_lpvColorTable(NULL),m_nColorTableEntries(0),
        m_pBMIH(NULL) {
    };
    CImg(const CImg& gray);
    void Construtor(const CImg& gray);
    ~CImg();

public:
    CImg& operator =(const CImg& gray);
    CImg& operator +=(const CImg& gray);
    CImg operator +(const CImg& gray) const;
    CImg& operator -=(const CImg& gray);
    CImg operator -(const CImg& gray) const;

public:

    BOOL IsValidate() const;

    BOOL AttachFromFile(LPCTSTR lpFileName);
    BOOL AttachFromFile(CFile& file);

    BOOL SaveToFile(LPCTSTR lpFileName) const;
    BOOL SaveToFile(CFile& file) const;

    UINT GetHeight() const;
    UINT GetWidthPixel() const;
    UINT GetWidthBytes() const;
    UINT GetGray(int x, int y) const;			//获取给定像素位置的灰度值
    COLORREF GetPixel(int x, int y) const;			//获取给定像素位置的颜色
    void SetPixel(int x, int y, COLORREF color);	//设置指定位置的像素的颜色

    BOOL IsBinaryImg() const;					//判断是否是二值的图像
    BOOL IsIndexImg() const;					//判断是否是有索引的图像

public:
    BOOL OnDraw(CDC* pDC) const;
private:
    void CleanUp();

private:
};
inline BOOL CImg::IsValidate() const
{
    return (m_pBMIH) ? TRUE : FALSE;
}
inline UINT CImg::GetHeight() const
{
    return m_pBMIH->biHeight;
}
inline UINT CImg::GetWidthPixel() const
{
    return m_pBMIH->biWidth;
}
inline UINT CImg::GetWidthBytes() const
{
    return WIDTHBYTES(m_pBMIH->biWidth * m_pBMIH->biBitCount);
}
inline COLORREF CImg::GetPixel(int x, int y) const
{
    switch (m_pBMIH->biBitCount) {
    case 1:
        //单色图
    {
        int index = ((1 << (7 - x % 8)) & m_lpData[m_pBMIH->biHeight - 1 - y][x / 8]);
        if (((RGBQUAD *) m_lpvColorTable)->rgbBlue != 0)			//索引为0的地方要为黑色（即是0，0，0）
            index = !index;
        if (index)
            return RGB(255, 255, 255);
        else
            return RGB(0, 0, 0);
    }
    case 8:
        //灰度图
    {
        int col = m_lpData[m_pBMIH->biHeight - 1 - y][x];		//注意文件中行是倒着存放的
        return RGB(col, col, col);
    }
    case 24:
        //真彩图
    {
        BYTE bValue = m_lpData[m_pBMIH->biHeight - 1 - y][3 * x];		//注意是小端的存储的模式
        BYTE gValue = m_lpData[m_pBMIH->biHeight - 1 - y][3 * x + 1];
        BYTE rValue = m_lpData[m_pBMIH->biHeight - 1 - y][3 * x + 2];
        return RGB(rValue, gValue, bValue);
    }
    default: {
        throw CString("Not Support Format\n");
    }
    }
}

inline	UINT CImg::GetGray(int x, int y) const
{
    COLORREF ref = GetPixel(x, y);
    UINT rValue, gValue, bValue;
    rValue = GetRValue(ref);
    gValue = GetGValue(ref);
    bValue = GetBValue(ref);

    if (rValue == gValue && gValue == bValue)
        return rValue;

    double gray = 0.3 * rValue + 0.59 * gValue + 0.11 * bValue;
    return (int) gray;
}

inline BOOL CImg::IsBinaryImg() const
{
    return (m_pBMIH->biBitCount == 1);
}
inline BOOL CImg::IsIndexImg() const
{
    return (m_nColorTableEntries != 0 && m_lpvColorTable != NULL);
}
#endif
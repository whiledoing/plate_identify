#include "StdAfx.h"
#include "CImg.h"
#include <iostream>

//--------------------- Constructor And Destructor -----------------
////////////////////////////////////////////////////////////////////
void CImg::Construtor(const CImg& gray)
{
    //Initialization
    m_pBMIH = NULL;
    m_nColorTableEntries = 0;
    m_lpvColorTable = NULL;
    m_lpData = NULL;
    //fill the m_nBMIH
    UINT size = gray.m_pBMIH->biSize + gray.m_nColorTableEntries * 4;
    m_pBMIH = (BITMAPINFOHEADER *) new BYTE[size];
    memcpy(m_pBMIH, gray.m_pBMIH, size);
    m_nColorTableEntries = gray.m_nColorTableEntries;
    if (m_nColorTableEntries)
        m_lpvColorTable = (BYTE *) (m_pBMIH + 1);
    //fill the m_lpData
    m_lpData = new LPBYTE[gray.GetHeight()];
    for (int i = 0; i < m_pBMIH->biHeight; ++i) {
        m_lpData[i] = new BYTE[gray.GetWidthBytes()];
        memcpy(m_lpData[i], gray.m_lpData[i], gray.GetWidthBytes());
    }
}

CImg::CImg(const CImg& gray):m_pBMIH(NULL),m_lpvColorTable(NULL),
    m_lpData(NULL),m_nColorTableEntries(0)
{
    if (!gray.IsValidate()) {
        throw CString("Invalidate Construction\n");
    }
    this->Construtor(gray);
}

void CImg::CleanUp()
{
    if (m_pBMIH) {
        int nWidth = m_pBMIH->biWidth;
        if (m_lpData) {
            for (int i = 0; i < nWidth; ++i)
                if (m_lpData[i])
                    delete[] m_lpData[i];
            delete[] m_lpData;
            m_lpData = NULL;
        }
        delete[] m_pBMIH;
        m_pBMIH = NULL;
    }
}

CImg::~CImg()
{
    CleanUp();
}

//-------------------------- Overriding The Operator ---------------
////////////////////////////////////////////////////////////////////
CImg& CImg::operator=(const CImg& gray)
{
    if (!gray.IsValidate()) {
        throw CString("Invalidate Construction\n");
    }
    if(this == &gray)
        return *this;
    CleanUp();
    this->Construtor(gray);
    return *this;
}

CImg& CImg::operator +=(const CImg& gray)
{
    INT nWidth = GetWidthPixel();
    INT nHeight = GetHeight();
    //创建动态二维数组,保存灰度值相加的结果
    int i, j;
    int** data = new int* [nHeight];
    for (i = 0; i < nHeight; ++i)
        data[i] = new int [nWidth];
    //nMax和nMin记录数据和的最大和最小值
    int nMax = 0, nMin = 255 * 2, temp;
    for (i = 0; i < nHeight; ++i) {
        for (j = 0; j < nWidth; ++j) {
            temp = GetGray(j, i) + gray.GetGray(j, i);
            if (temp > nMax)
                nMax = temp;
            if (temp < nMin)
                nMin = temp;
            data[i][j] = temp;
        }
    }

    int nSpan = nMax - nMin;
    BYTE col;
    for (i = 0; i < nHeight; ++i) {
        for (j = 0; j < nWidth; ++j) {
            if (nSpan == 0) {
                //All in Same then the span is zero
                col = (data[i][j] <= 255) ? data[i][j] : 255;	//Ensure value is less equal than 255
            } else {
                //the most condition the span is more than zero
                col = (data[i][j] - nMin) / nSpan * 255;
            }
            SetPixel(j, i, RGB(col, col, col));
        }
    }
    for (i = 0; i < nHeight; ++i)
        delete[] data[i];
    delete[] data;
    return *this;
}

CImg CImg::operator +(const CImg& gray) const
{
    CImg pTemp(*this);
    pTemp += gray;
    return pTemp;
}

CImg& CImg::operator-=(const CImg& gray)
{
    INT nWidth = GetWidthPixel();
    INT nHeight = GetHeight();
    //创建动态二维数组,保存灰度值相加的结果
    int i, j;
    int** data = new int* [nHeight];
    for (i = 0; i < nHeight; ++i)
        data[i] = new int [nWidth];
    //nMax和nMin记录数据和的最大和最小值
    int nMax = -255, nMin = 255, temp;
    for (i = 0; i < nHeight; ++i) {
        for (j = 0; j < nWidth; ++j) {
            temp = GetGray(j, i) - gray.GetGray(j, i);
            if (temp > nMax)
                nMax = temp;
            if (temp < nMin)
                nMin = temp;
            data[i][j] = temp;
        }
    }

    int nSpan = nMax - nMin;
    BYTE col;
    for (i = 0; i < nHeight; ++i) {
        for (j = 0; j < nWidth; ++j) {
            if (nSpan == 0) {
                //All in Same then the span is zero
                col = data[i][j];
            } else {
                //the most condition the span is more than zero
                col = (data[i][j] - nMin) / nSpan * 255;
            }
            SetPixel(j, i, RGB(col, col, col));
        }
    }
    for (i = 0; i < nHeight; ++i)
        delete[] data[i];
    delete[] data;
    return *this;
}

CImg CImg::operator-(const CImg& gray) const
{
    CImg pTemp(*this);
    pTemp -= gray;
    return pTemp;
}

//-------------------------member function--------------------------
////////////////////////////////////////////////////////////////////
BOOL CImg::AttachFromFile(LPCTSTR lpFileName)
{
    CFile file;
    if (!file.Open(lpFileName, CFile::modeRead | CFile::shareDenyWrite))
        return FALSE;
    return AttachFromFile(file);
}

BOOL CImg::AttachFromFile(CFile& file)
{
    BITMAPFILEHEADER bmfHeader;
    if (!file.Read(&bmfHeader, sizeof(BITMAPFILEHEADER)))
        return FALSE;
    if (bmfHeader.bfType != MAKEWORD('B', 'M'))
        return FALSE;

    BITMAPINFOHEADER* pbmiHeader = NULL;
    int infoSize = bmfHeader.bfOffBits - sizeof(BITMAPFILEHEADER);
    pbmiHeader = (BITMAPINFOHEADER *)new BYTE[infoSize];
    if (!pbmiHeader)
        return FALSE;
    if (!file.Read(pbmiHeader, infoSize)) {
        delete pbmiHeader;
        return FALSE;
    }

    LPBYTE pvColorTable = NULL;
    int nSizePalette = 0;
    nSizePalette = (infoSize - sizeof(BITMAPINFOHEADER)) / sizeof(RGBQUAD);
    if (nSizePalette)		//if non-zero
        pvColorTable = (LPBYTE) (pbmiHeader + 1);

    LPBYTE* pData = new LPBYTE[pbmiHeader->biHeight];
    if (!pData)
        return FALSE;
    memset(pData, NULL, sizeof(LPBYTE) * pbmiHeader->biHeight);
    int nWidthBytes = WIDTHBYTES(pbmiHeader->biWidth* pbmiHeader->biBitCount);
    TRACE("%d %d\n", bmfHeader.bfOffBits, file.GetLength());
    for (int i = 0; i < pbmiHeader->biHeight; ++i) {
        pData[i] = new BYTE[nWidthBytes];
        TRACE("%d\n", file.GetPosition());
        TRACE("%x\n", pData[i]);
        ASSERT(pData);
        if (!file.Read(pData[i], nWidthBytes))
            return FALSE;
    }
    m_pBMIH = pbmiHeader;
    m_lpData = pData;
    m_lpvColorTable = pvColorTable;
    m_nColorTableEntries = nSizePalette;
    file.Close();
    return TRUE;
}

BOOL CImg::SaveToFile(LPCTSTR lpFileName) const
{
    if (!IsValidate())
        return FALSE;
    CFile file;
    if (!file.Open(lpFileName, CFile::modeWrite | CFile::modeCreate))
        return FALSE;
    return SaveToFile(file);
}

BOOL CImg::SaveToFile(CFile& file) const
{
    if (!IsValidate())
        return FALSE;
    BITMAPFILEHEADER bmfHeader = {
        0
    };
    int nWidthBytes = WIDTHBYTES(m_pBMIH->biWidth* m_pBMIH->biBitCount);
    bmfHeader.bfOffBits = sizeof(BITMAPFILEHEADER) +
                          sizeof(BITMAPINFOHEADER) +
                          4 * m_nColorTableEntries;
    bmfHeader.bfSize = bmfHeader.bfOffBits + nWidthBytes * m_pBMIH->biHeight;
    bmfHeader.bfType = MAKEWORD('B', 'M');

    file.Write(&bmfHeader, sizeof(BITMAPFILEHEADER));
    file.Write(m_pBMIH, sizeof(BITMAPINFOHEADER) + 4 * m_nColorTableEntries);
    for (int i = 0; i < m_pBMIH->biHeight; ++i)
        file.Write(m_lpData[i], nWidthBytes);
    return TRUE;
}

void CImg::SetPixel(int x, int y, COLORREF color)
{
    switch (m_pBMIH->biBitCount) {
    case 1:
        //单色图
    {
        int ret = (int) GetRValue(color);
        if (((RGBQUAD *) m_lpvColorTable)->rgbBlue != 0)
            ret = !ret;
        BYTE temp = m_lpData[m_pBMIH->biHeight - 1 - y][x];
        if (ret)
            m_lpData[m_pBMIH->biHeight - 1 - y][x] = (temp |
                    (1 << (7 - x % 8)));
        else
            m_lpData[m_pBMIH->biHeight - 1 - y][x] = (temp & (0xff -
                    (1 << (7 - x % 8))));
    }
    case 8:
        //灰度图
    {
        m_lpData[m_pBMIH->biHeight - 1 - y][x] = GetRValue(color);
        return;
    }
    case 24:
        //真彩图
    {
        m_lpData[m_pBMIH->biHeight - 1 - y][3 * x] = GetBValue(color);
        m_lpData[m_pBMIH->biHeight - 1 - y][3 * x + 1] = GetGValue(color);
        m_lpData[m_pBMIH->biHeight - 1 - y][3 * x + 2] = GetRValue(color);
        return;
    }
    default: {
        throw CString("Format is not Support\n");
    }
    }
}

BOOL CImg::OnDraw(CDC* pDC) const
{
    if (!IsValidate())
        return FALSE;
    ::SetDIBitsToDevice(pDC->m_hDC, 0, 0, m_pBMIH->biWidth, m_pBMIH->biHeight,
                        0, 0, 0, m_pBMIH->biHeight, m_lpData, (BITMAPINFO *) m_pBMIH,
                        DIB_RGB_COLORS);
    // 	for(int i=0; i<m_pBMIH->biHeight; i++)
    // 	{
    //
    // 		::SetDIBitsToDevice(*pDC, 0, 0, m_pBMIH->biWidth,
    // 			m_pBMIH->biHeight, 0, 0, i, 1, m_lpData[i], (BITMAPINFO*)m_pBMIH, DIB_RGB_COLORS);
    // 	}
    return TRUE;
}
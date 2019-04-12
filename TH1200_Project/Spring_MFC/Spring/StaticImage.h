
/*
 * StaticImage.h
 *
 * һ��CStatic�������࣬������ʾͼƬ
 *
 * Created on: 2018��4��17������10:24:07
 * Author: lixingcong
 */
#pragma once

#include <GdiPlus.h>

#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "Shlwapi.lib")

// CStaticImage

class CStaticImage : public CStatic
{
	DECLARE_DYNAMIC(CStaticImage)

public:
	CStaticImage();
	virtual ~CStaticImage();

	void setPicture(int picID, int x=0, int y=0, int width=0, int height=0);
	void setPicture(CString picFilename, int x=0, int y=0, int width=0, int height=0);
	void setRotate(float angle);
	void setScaleRatio(float ratio);

protected: // ��Ա
	//GDI+�ĳ�ʼ�� ����
	ULONG_PTR m_gdiToken;
	Gdiplus::GdiplusStartupInput m_gdiInput;

	// �ٽ����ṹ����
	CRITICAL_SECTION cs_redraw; // ˢ��Ƶ������

	int pictureID; // ͼ����ԴID
	CString m_picFilename; // ͼ���ļ���
	int m_x,m_y,m_width,m_height; // ͼ��λ����ߴ�
	float m_pos_center[2]; // ���ĵ������
	float m_rotateAngle; // ��ת��

	// �����Ż���������ʹ�õı�������Ϊ��Ա
	bool isSetPicture;
	bool isAllocatedMemory;
	Gdiplus::Bitmap* m_pBmpFile; // ����Դ�ļ����ص�λͼ
	CBitmap* m_pMemoryBmp; //�ڴ��г�����ʱͼ���λͼ
	CDC* m_pMemoryCDC; //���ڻ�����ͼ���ڴ�DC
	Gdiplus::Graphics* m_graph; // GDI��ͼ����

protected: // ��Ա����
	virtual void drawPicture(int x, int y, int width=0, int height=0); // ���Ʋ�ͼ
	void setPostion(int x, int y, int width, int height);
	Gdiplus::Bitmap* ResourceToBitmap(const int id);
	Gdiplus::Bitmap* FilenameToBitmap(const CString& filename);
	void createDestroyMemoryCDC(bool isCreate);

protected: // ��Ϣ����
	DECLARE_MESSAGE_MAP()
	virtual afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};



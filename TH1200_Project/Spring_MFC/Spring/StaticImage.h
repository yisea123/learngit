
/*
 * StaticImage.h
 *
 * 一个CStatic的派生类，用于显示图片
 *
 * Created on: 2018年4月17日上午10:24:07
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

protected: // 成员
	//GDI+的初始化 数据
	ULONG_PTR m_gdiToken;
	Gdiplus::GdiplusStartupInput m_gdiInput;

	// 临界区结构对象
	CRITICAL_SECTION cs_redraw; // 刷新频率限制

	int pictureID; // 图像资源ID
	CString m_picFilename; // 图像文件名
	int m_x,m_y,m_width,m_height; // 图像位置与尺寸
	float m_pos_center[2]; // 中心点的坐标
	float m_rotateAngle; // 旋转角

	// 性能优化：将反复使用的变量定义为成员
	bool isSetPicture;
	bool isAllocatedMemory;
	Gdiplus::Bitmap* m_pBmpFile; // 从资源文件加载的位图
	CBitmap* m_pMemoryBmp; //内存中承载临时图象的位图
	CDC* m_pMemoryCDC; //用于缓冲作图的内存DC
	Gdiplus::Graphics* m_graph; // GDI绘图对象

protected: // 成员函数
	virtual void drawPicture(int x, int y, int width=0, int height=0); // 绘制彩图
	void setPostion(int x, int y, int width, int height);
	Gdiplus::Bitmap* ResourceToBitmap(const int id);
	Gdiplus::Bitmap* FilenameToBitmap(const CString& filename);
	void createDestroyMemoryCDC(bool isCreate);

protected: // 消息函数
	DECLARE_MESSAGE_MAP()
	virtual afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};



/*
 * StaticImage.cpp
 *
 * 一个CStatic的派生类，用于显示图片
 *
 * Created on: 2018年4月17日上午10:24:07
 * Author: lixingcong
 */


#include "stdafx.h"
#include "Spring.h"
#include "StaticImage.h"
#include "Utils.h"

// CStaticImage

IMPLEMENT_DYNAMIC(CStaticImage, CStatic)

CStaticImage::CStaticImage()
{
	pictureID=0;
	m_rotateAngle=0.0;
	m_picFilename=CString();
	InitializeCriticalSection(&cs_redraw); // 创建临界区

	//初始化GDI+
	Gdiplus::GdiplusStartup(&m_gdiToken, &m_gdiInput, NULL);

	// 性能优化
	isAllocatedMemory=false;
	isSetPicture=false;
	m_pBmpFile=nullptr;
	m_pMemoryBmp=nullptr;
	m_pMemoryCDC=nullptr;
	m_graph=nullptr;
}

CStaticImage::~CStaticImage()
{
	DeleteCriticalSection(&cs_redraw); // 删除临界区

	if(true==isAllocatedMemory)
		createDestroyMemoryCDC(false);

	if(NULL!=m_gdiToken)
		Gdiplus::GdiplusShutdown(m_gdiToken);
}


BEGIN_MESSAGE_MAP(CStaticImage, CStatic)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()



void CStaticImage::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	drawPicture(m_x,m_y,m_width,m_height);//CStaticImageAxis::drawPicture
}


void CStaticImage::drawPicture(int x, int y, int width, int height)
{
	if(false==isSetPicture)
		return;

	if(false==isAllocatedMemory)
		return;

	if(FALSE == TryEnterCriticalSection(&cs_redraw))
		return;
	
	CDC* pDC = GetDC(); // 注意释放pDC的内存
/*****************************在DC中画的图像要复制拷贝出来******************************/
	// GDIPlus旋转：http://www.cppblog.com/wxtnote/archive/2015/03/18/210095.html
	Gdiplus::Matrix matrix;
	Gdiplus::PointF pointCenter(m_pos_center[0],m_pos_center[1]); // 旋转中心
	matrix.RotateAt(m_rotateAngle, pointCenter);//角度  中心
	m_graph->SetTransform(&matrix);
/****************************************************************************************/
	if(m_pBmpFile)
		m_graph->DrawImage(m_pBmpFile,
					//目标位置
					Gdiplus::Rect(x,y,width,height),
					//图片的剪切
					0,0,m_pBmpFile->GetWidth(),m_pBmpFile->GetHeight(),
					Gdiplus::UnitPixel
					);

	pDC->BitBlt(x,y,width,height,m_pMemoryCDC,x,y,SRCCOPY);//将内存DC上的图象拷贝到前台

	ReleaseDC(pDC);

	LeaveCriticalSection(&cs_redraw); // 退出
}

// http://www.davekb.com/browse_programming_tips:resource_bitmap_to_gdi_plus_bitmap:txt
Gdiplus::Bitmap* CStaticImage::ResourceToBitmap(const int id)
{
	Gdiplus::Bitmap* gdiBmp = nullptr;
	HBITMAP bBmp = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(id));
	if (bBmp){
		gdiBmp=Gdiplus::Bitmap::FromHBITMAP(bBmp, NULL);

		// Remark: serious memory leak if we forget to delete this object
		DeleteObject(bBmp);
	}

	return gdiBmp;
}

void CStaticImage::setPicture(int picID, int x, int y, int width, int height)
{
	pictureID=picID;
	m_picFilename=CString();
	isSetPicture=true;

	setPostion(x, y,width, height);
}

void CStaticImage::setPicture(CString picFilename, int x, int y, int width, int height)
{
	pictureID=0;
	m_picFilename=picFilename;
	isSetPicture=true;

	setPostion(x, y,width, height);
}

void CStaticImage::setRotate(float angle)
{
	m_rotateAngle=angle;

	Invalidate();
}

void CStaticImage::setPostion(int x, int y, int width, int height)
{
	CRect clientRect;
	GetClientRect(&clientRect);

	m_x=x;
	m_y=y;
	m_width=width>0?width:clientRect.Width();
	m_height=height>0?height:clientRect.Height();

	m_pos_center[0]=static_cast<float>(m_x)+static_cast<float>(m_width)/2;
	m_pos_center[1]=static_cast<float>(m_y)+static_cast<float>(m_height)/2;

	createDestroyMemoryCDC(true);

	Invalidate();
}

void CStaticImage::createDestroyMemoryCDC(bool isCreate)
{
	if(isCreate){
		if(true == isAllocatedMemory){ // 重复申请内存
			// 递归调用自己，释放资源
			createDestroyMemoryCDC(false);//析构
		}

		// 打开图片
		if(pictureID>0){
			m_pBmpFile = ResourceToBitmap(pictureID);
		}else if(m_picFilename.GetLength()>0){
			m_pBmpFile = FilenameToBitmap(m_picFilename);
		}

		CDC* pDC=GetDC(); // 注意释放pDC的内存

		m_pMemoryBmp=new CBitmap; //内存中承载临时图象的位图
		m_pMemoryCDC=new CDC; // 用于缓冲作图的内存DC

		m_pMemoryCDC->CreateCompatibleDC(pDC); // 依附窗口DC创建兼容内存DC
		m_pMemoryBmp->CreateCompatibleBitmap(pDC,m_width,m_height);//创建兼容位图
		m_pMemoryCDC->SelectObject(m_pMemoryBmp); //将位图选择进内存DC

		m_graph = new Gdiplus::Graphics(m_pMemoryCDC->m_hDC); // GDI绘图对象
		m_graph->SetSmoothingMode(Gdiplus::SmoothingModeHighSpeed); // 最流畅的体验（最差的画质）

		ReleaseDC(pDC); // 释放pDC的内存
		isAllocatedMemory=true;
	}else{
		m_graph->ReleaseHDC(m_pMemoryCDC->m_hDC); // GDI绘图对象释放CDC handler
		delete_pointer(m_graph);

		m_pMemoryCDC->DeleteDC(); // 删除CDC
		delete_pointer(m_pMemoryCDC);

		m_pMemoryBmp->DeleteObject(); //删除位图
		delete_pointer(m_pMemoryBmp); // 删除用于缓冲作图的内存DC

		delete_pointer(m_pBmpFile);

		isAllocatedMemory=false;
	}
}

BOOL CStaticImage::OnEraseBkgnd(CDC* pDC)
{
	// 重绘闪屏 http://blog.csdn.net/rushkid02/article/details/9733117
	return TRUE;
}

Gdiplus::Bitmap* CStaticImage::FilenameToBitmap(const CString& filename)
{
	// Gdiplus::Bitmap::FromFile会打开Handle句柄锁定图像文件，使用IStream方法可以解锁。lixingcong@20181105

	Gdiplus::Bitmap* gdiBmp = nullptr;

	// https://stackoverflow.com/questions/39312201/how-to-use-gdi-library-to-decode-a-jpeg-in-memory
	// read file
	HANDLE hfile = CreateFile(m_picFilename, GENERIC_READ, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (!hfile)
		return nullptr;

	DWORD bufsize = GetFileSize(hfile, NULL);
	BYTE *buf = new BYTE[bufsize];

	DWORD temp;
	ReadFile(hfile, buf, bufsize, &temp, 0);

	//convert buf to IStream    
	CComPtr<IStream> stream;
	stream.Attach(SHCreateMemStream(buf, bufsize));

	gdiBmp=Gdiplus::Bitmap::FromStream(stream);

	// clean up
	delete[] buf;
	CloseHandle(hfile);

	return gdiBmp;
}
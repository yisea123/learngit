/*
 * StaticImage.cpp
 *
 * һ��CStatic�������࣬������ʾͼƬ
 *
 * Created on: 2018��4��17������10:24:07
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
	InitializeCriticalSection(&cs_redraw); // �����ٽ���

	//��ʼ��GDI+
	Gdiplus::GdiplusStartup(&m_gdiToken, &m_gdiInput, NULL);

	// �����Ż�
	isAllocatedMemory=false;
	isSetPicture=false;
	m_pBmpFile=nullptr;
	m_pMemoryBmp=nullptr;
	m_pMemoryCDC=nullptr;
	m_graph=nullptr;
}

CStaticImage::~CStaticImage()
{
	DeleteCriticalSection(&cs_redraw); // ɾ���ٽ���

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
	
	CDC* pDC = GetDC(); // ע���ͷ�pDC���ڴ�
/*****************************��DC�л���ͼ��Ҫ���ƿ�������******************************/
	// GDIPlus��ת��http://www.cppblog.com/wxtnote/archive/2015/03/18/210095.html
	Gdiplus::Matrix matrix;
	Gdiplus::PointF pointCenter(m_pos_center[0],m_pos_center[1]); // ��ת����
	matrix.RotateAt(m_rotateAngle, pointCenter);//�Ƕ�  ����
	m_graph->SetTransform(&matrix);
/****************************************************************************************/
	if(m_pBmpFile)
		m_graph->DrawImage(m_pBmpFile,
					//Ŀ��λ��
					Gdiplus::Rect(x,y,width,height),
					//ͼƬ�ļ���
					0,0,m_pBmpFile->GetWidth(),m_pBmpFile->GetHeight(),
					Gdiplus::UnitPixel
					);

	pDC->BitBlt(x,y,width,height,m_pMemoryCDC,x,y,SRCCOPY);//���ڴ�DC�ϵ�ͼ�󿽱���ǰ̨

	ReleaseDC(pDC);

	LeaveCriticalSection(&cs_redraw); // �˳�
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
		if(true == isAllocatedMemory){ // �ظ������ڴ�
			// �ݹ�����Լ����ͷ���Դ
			createDestroyMemoryCDC(false);//����
		}

		// ��ͼƬ
		if(pictureID>0){
			m_pBmpFile = ResourceToBitmap(pictureID);
		}else if(m_picFilename.GetLength()>0){
			m_pBmpFile = FilenameToBitmap(m_picFilename);
		}

		CDC* pDC=GetDC(); // ע���ͷ�pDC���ڴ�

		m_pMemoryBmp=new CBitmap; //�ڴ��г�����ʱͼ���λͼ
		m_pMemoryCDC=new CDC; // ���ڻ�����ͼ���ڴ�DC

		m_pMemoryCDC->CreateCompatibleDC(pDC); // ��������DC���������ڴ�DC
		m_pMemoryBmp->CreateCompatibleBitmap(pDC,m_width,m_height);//��������λͼ
		m_pMemoryCDC->SelectObject(m_pMemoryBmp); //��λͼѡ����ڴ�DC

		m_graph = new Gdiplus::Graphics(m_pMemoryCDC->m_hDC); // GDI��ͼ����
		m_graph->SetSmoothingMode(Gdiplus::SmoothingModeHighSpeed); // �����������飨���Ļ��ʣ�

		ReleaseDC(pDC); // �ͷ�pDC���ڴ�
		isAllocatedMemory=true;
	}else{
		m_graph->ReleaseHDC(m_pMemoryCDC->m_hDC); // GDI��ͼ�����ͷ�CDC handler
		delete_pointer(m_graph);

		m_pMemoryCDC->DeleteDC(); // ɾ��CDC
		delete_pointer(m_pMemoryCDC);

		m_pMemoryBmp->DeleteObject(); //ɾ��λͼ
		delete_pointer(m_pMemoryBmp); // ɾ�����ڻ�����ͼ���ڴ�DC

		delete_pointer(m_pBmpFile);

		isAllocatedMemory=false;
	}
}

BOOL CStaticImage::OnEraseBkgnd(CDC* pDC)
{
	// �ػ����� http://blog.csdn.net/rushkid02/article/details/9733117
	return TRUE;
}

Gdiplus::Bitmap* CStaticImage::FilenameToBitmap(const CString& filename)
{
	// Gdiplus::Bitmap::FromFile���Handle�������ͼ���ļ���ʹ��IStream�������Խ�����lixingcong@20181105

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
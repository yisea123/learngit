/*
 * StaticImageAxis.cpp
 *
 * һ��StaticImage�������࣬��ʾ��Ķ�̬ͼ
 *
 * Created on: 2018��4��17������10:24:49
 * Author: lixingcong
 */

#include "StdAfx.h"
#include "StaticImageAxis.h"
#include <cmath>
#include "Utils.h"

IMPLEMENT_DYNAMIC(CStaticImageAxis, CStatic)

const CStaticImageAxis::MACHINE_TYPE_PROP_T CStaticImageAxis::machine_type_prop_wanneng={
	CStaticImageAxis::MACHINE_TYPE_WANNENG,
	8,
	{"X1","X2","X3","X4","X5","X6","X7","X8"}
};

const CStaticImageAxis::MACHINE_TYPE_PROP_T CStaticImageAxis::machine_type_prop_yahuang={
	CStaticImageAxis::MACHINE_TYPE_YAHUANG,
	4,
	{"���е�","�⾶1","�⾶2","���е�"}
};

CStaticImageAxis::CStaticImageAxis(void)
:CStaticImage()
{
	m_scale_ratio=1.0;
	m_activeAxis=0;
	memset(m_axisAngles,0,sizeof(m_axisAngles));
	memset(m_axisAnglesLast,0,sizeof(m_axisAnglesLast));
	memset(p_axisName,0,sizeof(p_axisName));
	m_prop=nullptr;
}


CStaticImageAxis::~CStaticImageAxis(void)
{
}

BEGIN_MESSAGE_MAP(CStaticImageAxis, CStatic)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

void CStaticImageAxis::drawPicture(int x, int y, int width, int height)//������ͼ����
{
	if(false==isSetPicture)
		return;

	if(false==isAllocatedMemory)
		return;

	if(nullptr != m_prop){
		switch(m_prop->machine_type){
		case MACHINE_TYPE_WANNENG:
			drawPicture_wanneng(x,y,width,height);break;
		case MACHINE_TYPE_YAHUANG:
			drawPicture_yahuang(x,y,width,height);break;
		default:break;
		}
	}
}

void CStaticImageAxis::drawPicture_wanneng(int x, int y, int width, int height)
{
	if(FALSE == TryEnterCriticalSection(&cs_redraw)){
		return;
	}

	static const double PI = std::acos(-1.0);

	CDC* pDC = GetDC(); // ע���ͷ�pDC���ڴ�

	m_pMemoryCDC->FillSolidRect(x,y,width,height,pDC->GetBkColor()); //��ԭ���������ͻ�������Ȼ���Ǻ�ɫ

	// GDIPlus��ת��http://www.cppblog.com/wxtnote/archive/2015/03/18/210095.html
	Gdiplus::Matrix matrix;
	Gdiplus::PointF pointCenter(m_pos_center[0],m_pos_center[1]); // ��ת����

	if(NULL != m_pBmpFile)
		m_graph->DrawImage(m_pBmpFile,
					//Ŀ��λ��
					Gdiplus::Rect(x,y,width,height),
					//ͼƬ�ļ���
					0,0,m_pBmpFile->GetWidth(),m_pBmpFile->GetHeight(),
					Gdiplus::UnitPixel
					);

	// ========================================
#define GDI_MAX_POINTS 5
	Gdiplus::REAL radius;
	static const Gdiplus::REAL CONST_OFFSET_Y_CIRCLE_SERVO=90.0;
	Gdiplus::PointF points[GDI_MAX_POINTS * 2];
	static const Gdiplus::REAL CONST_SOCKET_POSTIONS[GDI_MAX_POINTS][2]={ // (x,y)���꣬��������ֵ
		{5, 15},
		{12, 33},
		{12, 70},
		{15, 70},
		{15, 75}
	};
	static const Gdiplus::REAL CONST_LINE_POSTIONS[2][2]={ // (x,y)���꣬������������ֵ
		{4, 20},
		{4, 70}
	};
	Gdiplus::PointF p1,p2,p3,p4; // ��ֱ�ߵĵ�
#define PEN_WIDTH 4.0f // ��ֱ�ߵĿ��
#define INTERVAL_PIXEL_POINTS_SOCKET 6 // ��������СԲȦ�ļ������
	int point_offset; // ��������СԲȦ��Y����ƫ��
	int point_max_offset; // ��������СԲȦ��Y����ƫ�Ƶ����ֵ
#define POINTS_DAOJIA_PER_SIZE 4 // ���ܵĵ���������ϵ���࣬����X��ԳƵ�һ�ࣩ
#define DAOJIA_MIN_OFFSET 20.0f // ���ܵ�Y������Сƫ��
#define DAOJIA_MAX_OFFSET 40.0f // ���ܵ�Y�������ƫ��
	const Gdiplus::REAL daojia_run_length=(DAOJIA_MAX_OFFSET-DAOJIA_MIN_OFFSET) * m_scale_ratio; // �������г���
	static const Gdiplus::REAL CONST_DAOJIAO_POSTIONS[POINTS_DAOJIA_PER_SIZE][2]={ // (x,y)���꣬���ܵ�����ֵ
		{4, 0},
		{8, 5},
		{8, 35},
		{4, 40}
	};
	
	static const Gdiplus::REAL CONST_TEXT_POSTIONS[2]={10,100}; // ����λ��(x,y)

#define DAOJIA_TAIL_LENGTH 20.0f // ��������β��ĳ���

	// ��ɫ����
	static const Gdiplus::Color COLOR_BLACK(255, 0, 0, 0);
	static const Gdiplus::Color COLOR_WHITE(255, 0xff, 0xff, 0xff);
	static const Gdiplus::Color COLOR_GRAY(255, 0xc8, 0xc8, 0xc8);
	static const Gdiplus::Color COLOR_GRAYDARK(255, 0x4b, 0x4b, 0x4b);
	static const Gdiplus::Color COLOR_GRAY_TRANS(220, 0xa8, 0xa8, 0xa8);
	static const Gdiplus::Color COLOR_BLUE_TRANS(220, 0x1e, 0x90, 0xff);
	static const Gdiplus::Color COLOR_GREENLIGHT(255, 0x66, 0xff, 0xcc);
	static const Gdiplus::Color COLOR_BLUELIGHT(255, 0x99, 0xcc, 0xff);

	// ��ˢ����
	const Gdiplus::SolidBrush BRUSH_CIRCLE_CENTER1(COLOR_BLACK);
	const Gdiplus::SolidBrush BRUSH_CIRCLE_CENTER2(COLOR_GRAY);
	const Gdiplus::SolidBrush BRUSH_CIRCLE_SERVO1(COLOR_BLACK);
	const Gdiplus::SolidBrush BRUSH_CIRCLE_SERVO2(COLOR_GRAY);
	const Gdiplus::SolidBrush BRUSH_SOCKET(COLOR_BLACK);
	const Gdiplus::SolidBrush BRUSH_RAILWAY_POINT(COLOR_WHITE);
	const Gdiplus::SolidBrush BRUSH_DAOJIA(COLOR_GRAY_TRANS);
	const Gdiplus::SolidBrush BRUSH_DAOJIA_HIGHLIGHT(COLOR_BLUE_TRANS);

	// �ֱʳ���
	const Gdiplus::Pen PEN_RAILWAY(COLOR_GRAYDARK, PEN_WIDTH * m_scale_ratio);
	const Gdiplus::Pen PEN_SOCKET_TAIL(COLOR_GRAY, PEN_WIDTH * m_scale_ratio);
	const Gdiplus::Pen PEN_SOCKET_TAIL_HIGHLIGHT(COLOR_BLUE_TRANS, PEN_WIDTH * m_scale_ratio);

	// ��������
	const Gdiplus::FontFamily  font_family(L"Arial");
	Gdiplus::Font font(&font_family,12*m_scale_ratio, Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);

	// Բ������
	radius=20.0f * m_scale_ratio;
	m_graph->FillEllipse(&BRUSH_CIRCLE_CENTER1, (int)(m_pos_center[0]-radius/2), (int)(m_pos_center[1]-radius/2), (int)radius, (int)radius);
	radius=10.0f * m_scale_ratio;
	m_graph->FillEllipse(&BRUSH_CIRCLE_CENTER2, (int)(m_pos_center[0]-radius/2), (int)(m_pos_center[1]-radius/2), (int)radius, (int)radius);
	
	// �˸������λ�����
	for(int axis=1;axis<=m_prop->axis_count;axis++){
		// Բ�����
		radius=20 * m_scale_ratio;
		m_graph->FillEllipse(&BRUSH_CIRCLE_SERVO1, (int)(m_pos_center[0]-radius/2), (int)(m_pos_center[1]-radius/2-CONST_OFFSET_Y_CIRCLE_SERVO * m_scale_ratio), (int)radius, (int)radius);
		radius=8 * m_scale_ratio;
		m_graph->FillEllipse(&BRUSH_CIRCLE_SERVO2, (int)(m_pos_center[0]-radius/2), (int)(m_pos_center[1]-radius/2-CONST_OFFSET_Y_CIRCLE_SERVO * m_scale_ratio), (int)radius, (int)radius);

		// ��
		for(int i=0;i<GDI_MAX_POINTS;i++){
			points[i].X=m_pos_center[0]+(CONST_SOCKET_POSTIONS[i][0] * m_scale_ratio);
			points[i].Y=m_pos_center[1]-(CONST_SOCKET_POSTIONS[i][1] * m_scale_ratio);

			points[2*GDI_MAX_POINTS-i-1].X=m_pos_center[0]-(CONST_SOCKET_POSTIONS[i][0] * m_scale_ratio);
			points[2*GDI_MAX_POINTS-i-1].Y=m_pos_center[1]-(CONST_SOCKET_POSTIONS[i][1] * m_scale_ratio);
		}

		// Fill the polygon.
		m_graph->FillPolygon(&BRUSH_SOCKET, points, GDI_MAX_POINTS * 2);

		// ���(��������)
		p1.X=m_pos_center[0]+CONST_LINE_POSTIONS[0][0] * m_scale_ratio;
		p1.Y=m_pos_center[1]-CONST_LINE_POSTIONS[0][1] * m_scale_ratio;
		p2.X=m_pos_center[0]+CONST_LINE_POSTIONS[1][0] * m_scale_ratio;
		p2.Y=m_pos_center[0]-CONST_LINE_POSTIONS[1][1] * m_scale_ratio;

		m_graph->DrawLine(&PEN_RAILWAY,p1,p2);

		p3=p1;
		p4=p2;
		p3.X=m_pos_center[0]-CONST_LINE_POSTIONS[0][0] * m_scale_ratio;
		p4.X=m_pos_center[0]-CONST_LINE_POSTIONS[1][0] * m_scale_ratio;
		m_graph->DrawLine(&PEN_RAILWAY,p3,p4);

		// ����ϵĵ�
		radius=3 * m_scale_ratio;
		point_max_offset=static_cast<int>(p1.Y-p2.Y);
		for(int i_point=0;/*i_point<5*/;i_point++){
			point_offset=static_cast<int>(INTERVAL_PIXEL_POINTS_SOCKET*m_scale_ratio) * (i_point+1);
			if(point_offset >= point_max_offset)
				break;

			m_graph->FillEllipse(&BRUSH_RAILWAY_POINT, (int)(p1.X-radius/2), (int)(p1.Y-radius/2-point_offset), (int)radius, (int)radius);
			m_graph->FillEllipse(&BRUSH_RAILWAY_POINT, (int)(p3.X-radius/2), (int)(p3.Y-radius/2-point_offset), (int)radius, (int)radius);
		}

		// ��������
		for(int i=0;i<POINTS_DAOJIA_PER_SIZE;i++){
			points[i].X=m_pos_center[0]+(CONST_DAOJIAO_POSTIONS[i][0] * m_scale_ratio);
			points[i].Y=m_pos_center[1]-(CONST_DAOJIAO_POSTIONS[i][1] * m_scale_ratio);

			points[2*POINTS_DAOJIA_PER_SIZE-i-1].X=m_pos_center[0]-(CONST_DAOJIAO_POSTIONS[i][0] * m_scale_ratio);
			points[2*POINTS_DAOJIA_PER_SIZE-i-1].Y=m_pos_center[1]-(CONST_DAOJIAO_POSTIONS[i][1] * m_scale_ratio);
		
			// ��СYƫ��
			points[i].Y-=DAOJIA_MIN_OFFSET*m_scale_ratio;
			points[2*POINTS_DAOJIA_PER_SIZE-i-1].Y-=DAOJIA_MIN_OFFSET*m_scale_ratio;

			// ��ҡ�Ƕ�ƫ��
			points[i].Y-=(daojia_run_length/2 + (Gdiplus::REAL)0.5*daojia_run_length*(Gdiplus::REAL)std::cos(m_axisAngles[axis-1] / 180 * PI));
			points[2*POINTS_DAOJIA_PER_SIZE-i-1].Y-=(daojia_run_length/2 + (Gdiplus::REAL)0.5*daojia_run_length*(Gdiplus::REAL)std::cos(m_axisAngles[axis-1] / 180 * PI));
		}

		// Fill the polygon.
		if(m_activeAxis>0 && m_activeAxis == axis){
			m_graph->FillPolygon(&BRUSH_DAOJIA_HIGHLIGHT, points, POINTS_DAOJIA_PER_SIZE * 2);
		}else{
			m_graph->FillPolygon(&BRUSH_DAOJIA, points, POINTS_DAOJIA_PER_SIZE * 2);
		}

		// ��������β��
		p1.X=m_pos_center[0];
		p1.Y=points[POINTS_DAOJIA_PER_SIZE-1].Y;
		p2.X=m_pos_center[0];
		p2.Y=points[POINTS_DAOJIA_PER_SIZE-1].Y-DAOJIA_TAIL_LENGTH * m_scale_ratio;
		if(m_activeAxis>0 && m_activeAxis == axis)
			m_graph->DrawLine(&PEN_SOCKET_TAIL_HIGHLIGHT,p1,p2);
		else
			m_graph->DrawLine(&PEN_SOCKET_TAIL,p1,p2);
		radius=8 * m_scale_ratio;
		m_graph->FillEllipse(&BRUSH_RAILWAY_POINT, (int)(p2.X-radius/2), (int)(p2.Y-radius/2), (int)radius, (int)radius);

		// ����
		p1.X=m_pos_center[0];
		p1.Y=points[0].Y;
		p2.X=m_pos_center[0];
		p2.Y=points[0].Y-DAOJIA_TAIL_LENGTH * m_scale_ratio;
		m_graph->DrawLine(&PEN_RAILWAY,p1,p2);

		// ������
		if(nullptr != p_axisName[axis-1]){
			p1.X=m_pos_center[0]+CONST_TEXT_POSTIONS[0] * m_scale_ratio;
			p1.Y=m_pos_center[1]-CONST_TEXT_POSTIONS[1] * m_scale_ratio;
			m_graph->DrawString(CString(p_axisName[axis-1]), -1, &font, p1, &BRUSH_CIRCLE_CENTER1);
		}

		// �������ת45��
		matrix.RotateAt(45.0f, pointCenter);
		m_graph->SetTransform(&matrix);
	}

	// ========================================
	pDC->BitBlt(x,y,width,height,m_pMemoryCDC,x,y,SRCCOPY);//���ڴ�DC�ϵ�ͼ�󿽱���ǰ̨
	// =========================================
	ReleaseDC(pDC);

	LeaveCriticalSection(&cs_redraw); // �˳�
}

void CStaticImageAxis::drawPicture_yahuang(int x, int y, int width, int height)
{
	if(FALSE == TryEnterCriticalSection(&cs_redraw)){
		return;
	}

	static const double PI = std::acos(-1.0);

	CDC* pDC = GetDC(); // ע���ͷ�pDC���ڴ�

	m_pMemoryCDC->FillSolidRect(x,y,width,height,pDC->GetBkColor()); //��ԭ���������ͻ�������Ȼ���Ǻ�ɫ

	// GDIPlus��ת��http://www.cppblog.com/wxtnote/archive/2015/03/18/210095.html
	Gdiplus::Matrix matrix;
	Gdiplus::PointF pointCenter(m_pos_center[0],m_pos_center[1]); // ��ת����

	if(NULL != m_pBmpFile)
		m_graph->DrawImage(m_pBmpFile,
					//Ŀ��λ��
					Gdiplus::Rect(x,y,width,height),
					//ͼƬ�ļ���
					0,0,m_pBmpFile->GetWidth(),m_pBmpFile->GetHeight(),
					Gdiplus::UnitPixel
					);

	// ========================================
#define GDI_MAX_POINTS 5
	Gdiplus::REAL radius;
	static const Gdiplus::REAL CONST_OFFSET_Y_CIRCLE_SERVO=90.0;
	Gdiplus::PointF points[GDI_MAX_POINTS * 2];
	static const Gdiplus::REAL CONST_SOCKET_POSTIONS[GDI_MAX_POINTS][2]={ // (x,y)���꣬��������ֵ
		{5, 15},
		{12, 33},
		{12, 70},
		{15, 70},
		{15, 75}
	};
	static const Gdiplus::REAL CONST_LINE_POSTIONS[2][2]={ // (x,y)���꣬������������ֵ
		{4, 20},
		{4, 70}
	};
	Gdiplus::PointF p1,p2,p3,p4; // ��ֱ�ߵĵ�
#define PEN_WIDTH 4.0f // ��ֱ�ߵĿ��
#define INTERVAL_PIXEL_POINTS_SOCKET 6 // ��������СԲȦ�ļ������
	int point_offset; // ��������СԲȦ��Y����ƫ��
	int point_max_offset; // ��������СԲȦ��Y����ƫ�Ƶ����ֵ
#define POINTS_DAOJIA_PER_SIZE 4 // ���ܵĵ���������ϵ���࣬����X��ԳƵ�һ�ࣩ
#define DAOJIA_MIN_OFFSET 20.0f // ���ܵ�Y������Сƫ��
#define DAOJIA_MAX_OFFSET 40.0f // ���ܵ�Y�������ƫ��
	static const Gdiplus::REAL daojia_run_length=(DAOJIA_MAX_OFFSET-DAOJIA_MIN_OFFSET) * m_scale_ratio; // �������г���
	static const Gdiplus::REAL CONST_DAOJIAO_POSTIONS[POINTS_DAOJIA_PER_SIZE][2]={ // (x,y)���꣬���ܵ�����ֵ
		{4, 0},
		{8, 5},
		{8, 35},
		{4, 40}
	};
#define DAOJIA_TAIL_LENGTH 20.0f // ��������β��ĳ���

	// ��ɫ����
	static const Gdiplus::Color COLOR_BLACK(255, 0, 0, 0);
	static const Gdiplus::Color COLOR_WHITE(255, 0xff, 0xff, 0xff);
	static const Gdiplus::Color COLOR_GRAY(255, 0xc8, 0xc8, 0xc8);
	static const Gdiplus::Color COLOR_GRAYDARK(255, 0x4b, 0x4b, 0x4b);
	static const Gdiplus::Color COLOR_GRAY_TRANS(220, 0xa8, 0xa8, 0xa8);
	static const Gdiplus::Color COLOR_BLUE_TRANS(220, 0x1e, 0x90, 0xff);
	static const Gdiplus::Color COLOR_GREENLIGHT(255, 0x66, 0xff, 0xcc);
	static const Gdiplus::Color COLOR_BLUELIGHT(255, 0x99, 0xcc, 0xff);

	// ��ˢ����
	const Gdiplus::SolidBrush BRUSH_CIRCLE_CENTER1(COLOR_BLACK);
	const Gdiplus::SolidBrush BRUSH_CIRCLE_CENTER2(COLOR_GRAY);
	const Gdiplus::SolidBrush BRUSH_CIRCLE_SERVO1(COLOR_BLACK);
	const Gdiplus::SolidBrush BRUSH_CIRCLE_SERVO2(COLOR_GRAY);
	const Gdiplus::SolidBrush BRUSH_SOCKET(COLOR_BLACK);
	const Gdiplus::SolidBrush BRUSH_RAILWAY_POINT(COLOR_WHITE);
	const Gdiplus::SolidBrush BRUSH_DAOJIA(COLOR_GRAY_TRANS);
	const Gdiplus::SolidBrush BRUSH_DAOJIA_HIGHLIGHT(COLOR_BLUE_TRANS);
	const Gdiplus::SolidBrush BRUSH_SONGXIAN_CENTER(COLOR_GRAYDARK);

	// �ֱʳ���
	const Gdiplus::Pen PEN_RAILWAY(COLOR_GRAYDARK, PEN_WIDTH * m_scale_ratio);
	const Gdiplus::Pen PEN_SOCKET_TAIL(COLOR_GRAY, PEN_WIDTH * m_scale_ratio);
	const Gdiplus::Pen PEN_SOCKET_TAIL_HIGHLIGHT(COLOR_BLUE_TRANS, PEN_WIDTH * m_scale_ratio);

	// ��������
	const Gdiplus::FontFamily  font_family(L"Arial");
	Gdiplus::Font font(&font_family,12*m_scale_ratio, Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);

	// ��������
	static const Gdiplus::REAL CONST_TEXT_POSTIONS[4][2]={
		{-60, 100},
		{-60, -80},
		{70, 90},
		{70, -80}
	};

	for(int i=0;i<m_prop->axis_count;++i){
		if(nullptr == p_axisName[i])
			continue;

		p1.X= m_pos_center[0] + CONST_TEXT_POSTIONS[i][0] * m_scale_ratio;
		p1.Y= m_pos_center[1] - CONST_TEXT_POSTIONS[i][1] * m_scale_ratio;

		m_graph->DrawString(CString(p_axisName[i]), -1, &font, p1, &BRUSH_CIRCLE_CENTER1);
	}

	// Բ������
	radius=20.0f * m_scale_ratio;
	m_graph->FillEllipse(&BRUSH_CIRCLE_CENTER1, (int)(m_pos_center[0]-radius/2), (int)(m_pos_center[1]-radius/2), (int)radius, (int)radius);
	radius=10.0f * m_scale_ratio;
	m_graph->FillEllipse(&BRUSH_CIRCLE_CENTER2, (int)(m_pos_center[0]-radius/2), (int)(m_pos_center[1]-radius/2), (int)radius, (int)radius);

	// ���ߵļ���Բ
#define SONGXIAN_CIRCLES_COUNT 4 // ���ٸ�Բ
#define SONGXIAN_START_OFFSET_X 25
#define SONGXIAN_START_OFFSET_Y 8
#define SONGXIAN_SPRING_WIDTH 15
#define SONGXIAN_SPRING_HEIGHT 15
	for(int i=0;i<SONGXIAN_CIRCLES_COUNT;++i){
		// �˸����ּн�����
		Gdiplus::REAL radius_big=18.0f * m_scale_ratio;
		int circle_x,circle_y,circle_Y;

		circle_x=static_cast<int>(m_pos_center[0]-SONGXIAN_START_OFFSET_X * m_scale_ratio - radius_big*i);
		circle_y=static_cast<int>(m_pos_center[1]-SONGXIAN_START_OFFSET_Y * m_scale_ratio);
		circle_Y=static_cast<int>(m_pos_center[1]+SONGXIAN_START_OFFSET_Y * m_scale_ratio);

		// ��ͼ���ɣ������¹��ּн�
		int spring_width=(int)(SONGXIAN_SPRING_WIDTH * m_scale_ratio);
		int spring_height=(int)(SONGXIAN_SPRING_HEIGHT * m_scale_ratio);

		m_graph->FillRectangle(&BRUSH_SONGXIAN_CENTER, (int)(circle_x-spring_width/2), (int)(circle_y)-spring_height, spring_width, spring_height);

		// ���ֵ�Բ
		m_graph->FillEllipse(&BRUSH_CIRCLE_CENTER1, (int)(circle_x-radius_big/2), (int)(circle_y-radius_big/2), (int)radius_big, (int)radius_big);
		m_graph->FillEllipse(&BRUSH_CIRCLE_CENTER1, (int)(circle_x-radius_big/2), (int)(circle_Y-radius_big/2), (int)radius_big, (int)radius_big);

		radius=radius_big/2;
		m_graph->FillEllipse(&BRUSH_SONGXIAN_CENTER, (int)(circle_x-radius/2), (int)(circle_y-radius/2), (int)radius, (int)radius);
		m_graph->FillEllipse(&BRUSH_SONGXIAN_CENTER, (int)(circle_x-radius/2), (int)(circle_Y-radius/2), (int)radius, (int)radius);
	}
	
	// �˸�45�� ���λ�����
	int axis=1;
	for(int rotate_i=1;rotate_i<=8;rotate_i++){
		if(rotate_i != 1 && rotate_i != 2 && rotate_i != 4 && rotate_i != 5){
			// ���������ᣬ��ת45��
			matrix.RotateAt(45.0f, pointCenter);
			m_graph->SetTransform(&matrix);
			continue;
		}

		// Բ�����
		radius=20 * m_scale_ratio;
		m_graph->FillEllipse(&BRUSH_CIRCLE_SERVO1, (int)(m_pos_center[0]-radius/2), (int)(m_pos_center[1]-radius/2-CONST_OFFSET_Y_CIRCLE_SERVO * m_scale_ratio), (int)radius, (int)radius);
		radius=8 * m_scale_ratio;
		m_graph->FillEllipse(&BRUSH_CIRCLE_SERVO2, (int)(m_pos_center[0]-radius/2), (int)(m_pos_center[1]-radius/2-CONST_OFFSET_Y_CIRCLE_SERVO * m_scale_ratio), (int)radius, (int)radius);

		// ��
		for(int i=0;i<GDI_MAX_POINTS;i++){
			points[i].X=m_pos_center[0]+(CONST_SOCKET_POSTIONS[i][0] * m_scale_ratio);
			points[i].Y=m_pos_center[1]-(CONST_SOCKET_POSTIONS[i][1] * m_scale_ratio);

			points[2*GDI_MAX_POINTS-i-1].X=m_pos_center[0]-(CONST_SOCKET_POSTIONS[i][0] * m_scale_ratio);
			points[2*GDI_MAX_POINTS-i-1].Y=m_pos_center[1]-(CONST_SOCKET_POSTIONS[i][1] * m_scale_ratio);
		}

		// Fill the polygon.
		m_graph->FillPolygon(&BRUSH_SOCKET, points, GDI_MAX_POINTS * 2);

		// ���(��������)
		p1.X=m_pos_center[0]+CONST_LINE_POSTIONS[0][0] * m_scale_ratio;
		p1.Y=m_pos_center[1]-CONST_LINE_POSTIONS[0][1] * m_scale_ratio;
		p2.X=m_pos_center[0]+CONST_LINE_POSTIONS[1][0] * m_scale_ratio;
		p2.Y=m_pos_center[0]-CONST_LINE_POSTIONS[1][1] * m_scale_ratio;

		m_graph->DrawLine(&PEN_RAILWAY,p1,p2);

		p3=p1;
		p4=p2;
		p3.X=m_pos_center[0]-CONST_LINE_POSTIONS[0][0] * m_scale_ratio;
		p4.X=m_pos_center[0]-CONST_LINE_POSTIONS[1][0] * m_scale_ratio;
		m_graph->DrawLine(&PEN_RAILWAY,p3,p4);

		// ����ϵĵ�
		radius=3 * m_scale_ratio;
		point_max_offset=static_cast<int>(p1.Y-p2.Y);
		for(int i_point=0;/*i_point<5*/;i_point++){
			point_offset=static_cast<int>(INTERVAL_PIXEL_POINTS_SOCKET*m_scale_ratio) * (i_point+1);
			if(point_offset >= point_max_offset)
				break;

			m_graph->FillEllipse(&BRUSH_RAILWAY_POINT, (int)(p1.X-radius/2), (int)(p1.Y-radius/2-point_offset), (int)radius, (int)radius);
			m_graph->FillEllipse(&BRUSH_RAILWAY_POINT, (int)(p3.X-radius/2), (int)(p3.Y-radius/2-point_offset), (int)radius, (int)radius);
		}

		// ��������
		for(int i=0;i<POINTS_DAOJIA_PER_SIZE;i++){
			points[i].X=m_pos_center[0]+(CONST_DAOJIAO_POSTIONS[i][0] * m_scale_ratio);
			points[i].Y=m_pos_center[1]-(CONST_DAOJIAO_POSTIONS[i][1] * m_scale_ratio);

			points[2*POINTS_DAOJIA_PER_SIZE-i-1].X=m_pos_center[0]-(CONST_DAOJIAO_POSTIONS[i][0] * m_scale_ratio);
			points[2*POINTS_DAOJIA_PER_SIZE-i-1].Y=m_pos_center[1]-(CONST_DAOJIAO_POSTIONS[i][1] * m_scale_ratio);
		
			// ��СYƫ��
			points[i].Y-=DAOJIA_MIN_OFFSET*m_scale_ratio;
			points[2*POINTS_DAOJIA_PER_SIZE-i-1].Y-=DAOJIA_MIN_OFFSET*m_scale_ratio;

			// ��ҡ�Ƕ�ƫ��
			points[i].Y-=(daojia_run_length/2 + (Gdiplus::REAL)0.5*daojia_run_length*(Gdiplus::REAL)std::cos(m_axisAngles[axis-1] / 180 * PI));
			points[2*POINTS_DAOJIA_PER_SIZE-i-1].Y-=(daojia_run_length/2 + (Gdiplus::REAL)0.5*daojia_run_length*(Gdiplus::REAL)std::cos(m_axisAngles[axis-1] / 180 * PI));
		}

		// Fill the polygon.
		if(m_activeAxis>0 && m_activeAxis == axis){
			m_graph->FillPolygon(&BRUSH_DAOJIA_HIGHLIGHT, points, POINTS_DAOJIA_PER_SIZE * 2);
		}else{
			m_graph->FillPolygon(&BRUSH_DAOJIA, points, POINTS_DAOJIA_PER_SIZE * 2);
		}

		// ��������β��
		p1.X=m_pos_center[0];
		p1.Y=points[POINTS_DAOJIA_PER_SIZE-1].Y;
		p2.X=m_pos_center[0];
		p2.Y=points[POINTS_DAOJIA_PER_SIZE-1].Y-DAOJIA_TAIL_LENGTH * m_scale_ratio;
		if(m_activeAxis>0 && m_activeAxis == axis)
			m_graph->DrawLine(&PEN_SOCKET_TAIL_HIGHLIGHT,p1,p2);
		else
			m_graph->DrawLine(&PEN_SOCKET_TAIL,p1,p2);
		radius=8 * m_scale_ratio;
		m_graph->FillEllipse(&BRUSH_RAILWAY_POINT, (int)(p2.X-radius/2), (int)(p2.Y-radius/2), (int)radius, (int)radius);

		// ����
		p1.X=m_pos_center[0];
		p1.Y=points[0].Y;
		p2.X=m_pos_center[0];
		p2.Y=points[0].Y-DAOJIA_TAIL_LENGTH * m_scale_ratio;
		m_graph->DrawLine(&PEN_RAILWAY,p1,p2);

		// �������ת45��
		matrix.RotateAt(45.0f, pointCenter);
		m_graph->SetTransform(&matrix);

		// ��һ����ͼ��
		++axis;
	}

	// ========================================
	pDC->BitBlt(x,y,width,height,m_pMemoryCDC,x,y,SRCCOPY);//���ڴ�DC�ϵ�ͼ�󿽱���ǰ̨
	// =========================================
	ReleaseDC(pDC);

	LeaveCriticalSection(&cs_redraw); // �˳�
}

void CStaticImageAxis::setScaleRatio(double ratio)
{
	m_scale_ratio=(Gdiplus::REAL)ratio;
	Invalidate();
}


void CStaticImageAxis::setActiveAxis(int axisNum)
{
	m_activeAxis=axisNum;
	Invalidate();
}

void CStaticImageAxis::updateAxisAngle(double* angles, int axisCount)
{
	if(axisCount > IMAGE_AXIS_MAX_COUNT){
		debug_printf("error in updateAxisAngle: axisCount > IMAGE_AXIS_MAX_COUNT");
		return;
	}
	
	bool isChanged=false;

	for(int i=0;i<axisCount;i++){
		if(*(angles+i) != m_axisAnglesLast[i]){
			m_axisAngles[i]=(Gdiplus::REAL)(*(angles+i));
			m_axisAnglesLast[i]=*(angles+i);
			isChanged=true;
		}
	}
	
	if(isChanged) // ���ݸı���ˢ����Ļ
		Invalidate();
}

void CStaticImageAxis::setPicture(int picID, int machine_type, int x, int y, int width, int height)
{
	CStaticImage::setPicture(picID,x,y,width,height);
	setMachineType(machine_type);//���õ�������
}

void CStaticImageAxis::setPicture(CString picFilename, int machine_type, int x, int y, int width, int height)
{
	CStaticImage::setPicture(picFilename,x,y,width,height);
	setMachineType(machine_type);
}

void CStaticImageAxis::setAxisName(char* name, int index)
{
	if(index < 0 || index >= IMAGE_AXIS_MAX_COUNT)
		return;

	p_axisName[index]=name;
}

void CStaticImageAxis::setMachineType(int type)
{
	switch(type){
	case MACHINE_TYPE_WANNENG:
		m_prop=&machine_type_prop_wanneng;
		break;
	case MACHINE_TYPE_YAHUANG:
		m_prop=&machine_type_prop_yahuang;
		break;
	default:
		m_prop=nullptr;
		break;
	}
}
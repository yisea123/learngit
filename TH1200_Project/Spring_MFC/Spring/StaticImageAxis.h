/*
 * StaticImageAxis.h
 *
 * һ��StaticImage�������࣬��ʾ��Ķ�̬ͼ
 *
 * Created on: 2018��4��17������10:24:49
 * Author: lixingcong
 */

#pragma once
#include "staticimage.h"

#define IMAGE_AXIS_MAX_COUNT 8 // �������������ǰ��ͼ�õ���������

class CStaticImageAxis :
	public CStaticImage
{
	DECLARE_DYNAMIC(CStaticImageAxis)

public:
	// ��ʾ�Ļ�������
	enum{
		MACHINE_TYPE_WANNENG=0, // ����
		MACHINE_TYPE_YAHUANG, // ѹ��
	};

	// ��������
	typedef struct{
		int machine_type; // ��ʾ�Ļ�������
		int axis_count; // ��ͼ�е���������
		char name[IMAGE_AXIS_MAX_COUNT][30]; // ����
	} MACHINE_TYPE_PROP_T; // Property

public:
	CStaticImageAxis(void);
	~CStaticImageAxis(void);

	void setScaleRatio(double ratio);

	void setActiveAxis(int axisNum);
	void updateAxisAngle(double* angles, int axisCount);

	void setPicture(int picID, int machine_type, int x=0, int y=0, int width=0, int height=0);
	void setPicture(CString picFilename, int machine_type, int x=0, int y=0, int width=0, int height=0);
	
	// nameָ���������Ҫ��ֹ���ڴ���գ���Ϊÿ�λ�ͼ��Ҫ��name��ַȡ��������
	void setAxisName(char* name, int index);

	const MACHINE_TYPE_PROP_T* getMachineType(){return m_prop;};

	static const MACHINE_TYPE_PROP_T machine_type_prop_wanneng;
	static const MACHINE_TYPE_PROP_T machine_type_prop_yahuang;

protected:
	Gdiplus::REAL m_scale_ratio; // ���ű���
	Gdiplus::REAL m_axisAngles[IMAGE_AXIS_MAX_COUNT];
	double m_axisAnglesLast[IMAGE_AXIS_MAX_COUNT]; // ����ˢ��Ƶ�ʵı����ϴε�λ��
	int m_activeAxis; // ѡ���ᣬ��1��ʼ�����һ����ͼ�ᣬ0����û��ѡ��
	const MACHINE_TYPE_PROP_T* m_prop; // ��������
	char* p_axisName[IMAGE_AXIS_MAX_COUNT];

protected: // ��Ϣ����
	DECLARE_MESSAGE_MAP()
	virtual void drawPicture(int x, int y, int width=0, int height=0); // ���Ʋ�ͼ

	void setMachineType(int type);

	void drawPicture_wanneng(int x, int y, int width, int height);
	void drawPicture_yahuang(int x, int y, int width, int height);
};


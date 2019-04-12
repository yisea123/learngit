/*
 * StaticImageAxis.h
 *
 * 一个StaticImage的派生类，显示轴的动态图
 *
 * Created on: 2018年4月17日上午10:24:49
 * Author: lixingcong
 */

#pragma once
#include "staticimage.h"

#define IMAGE_AXIS_MAX_COUNT 8 // 保存了数组代表当前绘图用到的坐标数

class CStaticImageAxis :
	public CStaticImage
{
	DECLARE_DYNAMIC(CStaticImageAxis)

public:
	// 显示的机器类型
	enum{
		MACHINE_TYPE_WANNENG=0, // 万能
		MACHINE_TYPE_YAHUANG, // 压簧
	};

	// 机器属性
	typedef struct{
		int machine_type; // 显示的机器类型
		int axis_count; // 动图中的所需轴数
		char name[IMAGE_AXIS_MAX_COUNT][30]; // 轴名
	} MACHINE_TYPE_PROP_T; // Property

public:
	CStaticImageAxis(void);
	~CStaticImageAxis(void);

	void setScaleRatio(double ratio);

	void setActiveAxis(int axisNum);
	void updateAxisAngle(double* angles, int axisCount);

	void setPicture(int picID, int machine_type, int x=0, int y=0, int width=0, int height=0);
	void setPicture(CString picFilename, int machine_type, int x=0, int y=0, int width=0, int height=0);
	
	// name指向的内容需要防止被内存回收，因为每次绘图都要从name地址取出轴名字
	void setAxisName(char* name, int index);

	const MACHINE_TYPE_PROP_T* getMachineType(){return m_prop;};

	static const MACHINE_TYPE_PROP_T machine_type_prop_wanneng;
	static const MACHINE_TYPE_PROP_T machine_type_prop_yahuang;

protected:
	Gdiplus::REAL m_scale_ratio; // 缩放比率
	Gdiplus::REAL m_axisAngles[IMAGE_AXIS_MAX_COUNT];
	double m_axisAnglesLast[IMAGE_AXIS_MAX_COUNT]; // 减少刷新频率的保存上次的位置
	int m_activeAxis; // 选中轴，从1开始代表第一个动图轴，0代表没有选中
	const MACHINE_TYPE_PROP_T* m_prop; // 机器属性
	char* p_axisName[IMAGE_AXIS_MAX_COUNT];

protected: // 消息函数
	DECLARE_MESSAGE_MAP()
	virtual void drawPicture(int x, int y, int width=0, int height=0); // 绘制彩图

	void setMachineType(int type);

	void drawPicture_wanneng(int x, int y, int width, int height);
	void drawPicture_yahuang(int x, int y, int width, int height);
};


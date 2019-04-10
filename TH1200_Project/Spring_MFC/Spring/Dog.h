/*
 * Dog.h
 *
 * 软件加密狗类
 *
 * Created on: 2018年4月17日上午9:47:27
 * Author: lixingcong
 */

#pragma once

#include "Dog_public.h"

class CDog
{
public:
	CDog(void);
	~CDog(void);

	bool init(); // 初始化狗DOG结构体，即重新生成随机数

	bool writeDog(); // 把DOG结构体写入到下位机
	bool readDog(); // 从下位机读取DOG结构体

	bool validAdminPassword(int pwd);
	short getRemainDays();

	// 厂商序列号，机器序列号
	int getSerialNumber();
	int getFactoryNumber();

	// 狗1专用 begin
	bool unlockDogProgress(); // 让下位机解锁DogProgress
	bool setUserLevel(unsigned short newLevel);
	bool validUserPassword(int pwd, unsigned short* newLevel);
	// 狗1专用 end

	// 狗2专用 begin
	bool writeUserPassword(int pwd);
	// 狗2专用 end

	int getDogProcessCode(); // 当前加密Progress状态

	// 是否锁住用户
	bool isUnlocked();
	void setUnlock(bool isUnlock);

	// 狗1，狗2类型
	void setDogType(int dog_type);
	int getDogType();

	void copyToDog(DOG* destDog);
	void copyFromDog(const DOG* srcDog);

protected:
	bool isReadOk; // 是否已经从下位机读取Dog结构体
	DOG m_dog;
	unsigned short mb_addr_dogProgress; // modbus address
	unsigned short m_dogProcess; // 加密狗限制标志
};


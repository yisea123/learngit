/*
 * Dog.cpp
 *
 * 软件加密狗类
 *
 * Created on: 2018年4月17日上午9:47:27
 * Author: lixingcong
 */

#include "StdAfx.h"
#include "Dog.h"
#include "modbusAddress.h"
#include "Utils.h"

#define UNLOCKED_CODE 12345678

CDog::CDog(void)
{
	isReadOk=false;
}


CDog::~CDog(void)
{
}

bool CDog::init()
{
	if(false==isReadOk)
		return false;

	bool isOk;
	unsigned short dummmy_code=1;

	isOk= (readWriteModbus(&dummmy_code,sizeof(dummmy_code),MODBUS_ADDR_DOG_INIT,false) && // 写入让其初始化
		readDog()); // 重新从下位机读取DOG结构体

	return isOk;
}

bool CDog::readDog()
{
	isReadOk=readWriteModbus(&m_dog,sizeof(m_dog),MODBUS_ADDR_DOG);

	// 狗标志
	if(true == isReadOk){
		mb_addr_dogProgress = (m_dog.Dog_Type == 2 ? MODBUS_ADDR_DOG2_PROGRESS : MODBUS_ADDR_DOG_PROGRESS);
		isReadOk = readWriteModbus(&m_dogProcess,sizeof(m_dogProcess),mb_addr_dogProgress);
	}

	return isReadOk;
}

bool CDog::writeDog()
{
	if(false==isReadOk)
		return false;

	return readWriteModbus(&m_dog,sizeof(m_dog),MODBUS_ADDR_DOG,false);
}

bool CDog::validAdminPassword(int pwd)
{
	if(false==isReadOk)
		return false;

	int pwd_serial, pwd_isetl, pwd_iseth;
	
	pwd_serial=pwd; // 根据输入密码计算出的值
	
	pwd_isetl=(pwd_serial&0x3ff) << 10;
	pwd_iseth=(pwd_serial>>10) & 0x3ff;
	
	pwd_serial>>=20;
	pwd_serial<<=20;

	pwd_serial = (pwd_serial | pwd_iseth | pwd_isetl);
	pwd_serial -= 130509;

	// debug_printf("validAdminPassword=%d\n",pwd_serial);
	
	// 正确的Admin密码有三个
	return (pwd==m_dog.Machine_Password ||  // 1、设定值
		pwd==SuperDogPassW ||  // 2、后门密码
		pwd_serial==m_dog.Machine_SerialNo); // 3、根据输入密码计算出的值与机器序列号对比
}

// 狗1专用 begin
bool CDog::unlockDogProgress()
{
	unsigned short dummmy_code=0;
	return readWriteModbus(&dummmy_code,sizeof(dummmy_code),mb_addr_dogProgress,false);
}

bool CDog::setUserLevel(unsigned short newLevel)
{
	if(false==isReadOk)
		return false;

	if(newLevel>24){
		setUnlock(true);
		return true;
	}

	// 累加天数
	unsigned short freedomDays=0; // 新的使用天数
	unsigned short level=m_dog.User_Level;

	while(level<newLevel){
		freedomDays+=m_dog.User_Days[level-1];
		level++;
	}

	m_dog.User_Level=newLevel;

	if(m_dog.User_RemainDay < 1) // 逾期验证，有效期从当前天开始计算
		m_dog.User_RemainDay=freedomDays;
	else // 提前验证，有效期延长累加
		m_dog.User_RemainDay+=freedomDays;

	return true;
}

bool CDog::validUserPassword(int pwd, unsigned short* unlockedLevel)
{
	if(false==isReadOk)
		return false;

	// 正确的User密码有两个：备用密码和设定值
	int superPwd=(267*m_dog.RandomNum)%1000000; // 计算备用密码

	if(pwd==superPwd){
		*unlockedLevel=m_dog.User_Level;
		return true;
	}

	if(m_dog.User_Level<1 || m_dog.User_Level>24)
		return false;

	for(unsigned short level=m_dog.User_Level; level<=24; ++level){
		if(pwd==m_dog.User_Password[level-1]){
			*unlockedLevel=level;
			return true;
		}
	}

	return false;
}
// 狗1专用 end

// 狗2专用 begin
bool CDog::writeUserPassword(int pwd)
{
	return readWriteModbus(&pwd,sizeof(int),MODBUS_ADDR_DOG2_PWD,false);
}
// 狗2专用 end

int CDog::getDogProcessCode()
{
	return m_dogProcess;
}

short CDog::getRemainDays()
{
	return m_dog.User_RemainDay;
}

void CDog::setUnlock(bool isUnlock)
{
	m_dog.User_Active=isUnlock?UNLOCKED_CODE:0;
}

bool CDog::isUnlocked()
{
	return m_dog.User_Active==UNLOCKED_CODE;
}

void CDog::copyToDog(DOG* destDog)
{
	memcpy(destDog,&m_dog,sizeof(m_dog));
}

void CDog::copyFromDog(const DOG* srcDog)
{
	memcpy(&m_dog,srcDog,sizeof(m_dog));
}

int CDog::getSerialNumber()
{
	return m_dog.Machine_SerialNo;
}

int CDog::getFactoryNumber()
{
	return m_dog.Factory_SerialNo;
}

void CDog::setDogType(int dog_type)
{
	m_dog.Dog_Type=dog_type;
}

int CDog::getDogType()
{
	return m_dog.Dog_Type;
}

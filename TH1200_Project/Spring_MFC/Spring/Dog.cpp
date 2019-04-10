/*
 * Dog.cpp
 *
 * ������ܹ���
 *
 * Created on: 2018��4��17������9:47:27
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

	isOk= (readWriteModbus(&dummmy_code,sizeof(dummmy_code),MODBUS_ADDR_DOG_INIT,false) && // д�������ʼ��
		readDog()); // ���´���λ����ȡDOG�ṹ��

	return isOk;
}

bool CDog::readDog()
{
	isReadOk=readWriteModbus(&m_dog,sizeof(m_dog),MODBUS_ADDR_DOG);

	// ����־
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
	
	pwd_serial=pwd; // ������������������ֵ
	
	pwd_isetl=(pwd_serial&0x3ff) << 10;
	pwd_iseth=(pwd_serial>>10) & 0x3ff;
	
	pwd_serial>>=20;
	pwd_serial<<=20;

	pwd_serial = (pwd_serial | pwd_iseth | pwd_isetl);
	pwd_serial -= 130509;

	// debug_printf("validAdminPassword=%d\n",pwd_serial);
	
	// ��ȷ��Admin����������
	return (pwd==m_dog.Machine_Password ||  // 1���趨ֵ
		pwd==SuperDogPassW ||  // 2����������
		pwd_serial==m_dog.Machine_SerialNo); // 3��������������������ֵ��������кŶԱ�
}

// ��1ר�� begin
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

	// �ۼ�����
	unsigned short freedomDays=0; // �µ�ʹ������
	unsigned short level=m_dog.User_Level;

	while(level<newLevel){
		freedomDays+=m_dog.User_Days[level-1];
		level++;
	}

	m_dog.User_Level=newLevel;

	if(m_dog.User_RemainDay < 1) // ������֤����Ч�ڴӵ�ǰ�쿪ʼ����
		m_dog.User_RemainDay=freedomDays;
	else // ��ǰ��֤����Ч���ӳ��ۼ�
		m_dog.User_RemainDay+=freedomDays;

	return true;
}

bool CDog::validUserPassword(int pwd, unsigned short* unlockedLevel)
{
	if(false==isReadOk)
		return false;

	// ��ȷ��User����������������������趨ֵ
	int superPwd=(267*m_dog.RandomNum)%1000000; // ���㱸������

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
// ��1ר�� end

// ��2ר�� begin
bool CDog::writeUserPassword(int pwd)
{
	return readWriteModbus(&pwd,sizeof(int),MODBUS_ADDR_DOG2_PWD,false);
}
// ��2ר�� end

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

/*
 * Dog.h
 *
 * ������ܹ���
 *
 * Created on: 2018��4��17������9:47:27
 * Author: lixingcong
 */

#pragma once

#include "Dog_public.h"

class CDog
{
public:
	CDog(void);
	~CDog(void);

	bool init(); // ��ʼ����DOG�ṹ�壬���������������

	bool writeDog(); // ��DOG�ṹ��д�뵽��λ��
	bool readDog(); // ����λ����ȡDOG�ṹ��

	bool validAdminPassword(int pwd);
	short getRemainDays();

	// �������кţ��������к�
	int getSerialNumber();
	int getFactoryNumber();

	// ��1ר�� begin
	bool unlockDogProgress(); // ����λ������DogProgress
	bool setUserLevel(unsigned short newLevel);
	bool validUserPassword(int pwd, unsigned short* newLevel);
	// ��1ר�� end

	// ��2ר�� begin
	bool writeUserPassword(int pwd);
	// ��2ר�� end

	int getDogProcessCode(); // ��ǰ����Progress״̬

	// �Ƿ���ס�û�
	bool isUnlocked();
	void setUnlock(bool isUnlock);

	// ��1����2����
	void setDogType(int dog_type);
	int getDogType();

	void copyToDog(DOG* destDog);
	void copyFromDog(const DOG* srcDog);

protected:
	bool isReadOk; // �Ƿ��Ѿ�����λ����ȡDog�ṹ��
	DOG m_dog;
	unsigned short mb_addr_dogProgress; // modbus address
	unsigned short m_dogProcess; // ���ܹ����Ʊ�־
};


/*
 * Ver.h
 *
 *  Created on: 2015��10��14��
 *      Author: Administrator
 */

#ifndef VER_H_
#define VER_H_


//�Ƿ������λ�����ԣ���Ӱ����λ���Ƿ���д�ӡ���ܣ�
//#define	_PC_TEST_	0
#define	_PC_TEST_	1


/*Motion���߰汾����(���ݲ�ͬ�汾�ⶨ��)*/
//#define		ECAT_MOTION	0
#define		ECAT_MOTION	1

/*���Գ���汾����*/
#define		TEST_VER	2.07

#if _PC_TEST_
	#if ECAT_MOTION
		#define	TEST_NAME "BUS Release"
	#else
		#define	TEST_NAME "Release"
	#endif

#else
	#if ECAT_MOTION
		#define	TEST_NAME "BUS Debug"
	#else
		#define	TEST_NAME "Debug"
	#endif
#endif

#endif /* VER_H_ */

/*
 * Ver.h
 *
 *  Created on: 2015年10月14日
 *      Author: Administrator
 */

#ifndef VER_H_
#define VER_H_


//是否进行上位机测试（会影响下位机是否进行打印功能）
//#define	_PC_TEST_	0
#define	_PC_TEST_	1


/*Motion总线版本定义(根据不同版本库定义)*/
//#define		ECAT_MOTION	0
#define		ECAT_MOTION	1

/*测试程序版本定义*/
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

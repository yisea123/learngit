/*
 * modbusAddress.h
 *
 * Modbus地址表
 *
 * Created on: 2017年10月13日下午4:59:19
 * Author: lixingcong
 */

#ifndef SPRING_PUBLIC_MODBUS_ADDR_H_
#define SPRING_PUBLIC_MODBUS_ADDR_H_

#ifdef WIN32
// Modbus地址，来源：THResourceTable.c
enum MODBUS_ADDR_ENUM {
	MODBUS_ADDR_IO_STATUS_IN=1000,
	MODBUS_ADDR_IO_STATUS_OUT=1400,

	MODBUS_ADDR_CURRENT_USER=3000,
	MODBUS_ADDR_CURRENT_DIAGLOG=3001,
	MODBUS_ADDR_PASSWORD_SUPER=3002, // 超级用户密码
	MODBUS_ADDR_PASSWORD_PROGRAMMER=3004, // 调试员密码
	MODBUS_ADDR_PASSWORD_USER=3006, // 一般用户密码
	MODBUS_ADDR_MOTION_INIT_RETURN_VALUE=3010,
	MODBUS_ADDR_HEARTBEAT=3011, // 上下位机通信正常确认值
	MODBUS_ADDR_REGISTER_STATUS=3013,
	MODBUS_ADDR_ALARM_NUM=3930,

	MODBUS_ADDR_REBOOT=3960,
	MODBUS_ADDR_SYS_UPGRADE,
	MODBUS_ADDR_USB_CONNECT,
	MODBUS_ADDR_FTP_CONNECT,

	MODBUS_ADDR_KEY_VALUE=4300,
	MODBUS_ADDR_HANDBOX_SWITCH,
	MODBUS_ADDR_SET_CURRENT_AXIS,
	MODBUS_ADDR_SWITCH_CYLINDER_TEST,
	MODBUS_ADDR_SWITCH_TEST_MODE,
	MODBUS_ADDR_SET_PROBE,
	MODBUS_ADDR_SET_PROBE_FAIL=4307,
	MODBUS_ADDR_SET_PRODUCTION=4309,
	MODBUS_ADDR_SET_DEST_COUNT=4311,
	MODBUS_ADDR_RESET_SONXIAN=4313,
	MODBUS_ADDR_SET_SPEED_RATE=4314,
	MODBUS_ADDR_WORKFILE_PASTE_FILE_STATUS=4315, // 拷贝完毕的查询标志
	MODBUS_ADDR_HANDSPEED_RATE_PERCENT=4316, // 手动速度百分比值
	MODBUS_ADDR_WORKSPEED_RATE=4317,
	MODBUS_ADDR_TEST_AD_VALUE=4318, // 测试旋钮AD读取百分比值
	MODBUS_ADDR_SWITCH_DANBU_MODE=4319,
	MODBUS_ADDR_BACKZERO=4320,
	MODBUS_ADDR_TEACH_KEY_COUNTER=4321, // 手持盒教导按键

	MODBUS_ADDR_ENABLE_ALL_MOTORS=4400,
	MODBUS_ADDR_ENABLE_AXIS_1, // 4401, // each axis takes UINT16*1 space
	MODBUS_ADDR_RENAME_AXIS_1=4600, // each axis takes UINT16*12 space
	MODBUS_ADDR_POSITION_AXIS_1=5000, // each axis takes UINT16*12 space
	MODBUS_ADDR_CUT_WIRE=5200,
	MODBUS_ADDR_KNIFE_BACK=5300, // 退刀

	MODBUS_ADDR_SET_ORIGIN=5099,
	MODBUS_ADDR_HANDMOVE_MANUAL=5100,
	MODBUS_ADDR_IPADDR=5500, // IP设置

	MODBUS_ADDR_DOG=5600,
	MODBUS_ADDR_DOG_PROGRESS=5700,
	MODBUS_ADDR_DOG_SYSTIME=5701,
	//MODBUS_ADDR_DOG_TIME=5705,
	MODBUS_ADDR_DOG_CHECK=5708,
	MODBUS_ADDR_DOG_INIT=5714,

	MODBUS_ADDR_MODEL_1=5715, // 塔簧等模型
	MODBUS_ADDR_MODEL_2=5716,
	MODBUS_ADDR_MODEL_3=5717,
	MODBUS_ADDR_MODEL_4=5718,
	MODBUS_ADDR_MODEL_FINISH=5719,

	MODBUS_ADDR_DOG2=5800,
	MODBUS_ADDR_DOG2_PROGRESS=5850,
	MODBUS_ADDR_DOG2_INIT=5851,
	MODBUS_ADDR_DOG2_PWD=5852,

	MODBUS_ADDR_IOCONFIG_IN=7000,
	MODBUS_ADDR_IOCONFIG_OUT=7500,

	MODBUS_ADDR_WORKFILE_SET_DIR=33100,
	MODBUS_ADDR_WORKFILE_DIR_COUNT=33228,
	MODBUS_ADDR_WORKFILE_FILELIST=20000,
	MODBUS_ADDR_WORKFILE_CREATE_FILE=33230,
	MODBUS_ADDR_WORKFILE_DELETE_FILE=33238,
	MODBUS_ADDR_WORKFILE_COPY_FILE=33246,
	MODBUS_ADDR_WORKFILE_PASTE_FILE=33254,
	
	MODBUS_ADDR_WORKFILE_TOTAL_LINES=33290, // 文件总行数
	MODBUS_ADDR_WORKFILE_RW_LINE_1=34000, // 读写加工文件条目
	MODBUS_ADDR_WORKFILE_RW_LINE_2=34100, // 读写加工文件数据

	MODBUS_ADDR_CONTROLER_VERSION=33270,

	MODBUS_ADDR_PRODUCT_COUNTER=33300,
	MODBUS_ADDR_PRODUCT_RUN_TIME=33302,
	MODBUS_ADDR_PRODUCT_COUNTER_GOOD=33320,

	MODBUS_ADDR_TH_1600E_SYS=33500,
	MODBUS_ADDR_TH_TEACH_COORD=33510,

	MODBUS_ADDR_WORKFILE_LOADING=37000
};

#endif // win32

#endif /* SPRING_PUBLIC_MODBUS_ADDR_H_ */

/*
 * Teach_public.h
 *
 * Created on: 2017年11月11日下午4:03:03
 * Author: lixingcong
 */

#ifndef SPRING_PUBLIC_TEACH_PUBLIC_H_
#define SPRING_PUBLIC_TEACH_PUBLIC_H_

#include "adt_datatype.h"

#ifdef __cplusplus
extern "C" {
#endif

#define		MAXVALUECOL	22
#define		MAXLINE		200//加工文件最多200行
#define		MAXCYLINDER	20//加工文件可编程控制气缸20个  0-19

#define TEACH_NOACTION	0x00	//无操作，仅更新行号
#define TEACH_INSERT		0x01	//插入~~1条发送
#define TEACH_AMEND			0x02	//修改
#define TEACH_DELETE_ALL	0x03	//全部删除
#define TEACH_DELETE		0x04	//逐条删除

/*
 * 教导指令数据结构体
value[0]为指令，value[1]1轴，value[2]2轴，value[3]3轴，value[4]4轴，value[5]5轴
value[6]6轴，value[7]7轴，....... value[16]16轴            value[17]为速比，value[18]为探针，value[19]为气缸，value[20]为延时

value[21]为万能机结束角
*/

typedef struct
{
  INT8S	value[MAXVALUECOL][10];
}TEACH_COMMAND;

//教导数据类型发送指令结构体
typedef struct TEACH_SEND_DATA
{
	INT16U	Type;	//当前操作类型 (删除，添加，插入，修改一条指令)
	INT16U	Num;	//当前操作点索引,从0开始
	//TEACH_COMMAND Data;//要操作的数据
	INT16U	Line_Size;//上位机进行数据操作后的数据行数，发给下位机用作防止因延时等重发的删除，添加指令进行误操作
}TEACH_SEND;




#ifdef __cplusplus
}
#endif

#endif /* SPRING_PUBLIC_TEACH_PUBLIC_H_ */

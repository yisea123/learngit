/*
 * ParamTable.cpp
 *
 * modbus参数表
 *
 * Created on: 2017年12月13日下午9:05:15
 * Author: lixingcong
 */

#include "StdAfx.h"
#include "ParamTable.h"
#include "SysText.h"
#include "Utils.h"
#include "Data.h"
#include "Modbus\ModbusQueue.h"
#include "modbusAddress.h"


ParamTable::ParamTable(void)
{
}


ParamTable::~ParamTable(void)
{
}

void ParamTable::init()
{
	ParaManage_Init();
	int index=0;
	TABINDEX_T item;

	// 载入为自己的数组
	for(int i=0; ParaRegTab[i].DataType!=DT_END; i++){
		switch(ParaRegTab[i].DataType){
		case DT_TITLE:
			index=i;
			//debug_printf("ParamTable:: found title=%s\n",ParaRegTab[i].pDescText);
			break;

		case DT_NONE:
			item.startIndex=index;
			item.count=(i-1)-item.startIndex;
			titleArray.push_back(item);
			break;

		case DT_INT8S:
		case DT_INT8U:
		case DT_INT16S:
		case DT_INT16U:
		case DT_INT32S:
		case DT_INT32U:
		case DT_PPS:
		case DT_FLOAT:
		case DT_DOUBLE:
			map_addr_index[ParaRegTab[i].pData]=i;
			break;

		default:break;
		}
	}
}

int ParamTable::getParaRegTableIndex(void* data)
{
	int offset=-1;
	
	if(map_addr_index.find(data) != map_addr_index.end())
		offset=map_addr_index[data];

	return offset;
}

bool ParamTable::queryAllParams()
{
	bool isOk=true;
	modbusQ_push_t mbq;

	mbq.operation=MODBUSQ_OP_READ_DATA;

	for(size_t tabIndex=0;tabIndex<getTitleArraySize();++tabIndex){//获取每一个title
		auto pt=getTitleArrayItem(tabIndex);

		auto paraRegTableIndex=pt->startIndex+1;//开始向没有用，需要+1
		auto paraRegTableIndexEnd=paraRegTableIndex+pt->count;//count是个数（即总数）

		while(true == isOk && paraRegTableIndex < paraRegTableIndexEnd){//取出 数据、长度、地址
			mbq.data=ParaRegTab[paraRegTableIndex].pData;
			mbq.len=getSizeFromDataType(ParaRegTab[paraRegTableIndex].DataType);
			mbq.start_addr=ParaRegTab[paraRegTableIndex].PlcAddr;

			isOk=g_mbq->sendQuery(mbq,false);//往后发送队列

			//debug_printf("read p=%d\n",paraRegTableIndex);
			++paraRegTableIndex; // next one		
		}

		if(false == isOk)
			break;
	}

	if(isOk)
		isOk=queryAxisNames();

	return isOk;
}

bool ParamTable::queryAxisNames()
{
	bool isOk=true;
	static unsigned char isQueried[AXIS_NUM]={0};

	modbusQ_push_t mbq;
	mbq.len=MAX_LEN_AXIS_NAME;
	mbq.operation=MODBUSQ_OP_READ_DATA;

	for(auto i=0;i<AXIS_NUM;++i){
		if(1==isQueried[i])
			continue;

		mbq.data=g_axisName[i];
		mbq.start_addr=MODBUS_ADDR_RENAME_AXIS_1+(int)(MAX_LEN_AXIS_NAME/2)*i;

		isOk=g_mbq->sendQuery(mbq,false);

		if(false==isOk){
			break;
		}else{
			isQueried[i]=1; // 避免重复加载
		}
	}

	return isOk;
}

size_t ParamTable::getSizeFromDataType(unsigned char type)
{
	size_t size=0;
	size=(type&0xf0)>>4; // 取高四位
	return size;
}

size_t ParamTable::getTitleArraySize()
{
	return titleArray.size();
}

ParamTable::TABINDEX_T* ParamTable::getTitleArrayItem(size_t arrayIndex)
{
	TABINDEX_T* pt=nullptr;
	
	if(arrayIndex < titleArray.size())
		pt=&titleArray[arrayIndex];

	return pt;
}

bool ParamTable::isAllowEdit(int paramTableIndex, unsigned short currentUser)
{
	unsigned short targetUser=0xffff;

	if(REG_RW == (ParaRegTab[paramTableIndex].Attribute & 0x80)){
		unsigned short user=getParamUser(paramTableIndex);

		switch(user){
		case REG_GUEST:
		case REG_USER:
		case REG_PROGRAMER:
		case REG_SUPER:
			targetUser=user;
			break;
		default:
			break;
		}
	}

	return currentUser>=targetUser;
}

unsigned short ParamTable::getParamUser(int paramTableIndex)
{
	return static_cast<unsigned short>(ParaRegTab[paramTableIndex].Attribute) & 0x3f;
}

CString ParamTable::getValueString(int paramTableIndex)
{
	CString valueString;
	void* pData=ParaRegTab[paramTableIndex].pData;

	switch(ParaRegTab[paramTableIndex].DataType){
	case DT_INT8U:
		valueString.Format(_T("%u"),*(reinterpret_cast<unsigned char*>(pData)));
		break;
	case DT_INT16U:
		valueString.Format(_T("%u"),*(reinterpret_cast<unsigned short*>(pData)));
		break;
	case DT_INT32U:
		valueString.Format(_T("%u"),*(reinterpret_cast<unsigned int*>(pData)));
		break;
	case DT_INT8S:
		valueString.Format(_T("%d"),*(reinterpret_cast<char*>(pData)));
		break;
	case DT_INT16S:
		valueString.Format(_T("%d"),*(reinterpret_cast<short*>(pData)));
		break;
	case DT_INT32S:
		valueString.Format(_T("%d"),*(reinterpret_cast<int*>(pData)));
		break;
	case DT_PPS:
	case DT_FLOAT:
		valueString.Format(_T("%.3f"),*(reinterpret_cast<float*>(pData)));
		break;
	case DT_DOUBLE:
		valueString.Format(_T("%.3f"),*(reinterpret_cast<double*>(pData)));
		break;
	default:
		valueString=_T("?");
		break;
	}

	return valueString;
}

void ParamTable::writeParamFromString(int paramTableIndex, const CString& str)
{
	void* pData=ParaRegTab[paramTableIndex].pData;

	switch(ParaRegTab[paramTableIndex].DataType){
	case DT_INT8U:
		*(reinterpret_cast<unsigned char*>(pData))=static_cast<unsigned char>(_ttoi(str));
		break;
	case DT_INT16U:
		*(reinterpret_cast<unsigned short*>(pData))=static_cast<unsigned short>(_ttoi(str));
		break;
	case DT_INT32U:
		*(reinterpret_cast<unsigned int*>(pData))=static_cast<unsigned int>(_ttoi(str));
		break;
	case DT_INT8S:
		*(reinterpret_cast<char*>(pData))=static_cast<char>(_ttoi(str));
		break;
	case DT_INT16S:
		*(reinterpret_cast<short*>(pData))=static_cast<short>(_ttoi(str));
		break;
	case DT_INT32S:
		*(reinterpret_cast<int*>(pData))=static_cast<int>(_ttoi(str));
		break;
	case DT_PPS:
	case DT_FLOAT:
		*(reinterpret_cast<float*>(pData))=static_cast<float>(_ttoi(str));
		break;
	case DT_DOUBLE:
		*(reinterpret_cast<double*>(pData))=static_cast<double>(_ttoi(str));
		break;
	default:
		*(reinterpret_cast<unsigned char*>(pData))=0;
		break;
	}
}
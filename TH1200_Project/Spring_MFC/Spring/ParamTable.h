#pragma once

#include <vector>
#include <map>
#include <string>

class ParamTable
{
public:
	struct TABINDEX_T{
		int startIndex; // DT_TITLE开始项，其实下一项才是真实参数
		int count; // 个数
	};

public:
	ParamTable(void);
	~ParamTable(void);

	void init();
	int getParaRegTableIndex(void* data);

	size_t getTitleArraySize();
	TABINDEX_T* getTitleArrayItem(size_t arrayIndex);

	size_t getSizeFromDataType(unsigned char type);

	bool isAllowEdit(int paramTableIndex, unsigned short currentUser);
	unsigned short getParamUser(int paramTableIndex);
	CString getValueString(int paramTableIndex);
	void writeParamFromString(int paramTableIndex, const CString& str);

	bool queryAllParams();
	
protected:
	std::vector<TABINDEX_T> titleArray;
	std::map<void*, int> map_addr_index;
	bool queryAxisNames();
};


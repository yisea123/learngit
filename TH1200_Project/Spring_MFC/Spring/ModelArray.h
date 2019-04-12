/*
 * ModelArray.h
 *
 * Created on: 2018年3月30日下午2:44:14
 * Author: lixingcong
 */

#ifndef MODELARRAY_H_
#define MODELARRAY_H_

#include <string>
#include <vector>

class CModelArray
{
public:
	struct MODEL_PARAM_T{
		std::string description;
		double value;
		double minval,maxval;
		std::string range_text;
	};

	struct MODEL_T{
		std::string name;
		int picture_id;
		std::vector<MODEL_PARAM_T> params;
	};

public:
	CModelArray();
	virtual ~CModelArray();

	void loadDefault();
	void add(std::string name,int pic_id, MODEL_PARAM_T* p);
	void set(size_t index, float* val);

	MODEL_T* get(size_t index);
	size_t getCount();

protected: // members
	std::vector<MODEL_T> models;
};

#endif /* MODELARRAY_H_ */

/*
 * ListCtrlUndoRedo.h
 *
 * 撤销和重做的实现，使用双向队列实现
 *
 * Created on: 2017年9月27日下午9:55:03
 * Author: lixingcong
 */

#pragma once

#include "stdafx.h"
#include <list>

// 指令单元格最大的宽度
#define MAX_WIDTH_OF_A_CELL 10

// 对指令list的操作：删除，添加一行，编辑单元格
typedef enum{
	OPERATION_LISTEDIT_ADD_LINE,
	OPERATION_LISTEDIT_INSERT_LINE,
	OPERATION_LISTEDIT_DELETE_LINE_START, // 开始删行
	OPERATION_LISTEDIT_DELETE_LINE_DONE, // 结束删行
	OPERATION_LISTEDIT_EDIT_CELL
}OPERATION_TYPE_T;

// 对指令list的一次操作
typedef struct{
	OPERATION_TYPE_T operation;
	int row;
	int col; // 当“operation为删除”时候，表示删除的一行中，含有多少列已存在的数据
	char data_old[MAX_WIDTH_OF_A_CELL]; // 该单元位置(row,col)的数据
	char data_new[MAX_WIDTH_OF_A_CELL]; // 该单元位置(row,col)的新数据
} OPERATION_LISTEDIT_T;

// 撤销 恢复 历史记录
class ListCtrlUndoRedo
{
public:
	ListCtrlUndoRedo(unsigned int max_undo_times, unsigned int width_of_a_cell);
	~ListCtrlUndoRedo(void);

	// 设置最大撤销次数
	void set_max_undo_times(int times);
	int get_max_undo_times();

	// 历史记录入栈，出栈
	void push(OPERATION_TYPE_T operation, int row, int col, char* data_old, char* data_new);
	int pop_undo(OPERATION_TYPE_T* operation, int* row, int* col, char* data);
	int pop_redo(OPERATION_TYPE_T* operation, int* row, int* col, char* data);

	// 清空
	void clear();
private:
	unsigned int _max_undo_times;
	unsigned int _width_of_a_cell;
	unsigned int is_last_pop_undo; // 上一次操作是否是undo
	std::list<OPERATION_LISTEDIT_T> history_list;
	std::list<OPERATION_LISTEDIT_T>::iterator iterator_current;
};



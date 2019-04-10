/*
 * ListCtrlUndoRedo.cpp
 *
 * 撤销和重做的实现，使用双向队列实现
 *
 * Created on: 2017年9月27日下午9:55:03
 * Author: lixingcong
 */

#include "StdAfx.h"
#include "ListCtrlUndoRedo.h"
#include <cstring>
#include "Utils.h"

ListCtrlUndoRedo::ListCtrlUndoRedo(unsigned int max_undo_times, unsigned int width_of_a_cell)
{
	_max_undo_times=max_undo_times;

	if(width_of_a_cell>MAX_WIDTH_OF_A_CELL)
		_width_of_a_cell=MAX_WIDTH_OF_A_CELL;
	else
		_width_of_a_cell=width_of_a_cell;

	iterator_current=history_list.end();
	is_last_pop_undo=0;
}


ListCtrlUndoRedo::~ListCtrlUndoRedo(void)
{
}

void ListCtrlUndoRedo::set_max_undo_times(int times)
{
	_max_undo_times=times;
}

int ListCtrlUndoRedo::get_max_undo_times()
{
	return _max_undo_times;
}

void ListCtrlUndoRedo::push(OPERATION_TYPE_T operation, int row, int col, char* data_old, char* data_new)
{
	int number_to_delete;

	OPERATION_LISTEDIT_T item={operation, row, col, {0}, {0}};
		if(operation==OPERATION_LISTEDIT_EDIT_CELL && 0==strcmp(data_old,data_new)) // no need to undo or redo
		return;
	memcpy(item.data_old,data_old,_width_of_a_cell);
	memcpy(item.data_new,data_new,_width_of_a_cell);
	// debug_printf("-push op=%u, pos=(%u,%u), d1=%s, d2=%s\r\n",operation,row,col,data_old,data_new);

	if(iterator_current!=history_list.begin()){ // remove the previous history
		std::list<OPERATION_LISTEDIT_T>::iterator it;

		number_to_delete=0;
		for(it=history_list.begin();it!=iterator_current;++it)
			number_to_delete+=1;

		for(int i=0;i<number_to_delete;i++)
			history_list.pop_front(); // remove the out-dated items
	}

	history_list.push_front(item); // in queue

	if(history_list.size()>_max_undo_times)
		history_list.pop_back(); // dequeue the last one, if the list is full

	iterator_current=history_list.begin(); // point the iterator to begin
	is_last_pop_undo=0;
}

int ListCtrlUndoRedo::pop_undo(OPERATION_TYPE_T* operation, int* row, int* col, char* data)
{
	if(0==history_list.size()) // no item
		return -1;

	if(1==is_last_pop_undo){ // move iterator if last pop-up is undo
		if(++iterator_current==history_list.end()){ // if the next item is the last one
			--iterator_current;
			return -1;
		}
	}

	*operation=(*iterator_current).operation;
	*row=(*iterator_current).row;
	*col=(*iterator_current).col;
	memcpy(data,(*iterator_current).data_old,_width_of_a_cell);

	is_last_pop_undo=1;

	//debug_printf("undo, op=%d, row=%d, col=%d\n",*operation,*row,*col);

	return 0;
}

int ListCtrlUndoRedo::pop_redo(OPERATION_TYPE_T* operation, int* row, int* col, char* data)
{
	if(0==is_last_pop_undo){
		if(iterator_current==history_list.begin()) // if locate 1st, then couldn't move the iterator to previous one
			return -1;
		--iterator_current;
	}

	*operation=(*iterator_current).operation;
	*row=(*iterator_current).row;
	*col=(*iterator_current).col;
	memcpy(data,(*iterator_current).data_new,_width_of_a_cell);

	is_last_pop_undo=0;

	//debug_printf("redo, op=%d, row=%d, col=%d\n",*operation,*row,*col);

	return 0;
}

void ListCtrlUndoRedo::clear()
{
	history_list.clear();
	iterator_current=history_list.end();
	is_last_pop_undo=0;
}

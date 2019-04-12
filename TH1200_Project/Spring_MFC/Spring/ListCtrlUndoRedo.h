/*
 * ListCtrlUndoRedo.h
 *
 * ������������ʵ�֣�ʹ��˫�����ʵ��
 *
 * Created on: 2017��9��27������9:55:03
 * Author: lixingcong
 */

#pragma once

#include "stdafx.h"
#include <list>

// ָ�Ԫ�����Ŀ��
#define MAX_WIDTH_OF_A_CELL 10

// ��ָ��list�Ĳ�����ɾ�������һ�У��༭��Ԫ��
typedef enum{
	OPERATION_LISTEDIT_ADD_LINE,
	OPERATION_LISTEDIT_INSERT_LINE,
	OPERATION_LISTEDIT_DELETE_LINE_START, // ��ʼɾ��
	OPERATION_LISTEDIT_DELETE_LINE_DONE, // ����ɾ��
	OPERATION_LISTEDIT_EDIT_CELL
}OPERATION_TYPE_T;

// ��ָ��list��һ�β���
typedef struct{
	OPERATION_TYPE_T operation;
	int row;
	int col; // ����operationΪɾ����ʱ�򣬱�ʾɾ����һ���У����ж������Ѵ��ڵ�����
	char data_old[MAX_WIDTH_OF_A_CELL]; // �õ�Ԫλ��(row,col)������
	char data_new[MAX_WIDTH_OF_A_CELL]; // �õ�Ԫλ��(row,col)��������
} OPERATION_LISTEDIT_T;

// ���� �ָ� ��ʷ��¼
class ListCtrlUndoRedo
{
public:
	ListCtrlUndoRedo(unsigned int max_undo_times, unsigned int width_of_a_cell);
	~ListCtrlUndoRedo(void);

	// �������������
	void set_max_undo_times(int times);
	int get_max_undo_times();

	// ��ʷ��¼��ջ����ջ
	void push(OPERATION_TYPE_T operation, int row, int col, char* data_old, char* data_new);
	int pop_undo(OPERATION_TYPE_T* operation, int* row, int* col, char* data);
	int pop_redo(OPERATION_TYPE_T* operation, int* row, int* col, char* data);

	// ���
	void clear();
private:
	unsigned int _max_undo_times;
	unsigned int _width_of_a_cell;
	unsigned int is_last_pop_undo; // ��һ�β����Ƿ���undo
	std::list<OPERATION_LISTEDIT_T> history_list;
	std::list<OPERATION_LISTEDIT_T>::iterator iterator_current;
};



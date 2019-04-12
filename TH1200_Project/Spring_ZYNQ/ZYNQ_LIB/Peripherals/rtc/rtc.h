#ifndef __RTC_H__
#define __RTC_H__

#ifdef __cplusplus
extern "C" {
#endif


#include "adt_datatype.h"

typedef struct tagDATETIME
{
    U16 year;
    U16 month;
    U16 week;
    U16 day;
    U16 hour;
    U16 minute;
    U16 second;
    U16 millisecond;
}DATETIME_T;

typedef struct _SYSTEMTIME {
	U16 wYear;
	U16 wMonth;
	U16 wDayOfWeek;
	U16 wDay;
	U16 wHour;
	U16 wMinute;
	U16 wSecond;
	U16 wMilliseconds;
}SYSTEMTIME, *PSYSTEMTIME, *LPSYSTEMTIME;

typedef struct tagDATE
{
	U16 year;
	U16 month;
	U16 week;//for padding
	U16 day;
}DATE_T;

typedef struct tagTIME
{
	U16 hour;
	U16 minute;
	U16 second;
}TIME_T;

int RtcGetDateTime(DATETIME_T *pDate);
int RtcSetDateTime(DATETIME_T *pDate);

int RtcGetDate(DATE_T *date);
int RtcSetDate(DATE_T *date);

int RtcGetTime(TIME_T *date);
int RtcSetTime(TIME_T *date);

U32 GetTotDays(U16 year, U16 month, U16 day);

U32 GetDateDays(DATETIME_T date);
U32 GetDays(DATE_T date);
U32 GetTimes(TIME_T time);

U8 RtcIsLeapYear(U16 year);
U8 RtcCheckDateValid(U16 year, U16 month, U16 day);
//U8 RtcGetWeekNo(U16 year, U16 month, U16 day);
//U8 RtcGetDaysOfMonth(U16 year, U16 month);
//char RtcCmpDateTime(DATETIME_T *date1, DATETIME_T *date2);
//U32 RtcGetDateInterval(DATE_T *date1, DATE_T *date2);
//U32 RtcGetTimeInterval(DATETIME_T* pTime1, DATETIME_T *pTime2);
//U16 RtcGetPastDays(U16 year, U16 month, U16 day);
//U8 RtcDateAddDays(DATE_T *date, U16 days);
//U8 RtcDateSubDays(DATE_T *date, U16 days);
//U8 RtcGetNextMonth(DATE_T *date);
//U8 RtcGetPreMonth(DATE_T *date);
//U8 RtcGetNextYear(DATE_T *date);
//U8 RtcGetPreYear(DATE_T *date);

//U8 RtcGetAnimalYear(DATE_T *date, U8 solar);
//U8 RtcSolarConvertLunar(DATE_T *src_date);
//U8 RtcLunarConvertSolar(DATE_T *src_date, DATE_T *dec_date);
//U8 RtcLunarDayOfMonth(U16 year, U16 month);

/********************************************
函数名:		RtcGetDateStr
主要功能:	以字符串形式获取当前日期 格式为：xxxx/xx/xx 年/月/日
输入参数:	无
输出参数:	无
返回值:		获取日期之字符串
********************************************/
void	RtcGetDateStr(INT8S *da);

/********************************************
函数名:		RtcGetTimeStr
主要功能:	以字符串形式获取当前时间 格式为：xx:xx:xx 时/分/秒
输入参数:	无
输出参数:	无
返回值:		获取时间之字符串
********************************************/
void	RtcGetTimeStr(INT8S *ti);

#ifdef __cplusplus
}
#endif

#endif

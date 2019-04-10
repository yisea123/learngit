#ifndef _STRTOOL_H_
#define _STRTOOL_H_
//#include	"os_cpu.h"
#include "adt_datatype.h"

/*检查参数c是否为ASCII码字符，也就是判断c的范围是否在0到127之间。
返回值  若参数c为ASCII码字符，则返回TRUE，否则返回NULL(0)。
 */
//#define isascii(c)  ((unsigned)(c) < 128)


void 	StringCopy(INT8S* dest,INT8S* src);
void 	MemoryCopy (INT8S* dest,INT8S* src,INT16S size);
int		MemoryCmp(INT8U* str1,INT8U* str2,INT32U size);
void 	MemorySet(INT8U* Target,INT8U number,INT16S size);
void 	itoa(INT32U num,INT8U *str,INT8U mode);
void 	ltoa(INT32S num,INT8U *str);
BOOL 	ftoa( float op1, INT8U *str, INT8U dotlen,INT16S len);
INT32U  stringtoul(INT8U *str);
INT32S 	stringtol(INT8U *str);
INT16S  stringtoi(INT8U *str);
FP64	stringtof(INT8U *str);
INT16S 	Stringcmp(INT8U *str1,INT8U *str2);
INT8U  	*charins(INT8U *char1,INT8U char2,INT8U len);
INT8U  	*charinsend(INT8U *char1,INT8U char2,INT8U len);
void  	FixtureFtoStr(FP32 nfloat,INT8U *str);
void 	IntTO2Str(INT8U integer,INT8U *str);
void 	IntTO4Str(INT32U integer,INT8U *str);
#endif

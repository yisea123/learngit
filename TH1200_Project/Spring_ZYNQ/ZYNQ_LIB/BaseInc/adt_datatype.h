#ifndef __DAT_DATATYPE_H__
#define __DAT_DATATYPE_H__

typedef   signed char  s8;
typedef   signed short s16;
typedef   signed long  s32;

typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned long  u32;
typedef unsigned long long u64;

typedef unsigned char  U8;
typedef unsigned short U16;
typedef unsigned long  U32;
typedef unsigned long long U64;

typedef unsigned char	uchar;
typedef	unsigned short	ushort;
typedef unsigned long	ulong;
typedef	unsigned int	uint;
typedef ulong lbaint_t;

typedef unsigned char    BYTE;
typedef unsigned char  	 BOOLEAN;
typedef unsigned char    INT8U;
typedef   signed char    INT8S;
typedef unsigned short   INT16U;
typedef   signed short   INT16S;
typedef unsigned int     INT32U;
typedef   signed int     INT32S;

typedef float          		FP32;                     /* Single precision floating point                    */
typedef double         		FP64;                     /* Double precision floating point                    */


#if !defined(LONG)
typedef long LONG;
#endif

#if !defined(ULONG)
typedef unsigned long ULONG;
#endif

#ifndef	BOOL
	#define	BOOL	BOOLEAN
#endif

#ifndef bool
#define bool BOOLEAN
#endif

#ifndef NULL
#define NULL 		((void*)0)
#endif

#ifndef TRUE
#  define TRUE		1U
#endif

#ifndef FALSE
#  define FALSE		0U
#endif

#ifndef true
#define true		1u
#endif

#ifndef false
#define false		0u
#endif

#endif


/****************************************************************************
【文  件  名  称】StrTool.c
【功  能  描  述】三星S3C44B0X板 uC/OS-II 程序代码
【程  序  版  本】1.0
【创建人及创建日期】
【修改人及修改日期】
****************************************************************************/

#include <stdio.h>
#include "string.h"
#include "StrTool.h"
#include "math.h"
#include "stdlib.h"
#include "adt_datatype.h"
#include "ctype.h"

static INT8U numtable[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
/*
替代extern char *strcpy(char * s1, const char * s2);
该函数在使用过程中需要十分注意参数类型地带入，而在我们系统中多数INT8S 指针型
*/
void StringCopy(INT8S* buf,INT8S* message)
{
    while(*message)
    {
        *buf=*message;
        message++;
        buf++;
    }
    *buf=0;
}


/*
该函数硬拷贝一段缓冲区，指定长度，由于strcpy函数copy一段缓冲区地时候遇到 *char等于0的时候则后面的就不能拷贝了
message： 原缓冲区，
buf:      目标缓冲区
size：    长度
void MemoryCopy (INT8S* Target,INT8S* Source,INT16S size);
*/

void MemoryCopy (INT8S* Target,INT8S* Source,INT16S size)
{
    while(size)
    {
        *Target=*Source;
        Source++;
        Target++;
        size--;
    }
}

/*  设定特定内存指定长度的指定值 */
void MemorySet(INT8U* Target,INT8U number,INT16S size)
{
    while(size)
    {
        *Target=number;
        Target++;
        size--;
    }
}

/***********************************************************
返回值                   含义
小于0                   str1 小于 str2
等于0                   str1 等于 str2
大于0                   str1 大于 str2
**********************************************************/
int MemoryCmp(INT8U* str1,INT8U* str2,INT32U size)
{
    INT32U i;

    i = 0;
   
    while(size)
    {
     if((str1[i]=='\0') && (str2[i] =='\0'))   //如果size大于两者的实际长度
       return 0;
     if(str1[i]>str2[i])  
       return 1;
     if(str1[i]< str2[i])  
       return -1;

        i ++ ;
        size--;
    }
    
    return i;
}



INT16S  Stringcmp(INT8U *str1,INT8U *str2)
{
    int i;

   for(i = 0 ; str1[i] == str2[i]; i++)
   {
     if((str1[i]=='\0') && (str2[i] =='\0'))   //如果size大于两者的实际长度
       return 0;

     if(str1[i]>str2[i])
       return 1;

     if(str1[i]<str2[i])
        return -1;
   }
	printf("str1[%d] = 0x%x,str2[%d] = 0x%x\r\n",i,str1[i],i,str2[i]);
	return -2;
}


/**********************
插入字符串，在任意位置插入任意长的字符串
****************************/
INT8U  *strins(INT8U *char1,INT8U* char2,INT32U pos)
{
   INT32U s1_length;   /* 字符串S1之长度 */
   INT32U s2_length;   /* 字符串S2之长度 */
   INT32U i,j;
   
   pos -- ;                     /*字符串的开始位置 */
   s1_length = strlen((const char*)char1);
   s2_length = strlen((const char*)char2);
   
   /* 将预插入之位置之后的所有字符往后移，以空S2所需的字符串长度*/
   for(i= s1_length ; i >=pos ; i--)
   char1[i+s2_length] = char1[i];
   
   /*  将字符串2的内容填入所空出的字符串s1中*/
   for(j=pos; char2[j-pos] != '\0'; j ++)
     char1[j] = char2[j - pos];
     
     return char1;
}

/**********************
    插入字到字符串的0位，
****************************/
INT8U  *charins(INT8U *char1,INT8U char2,INT8U len)
{
   /* 将预插入之位置之后的所有字符往后移，以空S2所需的字符串长度*/
  
  do
  {
    len -- ;
   	char1[len+1] = char1[len];
  }while(len);
  
   /*  将字符2的内容填入所空出的字符串s1中*/
   char1[0] = char2;
   return char1;
}


/**********************
    功能：插入字到字符串的指定位置或者最末一位
****************************/
INT8U  *charinsend(INT8U *char1,INT8U char2,INT8U len)
{
                /* 将预插入之位置之后的所有字符往后移，以空S2所需的字符串长度*/
  INT8U i;
  INT8U s1_length;

  s1_length = strlen((const char*)char1);
  
  if(s1_length == 0)
  {
   char1[0] = char2;
   return char1;
  }
  else  if(s1_length == 1 )
  {
    char1[1] = char2;
    return char1;
  }
  else if(len < s1_length )
  {
        s1_length = len;
        for(i = 0 ; i < s1_length; i ++)
        {
           	char1[i] = char1[i+1];
        }
     char1[s1_length] = char2;
     char1[s1_length +1] = '\0';
     return char1;
   }
   char1[s1_length] = char2;
   char1[s1_length +1] = '\0';
   return char1;
}

/**********************
字符串反转  “123456789”  变成 “987654321”
****************************/
INT8U  *strrev(INT8U *str)
{
    INT8U  buf[100];
    INT32U  s_length;
        

    StringCopy((INT8S *)buf,(INT8S *)str);
    s_length = strlen((const char*)str);
    while(s_length)
    {
        *str =*(buf + s_length -1);
         str++;
        s_length--;
    } 
    return str;
}

/*
     该函数完成 数值转换为字符串：
     num: 需要转换的数值
     str： 字符串指针
     mode： 2  二进制       输出形式：  XXXXXXb
            8  八进制       输出形式：  XXXXXX    
            10 十进制       输出形式：  XXXXXX    
            16 十六进制     输出形式：  0xXXXXXX    
     注意：该函数只能完成无符号数地转换       
            
*/
void itoa(INT32U num,INT8U *str,INT8U mode)
{
    INT32U  snum,spare;
    INT8U  buf[32];
    INT8U   i;
    
   snum = num;
   memset(buf,0,32);
   if(num)
   {
       switch(mode)
       {
        case 2:
            for(i = 0 ; i < 32; i ++)
            {
                if(snum)
                {
                    spare = snum %2;
                    buf[i] = numtable[spare];   //数值转换为 字符
                    snum = snum/2;
                }
            }
          break;
        case 8:
            for(i = 0 ; i < 16; i ++)
            {
                if(snum)
                {
                    spare = snum %8;      //
                    buf[i] = numtable[spare];   //数值转换为 字符
                    snum = snum/8;
                }
                *str = 0;
            }
          break;
        case 10:
            for(i = 0 ; i < 16; i ++)
            {
                if(snum)
                {
                    spare = snum %10;      //
                    buf[i] = numtable[spare];   //数值转换为 字符
                    snum = snum/10;
                }
            }
          break;    
        case 16:
           for(i = 0 ; i < 8; i ++)
            {
                if(snum)
                {
                    spare = snum %16;      //
                    buf[i] = numtable[spare];   //数值转换为 字符
                    snum = snum/16;
                }
            }
          break;
       }  
    }
    else
      buf[0] = '0';
    strrev(buf);
    StringCopy((INT8S *)str,(INT8S *)buf);
}

/*
     该函数完成 带符号数 数值转换为字符串：
     num: 需要转换的数值
     str： 字符串指针
     
     注意入口 为INT32S
*/
void ltoa(INT32S num,INT8U *str)
{
    INT32U  snum,spare;
    INT8U  buf[32];
    INT8U   i;
    INT8U  flag;
    
    
   memset(buf,0,32);
   
   flag = 0;
   
   if(num != 0)
   {
     if(num < 0 )
     {
      flag = 1;     //表示为负数
      num = labs(num); 
     }
    snum = num;
     i = 0;
    do
    {
           spare = snum %10;      //
           buf[i] = numtable[spare];   //数值转换为 字符
           snum = snum/10;
           i  ++ ;
     }while(snum);
     
     if(flag)
        buf[i] = '-';     
   }
   else
      buf[0] = '0';
    strrev(buf);
    StringCopy((INT8S *)str,(INT8S *)buf);
}

/********************************************************************
作用：转换浮点数到字符串。
参数：op1 - [IN] 浮点数。
      str - [OUT] 字符串。
      dotlen - [IN] 小数点后字符串长度。
      len - [IN]生成的字符串总长,如果不为0则进行相应的补充空格操作,正值为右端补空格，负值为左端补空格
返回值：是否溢出或出错。
其它：当字符串不能完整表达浮点数时，返回字符串自动使用科学计数法。
********************************************************************/
BOOL	 ftoa( FP32 op1, INT8U *str, INT8U dotlen,INT16S len)
{
  
  INT16S i=0,count=0;
  INT8U str1[16];
  INT8U str2[16]="\0";
  
  INT32S integer;   //整数
  FP32  fract;     //分数
  /*
  #if OS_CRITICAL_METHOD == 3                     
   	OS_CPU_SR  cpu_sr;
    cpu_sr = 0;                                  
  #endif 
  OS_ENTER_CRITICAL();
  //*/
  integer = (INT32S)op1;
  //itoa(integer,str1,10);
  ltoa(integer,str1);
  if(integer>0)
	  fract = (op1-(FP32)integer)+0.00001;
  else
  {
  	  if(integer<0)
  	  {
	  	  fract = -op1 +(FP32)(integer)+0.0001;	
  	  }
  	  else //integer==0
  	  {
  	  	  //if(op1>0||fabs(op1-0.0)<0.00001)
  	  	  if((INT32S)(op1*100000.0)>=0)
  	  	  	  fract = op1+0.0001;
  	  	  else
  	  	  {
  	  	  	  str1[0]='-';
  	  	  	  str1[1]='0';
  	  	  	  str1[2]=0;
	  	  	  fract = -op1+0.0001;
	  	  }
  	  }
  }
  //*
  integer=(INT32S)(fract*pow(10.0,(FP32)dotlen));  
  {
  //	Uart_Printf("integer=%d\n",integer);
  }
  str2[0] = '.';
  //if(integer==0&&dotlen>1)
  if(integer==0&&dotlen>1)
  {
  	INT16S i;  	
  	for(i=0;i<dotlen;i++)
  		str2[i+1]='0';
  	str2[i+1]=0;
  }
  else
  {
	//ltoa(integer,str2+1);	  
	//*
		if(dotlen!=0)
		{
	  	  count=0;
	  	  for(i=dotlen;i>0;i--)
	  	  {
	  	  	if(integer<pow(10.0,(FP32)i))
	  	  	{
	  	  		//str2[count]='0';
	  	  		count++;
	  	  	}
	  	  }
	  	 // Uart_Printf("count=%d\n",count);
	  	  for(i=1;i<count;i++)
	  	  	str2[i]='0';
	  	  //Uart_Printf("str2_1=%s\n" ,str2);
	  	  ltoa(integer,str2+count);	 
	  	  //Uart_Printf("str2_2=%s\n" ,str2);
  	     }
  	     else
  	     	ltoa(integer,str2+1);
  	  //*/
  }
  //*/    
  strcat((char *)str1,(const char*)str2);
  StringCopy((INT8S *)str,(INT8S *)str1);
  if(len>0)
  {
  	INT16S i;
  	if(strlen((char *)str)<len)
  	{
  		for(i=strlen((char *)str);i<len;i++)
  			str[i]=' ';
  		str[len]=0;
  	}
  }
  else
  {
  	  if(len<0)
  	  {
  	  	INT16S tmp=-len;
  	  	INT16S i;  	  	  	  	
		if(strlen((char *)str)<tmp)
		{
  			for(i=0;i<tmp-strlen((char *)str);i++)
  			{
  				str2[i]=' ';
  			}
  			str2[i]=0;
  			strcat((char *)str2,(char *)str);
  			StringCopy((INT8S *)str,(INT8S *)str2);
	  		//str[len]='\0';
	  	}
  	  }
  }
//  OS_EXIT_CRITICAL();
  return TRUE;

}

/********************************************************************
作用：字符串转无符号长整型   结果为32位，  只支持正数，10进制 16进制
参数：str - [OUT] 字符串。
返回值：结果值
如果字符串不合法，则还回为 0 
********************************************************************/
INT32U  stringtoul(INT8U *string)
{
  INT32U  lastIndex;
  INT32U  result;
  INT8U   base;
  INT8U   i;
  INT8U  str[40]; 
  
   base = 10;
   result = 0;
    
    
   while ( *string == ' ' )
		string++;  
   StringCopy((INT8S *)str,(INT8S *)string);

   if(str[0]=='0' && (str[1]=='x' || str[1]=='X'))
    {
	    base=16;
	    StringCopy((INT8S *)str,(INT8S *)string+2);
    }
    
    lastIndex=strlen((const char*)str)-1;
    
    if( str[lastIndex]=='h' || str[lastIndex]=='H' )
    {
	    base=16;
	    str[lastIndex]=0;
	    lastIndex--;
    }
    
    
    if(base==10)
    {
	   for(i=0; i< lastIndex +1; i++)
       { 
         if(isdigit(str[i]))   //检查参数c是否为阿拉伯数字0到9,如果是连续则转换 
         {
            result = result *10 + (str[i] - '0');
         }
	   }
    }
    else
    {
	for(i=0;i<=lastIndex;i++)
	{
    	if(isxdigit(str[i]))   // 检查参数c是否为16进制数字，只要c为下列其中一个情况则返回TRUE。16进制数字:0123456789ABCDEF
	    {
		if(isupper(str[i]))
		    result=(result<<4)+str[i]-'A'+10;
		if(islower(str[i]))
		    result=(result<<4)+str[i]-'a'+10;
	    if(isdigit(str[i])) 
            result=(result<< 4) + str[i] - '0';
        }
      }
    }
    return result;
}



/********************************************************************
作用：字符串转带符号长整型   结果为32位，  只支持正数，10进制 16进制
参数：str - [OUT] 字符串。
返回值：结果值
如果字符串不合法，则还回为 0 
********************************************************************/
INT32S stringtol(INT8U *string)
{
    //INT8U* str;
    INT32S result;        
   
    while ( *string == ' ' )
	    string++;
            
    if(string[0]=='-')
    {
      //StringCopy((INT8S *)str,(INT8S *)(string +1));
      result = stringtoul(string);
      result = 0-result;
    }
    else
    {
        result = stringtoul(string);
    }
   return result;
}

INT16S stringtoi(INT8U *str)
{
	return (INT16S)stringtol(str);
}

FP64 stringtof(INT8U* str)
{
    //INT8U  isNeg,isDot,nDotCount;
    BOOL	isNeg,isDot;
    INT8U	nDotCount;
    INT32U integer;
    FP64   fract;     //分数
    FP64   result;

    while(*str == ' ')str++;

	if (*str == '-'){
		str++;isNeg = TRUE;
	}
	else{
		if(*str == '+')
			str++;
		isNeg = FALSE;
	}

    isDot = FALSE;
    nDotCount = 0;
    integer = 0;
    fract = 0;
    while(*str)
    {
    	if((*str>='0'&&*str<='9')||*str=='-'||*str=='.')
    	{
			if(isDot)nDotCount++;
			if( *str == '.')
			{
		   		str ++;
		   		isDot = TRUE;
			}

	   		if(isDot && nDotCount)
	   		{
		   		fract += (*str - '0') * pow(0.1,(FP64)nDotCount);
		    	str ++;
	       	}
	       	else
	       	{
	        	if(isDot == FALSE)
	        	{
		        	integer  = integer *10 + *str - '0';
		        	str ++; 
		    	}
	       	}
       	}
       	else
       		str++;
    }
    result =(FP64)integer + fract+0.0001;
    /*
    {
		char b[0x10];
		ftoa(result,(INT8U *)b,4,8);
		Uart_Printf("result=");
		Uart_Printf(b);
		Uart_Printf("\n");
	}
	*/
    if(isNeg)
        result = 0 -result;    
   
   	return result;
}


/*浮点数转换成字符串，四位整数，三位小数，加上符号显示,不够位的则补零,多余的去掉  */ 
void  FixtureFtoStr(FP32 nfloat,INT8U *str)
{
    INT32U integer;   //整数
    FP32  fract;     //分数
    
    memset(str,'0',10);
    if(nfloat < 0)
    {    
        str[0] = '-';
        nfloat = fabs(nfloat);
    }    
    else
        str[0] = '+';
    
    integer = (INT32U)nfloat;
    
    if(integer >9999)               //如果大于四位数
        integer = integer%10000;    
    
    if(integer>999)
    {
        str[1] =  integer/1000 + '0';    
        integer = integer%1000;    
    }
    
    if(integer>99)
    {
        str[2] =  integer/100 + '0';    
        integer = integer%100;    
     }
    
    if(integer>9)
    {
        str[3] =  integer/10 + '0';    
        integer = integer%10;    
     }
     
     str[4] =  integer + '0';    
     str[5] =   '.';    
     
     integer = (INT32U)nfloat;
     fract = nfloat -(float)integer;
     
     integer =(INT32U)( fract * 1000);            //3位有效位 我们只显示3位 
     
     if(integer>99)
    {
        str[6] =  integer/100 + '0';    
        integer = integer%100;    
     }
    if(integer>9)
    {
        str[7] =  integer/10 + '0';    
        integer = integer%10;    
    }
     str[8] =  integer + '0'; 
     str[9] = '\0';
}

/*
  整数转换为固定位数的字符串，不足位数的高位补零表示
*/
void IntTO2Str(INT8U integer,INT8U *str)
{
    memset(str,'0',2);
    
    if(integer>9)
    {
        str[0] =  integer/10 + '0';    //十
        integer = integer%10;    
     }
     str[1] =  integer + '0';         //个
     str[2] = '\0';
}

/*
  整数转换为固定位数的字符串，不足位数的高位补零表示
*/
void IntTO4Str(INT32U integer,INT8U *str)
{
    memset(str,'0',4);
    
    if(integer>999)
    {
        str[0] =  integer/1000 + '0';    //千
        integer = integer%1000;    
     }
     
    if(integer>99)
    {
        str[1] =  integer/100 + '0';    //百
        integer = integer%100;    
    }
    
    if(integer>9)
    {
        str[2] =  integer/10 + '0';    //百
        integer = integer%10;    
    }
     
     str[3] =  integer + '0';         //个
     
     str[4] = '\0';
}



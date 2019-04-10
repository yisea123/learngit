/****************************************************************************
����  ��  ��  �ơ�StrTool.c
����  ��  ��  ��������S3C44B0X�� uC/OS-II �������
����  ��  ��  ����1.0
�������˼��������ڡ�
���޸��˼��޸����ڡ�
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
���extern char *strcpy(char * s1, const char * s2);
�ú�����ʹ�ù�������Ҫʮ��ע��������͵ش��룬��������ϵͳ�ж���INT8S ָ����
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
�ú���Ӳ����һ�λ�������ָ�����ȣ�����strcpy����copyһ�λ�������ʱ������ *char����0��ʱ�������ľͲ��ܿ�����
message�� ԭ��������
buf:      Ŀ�껺����
size��    ����
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

/*  �趨�ض��ڴ�ָ�����ȵ�ָ��ֵ */
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
����ֵ                   ����
С��0                   str1 С�� str2
����0                   str1 ���� str2
����0                   str1 ���� str2
**********************************************************/
int MemoryCmp(INT8U* str1,INT8U* str2,INT32U size)
{
    INT32U i;

    i = 0;
   
    while(size)
    {
     if((str1[i]=='\0') && (str2[i] =='\0'))   //���size�������ߵ�ʵ�ʳ���
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
     if((str1[i]=='\0') && (str2[i] =='\0'))   //���size�������ߵ�ʵ�ʳ���
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
�����ַ�����������λ�ò������ⳤ���ַ���
****************************/
INT8U  *strins(INT8U *char1,INT8U* char2,INT32U pos)
{
   INT32U s1_length;   /* �ַ���S1֮���� */
   INT32U s2_length;   /* �ַ���S2֮���� */
   INT32U i,j;
   
   pos -- ;                     /*�ַ����Ŀ�ʼλ�� */
   s1_length = strlen((const char*)char1);
   s2_length = strlen((const char*)char2);
   
   /* ��Ԥ����֮λ��֮��������ַ������ƣ��Կ�S2������ַ�������*/
   for(i= s1_length ; i >=pos ; i--)
   char1[i+s2_length] = char1[i];
   
   /*  ���ַ���2�������������ճ����ַ���s1��*/
   for(j=pos; char2[j-pos] != '\0'; j ++)
     char1[j] = char2[j - pos];
     
     return char1;
}

/**********************
    �����ֵ��ַ�����0λ��
****************************/
INT8U  *charins(INT8U *char1,INT8U char2,INT8U len)
{
   /* ��Ԥ����֮λ��֮��������ַ������ƣ��Կ�S2������ַ�������*/
  
  do
  {
    len -- ;
   	char1[len+1] = char1[len];
  }while(len);
  
   /*  ���ַ�2�������������ճ����ַ���s1��*/
   char1[0] = char2;
   return char1;
}


/**********************
    ���ܣ������ֵ��ַ�����ָ��λ�û�����ĩһλ
****************************/
INT8U  *charinsend(INT8U *char1,INT8U char2,INT8U len)
{
                /* ��Ԥ����֮λ��֮��������ַ������ƣ��Կ�S2������ַ�������*/
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
�ַ�����ת  ��123456789��  ��� ��987654321��
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
     �ú������ ��ֵת��Ϊ�ַ�����
     num: ��Ҫת������ֵ
     str�� �ַ���ָ��
     mode�� 2  ������       �����ʽ��  XXXXXXb
            8  �˽���       �����ʽ��  XXXXXX    
            10 ʮ����       �����ʽ��  XXXXXX    
            16 ʮ������     �����ʽ��  0xXXXXXX    
     ע�⣺�ú���ֻ������޷�������ת��       
            
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
                    buf[i] = numtable[spare];   //��ֵת��Ϊ �ַ�
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
                    buf[i] = numtable[spare];   //��ֵת��Ϊ �ַ�
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
                    buf[i] = numtable[spare];   //��ֵת��Ϊ �ַ�
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
                    buf[i] = numtable[spare];   //��ֵת��Ϊ �ַ�
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
     �ú������ �������� ��ֵת��Ϊ�ַ�����
     num: ��Ҫת������ֵ
     str�� �ַ���ָ��
     
     ע����� ΪINT32S
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
      flag = 1;     //��ʾΪ����
      num = labs(num); 
     }
    snum = num;
     i = 0;
    do
    {
           spare = snum %10;      //
           buf[i] = numtable[spare];   //��ֵת��Ϊ �ַ�
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
���ã�ת�����������ַ�����
������op1 - [IN] ��������
      str - [OUT] �ַ�����
      dotlen - [IN] С������ַ������ȡ�
      len - [IN]���ɵ��ַ����ܳ�,�����Ϊ0�������Ӧ�Ĳ���ո����,��ֵΪ�Ҷ˲��ո񣬸�ֵΪ��˲��ո�
����ֵ���Ƿ���������
���������ַ�������������︡����ʱ�������ַ����Զ�ʹ�ÿ�ѧ��������
********************************************************************/
BOOL	 ftoa( FP32 op1, INT8U *str, INT8U dotlen,INT16S len)
{
  
  INT16S i=0,count=0;
  INT8U str1[16];
  INT8U str2[16]="\0";
  
  INT32S integer;   //����
  FP32  fract;     //����
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
���ã��ַ���ת�޷��ų�����   ���Ϊ32λ��  ֻ֧��������10���� 16����
������str - [OUT] �ַ�����
����ֵ�����ֵ
����ַ������Ϸ����򻹻�Ϊ 0 
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
         if(isdigit(str[i]))   //������c�Ƿ�Ϊ����������0��9,�����������ת�� 
         {
            result = result *10 + (str[i] - '0');
         }
	   }
    }
    else
    {
	for(i=0;i<=lastIndex;i++)
	{
    	if(isxdigit(str[i]))   // ������c�Ƿ�Ϊ16�������֣�ֻҪcΪ��������һ������򷵻�TRUE��16��������:0123456789ABCDEF
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
���ã��ַ���ת�����ų�����   ���Ϊ32λ��  ֻ֧��������10���� 16����
������str - [OUT] �ַ�����
����ֵ�����ֵ
����ַ������Ϸ����򻹻�Ϊ 0 
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
    FP64   fract;     //����
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


/*������ת�����ַ�������λ��������λС�������Ϸ�����ʾ,����λ������,�����ȥ��  */ 
void  FixtureFtoStr(FP32 nfloat,INT8U *str)
{
    INT32U integer;   //����
    FP32  fract;     //����
    
    memset(str,'0',10);
    if(nfloat < 0)
    {    
        str[0] = '-';
        nfloat = fabs(nfloat);
    }    
    else
        str[0] = '+';
    
    integer = (INT32U)nfloat;
    
    if(integer >9999)               //���������λ��
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
     
     integer =(INT32U)( fract * 1000);            //3λ��Чλ ����ֻ��ʾ3λ 
     
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
  ����ת��Ϊ�̶�λ�����ַ���������λ���ĸ�λ�����ʾ
*/
void IntTO2Str(INT8U integer,INT8U *str)
{
    memset(str,'0',2);
    
    if(integer>9)
    {
        str[0] =  integer/10 + '0';    //ʮ
        integer = integer%10;    
     }
     str[1] =  integer + '0';         //��
     str[2] = '\0';
}

/*
  ����ת��Ϊ�̶�λ�����ַ���������λ���ĸ�λ�����ʾ
*/
void IntTO4Str(INT32U integer,INT8U *str)
{
    memset(str,'0',4);
    
    if(integer>999)
    {
        str[0] =  integer/1000 + '0';    //ǧ
        integer = integer%1000;    
     }
     
    if(integer>99)
    {
        str[1] =  integer/100 + '0';    //��
        integer = integer%100;    
    }
    
    if(integer>9)
    {
        str[2] =  integer/10 + '0';    //��
        integer = integer%10;    
    }
     
     str[3] =  integer + '0';         //��
     
     str[4] = '\0';
}



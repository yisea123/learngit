/*
 * Copyright (c) 2002 Florian Schulze.
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the authors nor the names of the contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE AUTHORS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * ftpd.c - This file is part of the FTP daemon for lwIP
 *
 */

#include <stdio.h>
#include <stdarg.h>
#include <malloc.h>
#include <string.h>
#include <ctype.h>

#include "os.h"
#include "lwip_net/lwip_net.h"
#include "rtc/rtc.h"
#include "fatfs/ff.h"
#include "ftpd_base.h"
#include "ftpd_msg.h"
#include "ftpd.h"
#include "base.h"


void ftpd_cmd_thread(void *p_arg);
void ftpd_data_thread(void *p_arg);
void ftpd_msg_thread(void *p_arg);

#define FTPD_CMD_THREAD_STACK_SIZE	1024
#define FTPD_DATA_THREAD_STACK_SIZE	1024
#define FTPD_MSG_THREAD_STACK_SIZE	1024

static OS_TCB  FTPD_CMD_THREAD_TCB;
static CPU_STK FTPD_CMD_THREAD_STK[FTPD_CMD_THREAD_STACK_SIZE];

static OS_TCB  FTPD_DATA_THREAD_TCB;
static CPU_STK FTPD_DATA_THREAD_STK[FTPD_DATA_THREAD_STACK_SIZE];

static OS_TCB  FTPD_MSG_THREAD_TCB[MAX_FTPD_CLIENT];
static CPU_STK FTPD_MSG_THREAD_STK[MAX_FTPD_CLIENT][FTPD_MSG_THREAD_STACK_SIZE];

static OS_SEM FTPD_DATA_TRAN_START_SEM;	//通知数据传输任务开始传输数据
static OS_SEM FTPD_DATA_TRAN_COMPL_SEM;	//等待数据传输完成
static OS_SEM FTPD_USER_DATA_CONN_SEM;	//用户数据连接信号量
static OS_SEM FTPD_MSG_START_SEM[MAX_FTPD_CLIENT];	//等待远程连接端口

static struct ftpd_ulp 		 FTPD_ULP;
static struct ftpd_msgstate	 FTPD_MSG;
static struct ftpd_datastate FTPD_DATA;

static int is_ftpd_close = FALSE;


static const char *month_table[13] = {
	"Nul",
	"Jan",
	"Feb",
	"Mar",
	"Apr",
	"May",
	"Jun",
	"Jul",
	"Aug",
	"Sep",
	"Oct",
	"Nov",
	"Dec"
};


void ftpd_server_close(void)
{
	is_ftpd_close = TRUE;
}

void ftpd_server_open(void)
{
	is_ftpd_close = FALSE;
}

int is_ftpd_server_close(void)
{
	return is_ftpd_close;
}

int ftpd_reinit(void)
{
//	FRESULT res;
//
//	/* 指定当前目录为根目录 */
//	res = f_chdir("0:/");
//	if(res!=FR_OK){
//		Uart_Printf("change dir to 0:/ fail\r\n");
//	}

	sprintf((void*)FTPD_DATA.fs.Path, "%s", "0:/");

	return 0;
}

//static FATFS Fs;
//FRESULT FS_mount(void)
//{
//	FRESULT res;
//
//	res = f_mount(&Fs, "0", 1);
//	if(res != FR_OK){
//		Uart_Printf("fatfs f_mount error 0x%x\r\n", res);
//	}
//	else{
//		Uart_Printf("fatfs f_mount success!\r\n");
//	}
//
//	return res;
//}

int ftpd_init(int TaskPrio)
{
	OS_ERR ERR;
	int id;

	ps_debugout("ftpd_init\r\n");
	memset(&FTPD_ULP, 0x0, sizeof(FTPD_ULP));
	FTPD_MSG.datastate = &FTPD_DATA;
	FTPD_DATA.msgstate = &FTPD_MSG;

	FTPD_ULP.CmdServer	  = -1;
	FTPD_ULP.DataServer   = -1;
	FTPD_ULP.DataClient   = -1;

	for(id=0; id<MAX_FTPD_CLIENT; id++){
		FTPD_ULP.idle_table[id]   = TRUE;
		FTPD_ULP.CmdClient[id]    = -1;
		FTPD_ULP.rdwr_protect[id] = TRUE;
		memset(&FTPD_ULP.username[id], 0x00, CHAR_LEN);
		memset(&FTPD_ULP.password[id], 0x00, CHAR_LEN);
	}

	ftpd_reinit();

	//ftpd_server_open();
	ftpd_server_close();

	OSSemCreate(&FTPD_DATA_TRAN_START_SEM,
			"ftpd_data_sem1",
			0,
			&ERR);

	OSSemCreate(&FTPD_DATA_TRAN_COMPL_SEM,
			"ftpd_data_sem2",
			0,
			&ERR);

	OSSemCreate(&FTPD_USER_DATA_CONN_SEM,
			"ftpd_data_sem3",
			0,
			&ERR);

	for(id=0; id<MAX_FTPD_CLIENT; id++){

		OSSemCreate(&FTPD_MSG_START_SEM[id],
				"ftpd_start_sem",
				0,
				&ERR);
	}

	OSTaskCreate((OS_TCB     *)&FTPD_CMD_THREAD_TCB,
				 (CPU_CHAR   *) "ftpd_cmd_thread",
				 (OS_TASK_PTR ) ftpd_cmd_thread,
				 (void       *) 0,
				 (OS_PRIO     ) (TaskPrio),
				 (CPU_STK    *)&FTPD_CMD_THREAD_STK[0],
				 (CPU_STK_SIZE) FTPD_CMD_THREAD_STACK_SIZE / 10u,
				 (CPU_STK_SIZE) FTPD_CMD_THREAD_STACK_SIZE,
				 (OS_MSG_QTY  ) 0u,
				 (OS_TICK     ) 1u,
				 (void       *) 0,
				 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
				 (OS_ERR     *)&ERR);

	OSTaskCreate((OS_TCB     *)&FTPD_DATA_THREAD_TCB,
				 (CPU_CHAR   *) "ftpd_data_thread",
				 (OS_TASK_PTR ) ftpd_data_thread,
				 (void       *) 0,
				 (OS_PRIO     ) (TaskPrio),
				 (CPU_STK    *)&FTPD_DATA_THREAD_STK[0],
				 (CPU_STK_SIZE) FTPD_DATA_THREAD_STACK_SIZE / 10u,
				 (CPU_STK_SIZE) FTPD_DATA_THREAD_STACK_SIZE,
				 (OS_MSG_QTY  ) 0u,
				 (OS_TICK     ) 1u,
				 (void       *) 0,
				 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
				 (OS_ERR     *)&ERR);


	for(id=0; id<MAX_FTPD_CLIENT; id++){

		OSTaskCreate((OS_TCB     *)&FTPD_MSG_THREAD_TCB[id],
					 (CPU_CHAR   *) "ftpd_msg_thread",
					 (OS_TASK_PTR ) ftpd_msg_thread,
					 (void       *) id,
					 (OS_PRIO     ) (TaskPrio),
					 (CPU_STK    *)&FTPD_MSG_THREAD_STK[id][0],
					 (CPU_STK_SIZE) FTPD_MSG_THREAD_STACK_SIZE / 10u,
					 (CPU_STK_SIZE) FTPD_MSG_THREAD_STACK_SIZE,
					 (OS_MSG_QTY  ) 0u,
					 (OS_TICK     ) 1u,
					 (void       *) 0,
					 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
					 (OS_ERR     *)&ERR);
	}

	return 0;
}

int get_ftpd_next_idle(struct ftpd_ulp *ulp)
{
	int id;

	if( TRUE == is_ftpd_server_close() )
		return FTPD_CLIENT_ERR;

	for(id=0; id<MAX_FTPD_CLIENT; id++){
		if(ulp->idle_table[id] == TRUE){
			ulp->idle_table[id] = FALSE;
			break;
		}
	}

	if(id < MAX_FTPD_CLIENT)
		return id;
	else
		return FTPD_CLIENT_ERR;
}

void set_ftpd_one_idle(struct ftpd_ulp *ulp, int id)
{
	ulp->idle_table[id] = TRUE;
}

void process_time_info(DATETIME_T *time, struct ftpd_datastate *fsd)
{
	/* 保存时间戳 */
	time->year   = ((fsd->fs.pFileInfo->fdate>>9) & 0x2f) + 1980;
	time->month  =  (fsd->fs.pFileInfo->fdate>>5) & 0x0f;
	time->day    =  (fsd->fs.pFileInfo->fdate>>0) & 0x1f;
	time->hour   =  (fsd->fs.pFileInfo->ftime>>11)& 0x1f;
	time->minute =  (fsd->fs.pFileInfo->ftime>>5) & 0x2f;
	time->second = ((fsd->fs.pFileInfo->ftime>>0) & 0x1f) << 1;
	/* 修正月日，防止出错 */
	//if((time->year == 0));
	if((time->month == 0) || (time->month > 12)) time->month = 1;
	if((time->day   == 0) || (time->day   > 31)) time->day   = 1;
	//if( time->hour   > 24 ) time->hour   = 0;
	//if( time->minute > 60 ) time->minute = 0;
	//if( time->second > 60 ) time->second = 0;
}

static void send_next_directory(struct ftpd_datastate *fsd, struct ftpd_ulp *ulp, int shortlist)
{
	FRESULT res;
	char *data;
	int len;
	TCHAR *fname = NULL;
#ifdef _USE_LFN
	static TCHAR lfnameBuf[_MAX_LFN];
#endif

	if(!fsd || !ulp || !fsd->fs.pDir || !fsd->fs.pFileInfo) return ;

	data = malloc(FTPD_DATA_LEN);
	if(!data) return ;

	while (1) {

	#ifdef _USE_LFN
		memset(lfnameBuf, 0x00, _MAX_LFN);
		fsd->fs.pFileInfo->lfsize = _MAX_LFN;
		fsd->fs.pFileInfo->lfname = lfnameBuf;
	#endif

		res = f_readdir(fsd->fs.pDir, fsd->fs.pFileInfo);

		if( FR_OK == res ) {

		#ifdef _USE_LFN
			fname = *fsd->fs.pFileInfo->lfname ? fsd->fs.pFileInfo->lfname : fsd->fs.pFileInfo->fname;
		#else
			fname = fsd->fs.pFileInfo->fname;
		#endif

			if (shortlist) {

				len = sprintf(data, "%s\r\n", fname);
				ftpd_send_data(fsd, ulp, data, len);

			}
			else {
//				len = sprintf(	data, "-rw-rw-rw-   1 user     ftp  %11ld %s %02i %02i:%02i %s\r\n",
//								fsd->fs.pFileInfo->fsize,					//文件大小
//								//((fsd->fno->fdate>>9)&0x2f)+1980,			//年
//								month_table[(fsd->fs.pFileInfo->fdate>>5) &0x0f],	//月
//								(fsd->fs.pFileInfo->fdate>>0) &0x1f,		//日
//								(fsd->fs.pFileInfo->ftime>>11)&0x1f,		//时
//								(fsd->fs.pFileInfo->ftime>>5) &0x2f,		//分
//								//((fsd->fno->ftime>>0) &0x1f)<<1,			//秒
//								fname );				//文件名

				DATETIME_T time;
				process_time_info(&time, fsd);

				len = sprintf(	data, "-rw-rw-rw-   1 user     ftp  %11ld %s %02i %02i:%02i %s\r\n",
								fsd->fs.pFileInfo->fsize,	//文件大小
								//time.year,				//年
								month_table[time.month],	//月
								time.day,					//日
								time.hour,					//时
								time.minute,				//分
								//time.second,				//秒
								fname );					//文件名

				if (fsd->fs.pFileInfo->fattrib & AM_DIR)
					data[0] = 'd';

				ftpd_send_data(fsd, ulp, data, len);
			}
		}
		else {

//			Uart_Printf("res = %d\r\n", res);

			fsd->fs.pDir		= NULL;
			fsd->fs.pFile 		= NULL;
			fsd->fs.pFileInfo 	= NULL;
			fsd->msgstate->state= FTPD_IDLE;
			free(data);

			return;
		}
	}
}

static void ftpd_read_file(struct ftpd_datastate *fsd, struct ftpd_ulp *ulp)
{
	int result = -1;
	FRESULT res;
	int totlen, len, lenth;
	char *data;

	if(!fsd || !fsd->fs.pFile || !ulp) return ;

	totlen = fsd->fs.pFileInfo->fsize;
	len = FTPD_DATA_LEN;
	data = malloc(len);
	if(!data) return ;

//	Uart_Printf("send file start.\r\n");

	while(1){

		res = f_read(fsd->fs.pFile, data, len, (UINT*)&lenth);

		if(res!=FR_OK) ps_debugout("file read error! ## %d ##\r\n", res);

		if(lenth>0){

			result = ftpd_send_data(fsd, ulp, data, lenth);

			if(result < 0){
				ps_debugout("send file connect break.\r\n");
				break;
			}
			else{
				totlen -= lenth;
			}
		}
		else{
			if (f_eof(fsd->fs.pFile) == 0)	{
				ps_debugout("file not end.\r\n");
				continue;
			}

			if(totlen>0) {
				ps_debugout("send file incomplete. left len = %d\r\n", totlen);
			}
			else{
//				Uart_Printf("send file complete.\r\n");
			}

			break;
		}
	}

	fsd->fs.pDir 	  = NULL;
	fsd->fs.pFile 	  = NULL;
	fsd->fs.pFileInfo = NULL;

	free(data);
	return ;
}

static void ftpd_write_file(struct ftpd_datastate *fsd, struct ftpd_ulp *ulp)
{
	FRESULT res;
	char *data;
	int totlen, len, lenth;

	if(!fsd || !fsd->fs.pFile || !ulp) return ;

	totlen = 0;
	data = malloc(FTPD_DATA_LEN);
	if(!data) return ;

//	Uart_Printf("recv file start.\r\n");

	while(1){

		len = recv(ulp->DataServer, data, FTPD_DATA_LEN, 0);
		if(len<=0) {
//			Uart_Printf("recv file complete. file len: %d\r\n", totlen);
			break;
		}

		res = f_write(fsd->fs.pFile, data, len, (UINT*)&lenth);
		if(res != FR_OK){
			ps_debugout("file write error! ## %d ##\r\n", res);
			break;
		}
		else{
			totlen += len;
			//Uart_Printf("file write len : %d  totlen: %d\r\n", len, totlen);
		}
	}

	free(data);
	return ;
}

int ftpd_data_close(struct ftpd_datastate *fsd, struct ftpd_ulp *ulp)
{
	switch(fsd->mode){

	case FTPD_PORT:
		if(closesocket(ulp->DataServer) != -1) ulp->DataServer = -1;
		fsd->connected = 0;

		break;

	case FTPD_PASV:
		if(closesocket(ulp->DataServer) != -1) ulp->DataServer = -1;
		if(closesocket(ulp->DataClient) != -1) ulp->DataClient = -1;
		fsd->connected = 0;

		break;

	default:
		return -1;
	}

	return 0;
}

int ftpd_data_connect(struct ftpd_datastate *fsd, struct ftpd_ulp *ulp)
{
	int result;
	int on;

	ulp->DataLocalAddr.sin_family 		= AF_INET;
	ulp->DataLocalAddr.sin_addr.s_addr	= htonl(INADDR_ANY);
	ulp->DataLocalAddr.sin_port			= htons(FTPD_DATA_PORT);

	ulp->DataServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	setsockopt(ulp->DataServer, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
	result = bind(ulp->DataServer,(struct sockaddr*)&ulp->DataLocalAddr,sizeof(ulp->DataLocalAddr));
	if(result<0) ps_debugout("data bind error ## %d ##\r\n", result);

	result = connect(ulp->DataServer, (struct sockaddr*)&ulp->DataRemoteAddr, sizeof(ulp->DataRemoteAddr));
	if(result<0){
		ps_debugout("connect error ## %d ##\r\n",result);
		fsd->connected = 0;
	}
	else{
		fsd->connected = 1;
	}

	return 0;
}

int ftpd_send_data(struct ftpd_datastate *fsd, struct ftpd_ulp *ulp, char *data, int len)
{
	int result = -1;

	if((!fsd) || (!ulp) || (!data)) return -1;

//	Uart_Printf("%s\r\n",data);

//	err = (u8)netconn_write(ulp->pDataConn, data, len, NETCONN_COPY);
	switch(fsd->mode){
	case FTPD_PORT:
		result = send(ulp->DataServer, data, len, 0);
		if(result<0)
			ps_debugout("ftpd_send_data err = %d\r\n", result);
		break;

	case FTPD_PASV:
		result = send(ulp->DataClient, data, len, 0);
		if(result<0)
			ps_debugout("ftpd_send_data err = %d\r\n", result);
		break;

	default:
		break;
	}

	return result;
}


int ftpd_send_msg(struct ftpd_msgstate *fsm, struct ftpd_ulp *ulp, int id, char *msg, ...)
{
	va_list arg;
	int result, len;
	char *data;

	if((!fsm) || (!ulp)) return -1;

	data = (char*)malloc(FTPD_DATA_LEN); //1k
	if(!data) return -1;

	va_start(arg, msg);
	vsprintf(data, msg, arg);
	va_end(arg);
	strcat(data, "\r\n");
	len = strlen(data);

	//err = (u8)netconn_write(ulp->pCmdUserConn, data, len, NETCONN_COPY);
	result = send(ulp->CmdClient[id], data, len, 0);

	if(result < 0) ps_debugout("ftpd_send_msg err = %d\r\n", result);

	free(data);

	return result;
}

void ftpd_msg_server(struct ftpd_ulp *ulp, int id)
{
	struct ftpd_msgstate *fsm = &FTPD_MSG;
	int len;
	char *data;

	data = malloc(FTPD_DATA_LEN);
	if(!data) return ;

	ftpd_send_msg(fsm, ulp, id, msg220);
	while(1){

		if( TRUE == is_ftpd_server_close() )
			break;

		len = recv(ulp->CmdClient[id], data, FTPD_DATA_LEN, 0);

		if(len<=0){
			ps_debugout("error\r\n");
			break;
		}

		*((u8*)(data+len)) = '\0';
//		Uart_Printf("%s",data);
		ftpd_do_cmd(fsm, ulp, id, data, len);
	}

	free(data);
}


#if 0
void ftpd_msg_server(struct ftpd_ulp *ulp)
{
	OS_ERR err;
	u8 *data;
	u32 iPackLen;
	struct ftpd_msgstate *fsm = &FTPD_MSG;
	int fifo_err;

 	/* Initialize the FTPD_MSG */
	if(fsm)
		memset(fsm, 0x0, sizeof(struct ftpd_msgstate));

	/* Initialize the structure. */
	fifo_err = sfifo_init(&fsm->fifo, FIFO_LEN);

	fsm->state = FTPD_IDLE;
	if (0 != fifo_err) return ;

	ftpd_send_msg(fsm, ulp, msg220);

	while(1){
		err = netconn_recv(ulp->pCmdUserConn,&ulp->pCmdNetBuf);
		if(err == ERR_OK){
			iPackLen = netbuf_len(ulp->pCmdNetBuf);

			data = malloc(iPackLen+1);
			if(data){
				netbuf_copy(ulp->pCmdNetBuf, (void*)data, iPackLen);
				*((u8*)(data+iPackLen)) = '\0';

//				{
//					int k;
//
//					Uart_Printf("\r\n *** iPackLen = %d\r\n",iPackLen);
//					for(k=0;k<iPackLen;k++){
//						Uart_Printf("%2x ",*(data+k));
//					}
//					Uart_Printf("\r\n");
//				}
				Uart_Printf("%s\r\n",data);
			}
			else{
				Uart_Printf("#1 malloc error\r\n");
			}

			netbuf_delete(ulp->pCmdNetBuf);/* delete net buffer */

			if(data){
				ftpd_do_cmd(fsm, ulp, data, iPackLen);
				free(data);
			}
		}
		else{
			if(ulp->pCmdNetBuf != NULL){
				netbuf_delete(ulp->pCmdNetBuf);/* delete net buffer */
			}

			break;
		}
	}

	netconn_close(ulp->pCmdUserConn);
	netconn_delete(ulp->pCmdUserConn);
}
#endif

void ftpd_data_server(struct ftpd_ulp *ulp)
{
	struct ftpd_datastate *fsd = &FTPD_DATA;
	struct ftpd_msgstate  *fsm = fsd->msgstate;

//	Uart_Printf("connect successful.\r\n");

	switch (fsm->state) {
	case FTPD_LIST:
		send_next_directory(fsd, ulp, 0);
		break;

	case FTPD_NLST:
		send_next_directory(fsd, ulp, 1);
		break;

	case FTPD_RETR:
		ftpd_read_file(fsd, ulp);
		break;

	case FTPD_STOR:
		ftpd_write_file(fsd, ulp);
		break;

	default:
		break;
	}
}

void ftpd_user_data_conn_setidle(void)
{
	OS_ERR err;

	OSSemSet(&FTPD_USER_DATA_CONN_SEM, 0, &err);

	OSSemPost(&FTPD_USER_DATA_CONN_SEM,
			OS_OPT_POST_1,
			&err);
}

void ftpd_user_data_conn_waitting(int timeout)
{
	OS_ERR err;
	CPU_TS ts;

	OSSemPend(&FTPD_USER_DATA_CONN_SEM,
			(OS_TICK)timeout,
			OS_OPT_PEND_BLOCKING,
			&ts,
			&err);
}

void ftpd_data_transfer_startup(void)
{
	OS_ERR err;

	OSSemSet(&FTPD_DATA_TRAN_START_SEM, 0, &err);
	OSSemSet(&FTPD_DATA_TRAN_COMPL_SEM, 0, &err);

	OSSemPost (&FTPD_DATA_TRAN_START_SEM,
			OS_OPT_POST_1,
			&err);
}

void ftpd_data_transfer_waitting(int timeout)
{
	OS_ERR err;
	CPU_TS ts;

	OSSemPend(&FTPD_DATA_TRAN_COMPL_SEM,
			(OS_TICK)timeout,
			OS_OPT_PEND_BLOCKING,
			&ts,
			&err);
}

/* ftpd thread.
 *  process ftp command.
 *
 *  */
void ftpd_cmd_thread(void *p_arg)
{
	OS_ERR ERR;
	struct ftpd_ulp *ulp = &FTPD_ULP;
	int id = FTPD_CLIENT_ERR;
	int result, on;

	ulp->CmdLocalAddr.sin_family 		= AF_INET;
	ulp->CmdLocalAddr.sin_addr.s_addr	= htonl(INADDR_ANY);
	ulp->CmdLocalAddr.sin_port			= htons(FTPD_CMD_PORT);

	ulp->CmdServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	setsockopt(ulp->CmdServer, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
	result = bind(ulp->CmdServer,(struct sockaddr*)&ulp->CmdLocalAddr,sizeof(ulp->CmdLocalAddr));
	if(result<0) ps_debugout("command bind error ## %d ##\r\n", result);
	result = listen(ulp->CmdServer,1);
	if(result<0) ps_debugout("command listen error ## %d ##\r\n", result);

	ftpd_user_data_conn_setidle();

	while(1){

		id = get_ftpd_next_idle(ulp);

		if( id == FTPD_CLIENT_ERR ){

			OSTimeDly(1000, OS_OPT_TIME_DLY, &ERR); /* 1秒后，重新开始监听  */
		}
		else{

			ulp->CmdClient[id] = accept(ulp->CmdServer, 0, 0);

			if(ulp->CmdClient[id]<=0){

				closesocket(ulp->CmdClient[id]);
				set_ftpd_one_idle(ulp, id);

				ps_debugout("NET %d ACCEPT ERROR\r\n", id);
				OSTimeDly(1000, OS_OPT_TIME_DLY, &ERR);	/* 1s后,重新进入 ACCEPT状态  */

				continue;
			}

			OSSemSet(&FTPD_MSG_START_SEM[id], 0, &ERR);

			OSSemPost(&FTPD_MSG_START_SEM[id],
					OS_OPT_POST_1,
					&ERR);

//			Uart_Printf("client %d accept\r\n", id);
		}
	}
}

/* ftpd thread.
 *  process ftp data.
 *
 *  */
void ftpd_data_thread(void *p_arg)
{
	OS_ERR ERR;
	CPU_TS ts;

	struct ftpd_ulp *ulp       = &FTPD_ULP;
	struct ftpd_datastate *fsd = &FTPD_DATA;
//	static int port = 4096;
//	int result, on;

	while(1){

		OSSemPend(&FTPD_DATA_TRAN_START_SEM,
				(OS_TICK)0,
				OS_OPT_PEND_BLOCKING,
				&ts,
				&ERR);

		switch(fsd->mode){

		case FTPD_PORT:

			ftpd_data_connect(fsd, ulp);

			if(fsd->connected){
				/* data transfer */
				ftpd_data_server(ulp);
			}

			if(fsd->connected){
				ftpd_data_close(fsd, ulp);
//				Uart_Printf("connect close.\r\n");
			}

			break;

		case FTPD_PASV:

//			if(fsd->connected==0){
//			ulp->DataLocalAddr.sin_family 		= AF_INET;
//			ulp->DataLocalAddr.sin_addr.s_addr	= htonl(INADDR_ANY);
//			ulp->DataLocalAddr.sin_port			= htons(port);
//
//			ulp->DataServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
//			setsockopt(ulp->DataServer, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
//			do{
//				result = bind(ulp->DataServer,(struct sockaddr*)&ulp->DataLocalAddr,sizeof(ulp->DataLocalAddr));
//				if(result<0) {
//					Uart_Printf("data bind error ## %d ##\r\n", result);
//					if(port>0x7fff) port = 4096; else port += 1;
//				}
//			}while(result<0);
//
//			result = listen(ulp->DataServer,1);
//			if(result<0) {
//				fsd->connected = 0;
//				Uart_Printf("data listen error ## %d ##\r\n", result);
//			}
//			else{
//
//				OSSemPost (&FTPD_DATA_TRAN_COMPL_SEM,
//						OS_OPT_POST_1,
//						&ERR);
//
//				if(port>0x7fff) port = 4096; else port += 1;
//
//				Uart_Printf(" pasv mode : accept!\r\n");
//
//				ulp->DataClient = accept(ulp->DataServer, 0, 0);
//
//				Uart_Printf(" pasv mode : accept EXIT !\r\n");
//
//				if(ulp->DataClient<=0){
//					fsd->connected = 0;
//				}
//				else{
//					fsd->connected  = 1;
//					break;
//				}
//			}
//
//			}
//			else{
//				/* data transfer */
//				ftpd_data_server(ulp);
//
//				closesocket(ulp->DataServer);
//				closesocket(ulp->DataClient);
//				fsd->connected = 0;
//				Uart_Printf("connect close.\r\n");
//			}

			/*
			ulp->DataLocalAddr.sin_family 		= AF_INET;
			ulp->DataLocalAddr.sin_addr.s_addr	= htonl(INADDR_ANY);
			ulp->DataLocalAddr.sin_port			= htons(FTPD_DATA_PORT);

			ulp->DataServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			setsockopt(ulp->DataServer, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
			result = bind(ulp->DataServer,(struct sockaddr*)&ulp->DataLocalAddr,sizeof(ulp->DataLocalAddr));
			if(result<0) Uart_Printf("data bind error ## %d ##\r\n", result);

			result = listen(ulp->DataServer,1);
			if(result<0) {
				fsd->connected = 0;
				Uart_Printf("data listen error ## %d ##\r\n", result);
			}
			else{

				OSSemPost (&FTPD_DATA_TRAN_COMPL_SEM,
						OS_OPT_POST_1,
						&ERR);

				Uart_Printf(" pasv mode : accept!\r\n");

				ulp->DataClient = accept(ulp->DataServer, 0, 0);

				Uart_Printf(" pasv mode : accept EXIT !\r\n");

				if(ulp->DataClient<=0){
					fsd->connected = 0;
				}
				else{
					fsd->connected = 1;
				}
			}
*/
			break;

		default:
			break;
		}



		OSSemPost (&FTPD_DATA_TRAN_COMPL_SEM,
				OS_OPT_POST_1,
				&ERR);
	}
}

void ftpd_msg_thread(void *p_arg)
{
	OS_ERR ERR;
	CPU_TS ts;
	struct ftpd_ulp *ulp = &FTPD_ULP;
	int id = (int)p_arg;

	if(id >= MAX_FTPD_CLIENT)
		return;

	while(1){

		OSSemPend(&FTPD_MSG_START_SEM[id],
				(OS_TICK)0,
				OS_OPT_PEND_BLOCKING,
				&ts,
				&ERR);

		ftpd_reinit();

		/* ftp server */
		ftpd_msg_server(ulp, id);

		if(closesocket(ulp->CmdClient[id]) != -1) ulp->CmdClient[id] = -1;

		set_ftpd_one_idle(ulp, id);

//		Uart_Printf("client %d idle\r\n", id);

		//OSTimeDly(1000, OS_OPT_TIME_DLY, &ERR); /* 1秒后，重新开始监听  */
	}
}


#if 0
/* ftpd thread.
 *  process ftp command.
 *
 *  */
void ftpd_cmd_thread(void *p_arg)
{
	OS_ERR err;
	struct ftpd_ulp *ulp = &FTPD_ULP;

	/* CREATE NET */
	ulp->pCmdSevConn = netconn_new(NETCONN_TCP);
	if(ulp->pCmdSevConn == NULL){
		Uart_Printf("NET CREATE ERROR\r\n");
		goto FtpdCmdThreadError;
	}

	/* BIND */
	err = netconn_bind(ulp->pCmdSevConn, IP_ADDR_ANY, ulp->CmdLocalPort);
	if(err != ERR_OK){
		netconn_delete(ulp->pCmdSevConn);
		Uart_Printf("NET BIND ERROR ## %d ##\r\n",err);
		goto FtpdCmdThreadError;
	}

	/* LISTEN */
	err = netconn_listen(ulp->pCmdSevConn);
	if(err != ERR_OK){
		netconn_delete(ulp->pCmdSevConn);
		Uart_Printf("NET LISTEN ERROR ## %d ##\r\n",err);
		goto FtpdCmdThreadError;
	}

	while(1){

		/* ACCEPT */

		Uart_Printf("waitting user login\r\n");

		err = netconn_accept(ulp->pCmdSevConn,&ulp->pCmdUserConn);
		if(err != ERR_OK){
			if(ulp->pCmdUserConn != NULL){
				netconn_delete(ulp->pCmdUserConn);
			}
			Uart_Printf("NET ACCEPT ERROR ## %d ##\r\n",err);

			OSTimeDly(1000, OS_OPT_TIME_DLY, &err);	/* 1s后,重新进入 ACCEPT状态  */
			continue;
		}

		/* ftp server */
		ftpd_msg_server(ulp);

		OSTimeDly(1000, OS_OPT_TIME_DLY, &err); /* 1秒后，重新开始监听  */
	}

	/* exit */
	netconn_close(ulp->pCmdSevConn);
	netconn_delete(ulp->pCmdSevConn);

FtpdCmdThreadError:
	while(1) OSTimeDly(1000, OS_OPT_TIME_DLY, &err); /* loop */
}



/* ftpd thread.
 *  process ftp data.
 *
 *  */
void ftpd_data_thread(void *p_arg)
{
	int err;
	OS_ERR ERR;
	CPU_TS ts;
	struct ftpd_ulp		  *ulp = &FTPD_ULP;

	while(1){

		OSSemPend(&FTPD_DATA_TRAN_START_SEM,
				(OS_TICK)0,
				OS_OPT_PEND_BLOCKING,
				&ts,
				&ERR);

		Uart_Printf("ftpd_data_thread sem pend!\r\n");

		if(ERR == OS_ERR_NONE){

			Uart_Printf(" ftpd data create net!\r\n");

			/* CREATE NET */
			ulp->pDataConn = netconn_new(NETCONN_TCP);
			if(ulp->pDataConn == NULL){
				Uart_Printf("NET CREATE ERROR\r\n");
				OSTimeDly(1000, OS_OPT_TIME_DLY, &ERR);
				continue;
			}

			/* BIND */
			err = netconn_bind(ulp->pDataConn, IP_ADDR_ANY, ulp->DataLocalPort);
			if(err != ERR_OK){
//				netconn_delete(ulp->pDataConn);
				Uart_Printf("NET BIND ERROR ## %d ##\r\n",err);
//				OSTimeDly(1000, OS_OPT_TIME_DLY, &ERR);
//				continue;
			}

			/* CONNECT TARGET SERVER */
			err = netconn_connect(ulp->pDataConn, &ulp->DataIpAddr, ulp->DataDestPort);
			if(err != ERR_OK){
//				netconn_delete(ulp->pDataConn); /* delete net */
				Uart_Printf("CONNETC TARGET ERROR ## %d ##\r\n",err);
//				OSTimeDly(1000, OS_OPT_TIME_DLY, &ERR);
//				continue;
			}

			/* data transfer */
			ftpd_data_server(ulp);

			err = netconn_close(ulp->pDataConn);
			if(err!=ERR_OK) Uart_Printf("net close error! %d\r\n",err);
			err = netconn_delete(ulp->pDataConn);
			if(err!=ERR_OK) Uart_Printf("net delete error! %d\r\n",err);
			Uart_Printf("connect close.\r\n");
		}

		OSSemPost (&FTPD_DATA_TRAN_COMPL_SEM,
				OS_OPT_POST_1,
				&ERR);
	}

//FtpdDataThreadError:
//	while(1) OSTimeDly(1000, OS_OPT_TIME_DLY, &ERR); /* loop */
}
#endif

#ifndef _FTPD_BASE_H_
#define _FTPD_BASE_H_

#include "lwip/sockets.h"

#define FTPD_DEBUG

#define PATH_LEN	160
#define CHAR_LEN	20

#define FTPD_DATA_LEN	1072

#define FTPD_DATA_PORT	20
#define FTPD_CMD_PORT	21

#define MAX_FTPD_CLIENT	4
#define FTPD_CLIENT_ERR	0xFF

enum ftpd_state_e {
	FTPD_USER,
	FTPD_PASS,
	FTPD_IDLE,
	FTPD_NLST,
	FTPD_LIST,
	FTPD_RETR,
	FTPD_RNFR,
	FTPD_STOR,
	FTPD_QUIT
};

enum ftpd_mode_e{
	FTPD_PORT,
	FTPD_PASV
};


struct ftpd_ulp{
	/* idle client */
	int idle_table[MAX_FTPD_CLIENT];

	/* socket command*/
	int CmdServer;
	int CmdClient[MAX_FTPD_CLIENT];
	struct sockaddr_in CmdLocalAddr;

	/* socket data */
	int DataServer;
	int DataClient;
	struct sockaddr_in DataLocalAddr;
	struct sockaddr_in DataRemoteAddr;

	int  rdwr_protect[MAX_FTPD_CLIENT];
	char username[MAX_FTPD_CLIENT][CHAR_LEN];
	char password[MAX_FTPD_CLIENT][CHAR_LEN];

#if 0
	/* Command */
	struct netconn  *pCmdSevConn;
	struct netconn  *pCmdUserConn;
	struct netbuf	*pCmdNetBuf;
	INT16U CmdLocalPort;	/* 本地端口  */

	/* data */
	struct netconn  *pDataConn;
	struct netbuf   *pDataNetBuf;
	struct ip_addr   DataIpAddr; /* 目标IP地址 */
	INT16U DataLocalPort;		 /* 本地端口  */
	INT16U DataDestPort;		 /* 远程端口  */
#endif
};


struct ftpd_datastate {
	enum ftpd_mode_e mode;
	int connected;

	struct ftpd_fs{
		char    Path[PATH_LEN];
		FIL 	*pFile;
		FILINFO	*pFileInfo;
		DIR 	*pDir;
	}fs;

	struct ftpd_msgstate *msgstate;
};

struct ftpd_msgstate {
	enum ftpd_state_e state;

	struct ftpd_datastate *datastate;
	int passive;
	char *renamefrom;
};

extern FIL 		FsFile;
extern DIR 		FsDir;
extern FILINFO 	FsFileInfo;

#ifdef FTPD_DEBUG
extern void Uart_Printf(char *fmt, ...);
#else
#define Uart_Printf(f, ...)
#endif



void ftpd_do_cmd(struct ftpd_msgstate *fsm, struct ftpd_ulp *ulp, int id, void *data, int len);
int  ftpd_data_connect(struct ftpd_datastate *fsd, struct ftpd_ulp *ulp);
int  ftpd_data_close(struct ftpd_datastate *fsd, struct ftpd_ulp *ulp);
int  ftpd_send_data(struct ftpd_datastate *fsd, struct ftpd_ulp *ulp, char *data, int len);
int  ftpd_send_msg(struct ftpd_msgstate *fsm, struct ftpd_ulp *ulp, int id, char *msg, ...);
void ftpd_data_transfer_startup(void);
void ftpd_data_transfer_waitting(int timeout);
void ftpd_user_data_conn_setidle(void);
void ftpd_user_data_conn_waitting(int timeout);
void ftpd_server_close(void);
void ftpd_server_open(void);

#endif

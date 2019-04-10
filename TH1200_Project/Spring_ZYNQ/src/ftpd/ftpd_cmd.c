#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <ctype.h>

#include "adt_datatype.h"
#include "fatfs/ff.h"
#include "lwip_net/lwip_net.h"

#include "ftpd_base.h"
#include "ftpd_msg.h"
#include "base.h"


FIL 		FsFile;
DIR 		FsDir;
FILINFO 	FsFileInfo;

static void cmd_user(struct ftpd_msgstate *fsm, struct ftpd_ulp *ulp, int id, const char *arg)
{
	memcpy(ulp->username[id], arg, CHAR_LEN);

	ftpd_send_msg(fsm, ulp, id, msg331);
	fsm->state = FTPD_PASS;
}

static void cmd_pass(struct ftpd_msgstate *fsm, struct ftpd_ulp *ulp, int id, const char *arg)
{
	memcpy(ulp->password[id], arg, CHAR_LEN);

	if( (0 == memcmp(ulp->username[id], "adt", 4)) && (0 == memcmp(ulp->password[id], "adt", 4)) ){
		ulp->rdwr_protect[id] = FALSE;
	}
	else{
		ulp->rdwr_protect[id] = TRUE;
	}

	ftpd_send_msg(fsm, ulp, id, msg230);
	fsm->state = FTPD_IDLE;
}

static void cmd_port(struct ftpd_msgstate *fsm, struct ftpd_ulp *ulp, int id, const char *arg)
{
	struct ftpd_datastate *fsd = fsm->datastate;
	int nr;
	unsigned int pHi, pLo;
	unsigned int ip[4];
	u32 ipaddr;
	u16 port;

	nr = sscanf(arg, "%u,%u,%u,%u,%u,%u", &(ip[0]), &(ip[1]), &(ip[2]), &(ip[3]), &pHi, &pLo);
	if (nr != 6) {
		ftpd_send_msg(fsm, ulp, id, msg501);
	} else {
		ipaddr = (((ip[0]&0xff)<<24) | ((ip[1]&0xff)<<16) | ((ip[2]&0xff)<<8) | ((ip[3]&0xff)<<0));
		port   = ((u16_t)pHi << 8) | (u16_t)pLo;
		ulp->DataRemoteAddr.sin_family 		= AF_INET;
		ulp->DataRemoteAddr.sin_addr.s_addr	= htonl(ipaddr);
		ulp->DataRemoteAddr.sin_port		= htons(port);

		if(ulp->DataServer != -1){
			shutdown(ulp->DataServer, SHUT_RDWR);
			closesocket(ulp->DataServer);
			ps_debugout("closesocket DataServer!\r\n");
		}

		if(ulp->DataClient != -1){
			shutdown(ulp->DataClient, SHUT_RDWR);
			closesocket(ulp->DataClient);
			ps_debugout("closesocket DataClient!\r\n");
		}

		fsd->mode = FTPD_PORT;

//		Uart_Printf("IP: %d.%d.%d.%d port:%d\r\n",
//				(ipaddr>>24)&0xff,
//				(ipaddr>>16)&0xff,
//				(ipaddr>>8) &0xff,
//				(ipaddr>>0) &0xff,
//				 port);



		/*
		IP4_ADDR(&fsm->dataip, (u8_t) ip[0], (u8_t) ip[1], (u8_t) ip[2], (u8_t) ip[3]);
		fsm->dataport = ((u16_t) pHi << 8) | (u16_t) pLo;
		*/
//		IP4_ADDR(&ulp->DataIpAddr,(u8_t)ip[0],(u8_t)ip[1], (u8_t)ip[2], (u8_t)ip[3]);
//		ulp->DataDestPort = ((u16_t)pHi << 8) | (u16_t)pLo;
//		Uart_Printf("IP: %d.%d.%d.%d port:%d\r\n",
//				(ulp->DataIpAddr.addr>>0) &0xff,
//				(ulp->DataIpAddr.addr>>8) &0xff,
//				(ulp->DataIpAddr.addr>>16)&0xff,
//				(ulp->DataIpAddr.addr>>24)&0xff,
//				ulp->DataDestPort);

		ftpd_send_msg(fsm, ulp, id, msg200);
	}
}

static void cmd_quit(struct ftpd_msgstate *fsm, struct ftpd_ulp *ulp, int id, const char *arg)
{
	ftpd_send_msg(fsm, ulp, id, msg221);
	fsm->state = FTPD_QUIT;
}

static void cmd_cwd(struct ftpd_msgstate *fsm, struct ftpd_ulp *ulp, int id, const char *arg)
{
	struct ftpd_datastate *fsd = fsm->datastate;
//	FRESULT res;

	/* 这里的问题是/0:/test/第一个/和最后一个/的问题 */
	int  i = 0;
	int  iLen;
	char *pt = (char*)arg;

//Uart_Printf("arg = %s \r\n",arg);

	//去掉第一个/
	if( 0 == memcmp("/0:", arg, 3) ) pt++;
	//去掉最后的一个/
	while( pt[i] != '\0' ) i++;
	if( (i > 3) && (pt[i-1] == '/') ) pt[i-1] = '\0';

	if(0==memcmp("0:", pt, 2)){
		sprintf((void*)fsd->fs.Path, "%s", pt);
	}
	else if(*pt == '/' || *pt == '\\'){
		sprintf((void*)fsd->fs.Path, "%s", "0:");
		strcat((void*)fsd->fs.Path, pt);
	}
	else{
		iLen=strlen(fsd->fs.Path);
		if(iLen && fsd->fs.Path[iLen-1]!='/')
			strcat(fsd->fs.Path, "/");
		strcat(fsd->fs.Path, pt);
	}
	ftpd_send_msg(fsm, ulp, id, msg250);

//	if (FR_OK == res) {
//		ftpd_send_msg(fsm, ulp, id, msg250);
//	} else {
//		ftpd_send_msg(fsm, ulp, id, msg550);
//	}
}

static void cmd_cdup(struct ftpd_msgstate *fsm, struct ftpd_ulp *ulp, int id, const char *arg)
{
	struct ftpd_datastate *fsd = fsm->datastate;
	char *pt;
	int  len;

	pt  = (char*)fsd->fs.Path;
	len = strlen(pt);
	pt += len-1; //指向最后一个字符

	if(len<4)
		ftpd_send_msg(fsm, ulp, id, msg550);

	while(!(*pt=='/' || *pt==':')) pt--;
	*pt = '\0';
	ftpd_send_msg(fsm, ulp, id, msg250);

//	if (FR_OK == f_chdir("..")) {
//		ftpd_send_msg(fsm, ulp, id, msg250);
//	} else {
//		ftpd_send_msg(fsm, ulp, id, msg550);
//	}
}

static void cmd_pwd(struct ftpd_msgstate *fsm, struct ftpd_ulp *ulp, int id, const char *arg)
{
	struct ftpd_datastate *fsd = fsm->datastate;
	char Path[PATH_LEN] = {0};

	strcpy(Path, fsd->fs.Path);
	ftpd_send_msg(fsm, ulp, id, msg257PWD, Path);
//	if (FR_OK == f_getcwd(Path, PATH_LEN)) {
//		ftpd_send_msg(fsm, ulp, id, msg257PWD, Path);
//	} else {
//		ftpd_send_msg(fsm, ulp, id, msg451);
//	}
}

static void cmd_list_common(struct ftpd_msgstate *fsm, struct ftpd_ulp *ulp, int id, const char *arg, int shortlist)
{
	char Path[PATH_LEN] = {0};
	struct ftpd_datastate *fsd = fsm->datastate;
	FRESULT res;
	DIR 	*tdp  = &FsDir;
	FILINFO *tfno = &FsFileInfo;

	/* 获取当前路径  */
	strcpy(Path, fsd->fs.Path);
//	res = f_getcwd(Path, PATH_LEN);
//	if (FR_OK != res) {
//		ftpd_send_msg(fsm, ulp, id, msg451);
//		return;
//	}

	/* 打开当前目录  */
	res = f_opendir(tdp, Path);
	if (FR_OK != res) {
		ftpd_send_msg(fsm, ulp, id, msg451);
		return;
	}

	if (shortlist != 0)
		fsm->state = FTPD_NLST;
	else
		fsm->state = FTPD_LIST;

	fsd->fs.pDir 	   	= tdp;
	fsd->fs.pFileInfo  	= tfno;

	ftpd_send_msg(fsm, ulp, id, msg150);

	ftpd_data_transfer_startup();

	ftpd_data_transfer_waitting(0);

	f_closedir(tdp);
	ftpd_send_msg(fsd->msgstate, ulp, id, msg226);
}

static void cmd_nlst(struct ftpd_msgstate *fsm, struct ftpd_ulp *ulp, int id, const char *arg)
{
	cmd_list_common(fsm, ulp, id, arg, 1);
}

static void cmd_list(struct ftpd_msgstate *fsm, struct ftpd_ulp *ulp, int id, const char *arg)
{
	cmd_list_common(fsm, ulp, id, arg, 0);
}

static void cmd_retr(struct ftpd_msgstate *fsm, struct ftpd_ulp *ulp, int id, const char *arg)
{
	char Path[PATH_LEN] = {0};
	struct ftpd_datastate *fsd = fsm->datastate;
	FRESULT res;
	FILINFO *tfno = &FsFileInfo;
	FIL 	*tfp  = &FsFile;
	int  iLen;

	//判断是不是绝对路径
	if(0 == memcmp("/0:", arg, 3))
	{
		strcpy(Path, arg+1);
	}
	else{
		strcpy(Path, fsd->fs.Path);
		iLen=strlen(Path);
		if(iLen && Path[iLen-1]!='/')
			strcat(Path, "/");
		strcat(Path, arg);
	}

	res = f_stat(Path, tfno);
	if (FR_OK != res) {
		ftpd_send_msg(fsm, ulp, id, msg550);
		return;
	}

	res = f_open(tfp, Path, FA_READ|FA_WRITE);
	if(res != FR_OK){
		ps_debugout("retr ftp f_open error 0x%x\r\n",res);
		ftpd_send_msg(fsm, ulp, id, msg550);
		return ;
	}

	fsm->datastate->fs.pFile     = tfp;
	fsm->datastate->fs.pFileInfo = tfno;
	fsm->state = FTPD_RETR;

	ftpd_send_msg(fsm, ulp, id, msg150recv, arg, tfp->fsize);

	ftpd_data_transfer_startup();

	ftpd_data_transfer_waitting(0);

	f_close(tfp);
	ftpd_send_msg(fsm, ulp, id, msg226);
}

static void cmd_stor(struct ftpd_msgstate *fsm, struct ftpd_ulp *ulp, int id, const char *arg)
{
	char Path[PATH_LEN] = {0};
	struct ftpd_datastate *fsd = fsm->datastate;
	FRESULT res;
	FILINFO *tfno = &FsFileInfo;
	FIL 	*tfp  = &FsFile;
	int  iLen;

	//判断是不是绝对路径
	if(0 == memcmp("/0:", arg, 3))
	{
		strcpy(Path, arg+1);
	}
	else{
		strcpy(Path, fsd->fs.Path);
		iLen=strlen(Path);
		if(iLen && Path[iLen-1]!='/')
			strcat(Path, "/");
		strcat(Path, arg);
	}

	res = f_open(tfp, Path, FA_CREATE_ALWAYS|FA_READ|FA_WRITE);
	if (res != FR_OK) {
		ps_debugout("stor ftp f_open error 0x%x\r\n", res);
		ftpd_send_msg(fsm, ulp, id, msg550);
		return;
	}

	fsm->datastate->fs.pFile 	 = tfp;
	fsm->datastate->fs.pFileInfo = tfno;
	fsm->state = FTPD_STOR;

	ftpd_send_msg(fsm, ulp, id, msg150stor, arg);

	ftpd_data_transfer_startup();

	ftpd_data_transfer_waitting(0);

	f_close(tfp);
	ftpd_send_msg(fsm, ulp, id, msg226);
}

static void cmd_noop(struct ftpd_msgstate *fsm, struct ftpd_ulp *ulp, int id, const char *arg)
{
	ftpd_send_msg(fsm, ulp, id, msg200);
}

static void cmd_syst(struct ftpd_msgstate *fsm, struct ftpd_ulp *ulp, int id, const char *arg)
{
	ftpd_send_msg(fsm, ulp, id, msg214SYST, "UNIX");
}

#if 0
static void cmd_pasv(struct ftpd_msgstate *fsm, struct ftpd_ulp *ulp, int id, const char *arg)
{
	struct ftpd_datastate *fsd = fsm->datastate;
	u16 port;

	if(ulp->DataServer != -1){
		shutdown(ulp->DataServer, SHUT_RDWR);
		closesocket(ulp->DataServer);
	}

	if(ulp->DataClient != -1){
		shutdown(ulp->DataClient, SHUT_RDWR);
		closesocket(ulp->DataClient);
	}

	fsd->connected 	= 0;
	fsd->mode 		= FTPD_PASV;

	ftpd_data_transfer_startup();

	ftpd_data_transfer_waitting(0);

	port = htons(ulp->DataLocalAddr.sin_port);
	ftpd_send_msg(fsm, ulp, id, msg227, 192,168,1,214,(port>>8)&0xff, (port&0xff));
}
#endif

static void cmd_abrt(struct ftpd_msgstate *fsm, struct ftpd_ulp *ulp, int id, const char *arg)
{
//	if (fsm->datafs != NULL) {
//		tcp_arg(fsm->datapcb, NULL);
//		tcp_sent(fsm->datapcb, NULL);
//		tcp_recv(fsm->datapcb, NULL);
//		tcp_arg(fsm->datapcb, NULL);
//		tcp_abort(pcb);
//		sfifo_close(&fsm->datafs->fifo);
//		free(fsm->datafs);
//		fsm->datafs = NULL;
//	}
	fsm->state = FTPD_IDLE;
}

static void cmd_type(struct ftpd_msgstate *fsm, struct ftpd_ulp *ulp, int id, const char *arg)
{
	ftpd_send_msg(fsm, ulp, id, msg200);
}

static void cmd_mode(struct ftpd_msgstate *fsm, struct ftpd_ulp *ulp, int id, const char *arg)
{
	ftpd_send_msg(fsm, ulp, id, msg200);
}

static void cmd_rnfr(struct ftpd_msgstate *fsm, struct ftpd_ulp *ulp, int id, const char *arg)
{
	int len;

	if (arg == NULL) {
		ftpd_send_msg(fsm, ulp, id, msg501);
		return;
	}

	if (*arg == '\0') {
		ftpd_send_msg(fsm, ulp, id, msg501);
		return;
	}

	if (fsm->renamefrom){
		free(fsm->renamefrom);
		fsm->renamefrom = 0;
	}

	len = strlen(arg)+1;
	fsm->renamefrom = malloc(len);
	if (!fsm->renamefrom) {
		ftpd_send_msg(fsm, ulp, id, msg451);
		return;
	}

	strcpy(fsm->renamefrom, arg);
	fsm->state = FTPD_RNFR;

	ftpd_send_msg(fsm, ulp, id, msg350);
}

static void cmd_rnto(struct ftpd_msgstate *fsm, struct ftpd_ulp *ulp, int id, const char *arg)
{
	char Path1[PATH_LEN] = {0};
	char Path2[PATH_LEN] = {0};
	struct ftpd_datastate *fsd = fsm->datastate;
	int  iLen;

	if (fsm->state != FTPD_RNFR) {
		ftpd_send_msg(fsm, ulp, id, msg503);
		return;
	}
	fsm->state = FTPD_IDLE;
	if (arg == NULL) {
		ftpd_send_msg(fsm, ulp, id, msg501);
		return;
	}
	if (*arg == '\0') {
		ftpd_send_msg(fsm, ulp, id, msg501);
		return;
	}

	//判断是不是绝对路径
	if(0 == memcmp("/0:",fsm->renamefrom, 3))
	{
		strcpy(Path1, fsm->renamefrom+1);
	}
	else{
		strcpy(Path1, fsd->fs.Path);
		iLen=strlen(Path1);
		if(iLen && Path1[iLen-1]!='/')
			strcat(Path1, "/");
		strcat(Path1, fsm->renamefrom);
	}

	//判断是不是绝对路径
	if(0 == memcmp("/0:", arg, 3))
	{
		strcpy(Path2, arg+1);
	}
	else{
		strcpy(Path2, fsd->fs.Path);
		iLen=strlen(Path2);
		if(iLen && Path2[iLen-1]!='/')
			strcat(Path2, "/");
		strcat(Path2, arg);
	}
//Uart_Printf("arg: %s \r\n",arg);
//Uart_Printf("fsm->renamefrom: %s \r\n",fsm->renamefrom);
//Uart_Printf("fsd->fs.Path: %s \r\n",fsd->fs.Path);
//Uart_Printf("Path1: %s \r\n",Path1);
//Uart_Printf("Path2: %s \r\n",Path2);

	if ( FR_OK != f_rename(Path1, Path2) ) {
		ftpd_send_msg(fsm, ulp, id, msg450);
	} else {
		ftpd_send_msg(fsm, ulp, id, msg250);
	}

	if(fsm->renamefrom){
//		Uart_Printf("@@free: 0x%x\r\n",fsm->renamefrom);
		free(fsm->renamefrom);
		fsm->renamefrom = 0;
	}

}

static void cmd_mkd(struct ftpd_msgstate *fsm, struct ftpd_ulp *ulp, int id, const char *arg)
{
	char Path[PATH_LEN] = {0};
	struct ftpd_datastate *fsd = fsm->datastate;
	int  iLen;

	if (arg == NULL) {
		ftpd_send_msg(fsm, ulp, id, msg501);
		return;
	}
	if (*arg == '\0') {
		ftpd_send_msg(fsm, ulp, id, msg501);
		return;
	}

	//判断是不是绝对路径
	if(0 == memcmp("/0:", arg, 3))
	{
		strcpy(Path, arg+1);
	}
	else{
		strcpy(Path, fsd->fs.Path);
		iLen=strlen(Path);
		if(iLen && Path[iLen-1]!='/')
			strcat(Path, "/");
		strcat(Path, arg);
	}
//	if ( FR_OK != f_mkdir(arg) ) {
	if ( FR_OK != f_mkdir(Path) ) {
		ftpd_send_msg(fsm, ulp, id, msg550);
	} else {
		ftpd_send_msg(fsm, ulp, id, msg257, arg);
	}
}

static void cmd_rmd(struct ftpd_msgstate *fsm, struct ftpd_ulp *ulp, int id, const char *arg)
{
	char Path[PATH_LEN] = {0};
	struct ftpd_datastate *fsd = fsm->datastate;
	int  iLen;

	FILINFO *tfno = &FsFileInfo;

	if (arg == NULL) {
		ftpd_send_msg(fsm, ulp, id, msg501);
		return;
	}
	if (*arg == '\0') {
		ftpd_send_msg(fsm, ulp, id, msg501);
		return;
	}

	//判断是不是绝对路径
	if(0 == memcmp("/0:", arg, 3))
	{
		strcpy(Path, arg+1);
	}
	else{
		strcpy(Path, fsd->fs.Path);
		iLen=strlen(Path);
		if(iLen && Path[iLen-1]!='/')
			strcat(Path, "/");
		strcat(Path, arg);
	}

//	if (f_stat(arg, tfno) != FR_OK) {
	if (f_stat(Path, tfno) != FR_OK) {
		ftpd_send_msg(fsm, ulp, id, msg550);
		return;
	}
//	if (f_unlink(arg) != FR_OK) {
	if (f_unlink(Path) != FR_OK) {
		ftpd_send_msg(fsm, ulp, id, msg550);
	} else {
		ftpd_send_msg(fsm, ulp, id, msg250);
	}
}

static void cmd_dele(struct ftpd_msgstate *fsm, struct ftpd_ulp *ulp, int id, const char *arg)
{
	char Path[PATH_LEN] = {0};
	struct ftpd_datastate *fsd = fsm->datastate;
	FILINFO *tfno = &FsFileInfo;
	int  iLen;

	if (arg == NULL) {
		ftpd_send_msg(fsm, ulp, id, msg501);
		return;
	}
	if (*arg == '\0') {
		ftpd_send_msg(fsm, ulp, id, msg501);
		return;
	}

	//判断是不是绝对路径
	if(0 == memcmp("/0:", arg, 3))
	{
		strcpy(Path, arg+1);
	}
	else{
		strcpy(Path, fsd->fs.Path);
		iLen=strlen(Path);
		if(iLen && Path[iLen-1]!='/')
			strcat(Path, "/");
		strcat(Path, arg);
	}

	if (f_stat(Path, tfno) != FR_OK) {
		ftpd_send_msg(fsm, ulp, id, msg550);
		return;
	}
	if (tfno->fattrib & AM_DIR) {
		ftpd_send_msg(fsm, ulp, id, msg550);
		return;
	}
	if (f_unlink(Path) != FR_OK) {
		ftpd_send_msg(fsm, ulp, id, msg550);
	} else {
		ftpd_send_msg(fsm, ulp, id, msg250);
	}
}

static void cmd_size(struct ftpd_msgstate *fsm, struct ftpd_ulp *ulp, int id, const char *arg)
{
	struct ftpd_datastate *fsd = fsm->datastate;

	if(!fsd->fs.pFile || !arg || !*arg) {
		ftpd_send_msg(fsm, ulp, id, msg501);
		return ;
	}

	ftpd_send_msg(fsm, ulp, id, msg213, fsd->fs.pFileInfo->fsize);
}

static void cmd_opts(struct ftpd_msgstate *fsm, struct ftpd_ulp *ulp, int id, const char *arg)
{
	ftpd_send_msg(fsm, ulp, id, msg501);
}

struct ftpd_command {
	char *cmd;
	void (*func) (struct ftpd_msgstate *fsm, struct ftpd_ulp *ulp, int id, const char *arg);
	int  data_conn_protect;
	int  user_rdwr_protect;
};

static struct ftpd_command ftpd_commands[] = {
	{"USER", cmd_user, FALSE, FALSE},
	{"PASS", cmd_pass, FALSE, FALSE},
	{"PORT", cmd_port, TRUE,  FALSE},
	{"QUIT", cmd_quit, FALSE, FALSE},
	{"CWD",  cmd_cwd,  TRUE,  FALSE},
	{"CDUP", cmd_cdup, TRUE,  FALSE},
	{"PWD",  cmd_pwd,  TRUE,  FALSE},
	{"XPWD", cmd_pwd,  TRUE,  FALSE},
	{"NLST", cmd_nlst, TRUE,  FALSE},
	{"LIST", cmd_list, TRUE,  FALSE},
	{"RETR", cmd_retr, TRUE,  FALSE},
	{"STOR", cmd_stor, TRUE,  TRUE },
	{"NOOP", cmd_noop, FALSE, FALSE},
	{"SYST", cmd_syst, FALSE, FALSE},
	{"ABOR", cmd_abrt, FALSE, FALSE},
	{"TYPE", cmd_type, FALSE, FALSE},
	{"MODE", cmd_mode, FALSE, FALSE},
	{"RNFR", cmd_rnfr, TRUE,  TRUE },
	{"RNTO", cmd_rnto, TRUE,  TRUE },
	{"MKD",  cmd_mkd,  TRUE,  TRUE },
	{"XMKD", cmd_mkd,  TRUE,  TRUE },
	{"RMD",  cmd_rmd,  TRUE,  TRUE },
	{"XRMD", cmd_rmd,  TRUE,  TRUE },
	{"DELE", cmd_dele, TRUE,  TRUE },
	{"SIZE", cmd_size, TRUE,  FALSE},
	{"OPTS", cmd_opts, FALSE, FALSE},
//	{"PASV", cmd_pasv, TRUE,  TRUE },
	{NULL, NULL, FALSE, FALSE}
};


void ftpd_do_cmd(struct ftpd_msgstate *fsm, struct ftpd_ulp *ulp, int id, void *data, int len)
{
	int i;
	struct ftpd_command *ftpd_cmd;
	u8 command[5];
	u8 *cmd,*arg;

	/* 去掉 '\r' 和 '\n' */
	i = 2;
	cmd = (u8*)(data+len-1);
	while (((*cmd == '\r') || (*cmd == '\n')) && i>0){
		*cmd-- = '\0';
		i -= 1;
	}

	strncpy((void*)command, data, 4);
	for (cmd = command; isalpha(*cmd) && cmd < &command[4]; cmd++)
		*cmd = toupper(*cmd); //转换为大写英文字符
	*cmd = '\0';

	/* 命令匹配  */
	for (ftpd_cmd = ftpd_commands; ftpd_cmd->cmd != NULL; ftpd_cmd++) {
		if (!strcmp(ftpd_cmd->cmd, (void*)command))
			break;
	}

	if (strlen(data) < (strlen((void*)command) + 1)){
		arg = (u8*)0;
	}
	else{
		arg = (u8*)(data+strlen((void*)command) + 1);
	}

	/* 执行相关命令  */
	if (ftpd_cmd->func){

		//if((ulp->rdwr_protect[id] == FALSE) || (ftpd_cmd->user_rdwr_protect == FALSE)){
		if(1){

			if(ftpd_cmd->data_conn_protect == TRUE){

				ftpd_user_data_conn_waitting(0);
				ftpd_cmd->func(fsm, ulp, id, (char*)arg);
				ftpd_user_data_conn_setidle();
			}
			else{
				ftpd_cmd->func(fsm, ulp, id, (char*)arg);
			}
		}
		else{
			ps_debugout("user not allow!\r\n");
			ftpd_send_msg(fsm, ulp, id, msg504);
		}
	}
	else{
		ps_debugout("cmd not found!\r\n");
		ftpd_send_msg(fsm, ulp, id, msg502);
	}
}

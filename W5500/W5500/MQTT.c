
#include "bsp.h"
#include "spi.h"




char ClientID[128]; // 存放客户端ID的缓冲区
int ClientID_len;	// 存放客户端ID的长度

char Username[128]; // 存放用户名的缓冲区
int Username_len;	// 存放用户名的长度

char Passward[128]; // 存放密码的缓冲区
int Passward_len;	// 存放密码的长度

unsigned char ServerIP[4];	   // 存放服务器IP
unsigned char ServerName[128]; // 存放服务器域名
int ServerPort;				   // 存放服务器的端口号


wiz_NetInfo gWIZNETINFO =				 // MAC地址自己写一个，不要和路由器下其他的设备一样即可
	{
		0x00,
		0x08,
		0xdc,
		0x00,
		0xab,
		0xcd,
};
	

int Fixed_len;						 // 固定报头长度
int Variable_len;					 // 可变报头长度
int Payload_len;					 // 有效负荷长度
uint8_t ping_open = 0;
uint8_t count = 0;



#define PRODUCTID "zjt"                              // 产品ID
#define PRODUCTID_LEN strlen(PRODUCTID)                     // 产品ID长度
#define DEVICENAME "zjt"                                   // 设备名
#define DEVICENAME_LEN strlen(DEVICENAME)                   // 设备名长度
#define DEVICESECRE "zjt"              					// 设备秘钥
#define DEVICESECRE_LEN strlen(DEVICESECRE)                 // 设备秘钥长度








char Connect_flag = 0;		// 同服务器连接状态  0：还没有连接服务器  1：连接上服务器了
char SubcribePack_flag = 0; // 订阅报文状态      1：订阅报文成功
char my_dhcp_retry = 0;					 // DHCP当前共计重试的次数





unsigned char temp_buff[RBUFF_UNIT]; // 临时缓冲区，构建报文用
unsigned char gDATABUF[DATA_BUF_SIZE];	 // 数据缓冲区


unsigned char MQTT_RxDataBuf[R_NUM][RBUFF_UNIT]; // 数据的接收缓冲区,所有服务器发来的数据，存放在该缓冲区,缓冲区第一个字节存放数据长度
unsigned char *MQTT_RxDataInPtr;				 // 指向接收缓冲区存放数据的位置
unsigned char *MQTT_RxDataOutPtr;				 // 指向接收缓冲区读取数据的位置
unsigned char *MQTT_RxDataEndPtr;				 // 指向接收缓冲区结束的位置


unsigned char MQTT_CMDBuf[C_NUM][CBUFF_UNIT]; // 命令数据的接收缓冲区
unsigned char *MQTT_CMDInPtr;				  // 指向命令缓冲区存放数据的位置
unsigned char *MQTT_CMDOutPtr;				  // 指向命令缓冲区读取数据的位置
unsigned char *MQTT_CMDEndPtr;				  // 指向命令缓冲区结束的位置


void MQTT_Buff_Init(void)
{
	MQTT_RxDataInPtr = MQTT_RxDataBuf[0];		   // 指向发送缓冲区存放数据的指针归位
	MQTT_RxDataOutPtr = MQTT_RxDataInPtr;		   // 指向发送缓冲区读取数据的指针归位
	MQTT_RxDataEndPtr = MQTT_RxDataBuf[R_NUM - 1]; // 指向发送缓冲区结束的指针归位

	MQTT_CMDInPtr = MQTT_CMDBuf[0];			 // 指向命令缓冲区存放数据的指针归位
	MQTT_CMDOutPtr = MQTT_CMDInPtr;			 // 指向命令缓冲区读取数据的指针归位
	MQTT_CMDEndPtr = MQTT_CMDBuf[C_NUM - 1]; // 指向命令缓冲区结束的指针归位

	
}

void network_init(void)
{
	char tmpstr[6] = {0};
	wiz_NetInfo netinfo;

	ctlnetwork(CN_SET_NETINFO, (void *)&gWIZNETINFO); // 设置网络参数
	ctlnetwork(CN_GET_NETINFO, (void *)&netinfo);	  // 读取网络参数
	ctlwizchip(CW_GET_ID, (void *)tmpstr);			  // 读取芯片ID

	// 打印网络参数
	if (netinfo.dhcp == NETINFO_DHCP)
		printf("\r\n=== %s NET CONF : DHCP ===\r\n", (char *)tmpstr);
	else
		printf("\r\n=== %s NET CONF : Static ===\r\n", (char *)tmpstr);
	printf("===========================\r\n");
	printf("MAC地址: %02X:%02X:%02X:%02X:%02X:%02X\r\n", netinfo.mac[0], netinfo.mac[1], netinfo.mac[2], netinfo.mac[3], netinfo.mac[4], netinfo.mac[5]);
	printf("IP地址: %d.%d.%d.%d\r\n", netinfo.ip[0], netinfo.ip[1], netinfo.ip[2], netinfo.ip[3]);
	printf("网关地址: %d.%d.%d.%d\r\n", netinfo.gw[0], netinfo.gw[1], netinfo.gw[2], netinfo.gw[3]);
	printf("子网掩码: %d.%d.%d.%d\r\n", netinfo.sn[0], netinfo.sn[1], netinfo.sn[2], netinfo.sn[3]);
	printf("DNS服务器: %d.%d.%d.%d\r\n", netinfo.dns[0], netinfo.dns[1], netinfo.dns[2], netinfo.dns[3]);
	printf("===========================\r\n");
}

void my_ip_assign(void)
{
	getIPfromDHCP(gWIZNETINFO.ip);	 // 把获取到的ip参数，记录到机构体中
	getGWfromDHCP(gWIZNETINFO.gw);	 // 把获取到的网关参数，记录到机构体中
	getSNfromDHCP(gWIZNETINFO.sn);	 // 把获取到的子网掩码参数，记录到机构体中
	getDNSfromDHCP(gWIZNETINFO.dns); // 把获取到的DNS服务器参数，记录到机构体中
	gWIZNETINFO.dhcp = NETINFO_DHCP; // 标记使用的是DHCP方式
	network_init();					 // 初始化网络
	printf("DHCP租期 : %d 秒\r\n", getDHCPLeasetime());
}

void my_ip_conflict(void)
{
	printf("获取IP失败，准备重启\r\n"); // 提示获取IP失败
	NVIC_SystemReset();					   // 重启
}



void MQTT_PingREQ(void)
{
	temp_buff[0] = 0xC0; // 第1个字节 ：固定0xC0
	temp_buff[1] = 0x00; // 第2个字节 ：固定0x00

	send(SOCK_TCPC,temp_buff, 2); // 加入数据到缓冲区
}


void AliIoT_Parameter_Init(void)
{
	char temp[128]; // 计算加密的时候，临时使用的缓冲区

	memset(ClientID, 0, 128);						  // 客户端ID的缓冲区全部清零
	sprintf(ClientID, "%s&%s", PRODUCTID, DEVICENAME); // 构建客户端ID，并存入缓冲区
	ClientID_len = strlen(ClientID);				  // 计算客户端ID的长度

	memset(Username, 0, 128);													// 用户名的缓冲区全部清零
	sprintf(Username, "%s&%s", PRODUCTID, DEVICENAME); // 构建用户名，并存入缓冲区
	Username_len = strlen(Username);											// 计算用户名的长度

	memset(temp, 0, 128);													   // 清除临时缓冲区
	base64_decode(DEVICESECRE, (unsigned char *)temp);						   // 对DEVICESECRE进行base64解码
	utils_hmac_sha1(Username, strlen(Username), Passward, temp, strlen(temp)); // 以上一步的结果为秘钥对Username中的明文，进行hmacsha1加密，结果就是密码，并保存到缓冲区中
	strcat(Passward, ";hmacsha1");											   // 末尾追加字符串
	Passward_len = strlen(Passward);										   // 计算用户名的长度

	memset(ServerIP, 0, 4);
	ServerIP[0] = 47;
	ServerIP[1] = 108;
	ServerIP[2] = 144;
	ServerIP[3] = 41;
	ServerPort = 1883;											

	printf("服 务 器：%d.%d.%d.%d:%d\r\n",ServerIP[0],ServerIP[1],ServerIP[1],ServerIP[1], ServerPort); // 串口输出调试信息
	printf("客户端ID：%s\r\n", ClientID);				  // 串口输出调试信息
	printf("用 户 名：%s\r\n", Username);				  // 串口输出调试信息
	printf("密    码：%s\r\n", Passward);				  // 串口输出调试信息
}


void W5500_init(void)
{
	// W5500收发内存分区，收发缓冲区各自总的空间是16K，（0-7）每个端口的收发缓冲区我们分配 2K
	char memsize[2][8] = {{2, 2, 2, 2, 2, 2, 2, 2}, {2, 2, 2, 2, 2, 2, 2, 2}};
	char tmp;

	reg_wizchip_cris_cbfunc(SPI_CrisEnter, SPI_CrisExit);  // 注册临界区函数
	reg_wizchip_cs_cbfunc(SPI_CS_Select, SPI_CS_Deselect); // 注册SPI片选信号函数
	reg_wizchip_spi_cbfunc(SPI_ReadByte, SPI_WriteByte);   // 注册读写函数
	if (ctlwizchip(CW_INIT_WIZCHIP, (void *)memsize) == -1)
	{												  // 如果if成立，表示收发内存分区失败
		printf("初始化收发分区失败,准备重启\r\n"); // 提示信息
		NVIC_SystemReset();							  // 重启
	}
	do
	{ // 检查连接状态
		if (ctlwizchip(CW_GET_PHYLINK, (void *)&tmp) == -1)
		{										 // 如果if成立，表示未知错误
			printf("未知错误，准备重启\r\n"); // 提示信息
			NVIC_SystemReset();					 // 重启
		}
		if (tmp == PHY_LINK_OFF)
		{
			printf("网线未连接\r\n"); // 如果检测到，网线没连接，提示连接网线
			bsp_DelayMS(2000);				 // 延时2s
		}
	} while (tmp == PHY_LINK_OFF); // 循环执行，直到连接上网线

	setSHAR(gWIZNETINFO.mac);									 // 设置MAC地址
	DHCP_init(SOCK_DHCP, gDATABUF);								 // 初始化DHCP
	reg_dhcp_cbfunc(my_ip_assign, my_ip_assign, my_ip_conflict); // 注册DHCP回调函数
}

uint8_t local_ip[4]  ={192,168,2,88};											/*定义W5500默认IP地址*/
uint16_t local_port=5000;	                       					/*定义本地端口*/

uint8_t  remote_ip[4]={47,108,144,41};											/*远端IP地址——服务器的IP*/
uint16_t remote_port=1883;	


uint8_t do_tcp_client(void)
{	
//   uint16_t len=0,i=0;	
  uint16_t ret=0;
  ret=getSn_SR(SOCK_TCPC);//读取专用于TCP的Socket的状态值
  printf(" getSn_SR = 0x%X\r\n",ret);
//  printf(" socket状态 = %s\r\n",Sn_SR_values[ret]);
  switch(ret)								  				         /*判断socket的状态*/
  {
  case SOCK_CLOSED:											        		         /*如果socket处于关闭状态=0x00*/
    printf(" socket的状态 = SOCK_CLOSED\r\n");
    socket(SOCK_TCPC,Sn_MR_TCP,local_port,Sn_MR_ND);       //先将该 专用于TCP的Socket 初始化
    break;
    
  case SOCK_INIT:												/*如果socket处于初始化状态=0x13*/
    printf(" socket的状态 = SOCK_INIT\r\n");
    getSIPR (local_ip);			
    printf(" W5500 IP地址: %d.%d.%d.%d\r\n", local_ip[0], local_ip[1], local_ip[2], local_ip[3]);
    printf(" 服务器IP地址: %d.%d.%d.%d：%d\r\n",remote_ip[0],remote_ip[1],remote_ip[2],remote_ip[3],remote_port); 
    connect(SOCK_TCPC,remote_ip,remote_port);                /*socket连接服务器*/ 
    break;
    
  case SOCK_ESTABLISHED: 								/*如果socket处于连接建立状态=0x17， 完成3次握手完成后，才会进入建立连接状态*/
    printf(" socket的状态 = SOCK_ESTABLISHED\r\n"); 
    return 1;
    
  case SOCK_CLOSE_WAIT: 								 /*如果socket处于等待关闭状态=SOCK_CLOSE_WAIT*/
    printf(" socket的状态 = SOCK_CLOSE_WAIT\r\n");
    close(SOCK_TCPC); 							   	/**@Q：socket处于等待关闭状态而不是关闭状态*/
    break;
    
  }
  return 0;
}


void MQTT_ConectPack(void)
{
	int temp, Remaining_len;

	Fixed_len = 1;														  // 连接报文中，固定报头长度暂时先=1
	Variable_len = 10;													  // 连接报文中，可变报头长度=10
	Payload_len = 2 + ClientID_len + 2 + Username_len + 2 + Passward_len; // 连接报文中，负载长度
	Remaining_len = Variable_len + Payload_len;							  // 剩余长度=可变报头长度+负载长度

	temp_buff[0] = 0x10; // 固定报头第1个字节 ：固定0x01
	do
	{										 // 循环处理固定报头中的剩余长度字节，字节量根据剩余字节的真实长度变化
		temp = Remaining_len % 128;			 // 剩余长度取余128
		Remaining_len = Remaining_len / 128; // 剩余长度取整128
		if (Remaining_len > 0)
			temp |= 0x80;			 // 按协议要求位7置位
		temp_buff[Fixed_len] = temp; // 剩余长度字节记录一个数据
		Fixed_len++;				 // 固定报头总长度+1
	} while (Remaining_len > 0);	 // 如果Remaining_len>0的话，再次进入循环

	temp_buff[Fixed_len + 0] = 0x00; // 可变报头第1个字节 ：固定0x00
	temp_buff[Fixed_len + 1] = 0x04; // 可变报头第2个字节 ：固定0x04
	temp_buff[Fixed_len + 2] = 0x4D; // 可变报头第3个字节 ：固定0x4D
	temp_buff[Fixed_len + 3] = 0x51; // 可变报头第4个字节 ：固定0x51
	temp_buff[Fixed_len + 4] = 0x54; // 可变报头第5个字节 ：固定0x54
	temp_buff[Fixed_len + 5] = 0x54; // 可变报头第6个字节 ：固定0x54
	temp_buff[Fixed_len + 6] = 0x04; // 可变报头第7个字节 ：固定0x04
	temp_buff[Fixed_len + 7] = 0xC2; // 可变报头第8个字节 ：使能用户名和密码校验，不使用遗嘱，不保留会话
	temp_buff[Fixed_len + 8] = 0x00; // 可变报头第9个字节 ：保活时间高字节 0x00
	temp_buff[Fixed_len + 9] = 0x64; // 可变报头第10个字节：保活时间高字节 0x64   100s

	/*     CLIENT_ID      */
	temp_buff[Fixed_len + 10] = ClientID_len / 256;				// 客户端ID长度高字节
	temp_buff[Fixed_len + 11] = ClientID_len % 256;				// 客户端ID长度低字节
	memcpy(&temp_buff[Fixed_len + 12], ClientID, ClientID_len); // 复制过来客户端ID字串
	/*     用户名        */
	temp_buff[Fixed_len + 12 + ClientID_len] = Username_len / 256;			   // 用户名长度高字节
	temp_buff[Fixed_len + 13 + ClientID_len] = Username_len % 256;			   // 用户名长度低字节
	memcpy(&temp_buff[Fixed_len + 14 + ClientID_len], Username, Username_len); // 复制过来用户名字串
	/*      密码        */
	temp_buff[Fixed_len + 14 + ClientID_len + Username_len] = Passward_len / 256;			  // 密码长度高字节
	temp_buff[Fixed_len + 15 + ClientID_len + Username_len] = Passward_len % 256;			  // 密码长度低字节
	memcpy(&temp_buff[Fixed_len + 16 + ClientID_len + Username_len], Passward, Passward_len); // 复制过来密码字串

	send(SOCK_TCPC,temp_buff, Fixed_len + Variable_len + Payload_len); // 加入发送数据缓冲区
}

uint8_t readCONNACK(void)
{
	uint8_t buff[0xff],i=0;	
	uint8_t ret=0xff;
	uint16_t len=0;

	len=getSn_RX_RSR(SOCK_TCPC); 		/*定义len为已接收数据的长度*/  

	if(len>0)
	{
	memset(buff,0,sizeof(buff));				// 串口接收缓存清0
	recv(SOCK_TCPC,buff,len); 				/*接收来自Server的数据*/
	//buff[len]=0x00;  					/*添加字符串结束符*/
	for(i=0;i<len;i++)
	  printf("%02X-",buff[i]);
	printf("\r\n");
	ret=buff[3];//返回确认码
	}	
	return ret;
}

void MQTT_Subscribe(char *topic_name, int QoS)
{
	Fixed_len = 2;							  // SUBSCRIBE报文中，固定报头长度=2
	Variable_len = 2;						  // SUBSCRIBE报文中，可变报头长度=2
	Payload_len = 2 + strlen(topic_name) + 1; // 计算有效负荷长度 = 2字节(topic_name长度)+ topic_name字符串的长度 + 1字节服务等级

	temp_buff[0] = 0x82;								   // 第1个字节 ：固定0x82
	temp_buff[1] = Variable_len + Payload_len;			   // 第2个字节 ：可变报头+有效负荷的长度
	temp_buff[2] = 0x00;								   // 第3个字节 ：报文标识符高字节，固定使用0x00
	temp_buff[3] = 0x01;								   // 第4个字节 ：报文标识符低字节，固定使用0x01
	temp_buff[4] = strlen(topic_name) / 256;			   // 第5个字节 ：topic_name长度高字节
	temp_buff[5] = strlen(topic_name) % 256;			   // 第6个字节 ：topic_name长度低字节
	memcpy(&temp_buff[6], topic_name, strlen(topic_name)); // 第7个字节开始 ：复制过来topic_name字串
	temp_buff[6 + strlen(topic_name)] = QoS;			   // 最后1个字节：订阅等级

	send(SOCK_TCPC,temp_buff, Fixed_len + Variable_len + Payload_len); // 加入发送数据缓冲区
}

void CMDBuf_Deal(unsigned char *data, int size)
{
	memcpy(&MQTT_CMDInPtr[2], data, size); // 拷贝数据到命令缓冲区
	MQTT_CMDInPtr[0] = size / 256;		   // 记录数据长度
	MQTT_CMDInPtr[1] = size % 256;		   // 记录数据长度
	MQTT_CMDInPtr[size + 2] = '\0';		   // 加入字符串结束符
	MQTT_CMDInPtr += CBUFF_UNIT;		   // 指针下移
	if (MQTT_CMDInPtr == MQTT_CMDEndPtr)   // 如果指针到缓冲区尾部了
		MQTT_CMDInPtr = MQTT_CMDBuf[0];	   // 指针归位到缓冲区开头
}

void MQTT_DealPushdata_Qs0(unsigned char *redata)
{
	int re_len;			// 定义一个变量，存放接收的数据总长度
	int pack_num;		// 定义一个变量，当多个推送一起过来时，保存推送的个数
	int temp, temp_len; // 定义一个变量，暂存数据
	int totle_len;		// 定义一个变量，存放已经统计的推送的总数据量
	int topic_len;		// 定义一个变量，存放推送中主题的长度
	int cmd_len;		// 定义一个变量，存放推送中包含的命令数据的长度
	int cmd_loca;		// 定义一个变量，存放推送中包含的命令的起始位置
	int i;				// 定义一个变量，用于for循环
	int local, multiplier;
	unsigned char tempbuff[RBUFF_UNIT]; // 临时缓冲区
	unsigned char *data;				// redata过来的时候，第一个字节是数据总量，data用于指向redata的第2个字节，真正的数据开始的地方

	re_len = redata[0] * 256 + redata[1];		// 获取接收的数据总长度
	data = &redata[2];							// data指向redata的第2个字节，真正的数据开始的
	pack_num = temp_len = totle_len = temp = 0; // 各个变量清零
	local = 1;
	multiplier = 1;
	do
	{
		pack_num++; // 开始循环统计推送的个数，每次循环推送的个数+1
		do
		{
			temp = data[totle_len + local];
			temp_len += (temp & 127) * multiplier;
			multiplier *= 128;
			local++;
		} while ((temp & 128) != 0);
		totle_len += (temp_len + local); // 累计统计的总的推送的数据长度
		re_len -= (temp_len + local);	 // 接收的数据总长度 减去 本次统计的推送的总长度
		local = 1;
		multiplier = 1;
		temp_len = 0;
	} while (re_len != 0);							   // 如果接收的数据总长度等于0了，说明统计完毕了
	printf("本次接收了%d个推送数据\r\n", pack_num); // 串口输出信息
	temp_len = totle_len = 0;						   // 各个变量清零
	local = 1;
	multiplier = 1;
	for (i = 0; i < pack_num; i++)
	{ // 已经统计到了接收的推送个数，开始for循环，取出每个推送的数据
		do
		{
			temp = data[totle_len + local];
			temp_len += (temp & 127) * multiplier;
			multiplier *= 128;
			local++;
		} while ((temp & 128) != 0);
		topic_len = data[local + totle_len] * 256 + data[local + 1 + totle_len] + 2; // 计算本次推送数据中主题占用的数据量
		cmd_len = temp_len - topic_len;												 // 计算本次推送数据中命令数据占用的数据量
		cmd_loca = totle_len + local + topic_len;									 // 计算本次推送数据中命令数据开始的位置
		memcpy(tempbuff, &data[cmd_loca], cmd_len);									 // 命令数据拷贝出来
		CMDBuf_Deal(tempbuff, cmd_len);												 // 加入命令到缓冲区
		totle_len += (temp_len + local);											 // 累计已经统计的推送的数据长度
		local = 1;
		multiplier = 1;
		temp_len = 0;
	}
}
void MQTT_PublishQs0(char *topic, char *data, int data_len)
{
	int temp, Remaining_len;

	Fixed_len = 1;								// 固定报头长度暂时先等于：1字节
	Variable_len = 2 + strlen(topic);			// 可变报头长度：2字节(topic长度)+ topic字符串的长度
	Payload_len = data_len;						// 有效负荷长度：就是data_len
	Remaining_len = Variable_len + Payload_len; // 剩余长度=可变报头长度+负载长度

	temp_buff[0] = 0x30; // 固定报头第1个字节 ：固定0x30
	do
	{										 // 循环处理固定报头中的剩余长度字节，字节量根据剩余字节的真实长度变化
		temp = Remaining_len % 128;			 // 剩余长度取余128
		Remaining_len = Remaining_len / 128; // 剩余长度取整128
		if (Remaining_len > 0)
			temp |= 0x80;			 // 按协议要求位7置位
		temp_buff[Fixed_len] = temp; // 剩余长度字节记录一个数据
		Fixed_len++;				 // 固定报头总长度+1
	} while (Remaining_len > 0);	 // 如果Remaining_len>0的话，再次进入循环

	temp_buff[Fixed_len + 0] = strlen(topic) / 256;					   // 可变报头第1个字节     ：topic长度高字节
	temp_buff[Fixed_len + 1] = strlen(topic) % 256;					   // 可变报头第2个字节     ：topic长度低字节
	memcpy(&temp_buff[Fixed_len + 2], topic, strlen(topic));		   // 可变报头第3个字节开始 ：拷贝topic字符串
	memcpy(&temp_buff[Fixed_len + 2 + strlen(topic)], data, data_len); // 有效负荷：拷贝data数据

	send(SOCK_TCPC,temp_buff, Fixed_len + Variable_len + Payload_len); // 加入发送数据缓冲区
}

void LED_PubState(unsigned char sta)
{
	char temp[RBUFF_UNIT];
	sprintf(temp, "{\"method\":\"report\",\"clientToke\":\"123\",\"params\":{\"power_switch\":%d}}", sta);
	MQTT_PublishQs0(P_TOPIC_NAME, temp, strlen(temp));
}



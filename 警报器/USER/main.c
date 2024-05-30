#include <stdio.h>
#include <stdlib.h>
#include "stm32f10x.h" 
#include "main.h"	   
#include "delay.h"	   
#include "usart1.h"	   
#include "usart3.h"	   
#include "timer2.h"	   
#include "timer3.h"	   
#include "timer4.h"	   
#include "cat1.h"	   
#include "led.h"	   
#include "mqtt.h"	   
#include "iwdg.h"	   
#include "bsp_timer.h"
#include "spi.h"
#include "wizchip_conf.h"
#include "dhcp.h"

/*
各参数修改在mqtt.h中修改，本例子在本地虚拟机部署emqx运行
开发板客户端登录mqtt服务器的用户名规则是DEVICENAME&PRODUCTKEY，本例子中为zjt&zjt
密码规则是clientId%sdeviceName%sproductKey%s，其中%s分别对应DEVICENAME,DEVICENAME,PRODUCTKEY，在本例子中为clientIdzjtdeviceNamezjtproductKeyzjt
加密方式为hmacsha1加密，加密的密钥是DEVICESECRE  本例子中为zjt
本例子中加密后为 70e12b4e92ad65d58e42408c328a807987620b6b
*/
void W5500_init(void);
void network_init(void);
void my_ip_conflict(void);
void my_ip_assign(void);
						 
wiz_NetInfo gWIZNETINFO;				 // MAC地址自己写一个，不要和路由器下其他的设备一样即可
#define SOCK_DHCP		    0      //DHCP功能用的端口，W5500有8个，0-7都行
#define SOCK_TCPS	        1      //TCP连接用的端口， W5500有8个，0-7都行
#define SOCKET_DNS          2      //DNS功能用的端口， W5500有8个，0-7都行
#define MY_MAX_DHCP_RETRY	3      //DHCP重试次数
#define DATA_BUF_SIZE       2048   //缓冲区大小
unsigned char gDATABUF[DATA_BUF_SIZE];	 // 数据缓冲区
char DNS_flag = 0;      // DNS解析标志  0:未成功  1：成功
int tcp_state;							 // 用于保存TCP返回的状态
int temp_state;							 // 用于临时保存TCP返回的状态
char my_dhcp_retry = 0;					 // DHCP当前共计重试的次数



void LED_PubState(unsigned char sta);

void MQTT_CheckConnectStatus(unsigned char status);
void MQTT_CheckSubcribeStatus(unsigned char *status);
void MAIN_CHECK_led(char *strCmd);
//
#define FW_VERSION "20220817L00"
unsigned char g_statusChanged = 1; // 首次数据需要更新
char g_connectPackFlag = 0;  //外部变量声明，CONNECT报文状态   1：CONNECT报文成功
char g_subcribePackFlag = 0;   //订阅报文状态      1：订阅报文成功
char g_connectFlag = 0;        //同服务器连接状态  0：还没有连接服务器  1：连接上服务器了
unsigned int g_timeJiffies = 0;

unsigned char resetCnt_x30s = 0;
unsigned int longTimeReset_x30s = 0;
char sub_ok = 0;




int main(void)
{
    //4G
    if(0)
    {
        bsp_InitTimer();
        Usart1_Init(115200); //串口1功能初始化，波特率9600
        TIM2_ENABLE_500MS();//喂狗和点动
        IWDG_Init(6, 1600); //只要在4*1280ms=5120内进行喂狗就不会复位系统
        Usart3_Init(115200); //串口3功能初始化，波特率115200
        LED_LocationStaInit(); //初始化
        // 数据处理
        u1_printf("设备版本号: %s\r\n", FW_VERSION); //打印版本号
        u1_printf("开始处理数据..\r\n"); //提示信息
        // 设置定位灯状态
        LED_LocationStaOff();
        LED_LocationStaOn();

        IoT_Parameter_Init(); //初始化连接云IoT平台MQTT服务器的参数
        CAT1_GIPO_Init();	  //初始化4G模块的控制IO
        bsp_StartAutoTimer(0,1000);//开启软件定时器，1s上报
        
        while (1)
        {
            if (g_connectFlag == 1)//4G连接成功的标志
            {
                // u1_printf("g_connectFlag == 1\r\n");//冲日志
            }
            if ((g_subcribePackFlag == S_TOPIC_NUM) && (g_connectFlag == 1))//订阅成功的标志
            {
                // u1_printf("(g_subcribePackFlag == S_TOPIC_NUM) && (g_connectFlag == 1)\r\n"); //冲日志
            }
            if (resetCnt_x30s >= 6) 
            {
                u1_printf("超过%d分钟未连接到服务器，重连\r\n", resetCnt_x30s /2);
                g_connectFlag = 0;
            }
            if (longTimeReset_x30s >= 720)             
            {
                u1_printf("连续%d小时正常连接服务器但无数据通信，重连\r\n", longTimeReset_x30s /2 /60);
                g_connectFlag = 0;
            }
            //定时1s发布属性
            if(bsp_CheckTimer(0))
            {
                if (g_subcribePackFlag == 1)
                {
                    if(sub_ok == 0)
                    {
                        sub_ok = 1;
                        LED_PubState(0);
                    }
                    else
                    {   
                        sub_ok = 0;
                        LED_PubState(1);
                    }        
                }
            }
            if (g_connectFlag == 1) /* 连接上服务器 */
            {
                if (MQTT_TxDataOutPtr != MQTT_TxDataInPtr) /*处理发送缓冲区数据*/
                { // if成立的话，说明发送缓冲区有数据
                    //第1种：0x10 连接报文
                    //第2种：0x82 订阅报文，且g_connectPackFlag置位，表示连接报文成功
                    //第3种：g_subcribePackFlag置位，说明连接和订阅均成功，其他报文可发
                    if ((DMA_flag == 0) && ((MQTT_TxDataOutPtr[2] == 0x10) ||
                        ((MQTT_TxDataOutPtr[2] == 0x82) && (g_connectPackFlag == 1)) ||
                        (g_subcribePackFlag == S_TOPIC_NUM)))
                    {
                        // u1_printf("发送数据:0x%x\r\n", MQTT_TxDataOutPtr[2]); //串口提示信息
                        MQTT_TxData(MQTT_TxDataOutPtr);						  //发送数据
                        MQTT_TxDataOutPtr += TBUFF_UNIT;					  //指针下移
                        if (MQTT_TxDataOutPtr == MQTT_TxDataEndPtr)			  //如果指针到缓冲区尾部了
                            MQTT_TxDataOutPtr = MQTT_TxDataBuf[0];			  //指针归位到缓冲区开头
                    }
                }

                if (MQTT_RxDataOutPtr != MQTT_RxDataInPtr) /* 处理接收缓冲区数据 */
                {
                    // u1_printf("接收到数据:");

                    /*处理CONNACK报文*/
                    //如果第一个字节是0x20，表示收到的是CONNACK报文
                    //接着我们要判断第4个字节，看看CONNECT报文是否成功
                    if (MQTT_RxDataOutPtr[2] == 0x20) {
                        MQTT_CheckConnectStatus(MQTT_RxDataOutPtr[5]);
                    }

                    /* 处理SUBACK报文 */
                    //第一个字节是0x90，表示收到的是SUBACK报文
                    //接着我们要判断订阅回复，看看是不是成功
                    else if (MQTT_RxDataOutPtr[2] == 0x90)
                    {
                        MQTT_CheckSubcribeStatus(&MQTT_RxDataOutPtr[6]);
                        u1_printf("\r\n");
                    }
                    /* 处理PINGRESP报文
                    // 第一个字节是0xD0，表示收到的是PINGRESP报文 */
                    else if (MQTT_RxDataOutPtr[2] == 0xD0)
                    {
                        resetCnt_x30s = 0;
                        u1_printf("PING-%d\r\n", longTimeReset_x30s);
                    }

                    /* 处理数据推送报文 */
                    // 如果第一个字节是0x30，表示收到的是服务器发来的推送数据
                    //我们要提取控制命令
                    else if ((MQTT_RxDataOutPtr[2] == 0x30))
                    {
                        u1_printf("服务器等级0推送\r\n");
                        MQTT_DealPushdata_Qs0(MQTT_RxDataOutPtr); //处理等级0推送数据
                    }

                    MQTT_RxDataOutPtr += RBUFF_UNIT;			//接收指针下移
                    if (MQTT_RxDataOutPtr == MQTT_RxDataEndPtr) //如果接收指针到接收缓冲区尾部了
                        MQTT_RxDataOutPtr = MQTT_RxDataBuf[0];	//接收指针归位到接收缓冲区开头
                }

                if (MQTT_CMDOutPtr != MQTT_CMDInPtr) /* 处理命令缓冲区数据 */
                {
                    longTimeReset_x30s = 0;
                    u1_printf("命令:%s\r\n", &MQTT_CMDOutPtr[2]);
                    MAIN_CHECK_led((char *)MQTT_CMDOutPtr + 2);
                    MQTT_CMDOutPtr += CBUFF_UNIT;		  //指针下移
                    if (MQTT_CMDOutPtr == MQTT_CMDEndPtr) //如果指针到缓冲区尾部了
                        MQTT_CMDOutPtr = MQTT_CMDBuf[0];  //指针归位到缓冲区开头
                }										  //处理命令缓冲区数据的else if分支结尾
            }
            else
            {
                bsp_DelayMS(500);
                u1_printf("连接服务器\r\n");
                bsp_DelayMS(500);
                CAT1_RxCounter = 0;//接收数据量变量清零
                g_connectPackFlag = 0;
                g_subcribePackFlag = 0;
                longTimeReset_x30s = 0;
                resetCnt_x30s = 0;

                memset(CAT1_RX_BUF, 0, CAT1_RXBUFF_SIZE); //清空接收缓冲区
                
                if (CAT1_Connect_IoTServer(30) == 0)
                {
                    u1_printf("连接服务器成功\r\n");		 
                    Usart3_IDELInit(115200);				  //串口3 开启DMA 和 空闲中断
                    g_connectFlag = 1;						  //置1，表示连接成功
                    CAT1_RxCounter = 0;						  //接收数据量变量清零
                    memset(CAT1_RX_BUF, 0, CAT1_RXBUFF_SIZE); //清空接收缓冲区
                    MQTT_Buff_Init();						  //初始化发送接收命令缓冲区
                }
            }
        }
    }
    else
    {
        Usart1_Init(115200); 
        bsp_InitTimer();
        bsp_StartAutoTimer(0,1000);
        IoT_Parameter_Init(); 
        W5500_init();
        
        while(1)
        {
            if(bsp_CheckTimer(0))
            {
              u1_printf("ok123\r\n");	
            }                       
        }
    }
  
}

/*
*********************************************************
                                                        *
                         4G                             *
                                                        *
*********************************************************
*/
void MQTT_CheckSubcribeStatus(unsigned char *status)
{
	unsigned char i=0;
	for (i = 0; i < S_TOPIC_NUM; i++)
	{ //循环查询订阅结果
		switch (*status)
		{
		case 0x00:
		case 0x01:
			u1_printf("订阅成功%d ", i + 1);
			g_subcribePackFlag++;

			break;
		default:
			u1_printf("订阅失败，准备重启\r\n");
			g_connectFlag = 0; //置零，重启连接
			break;
		}
		status++;
	}
	return;
}

void MQTT_CheckConnectStatus(unsigned char status)
{
	switch (status)
	{
	case 0x00:
		u1_printf("CONNECT报文成功\r\n");
		g_connectPackFlag = 1;			  // CONNECT报文成功
		break;
	case 0x01:
		u1_printf("连接已拒绝，不支持的协议版本，准备重启\r\n");
		g_connectFlag = 0; // 置零，重启连接
		break;
	case 0x02:
		u1_printf("连接已拒绝，不合格的客户端标识符，准备重启\r\n");
		g_connectFlag = 0; // 置零，重启连接
		break;
	case 0x03:
		u1_printf("连接已拒绝，服务端不可用，准备重启\r\n");
		g_connectFlag = 0; // 置零，重启连接
		break;
	case 0x04:
		u1_printf("连接已拒绝，无效的用户名或密码，准备重启\r\n");
		g_connectFlag = 0; // 置零，重启连接
		break;
	case 0x05:
		u1_printf("连接已拒绝，未授权，准备重启\r\n");
		g_connectFlag = 0; // 置零，重启连接
		break;
	default:
		u1_printf("连接已拒绝，未知状态，准备重启\r\n");
		g_connectFlag = 0; // 置零，重启连接
		break;
	}
	return;
}

void MAIN_CHECK_led(char *strCmd)
{
	unsigned int pwrState = 0xff;
	char *strCmdTmp = NULL;

	strCmdTmp = strstr(strCmd, "\"params\":{\"powerstate");
	if (strCmdTmp == NULL) {
//		u1_printf("解析定位灯模式NULL\r\n");
		return;
	}
	sscanf(strCmdTmp,"\"params\":{\"powerstate\":%d", &pwrState);//拆分数据
	u1_printf("解析定位灯模式:%d \r\n", pwrState);

	if (pwrState != 0 && pwrState != 1) {
		u1_printf("解析定位灯模式:%d错误\r\n", pwrState);
		return;
	}
	if (pwrState == 0) {
		LED_LocationStaOff();
		u1_printf("关闭灯\r\n");
		LED_PubState(0);
	}
	if (pwrState == 1) {
		LED_LocationStaOn();
		u1_printf("打开灯\r\n");
		LED_PubState(1);
	}
	return;
}

void LED_PubState(unsigned char sta)
{
	char temp[TBUFF_UNIT];
	sprintf(temp, "\"LEDSwitch\":%d}", sta);
	MQTT_PublishQs0(P_TOPIC_NAME, temp, strlen(temp));
}

/*
*********************************************************
                                                        *
                         网口                             *
                                                        *
*********************************************************
*/
void W5500_init(void)
{
	// W5500收发内存分区，收发缓冲区各自总的空间是16K，（0-7）每个端口的收发缓冲区我们分配 2K
	char memsize[2][8] = {{2, 2, 2, 2, 2, 2, 2, 2}, {2, 2, 2, 2, 2, 2, 2, 2}};
	char tmp;
    SPI_Configuration();								   // 初始化SPI接口

    reg_wizchip_cris_cbfunc(SPI_CrisEnter, SPI_CrisExit);  // 注册临界区函数
	reg_wizchip_cs_cbfunc(SPI_CS_Select, SPI_CS_Deselect); // 注册SPI片选信号函数
	reg_wizchip_spi_cbfunc(SPI_ReadByte, SPI_WriteByte);   // 注册读写函数
    if (ctlwizchip(CW_INIT_WIZCHIP, (void *)memsize) == -1)
	{												  // 如果if成立，表示收发内存分区失败
		u1_printf("初始化收发分区失败,准备重启\r\n"); // 提示信息
		NVIC_SystemReset();							  // 重启
	}	
    do
	{ // 检查连接状态
		if (ctlwizchip(CW_GET_PHYLINK, (void *)&tmp) == -1)
		{										 // 如果if成立，表示未知错误
			u1_printf("未知错误，准备重启\r\n"); // 提示信息
			NVIC_SystemReset();					 // 重启
		}
		if (tmp == PHY_LINK_OFF)
		{
			u1_printf("网线未连接\r\n"); // 如果检测到，网线没连接，提示连接网线
			bsp_DelayMS(2000);				 // 延时2s
		}
	} while (tmp == PHY_LINK_OFF); // 循环执行，直到连接上网线

    CreateSysMAC(gWIZNETINFO.mac);
    setSHAR(gWIZNETINFO.mac);    // 设置MAC地址
	DHCP_init(SOCK_DHCP, gDATABUF);								 // 初始化DHCP
    reg_dhcp_cbfunc(my_ip_assign, my_ip_assign, my_ip_conflict); // 注册DHCP回调函数
    DNS_flag = 0;												 // DNS解析标志=0
    my_dhcp_retry = 0;											 // DHCP重试次数=0
    tcp_state = 0;												 // TCP状态=0
    temp_state = -1;											 // 上一次TCP状态=-1
							
}
void my_ip_assign(void)
{
	getIPfromDHCP(gWIZNETINFO.ip);	 // 把获取到的ip参数，记录到机构体中
	getGWfromDHCP(gWIZNETINFO.gw);	 // 把获取到的网关参数，记录到机构体中
	getSNfromDHCP(gWIZNETINFO.sn);	 // 把获取到的子网掩码参数，记录到机构体中
	getDNSfromDHCP(gWIZNETINFO.dns); // 把获取到的DNS服务器参数，记录到机构体中
	gWIZNETINFO.dhcp = NETINFO_DHCP; // 标记使用的是DHCP方式
	network_init();					 // 初始化网络
	u1_printf("DHCP租期 : %d 秒\r\n", getDHCPLeasetime());
}
/*-------------------------------------------------*/
/*函数名：获取IP的失败函数                         */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void my_ip_conflict(void)
{
	u1_printf("获取IP失败，准备重启\r\n"); // 提示获取IP失败
	NVIC_SystemReset();					   // 重启
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
		u1_printf("\r\n=== %s NET CONF : DHCP ===\r\n", (char *)tmpstr);
	else
		u1_printf("\r\n=== %s NET CONF : Static ===\r\n", (char *)tmpstr);
	u1_printf("===========================\r\n");
	u1_printf("MAC地址: %02X:%02X:%02X:%02X:%02X:%02X\r\n", netinfo.mac[0], netinfo.mac[1], netinfo.mac[2], netinfo.mac[3], netinfo.mac[4], netinfo.mac[5]);
	u1_printf("IP地址: %d.%d.%d.%d\r\n", netinfo.ip[0], netinfo.ip[1], netinfo.ip[2], netinfo.ip[3]);
	u1_printf("网关地址: %d.%d.%d.%d\r\n", netinfo.gw[0], netinfo.gw[1], netinfo.gw[2], netinfo.gw[3]);
	u1_printf("子网掩码: %d.%d.%d.%d\r\n", netinfo.sn[0], netinfo.sn[1], netinfo.sn[2], netinfo.sn[3]);
	u1_printf("DNS服务器: %d.%d.%d.%d\r\n", netinfo.dns[0], netinfo.dns[1], netinfo.dns[2], netinfo.dns[3]);
	u1_printf("===========================\r\n");
}





/*
*********************************************************
                                                        *
               通用函数，执行其他任务                  *
                                                        *
*********************************************************
*/
void bsp_RunPer1ms(void)
{
    
}

void bsp_RunPer10ms(void)
{

}



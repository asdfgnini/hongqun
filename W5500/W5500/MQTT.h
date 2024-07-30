#ifndef __MQTT_H__
#define __MQTT_H__



#define SOCK_TCPS             0
#define SOCK_HUMTEM			  0
#define SOCK_PING			  0
#define SOCK_TCPC             1
#define SOCK_UDPS             2
#define SOCK_WEIBO      	  2
#define SOCK_DHCP             3
#define SOCK_HTTPS            4
#define SOCK_DNS              5
#define SOCK_SMTP             6
#define SOCK_NTP              7
#define MY_MAX_DHCP_RETRY	3      //DHCP重试次数
#define DATA_BUF_SIZE       2048   //缓冲区大小



void MQTT_PingREQ(void);
void AliIoT_Parameter_Init(void);

void W5500_init(void);
void my_ip_assign(void);
uint8_t do_tcp_client(void);
void MQTT_ConectPack(void);
uint8_t readCONNACK(void);
void MQTT_Subscribe(char *topic_name, int QoS);
void MQTT_DealPushdata_Qs0(unsigned char *redata);
void LED_PubState(unsigned char sta);
void MQTT_Buff_Init(void);


#define R_NUM 8        // 接收缓冲区个数
#define RBUFF_UNIT 300 // 接收缓冲区长度


#define C_NUM 8        // 命令缓冲区个数
#define CBUFF_UNIT 300 // 命令缓冲区长度

extern char Connect_flag;		// 同服务器连接状态  0：还没有连接服务器  1：连接上服务器了
extern char SubcribePack_flag; // 订阅报文状态      1：订阅报文成功
extern uint8_t count;
extern char my_dhcp_retry;					 // DHCP当前共计重试的次数

extern unsigned char gDATABUF[DATA_BUF_SIZE];	 // 数据缓冲区

extern unsigned char MQTT_RxDataBuf[R_NUM][RBUFF_UNIT]; // 数据的接收缓冲区,所有服务器发来的数据，存放在该缓冲区,缓冲区第一个字节存放数据长度
extern unsigned char *MQTT_RxDataInPtr;				 // 指向接收缓冲区存放数据的位置
extern unsigned char *MQTT_RxDataOutPtr;				 // 指向接收缓冲区读取数据的位置
extern unsigned char *MQTT_RxDataEndPtr;				 // 指向接收缓冲区结束的位置


extern unsigned char MQTT_CMDBuf[C_NUM][CBUFF_UNIT]; // 命令数据的接收缓冲区
extern unsigned char *MQTT_CMDInPtr;				  // 指向命令缓冲区存放数据的位置
extern unsigned char *MQTT_CMDOutPtr;				  // 指向命令缓冲区读取数据的位置
extern unsigned char *MQTT_CMDEndPtr;				  // 指向命令缓冲区结束的位置


extern uint8_t ping_open;


#define S_TOPIC_NAME "app_topic" // 需要订阅的主题
#define P_TOPIC_NAME "kfb_topic"   // 需要发布的主题

#endif



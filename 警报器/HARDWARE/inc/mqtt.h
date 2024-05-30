

#ifndef __MQTT_H
#define __MQTT_H

#define  R_NUM               8     //接收缓冲区个数
#define  RBUFF_UNIT          512   //接收缓冲区长度

#define  T_NUM               8     //发送缓冲区个数  
#define  TBUFF_UNIT          512   //发送缓冲区长度

#define  C_NUM               8     //命令缓冲区个数
#define  CBUFF_UNIT          512   //命令缓冲区长度

#define  MQTT_TxData(x)       u3_TxData(x)                                         //串口3负责数据发送

//{
//  "ProductKey": "a1iz3JmmXVM",
//  "DeviceName": "kaifaban",
//  "DeviceSecret": "d543370cb3683096765fa7738a1b8ea9"
//}


//    "product_key": "a1m13WLPrVP",
//    "device_name": "MD001",
//    "device_secret": "4e578df7145f346f5708224cb2a3ac33"

#define  PRODUCTKEY           "zjt"                                        //产品ID
#define  PRODUCTKEY_LEN       strlen(PRODUCTKEY)                                   //产品ID长度
#define  DEVICENAME           "zjt"                                              //设备名  
#define  DEVICENAME_LEN       strlen(DEVICENAME)                                   //设备名长度
#define  DEVICESECRE          "zjt"                   //设备秘钥   
#define  DEVICESECRE_LEN      strlen(DEVICESECRE)                                  //设备秘钥长度
#define  P_TOPIC_NAME         "kfb_topic"   //需要发布的主题 
#define  S_TOPIC_NAME         "app_topic"

#define  S_TOPIC_NUM          1                                             //订阅的主题的个数

//#ifndef __MQTT_GLOAL                                                        //如果没有__MQTT_GLOAL宏定义的c文件
//extern char Stopic_Buff[S_TOPIC_NUM][TBUFF_UNIT];                           //包含的是一个声明
//#else                                                                       //反之有__MQTT_GLOAL宏定义的c文件

//#endif

extern unsigned char  MQTT_RxDataBuf[R_NUM][RBUFF_UNIT];       //外部变量声明，数据的接收缓冲区,所有服务器发来的数据，存放在该缓冲区,缓冲区第一个字节存放数据长度
extern unsigned char *MQTT_RxDataInPtr;                        //外部变量声明，指向缓冲区存放数据的位置
extern unsigned char *MQTT_RxDataOutPtr;                       //外部变量声明，指向缓冲区读取数据的位置
extern unsigned char *MQTT_RxDataEndPtr;                       //外部变量声明，指向缓冲区结束的位置
extern unsigned char  MQTT_TxDataBuf[T_NUM][TBUFF_UNIT];       //外部变量声明，数据的发送缓冲区,所有发往服务器的数据，存放在该缓冲区,缓冲区第一个字节存放数据长度
extern unsigned char *MQTT_TxDataInPtr;                        //外部变量声明，指向缓冲区存放数据的位置
extern unsigned char *MQTT_TxDataOutPtr;                       //外部变量声明，指向缓冲区读取数据的位置
extern unsigned char *MQTT_TxDataEndPtr;                       //外部变量声明，指向缓冲区结束的位置
extern unsigned char  MQTT_CMDBuf[C_NUM][CBUFF_UNIT];          //外部变量声明，命令数据的接收缓冲区
extern unsigned char *MQTT_CMDInPtr;                           //外部变量声明，指向缓冲区存放数据的位置
extern unsigned char *MQTT_CMDOutPtr;                          //外部变量声明，指向缓冲区读取数据的位置
extern unsigned char *MQTT_CMDEndPtr;                          //外部变量声明，指向缓冲区结束的位置

extern char ClientID[128];     //外部变量声明，存放客户端ID的缓冲区
extern int  ClientID_len;      //外部变量声明，存放客户端ID的长度
extern char Username[128];     //外部变量声明，存放用户名的缓冲区
extern int  Username_len;	   //外部变量声明，存放用户名的长度
extern char Passward[128];     //外部变量声明，存放密码的缓冲区
extern int  Passward_len;	   //外部变量声明，存放密码的长度
extern char ServerIP[128];     //外部变量声明，存放服务器IP或是域名
extern int  ServerPort;        //外部变量声明，存放服务器的端口号
extern char Reg_connectFlag;    //外部变量声明，重连服务器状态    0：连接还存在  1：连接断开，重连

void MQTT_Buff_Init(void);
void IoT_Parameter_Init(void);
void MQTT_ConectPack(void);
void MQTT_SubscribeQs0(void);
void MQTT_PingREQ(void);
void MQTT_PublishQs0(char *, char *, int);
void MQTT_DealPushdata_Qs0(unsigned char *);	
void TxDataBuf_Deal(unsigned char *, int);
void CMDBuf_Deal(unsigned char *, int);
void MQTT_UNSubscribe(char *);
void MQTT_DISCONNECT(void);
void CreateSysMAC(uint8_t* pMacBuf);
#endif

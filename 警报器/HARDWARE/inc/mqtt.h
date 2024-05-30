

#ifndef __MQTT_H
#define __MQTT_H

#define  R_NUM               8     //���ջ���������
#define  RBUFF_UNIT          512   //���ջ���������

#define  T_NUM               8     //���ͻ���������  
#define  TBUFF_UNIT          512   //���ͻ���������

#define  C_NUM               8     //�����������
#define  CBUFF_UNIT          512   //�����������

#define  MQTT_TxData(x)       u3_TxData(x)                                         //����3�������ݷ���

//{
//  "ProductKey": "a1iz3JmmXVM",
//  "DeviceName": "kaifaban",
//  "DeviceSecret": "d543370cb3683096765fa7738a1b8ea9"
//}


//    "product_key": "a1m13WLPrVP",
//    "device_name": "MD001",
//    "device_secret": "4e578df7145f346f5708224cb2a3ac33"

#define  PRODUCTKEY           "zjt"                                        //��ƷID
#define  PRODUCTKEY_LEN       strlen(PRODUCTKEY)                                   //��ƷID����
#define  DEVICENAME           "zjt"                                              //�豸��  
#define  DEVICENAME_LEN       strlen(DEVICENAME)                                   //�豸������
#define  DEVICESECRE          "zjt"                   //�豸��Կ   
#define  DEVICESECRE_LEN      strlen(DEVICESECRE)                                  //�豸��Կ����
#define  P_TOPIC_NAME         "kfb_topic"   //��Ҫ���������� 
#define  S_TOPIC_NAME         "app_topic"

#define  S_TOPIC_NUM          1                                             //���ĵ�����ĸ���

//#ifndef __MQTT_GLOAL                                                        //���û��__MQTT_GLOAL�궨���c�ļ�
//extern char Stopic_Buff[S_TOPIC_NUM][TBUFF_UNIT];                           //��������һ������
//#else                                                                       //��֮��__MQTT_GLOAL�궨���c�ļ�

//#endif

extern unsigned char  MQTT_RxDataBuf[R_NUM][RBUFF_UNIT];       //�ⲿ�������������ݵĽ��ջ�����,���з��������������ݣ�����ڸû�����,��������һ���ֽڴ�����ݳ���
extern unsigned char *MQTT_RxDataInPtr;                        //�ⲿ����������ָ�򻺳���������ݵ�λ��
extern unsigned char *MQTT_RxDataOutPtr;                       //�ⲿ����������ָ�򻺳�����ȡ���ݵ�λ��
extern unsigned char *MQTT_RxDataEndPtr;                       //�ⲿ����������ָ�򻺳���������λ��
extern unsigned char  MQTT_TxDataBuf[T_NUM][TBUFF_UNIT];       //�ⲿ�������������ݵķ��ͻ�����,���з��������������ݣ�����ڸû�����,��������һ���ֽڴ�����ݳ���
extern unsigned char *MQTT_TxDataInPtr;                        //�ⲿ����������ָ�򻺳���������ݵ�λ��
extern unsigned char *MQTT_TxDataOutPtr;                       //�ⲿ����������ָ�򻺳�����ȡ���ݵ�λ��
extern unsigned char *MQTT_TxDataEndPtr;                       //�ⲿ����������ָ�򻺳���������λ��
extern unsigned char  MQTT_CMDBuf[C_NUM][CBUFF_UNIT];          //�ⲿ�����������������ݵĽ��ջ�����
extern unsigned char *MQTT_CMDInPtr;                           //�ⲿ����������ָ�򻺳���������ݵ�λ��
extern unsigned char *MQTT_CMDOutPtr;                          //�ⲿ����������ָ�򻺳�����ȡ���ݵ�λ��
extern unsigned char *MQTT_CMDEndPtr;                          //�ⲿ����������ָ�򻺳���������λ��

extern char ClientID[128];     //�ⲿ������������ſͻ���ID�Ļ�����
extern int  ClientID_len;      //�ⲿ������������ſͻ���ID�ĳ���
extern char Username[128];     //�ⲿ��������������û����Ļ�����
extern int  Username_len;	   //�ⲿ��������������û����ĳ���
extern char Passward[128];     //�ⲿ�����������������Ļ�����
extern int  Passward_len;	   //�ⲿ�����������������ĳ���
extern char ServerIP[128];     //�ⲿ������������ŷ�����IP��������
extern int  ServerPort;        //�ⲿ������������ŷ������Ķ˿ں�
extern char Reg_connectFlag;    //�ⲿ��������������������״̬    0�����ӻ�����  1�����ӶϿ�������

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

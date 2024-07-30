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
#define MY_MAX_DHCP_RETRY	3      //DHCP���Դ���
#define DATA_BUF_SIZE       2048   //��������С



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


#define R_NUM 8        // ���ջ���������
#define RBUFF_UNIT 300 // ���ջ���������


#define C_NUM 8        // �����������
#define CBUFF_UNIT 300 // �����������

extern char Connect_flag;		// ͬ����������״̬  0����û�����ӷ�����  1�������Ϸ�������
extern char SubcribePack_flag; // ���ı���״̬      1�����ı��ĳɹ�
extern uint8_t count;
extern char my_dhcp_retry;					 // DHCP��ǰ�������ԵĴ���

extern unsigned char gDATABUF[DATA_BUF_SIZE];	 // ���ݻ�����

extern unsigned char MQTT_RxDataBuf[R_NUM][RBUFF_UNIT]; // ���ݵĽ��ջ�����,���з��������������ݣ�����ڸû�����,��������һ���ֽڴ�����ݳ���
extern unsigned char *MQTT_RxDataInPtr;				 // ָ����ջ�����������ݵ�λ��
extern unsigned char *MQTT_RxDataOutPtr;				 // ָ����ջ�������ȡ���ݵ�λ��
extern unsigned char *MQTT_RxDataEndPtr;				 // ָ����ջ�����������λ��


extern unsigned char MQTT_CMDBuf[C_NUM][CBUFF_UNIT]; // �������ݵĽ��ջ�����
extern unsigned char *MQTT_CMDInPtr;				  // ָ���������������ݵ�λ��
extern unsigned char *MQTT_CMDOutPtr;				  // ָ�����������ȡ���ݵ�λ��
extern unsigned char *MQTT_CMDEndPtr;				  // ָ���������������λ��


extern uint8_t ping_open;


#define S_TOPIC_NAME "app_topic" // ��Ҫ���ĵ�����
#define P_TOPIC_NAME "kfb_topic"   // ��Ҫ����������

#endif



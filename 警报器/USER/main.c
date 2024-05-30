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
�������޸���mqtt.h���޸ģ��������ڱ������������emqx����
������ͻ��˵�¼mqtt���������û���������DEVICENAME&PRODUCTKEY����������Ϊzjt&zjt
���������clientId%sdeviceName%sproductKey%s������%s�ֱ��ӦDEVICENAME,DEVICENAME,PRODUCTKEY���ڱ�������ΪclientIdzjtdeviceNamezjtproductKeyzjt
���ܷ�ʽΪhmacsha1���ܣ����ܵ���Կ��DEVICESECRE  ��������Ϊzjt
�������м��ܺ�Ϊ 70e12b4e92ad65d58e42408c328a807987620b6b
*/
void W5500_init(void);
void network_init(void);
void my_ip_conflict(void);
void my_ip_assign(void);
						 
wiz_NetInfo gWIZNETINFO;				 // MAC��ַ�Լ�дһ������Ҫ��·�������������豸һ������
#define SOCK_DHCP		    0      //DHCP�����õĶ˿ڣ�W5500��8����0-7����
#define SOCK_TCPS	        1      //TCP�����õĶ˿ڣ� W5500��8����0-7����
#define SOCKET_DNS          2      //DNS�����õĶ˿ڣ� W5500��8����0-7����
#define MY_MAX_DHCP_RETRY	3      //DHCP���Դ���
#define DATA_BUF_SIZE       2048   //��������С
unsigned char gDATABUF[DATA_BUF_SIZE];	 // ���ݻ�����
char DNS_flag = 0;      // DNS������־  0:δ�ɹ�  1���ɹ�
int tcp_state;							 // ���ڱ���TCP���ص�״̬
int temp_state;							 // ������ʱ����TCP���ص�״̬
char my_dhcp_retry = 0;					 // DHCP��ǰ�������ԵĴ���



void LED_PubState(unsigned char sta);

void MQTT_CheckConnectStatus(unsigned char status);
void MQTT_CheckSubcribeStatus(unsigned char *status);
void MAIN_CHECK_led(char *strCmd);
//
#define FW_VERSION "20220817L00"
unsigned char g_statusChanged = 1; // �״�������Ҫ����
char g_connectPackFlag = 0;  //�ⲿ����������CONNECT����״̬   1��CONNECT���ĳɹ�
char g_subcribePackFlag = 0;   //���ı���״̬      1�����ı��ĳɹ�
char g_connectFlag = 0;        //ͬ����������״̬  0����û�����ӷ�����  1�������Ϸ�������
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
        Usart1_Init(115200); //����1���ܳ�ʼ����������9600
        TIM2_ENABLE_500MS();//ι���͵㶯
        IWDG_Init(6, 1600); //ֻҪ��4*1280ms=5120�ڽ���ι���Ͳ��Ḵλϵͳ
        Usart3_Init(115200); //����3���ܳ�ʼ����������115200
        LED_LocationStaInit(); //��ʼ��
        // ���ݴ���
        u1_printf("�豸�汾��: %s\r\n", FW_VERSION); //��ӡ�汾��
        u1_printf("��ʼ��������..\r\n"); //��ʾ��Ϣ
        // ���ö�λ��״̬
        LED_LocationStaOff();
        LED_LocationStaOn();

        IoT_Parameter_Init(); //��ʼ��������IoTƽ̨MQTT�������Ĳ���
        CAT1_GIPO_Init();	  //��ʼ��4Gģ��Ŀ���IO
        bsp_StartAutoTimer(0,1000);//���������ʱ����1s�ϱ�
        
        while (1)
        {
            if (g_connectFlag == 1)//4G���ӳɹ��ı�־
            {
                // u1_printf("g_connectFlag == 1\r\n");//����־
            }
            if ((g_subcribePackFlag == S_TOPIC_NUM) && (g_connectFlag == 1))//���ĳɹ��ı�־
            {
                // u1_printf("(g_subcribePackFlag == S_TOPIC_NUM) && (g_connectFlag == 1)\r\n"); //����־
            }
            if (resetCnt_x30s >= 6) 
            {
                u1_printf("����%d����δ���ӵ�������������\r\n", resetCnt_x30s /2);
                g_connectFlag = 0;
            }
            if (longTimeReset_x30s >= 720)             
            {
                u1_printf("����%dСʱ�������ӷ�������������ͨ�ţ�����\r\n", longTimeReset_x30s /2 /60);
                g_connectFlag = 0;
            }
            //��ʱ1s��������
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
            if (g_connectFlag == 1) /* �����Ϸ����� */
            {
                if (MQTT_TxDataOutPtr != MQTT_TxDataInPtr) /*�����ͻ���������*/
                { // if�����Ļ���˵�����ͻ�����������
                    //��1�֣�0x10 ���ӱ���
                    //��2�֣�0x82 ���ı��ģ���g_connectPackFlag��λ����ʾ���ӱ��ĳɹ�
                    //��3�֣�g_subcribePackFlag��λ��˵�����ӺͶ��ľ��ɹ����������Ŀɷ�
                    if ((DMA_flag == 0) && ((MQTT_TxDataOutPtr[2] == 0x10) ||
                        ((MQTT_TxDataOutPtr[2] == 0x82) && (g_connectPackFlag == 1)) ||
                        (g_subcribePackFlag == S_TOPIC_NUM)))
                    {
                        // u1_printf("��������:0x%x\r\n", MQTT_TxDataOutPtr[2]); //������ʾ��Ϣ
                        MQTT_TxData(MQTT_TxDataOutPtr);						  //��������
                        MQTT_TxDataOutPtr += TBUFF_UNIT;					  //ָ������
                        if (MQTT_TxDataOutPtr == MQTT_TxDataEndPtr)			  //���ָ�뵽������β����
                            MQTT_TxDataOutPtr = MQTT_TxDataBuf[0];			  //ָ���λ����������ͷ
                    }
                }

                if (MQTT_RxDataOutPtr != MQTT_RxDataInPtr) /* ������ջ��������� */
                {
                    // u1_printf("���յ�����:");

                    /*����CONNACK����*/
                    //�����һ���ֽ���0x20����ʾ�յ�����CONNACK����
                    //��������Ҫ�жϵ�4���ֽڣ�����CONNECT�����Ƿ�ɹ�
                    if (MQTT_RxDataOutPtr[2] == 0x20) {
                        MQTT_CheckConnectStatus(MQTT_RxDataOutPtr[5]);
                    }

                    /* ����SUBACK���� */
                    //��һ���ֽ���0x90����ʾ�յ�����SUBACK����
                    //��������Ҫ�ж϶��Ļظ��������ǲ��ǳɹ�
                    else if (MQTT_RxDataOutPtr[2] == 0x90)
                    {
                        MQTT_CheckSubcribeStatus(&MQTT_RxDataOutPtr[6]);
                        u1_printf("\r\n");
                    }
                    /* ����PINGRESP����
                    // ��һ���ֽ���0xD0����ʾ�յ�����PINGRESP���� */
                    else if (MQTT_RxDataOutPtr[2] == 0xD0)
                    {
                        resetCnt_x30s = 0;
                        u1_printf("PING-%d\r\n", longTimeReset_x30s);
                    }

                    /* �����������ͱ��� */
                    // �����һ���ֽ���0x30����ʾ�յ����Ƿ�������������������
                    //����Ҫ��ȡ��������
                    else if ((MQTT_RxDataOutPtr[2] == 0x30))
                    {
                        u1_printf("�������ȼ�0����\r\n");
                        MQTT_DealPushdata_Qs0(MQTT_RxDataOutPtr); //����ȼ�0��������
                    }

                    MQTT_RxDataOutPtr += RBUFF_UNIT;			//����ָ������
                    if (MQTT_RxDataOutPtr == MQTT_RxDataEndPtr) //�������ָ�뵽���ջ�����β����
                        MQTT_RxDataOutPtr = MQTT_RxDataBuf[0];	//����ָ���λ�����ջ�������ͷ
                }

                if (MQTT_CMDOutPtr != MQTT_CMDInPtr) /* ��������������� */
                {
                    longTimeReset_x30s = 0;
                    u1_printf("����:%s\r\n", &MQTT_CMDOutPtr[2]);
                    MAIN_CHECK_led((char *)MQTT_CMDOutPtr + 2);
                    MQTT_CMDOutPtr += CBUFF_UNIT;		  //ָ������
                    if (MQTT_CMDOutPtr == MQTT_CMDEndPtr) //���ָ�뵽������β����
                        MQTT_CMDOutPtr = MQTT_CMDBuf[0];  //ָ���λ����������ͷ
                }										  //��������������ݵ�else if��֧��β
            }
            else
            {
                bsp_DelayMS(500);
                u1_printf("���ӷ�����\r\n");
                bsp_DelayMS(500);
                CAT1_RxCounter = 0;//������������������
                g_connectPackFlag = 0;
                g_subcribePackFlag = 0;
                longTimeReset_x30s = 0;
                resetCnt_x30s = 0;

                memset(CAT1_RX_BUF, 0, CAT1_RXBUFF_SIZE); //��ս��ջ�����
                
                if (CAT1_Connect_IoTServer(30) == 0)
                {
                    u1_printf("���ӷ������ɹ�\r\n");		 
                    Usart3_IDELInit(115200);				  //����3 ����DMA �� �����ж�
                    g_connectFlag = 1;						  //��1����ʾ���ӳɹ�
                    CAT1_RxCounter = 0;						  //������������������
                    memset(CAT1_RX_BUF, 0, CAT1_RXBUFF_SIZE); //��ս��ջ�����
                    MQTT_Buff_Init();						  //��ʼ�����ͽ����������
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
	{ //ѭ����ѯ���Ľ��
		switch (*status)
		{
		case 0x00:
		case 0x01:
			u1_printf("���ĳɹ�%d ", i + 1);
			g_subcribePackFlag++;

			break;
		default:
			u1_printf("����ʧ�ܣ�׼������\r\n");
			g_connectFlag = 0; //���㣬��������
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
		u1_printf("CONNECT���ĳɹ�\r\n");
		g_connectPackFlag = 1;			  // CONNECT���ĳɹ�
		break;
	case 0x01:
		u1_printf("�����Ѿܾ�����֧�ֵ�Э��汾��׼������\r\n");
		g_connectFlag = 0; // ���㣬��������
		break;
	case 0x02:
		u1_printf("�����Ѿܾ������ϸ�Ŀͻ��˱�ʶ����׼������\r\n");
		g_connectFlag = 0; // ���㣬��������
		break;
	case 0x03:
		u1_printf("�����Ѿܾ�������˲����ã�׼������\r\n");
		g_connectFlag = 0; // ���㣬��������
		break;
	case 0x04:
		u1_printf("�����Ѿܾ�����Ч���û��������룬׼������\r\n");
		g_connectFlag = 0; // ���㣬��������
		break;
	case 0x05:
		u1_printf("�����Ѿܾ���δ��Ȩ��׼������\r\n");
		g_connectFlag = 0; // ���㣬��������
		break;
	default:
		u1_printf("�����Ѿܾ���δ֪״̬��׼������\r\n");
		g_connectFlag = 0; // ���㣬��������
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
//		u1_printf("������λ��ģʽNULL\r\n");
		return;
	}
	sscanf(strCmdTmp,"\"params\":{\"powerstate\":%d", &pwrState);//�������
	u1_printf("������λ��ģʽ:%d \r\n", pwrState);

	if (pwrState != 0 && pwrState != 1) {
		u1_printf("������λ��ģʽ:%d����\r\n", pwrState);
		return;
	}
	if (pwrState == 0) {
		LED_LocationStaOff();
		u1_printf("�رյ�\r\n");
		LED_PubState(0);
	}
	if (pwrState == 1) {
		LED_LocationStaOn();
		u1_printf("�򿪵�\r\n");
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
                         ����                             *
                                                        *
*********************************************************
*/
void W5500_init(void)
{
	// W5500�շ��ڴ�������շ������������ܵĿռ���16K����0-7��ÿ���˿ڵ��շ����������Ƿ��� 2K
	char memsize[2][8] = {{2, 2, 2, 2, 2, 2, 2, 2}, {2, 2, 2, 2, 2, 2, 2, 2}};
	char tmp;
    SPI_Configuration();								   // ��ʼ��SPI�ӿ�

    reg_wizchip_cris_cbfunc(SPI_CrisEnter, SPI_CrisExit);  // ע���ٽ�������
	reg_wizchip_cs_cbfunc(SPI_CS_Select, SPI_CS_Deselect); // ע��SPIƬѡ�źź���
	reg_wizchip_spi_cbfunc(SPI_ReadByte, SPI_WriteByte);   // ע���д����
    if (ctlwizchip(CW_INIT_WIZCHIP, (void *)memsize) == -1)
	{												  // ���if��������ʾ�շ��ڴ����ʧ��
		u1_printf("��ʼ���շ�����ʧ��,׼������\r\n"); // ��ʾ��Ϣ
		NVIC_SystemReset();							  // ����
	}	
    do
	{ // �������״̬
		if (ctlwizchip(CW_GET_PHYLINK, (void *)&tmp) == -1)
		{										 // ���if��������ʾδ֪����
			u1_printf("δ֪����׼������\r\n"); // ��ʾ��Ϣ
			NVIC_SystemReset();					 // ����
		}
		if (tmp == PHY_LINK_OFF)
		{
			u1_printf("����δ����\r\n"); // �����⵽������û���ӣ���ʾ��������
			bsp_DelayMS(2000);				 // ��ʱ2s
		}
	} while (tmp == PHY_LINK_OFF); // ѭ��ִ�У�ֱ������������

    CreateSysMAC(gWIZNETINFO.mac);
    setSHAR(gWIZNETINFO.mac);    // ����MAC��ַ
	DHCP_init(SOCK_DHCP, gDATABUF);								 // ��ʼ��DHCP
    reg_dhcp_cbfunc(my_ip_assign, my_ip_assign, my_ip_conflict); // ע��DHCP�ص�����
    DNS_flag = 0;												 // DNS������־=0
    my_dhcp_retry = 0;											 // DHCP���Դ���=0
    tcp_state = 0;												 // TCP״̬=0
    temp_state = -1;											 // ��һ��TCP״̬=-1
							
}
void my_ip_assign(void)
{
	getIPfromDHCP(gWIZNETINFO.ip);	 // �ѻ�ȡ����ip��������¼����������
	getGWfromDHCP(gWIZNETINFO.gw);	 // �ѻ�ȡ�������ز�������¼����������
	getSNfromDHCP(gWIZNETINFO.sn);	 // �ѻ�ȡ�������������������¼����������
	getDNSfromDHCP(gWIZNETINFO.dns); // �ѻ�ȡ����DNS��������������¼����������
	gWIZNETINFO.dhcp = NETINFO_DHCP; // ���ʹ�õ���DHCP��ʽ
	network_init();					 // ��ʼ������
	u1_printf("DHCP���� : %d ��\r\n", getDHCPLeasetime());
}
/*-------------------------------------------------*/
/*����������ȡIP��ʧ�ܺ���                         */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void my_ip_conflict(void)
{
	u1_printf("��ȡIPʧ�ܣ�׼������\r\n"); // ��ʾ��ȡIPʧ��
	NVIC_SystemReset();					   // ����
}

void network_init(void)
{
	char tmpstr[6] = {0};
	wiz_NetInfo netinfo;

	ctlnetwork(CN_SET_NETINFO, (void *)&gWIZNETINFO); // �����������
	ctlnetwork(CN_GET_NETINFO, (void *)&netinfo);	  // ��ȡ�������
	ctlwizchip(CW_GET_ID, (void *)tmpstr);			  // ��ȡоƬID

	// ��ӡ�������
	if (netinfo.dhcp == NETINFO_DHCP)
		u1_printf("\r\n=== %s NET CONF : DHCP ===\r\n", (char *)tmpstr);
	else
		u1_printf("\r\n=== %s NET CONF : Static ===\r\n", (char *)tmpstr);
	u1_printf("===========================\r\n");
	u1_printf("MAC��ַ: %02X:%02X:%02X:%02X:%02X:%02X\r\n", netinfo.mac[0], netinfo.mac[1], netinfo.mac[2], netinfo.mac[3], netinfo.mac[4], netinfo.mac[5]);
	u1_printf("IP��ַ: %d.%d.%d.%d\r\n", netinfo.ip[0], netinfo.ip[1], netinfo.ip[2], netinfo.ip[3]);
	u1_printf("���ص�ַ: %d.%d.%d.%d\r\n", netinfo.gw[0], netinfo.gw[1], netinfo.gw[2], netinfo.gw[3]);
	u1_printf("��������: %d.%d.%d.%d\r\n", netinfo.sn[0], netinfo.sn[1], netinfo.sn[2], netinfo.sn[3]);
	u1_printf("DNS������: %d.%d.%d.%d\r\n", netinfo.dns[0], netinfo.dns[1], netinfo.dns[2], netinfo.dns[3]);
	u1_printf("===========================\r\n");
}





/*
*********************************************************
                                                        *
               ͨ�ú�����ִ����������                  *
                                                        *
*********************************************************
*/
void bsp_RunPer1ms(void)
{
    
}

void bsp_RunPer10ms(void)
{

}



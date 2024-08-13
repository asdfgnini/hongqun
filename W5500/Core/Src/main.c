#include "main.h"
#include "spi.h"
#include "gpio.h"
#include "bsp.h"
 

uint8_t state = 0;


int main(void)
{

	int ret = 0;
	bsp_Init();
	
//	MX_GPIO_Init();
//	MX_SPI1_Init();
//	
//	AliIoT_Parameter_Init();
//	
//	W5500_init();
//	printf("�������ӳɹ�\r\n");
	
	bsp_StartAutoTimer(0,1000);
	
	CAT_Connect_IoTServer();
	
	bsp_InitJQ();
	uint8_t _data;
	while (1)
	{
		if(0)
		{
			if(comGetChar(COM3,&_data))//�Ⱦ���
			{
				if(_data == 'a')
				{
					bsp_JQ_On(2);
					bsp_DelayMS(100);
					bsp_JQ_Off(2);
					printf("%c\r\n",_data);
					u2_printf("AT+MPUB=\"kfb_topic\",0,0,\"help\"\r\n");
				}
				else if(_data == 'b')//ɵ��
				{
					
					bsp_JQ_On(1);
					bsp_DelayMS(100);
					bsp_JQ_Off(1);
					printf("%c\r\n",_data);
					u2_printf("AT+MPUB=\"kfb_topic\",0,0,\"NO\"\r\n");
				}
			}
			
		}
		else
		{
			switch(DHCP_run())
			{
				case DHCP_IP_ASSIGN:
				case DHCP_IP_CHANGED:
					my_ip_assign();
					break;	
				case DHCP_IP_LEASED:

					if (Connect_flag == 0)
					{
						while(do_tcp_client()!=1)
						{

							printf("== while(do_tcp_client()!=1)==  \r\n");
							bsp_DelayMS(500);
						}   
						if(getSn_IR(SOCK_TCPC) & Sn_IR_CON)
						{				
							setSn_IR(SOCK_TCPC, Sn_IR_CON); 			            /*��������жϱ�־λ�������� Sn_IR λ����Ļ�������Ӧ�ý���λ�á�1����*/
						}
						MQTT_Buff_Init();
						//CONNECT
						MQTT_ConectPack();					
						while((getSn_IR(SOCK_TCPC) & Sn_IR_RECV)==0) 				// Sn_IR��RECVλ��1��֪ͨW5500�յ�����
						{

							printf(" �ȴ�MQTT��������Ӧ ...\r\n");
							bsp_DelayMS(100);  
						}	
						setSn_IR(SOCK_TCPC, Sn_IR_RECV);
						Connect_flag = 1;
						
						//CONNACK
						switch(readCONNACK())
						{
						case 0: printf("�����ѽ���\r\n"); 

						break;//�����ѱ�����˽���
						case 1: printf("�����Ѿܾ�����֧�ֵ�Э��汾\r\n"); break;//����˲�֧�ֿͻ�������� MQTT Э�鼶��
						case 2: printf("�����Ѿܾ������ϸ�Ŀͻ��˱�ʶ��\r\n"); break;//�ͻ��˱�ʶ������ȷ�� UTF-8 ���룬 ������˲�����ʹ��
						case 3: printf("�����Ѿܾ�������˲����� \r\n"); break;//���������ѽ����� �� MQTT ���񲻿���
						case 4: printf("�����Ѿܾ�����Ч���û���������\r\n"); break;//�û�������������ݸ�ʽ��Ч
						case 5: printf("�����Ѿܾ���δ��Ȩ\r\n"); break;//δ��Ȩ �ͻ���δ����Ȩ���ӵ��˷�����
						case 0xff: printf("bug!!\r\n"); break;//δ��Ȩ �ͻ���δ����Ȩ���ӵ��˷�����
						default: printf("����\r\n"); break;//	
						}

						//�����������
						MQTT_Subscribe(S_TOPIC_NAME, 0);
					}
					//MQTT���ݽ���
					ret = recv(SOCK_TCPC, gDATABUF, DATA_BUF_SIZE); 
					if(ret > 0)
					{
						memcpy(&MQTT_RxDataInPtr[2], gDATABUF, ret); 
						MQTT_RxDataInPtr[0] = ret / 256;			 
						MQTT_RxDataInPtr[1] = ret % 256;			 
						MQTT_RxDataInPtr += RBUFF_UNIT;				 
						if (MQTT_RxDataInPtr == MQTT_RxDataEndPtr)	 
							MQTT_RxDataInPtr = MQTT_RxDataBuf[0];	
						if(ping_open == 1)
						{
							__HAL_TIM_SET_COUNTER(&TIM3_Handler,0);	
						}
					}
					//MQTT�Ѿ�����
					if(Connect_flag == 1)
					{
						count++;
						if (MQTT_RxDataOutPtr != MQTT_RxDataInPtr)
						{ // if�����Ļ���˵�����ջ�������������
							printf("���յ�����:");

							if (MQTT_RxDataOutPtr[2] == 0x90)
							{
								switch (MQTT_RxDataOutPtr[6])
								{
								case 0x00:
								case 0x01:
									printf("���ĳɹ�\r\n"); // ���������Ϣ
									SubcribePack_flag = 1;	   // SubcribePack_flag��1����ʾ���ı��ĳɹ����������Ŀɷ���
									TIM3_Init();		   // ����30s��PING��ʱ��
									ping_open = 1;
									break; // ������֧
								default:
									printf("����ʧ�ܣ�׼������\r\n"); // ���������Ϣ
									NVIC_SystemReset();					 // ����
									break;								 // ������֧
								}
							}
							// if�жϣ����һ��������2���ֽڣ���һ���ֽ���0xD0����ʾ�յ�����PINGRESP����
							else if (MQTT_RxDataOutPtr[2] == 0xD0)
							{
								printf("PING���Ļظ�\r\n"); // ���������Ϣ
							}
							// if�жϣ������һ���ֽ���0x30����ʾ�յ����Ƿ�������������������
							// ����Ҫ��ȡ��������
							else if ((MQTT_RxDataOutPtr[2] == 0x30))
							{
								printf("�������ȼ�0����\r\n");		  // ���������Ϣ
								MQTT_DealPushdata_Qs0(MQTT_RxDataOutPtr); // ����ȼ�0��������
							}
							MQTT_RxDataOutPtr += RBUFF_UNIT;			// ָ������
							if (MQTT_RxDataOutPtr == MQTT_RxDataEndPtr) // ���ָ�뵽������β����
								MQTT_RxDataOutPtr = MQTT_RxDataBuf[0];	// ָ���λ����������ͷ
						}
						if (MQTT_CMDOutPtr != MQTT_CMDInPtr)
						{												  // if�����Ļ���˵�����������������
							printf("����:%s\r\n", &MQTT_CMDOutPtr[2]); // ���������Ϣ
							if (strstr((char *)MQTT_CMDOutPtr + 2, "\"params\":{\"power_switch\":1}"))
							{						 // ���������"params":{"power_switch":1}˵���������·��򿪿���1
									bsp_LedOn(1);
									LED_PubState(1);	 // �жϿ���״̬����������������
							}
							else if (strstr((char *)MQTT_CMDOutPtr + 2, "\"params\":{\"power_switch\":0}"))
							{						  // ���������"params":{"power_switch":0}˵���������·��رտ���1
									bsp_LedOff(1); // �ر�LED1
									LED_PubState(0);	  // �жϿ���״̬����������������
							}
							MQTT_CMDOutPtr += CBUFF_UNIT;		  // ָ������
							if (MQTT_CMDOutPtr == MQTT_CMDEndPtr) // ���ָ�뵽������β����
								MQTT_CMDOutPtr = MQTT_CMDBuf[0];  // ָ���λ����������ͷ
						}

						//����ʱ��ѹ����������
						if(bsp_CheckTimer(0))
						{	
							if(state == 0)
							{
								LED_PubState(state);		
								state = 1;							
							}
							else
							{
								LED_PubState(state);
								state = 0;							
							}
						}
					}
					break;
				case DHCP_FAILED:
					my_dhcp_retry++; // ʧ�ܴ���+1
					if (my_dhcp_retry > MY_MAX_DHCP_RETRY)
					{										 // ���ʧ�ܴ�������������������if
						printf("DHCPʧ�ܣ�׼������\r\n"); // ������ʾ��Ϣ
						NVIC_SystemReset();					 // ����
					}
					break;
			}
		}
	}
	
}





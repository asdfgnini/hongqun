

#include "bsp.h"

#define _4G_DEBUG 1

uint8_t MQTT_4G_CONNECT = 0;

char CAT_AT_Test(void)
{
	char rxBuffer[128] = {0};
	uint8_t count = 0;
	u2_printf("AT\r\n");
	bsp_DelayMS(1000);
	
	while(count <= 128)
	{
		comGetChar(COM2,(uint8_t*)&rxBuffer[count]);
		count++;
	}
#if _4G_DEBUG
	uint8_t length = strlen(rxBuffer);
	printf("���ܵ���%s %d\n",rxBuffer,length);
#endif
	if(strstr(rxBuffer,"OK"))
	{
		return 0; //�ɹ���ʾ����0
	}

	bsp_DelayMS(200);
#if _4G_DEBUG
	printf("ATʧ�ܣ�%s\n",rxBuffer);
#endif	
	return 1;

}


char CAT1_Close_Back(void)
{
	char rxBuffer[128] = {0};
	uint8_t count = 0;
	u2_printf("ATE0\r\n");
	bsp_DelayMS(1000);
	
	while(count <= 128)
	{
		comGetChar(COM2,(uint8_t*)&rxBuffer[count]);
		count++;
	}
#if _4G_DEBUG
	uint8_t length = strlen(rxBuffer);
	printf("���ܵ���%s %d\n",rxBuffer,length);
#endif
	if(strstr(rxBuffer,"OK"))
	{
		return 0; //�ɹ���ʾ����0
	}

	bsp_DelayMS(200);
#if _4G_DEBUG
	printf("ATʧ�ܣ�%s\n",rxBuffer);
#endif
	return 1;
}



char CAT1_SendCmd(char *cmd)
{
	char rxBuffer[128] = {0};
	uint8_t count = 0;
	u2_printf("%s\r\n",cmd);
	
	bsp_DelayMS(1000);
	while(count <= 128)
	{
		comGetChar(COM2,(uint8_t*)&rxBuffer[count]);
		count++;
	}
#if _4G_DEBUG
	uint8_t length = strlen(rxBuffer);
	printf("���ܵ���%s %d\n",rxBuffer,length);
#endif
	if(strstr(rxBuffer,"OK"))
	{
		return 0; //�ɹ���ʾ����0
	}
	if(strstr(rxBuffer,"FAILURE"))
	{
		return 1;
	}
#if _4G_DEBUG
	printf("����ʧ�ܣ�%s\n",rxBuffer);
#endif
	return 1;
}

char CAT1_SendNetOpen()
{
	char rxBuffer[200] = {0};
	uint8_t count = 0;
	u2_printf("AT+NETOPEN?\r\n");
	
	bsp_DelayMS(1000);
	while(count <= 200)
	{
		comGetChar(COM2,(uint8_t*)&rxBuffer[count]);
		count++;
	}
#if _4G_DEBUG
	uint8_t length = strlen(rxBuffer);
	printf("���ܵ���%s %d\n",rxBuffer,length);
#endif
	if(strstr(rxBuffer,"1"))
	{
		return 0; //�ɹ���ʾ����0
	}
	if(strstr(rxBuffer,"0"))
	{
		return 1;
	}
#if _4G_DEBUG
	printf("����ʧ�ܣ�%s\n",rxBuffer);
#endif
	return 1;
}

char CAT1_SendCmd1(char *cmd)
{
	char rxBuffer[128] = {0};
	uint8_t count = 0;
	u2_printf("%s\r\n",cmd);
	
	bsp_DelayMS(1000);
	while(count <= 128)
	{
		comGetChar(COM2,(uint8_t*)&rxBuffer[count]);
		count++;
	}
#if _4G_DEBUG
	uint8_t length = strlen(rxBuffer);
	printf("���ܵ���%s %d\n",rxBuffer,length);
#endif
	if(strstr(rxBuffer,"SUCCESS"))
	{
		return 0; //�ɹ���ʾ����0
	}
	if(strstr(rxBuffer,"FAILURE"))
	{
		return 1;
	}
#if _4G_DEBUG
	printf("����ʧ�ܣ�%s\n",rxBuffer);
#endif
	return 1;
}

char CAT1_SendCmd2(char *cmd)
{
	
	char rxBuffer[128] = {0};
	uint8_t count = 0;
	u2_printf("%s\r\n",cmd);
	
	bsp_DelayMS(2000);
	while(count <= 128)
	{
		comGetChar(COM2,(uint8_t*)&rxBuffer[count]);
		count++;
	}
#if _4G_DEBUG
	uint8_t length = strlen(rxBuffer);
	printf("���ܵ���%s %d\n",rxBuffer,length);
#endif
	if(strstr(rxBuffer,"SUCCESS"))
	{
		return 0; //�ɹ���ʾ����0
	}
	if(strstr(rxBuffer,"FAILURE"))
	{
		return 1;
	}
#if _4G_DEBUG
	printf("����ʧ�ܣ�%s\n",rxBuffer);
#endif
	return 1;
}


char UserName[128]={0};
char PassWord[128]={0};

#define		SERIP					"47.108.144.41"
#define    PORT					1883
#define    SUB_TOPIC			"app_topic"
#define 	RECV_TOPIC			"kfb_topic"


char CAT_Connect_IoTServer(void)
{
	char recv_flag=0;
	char temp[256]={0};

	if(CAT_AT_Test())
	{
		printf("ATʧ��\n");
		return recv_flag = 1;
	}
	printf("AT�ɹ�\r\n");

	if(CAT1_Close_Back())
	{
		printf("AT����ʧ��\n");
		return recv_flag = 2;
	}
	printf("AT���Գɹ�\r\n");
	
	CAT1_SendCmd("AT+MDISCONNECT\r\n");
	CAT1_SendCmd("AT+MIPCLOSE\r\n");

	
	if(CAT1_SendCmd("AT+QICSGP=1,1,\"\",\"\",\"\"\r\n"))
	{
		printf("AT��������ʧ��\n");
		return recv_flag = 3;
	}
	printf("AT��������ɹ�\n");
		
	bsp_DelayMS(1000);
	//��ѯ����
	if(CAT1_SendNetOpen())
	{
		printf("ATû�д�����\n");
		if(CAT1_SendCmd2("AT+NETOPEN\r\n"))
		{
			printf("AT������ʧ��\n");
			NVIC_SystemReset();
			return recv_flag = 3;
		}
	}
	printf("AT������ɹ�\r\n");
	
	sprintf(temp,"AT+MCONFIG=%s\r\n","zjt_test");
	CAT1_SendCmd(temp);
	
	memset(temp,0,sizeof(temp));
	
	sprintf(temp,"AT+MIPSTART=%s,%d\r\n",SERIP,PORT);
	if(CAT1_SendCmd1(temp))
	{
		NVIC_SystemReset();
		return recv_flag = 5;
	}
	memset(temp,0,sizeof(temp));
	bsp_DelayMS(500);
	if(CAT1_SendCmd1("AT+MCONNECT=1,60\r\n"))
	{
		return recv_flag = 6;
	}
	bsp_DelayMS(500);
	
	sprintf(temp,"AT+MSUB=%s,0\r\n",SUB_TOPIC);
	if(CAT1_SendCmd1(temp))
	{
		
		return recv_flag = 5;
	}
	memset(temp,0,sizeof(temp));
	bsp_DelayMS(500);
	
	MQTT_4G_CONNECT = 1;

	
	return recv_flag;

}



#include "bsp.h"
#include "spi.h"




char ClientID[128]; // ��ſͻ���ID�Ļ�����
int ClientID_len;	// ��ſͻ���ID�ĳ���

char Username[128]; // ����û����Ļ�����
int Username_len;	// ����û����ĳ���

char Passward[128]; // �������Ļ�����
int Passward_len;	// �������ĳ���

unsigned char ServerIP[4];	   // ��ŷ�����IP
unsigned char ServerName[128]; // ��ŷ���������
int ServerPort;				   // ��ŷ������Ķ˿ں�


wiz_NetInfo gWIZNETINFO =				 // MAC��ַ�Լ�дһ������Ҫ��·�������������豸һ������
	{
		0x00,
		0x08,
		0xdc,
		0x00,
		0xab,
		0xcd,
};
	

int Fixed_len;						 // �̶���ͷ����
int Variable_len;					 // �ɱ䱨ͷ����
int Payload_len;					 // ��Ч���ɳ���
uint8_t ping_open = 0;
uint8_t count = 0;



#define PRODUCTID "zjt"                              // ��ƷID
#define PRODUCTID_LEN strlen(PRODUCTID)                     // ��ƷID����
#define DEVICENAME "zjt"                                   // �豸��
#define DEVICENAME_LEN strlen(DEVICENAME)                   // �豸������
#define DEVICESECRE "zjt"              					// �豸��Կ
#define DEVICESECRE_LEN strlen(DEVICESECRE)                 // �豸��Կ����








char Connect_flag = 0;		// ͬ����������״̬  0����û�����ӷ�����  1�������Ϸ�������
char SubcribePack_flag = 0; // ���ı���״̬      1�����ı��ĳɹ�
char my_dhcp_retry = 0;					 // DHCP��ǰ�������ԵĴ���





unsigned char temp_buff[RBUFF_UNIT]; // ��ʱ������������������
unsigned char gDATABUF[DATA_BUF_SIZE];	 // ���ݻ�����


unsigned char MQTT_RxDataBuf[R_NUM][RBUFF_UNIT]; // ���ݵĽ��ջ�����,���з��������������ݣ�����ڸû�����,��������һ���ֽڴ�����ݳ���
unsigned char *MQTT_RxDataInPtr;				 // ָ����ջ�����������ݵ�λ��
unsigned char *MQTT_RxDataOutPtr;				 // ָ����ջ�������ȡ���ݵ�λ��
unsigned char *MQTT_RxDataEndPtr;				 // ָ����ջ�����������λ��


unsigned char MQTT_CMDBuf[C_NUM][CBUFF_UNIT]; // �������ݵĽ��ջ�����
unsigned char *MQTT_CMDInPtr;				  // ָ���������������ݵ�λ��
unsigned char *MQTT_CMDOutPtr;				  // ָ�����������ȡ���ݵ�λ��
unsigned char *MQTT_CMDEndPtr;				  // ָ���������������λ��


void MQTT_Buff_Init(void)
{
	MQTT_RxDataInPtr = MQTT_RxDataBuf[0];		   // ָ���ͻ�����������ݵ�ָ���λ
	MQTT_RxDataOutPtr = MQTT_RxDataInPtr;		   // ָ���ͻ�������ȡ���ݵ�ָ���λ
	MQTT_RxDataEndPtr = MQTT_RxDataBuf[R_NUM - 1]; // ָ���ͻ�����������ָ���λ

	MQTT_CMDInPtr = MQTT_CMDBuf[0];			 // ָ���������������ݵ�ָ���λ
	MQTT_CMDOutPtr = MQTT_CMDInPtr;			 // ָ�����������ȡ���ݵ�ָ���λ
	MQTT_CMDEndPtr = MQTT_CMDBuf[C_NUM - 1]; // ָ���������������ָ���λ

	
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
		printf("\r\n=== %s NET CONF : DHCP ===\r\n", (char *)tmpstr);
	else
		printf("\r\n=== %s NET CONF : Static ===\r\n", (char *)tmpstr);
	printf("===========================\r\n");
	printf("MAC��ַ: %02X:%02X:%02X:%02X:%02X:%02X\r\n", netinfo.mac[0], netinfo.mac[1], netinfo.mac[2], netinfo.mac[3], netinfo.mac[4], netinfo.mac[5]);
	printf("IP��ַ: %d.%d.%d.%d\r\n", netinfo.ip[0], netinfo.ip[1], netinfo.ip[2], netinfo.ip[3]);
	printf("���ص�ַ: %d.%d.%d.%d\r\n", netinfo.gw[0], netinfo.gw[1], netinfo.gw[2], netinfo.gw[3]);
	printf("��������: %d.%d.%d.%d\r\n", netinfo.sn[0], netinfo.sn[1], netinfo.sn[2], netinfo.sn[3]);
	printf("DNS������: %d.%d.%d.%d\r\n", netinfo.dns[0], netinfo.dns[1], netinfo.dns[2], netinfo.dns[3]);
	printf("===========================\r\n");
}

void my_ip_assign(void)
{
	getIPfromDHCP(gWIZNETINFO.ip);	 // �ѻ�ȡ����ip��������¼����������
	getGWfromDHCP(gWIZNETINFO.gw);	 // �ѻ�ȡ�������ز�������¼����������
	getSNfromDHCP(gWIZNETINFO.sn);	 // �ѻ�ȡ�������������������¼����������
	getDNSfromDHCP(gWIZNETINFO.dns); // �ѻ�ȡ����DNS��������������¼����������
	gWIZNETINFO.dhcp = NETINFO_DHCP; // ���ʹ�õ���DHCP��ʽ
	network_init();					 // ��ʼ������
	printf("DHCP���� : %d ��\r\n", getDHCPLeasetime());
}

void my_ip_conflict(void)
{
	printf("��ȡIPʧ�ܣ�׼������\r\n"); // ��ʾ��ȡIPʧ��
	NVIC_SystemReset();					   // ����
}



void MQTT_PingREQ(void)
{
	temp_buff[0] = 0xC0; // ��1���ֽ� ���̶�0xC0
	temp_buff[1] = 0x00; // ��2���ֽ� ���̶�0x00

	send(SOCK_TCPC,temp_buff, 2); // �������ݵ�������
}


void AliIoT_Parameter_Init(void)
{
	char temp[128]; // ������ܵ�ʱ����ʱʹ�õĻ�����

	memset(ClientID, 0, 128);						  // �ͻ���ID�Ļ�����ȫ������
	sprintf(ClientID, "%s&%s", PRODUCTID, DEVICENAME); // �����ͻ���ID�������뻺����
	ClientID_len = strlen(ClientID);				  // ����ͻ���ID�ĳ���

	memset(Username, 0, 128);													// �û����Ļ�����ȫ������
	sprintf(Username, "%s&%s", PRODUCTID, DEVICENAME); // �����û����������뻺����
	Username_len = strlen(Username);											// �����û����ĳ���

	memset(temp, 0, 128);													   // �����ʱ������
	base64_decode(DEVICESECRE, (unsigned char *)temp);						   // ��DEVICESECRE����base64����
	utils_hmac_sha1(Username, strlen(Username), Passward, temp, strlen(temp)); // ����һ���Ľ��Ϊ��Կ��Username�е����ģ�����hmacsha1���ܣ�����������룬�����浽��������
	strcat(Passward, ";hmacsha1");											   // ĩβ׷���ַ���
	Passward_len = strlen(Passward);										   // �����û����ĳ���

	memset(ServerIP, 0, 4);
	ServerIP[0] = 47;
	ServerIP[1] = 108;
	ServerIP[2] = 144;
	ServerIP[3] = 41;
	ServerPort = 1883;											

	printf("�� �� ����%d.%d.%d.%d:%d\r\n",ServerIP[0],ServerIP[1],ServerIP[1],ServerIP[1], ServerPort); // �������������Ϣ
	printf("�ͻ���ID��%s\r\n", ClientID);				  // �������������Ϣ
	printf("�� �� ����%s\r\n", Username);				  // �������������Ϣ
	printf("��    �룺%s\r\n", Passward);				  // �������������Ϣ
}


void W5500_init(void)
{
	// W5500�շ��ڴ�������շ������������ܵĿռ���16K����0-7��ÿ���˿ڵ��շ����������Ƿ��� 2K
	char memsize[2][8] = {{2, 2, 2, 2, 2, 2, 2, 2}, {2, 2, 2, 2, 2, 2, 2, 2}};
	char tmp;

	reg_wizchip_cris_cbfunc(SPI_CrisEnter, SPI_CrisExit);  // ע���ٽ�������
	reg_wizchip_cs_cbfunc(SPI_CS_Select, SPI_CS_Deselect); // ע��SPIƬѡ�źź���
	reg_wizchip_spi_cbfunc(SPI_ReadByte, SPI_WriteByte);   // ע���д����
	if (ctlwizchip(CW_INIT_WIZCHIP, (void *)memsize) == -1)
	{												  // ���if��������ʾ�շ��ڴ����ʧ��
		printf("��ʼ���շ�����ʧ��,׼������\r\n"); // ��ʾ��Ϣ
		NVIC_SystemReset();							  // ����
	}
	do
	{ // �������״̬
		if (ctlwizchip(CW_GET_PHYLINK, (void *)&tmp) == -1)
		{										 // ���if��������ʾδ֪����
			printf("δ֪����׼������\r\n"); // ��ʾ��Ϣ
			NVIC_SystemReset();					 // ����
		}
		if (tmp == PHY_LINK_OFF)
		{
			printf("����δ����\r\n"); // �����⵽������û���ӣ���ʾ��������
			bsp_DelayMS(2000);				 // ��ʱ2s
		}
	} while (tmp == PHY_LINK_OFF); // ѭ��ִ�У�ֱ������������

	setSHAR(gWIZNETINFO.mac);									 // ����MAC��ַ
	DHCP_init(SOCK_DHCP, gDATABUF);								 // ��ʼ��DHCP
	reg_dhcp_cbfunc(my_ip_assign, my_ip_assign, my_ip_conflict); // ע��DHCP�ص�����
}

uint8_t local_ip[4]  ={192,168,2,88};											/*����W5500Ĭ��IP��ַ*/
uint16_t local_port=5000;	                       					/*���屾�ض˿�*/

uint8_t  remote_ip[4]={47,108,144,41};											/*Զ��IP��ַ������������IP*/
uint16_t remote_port=1883;	


uint8_t do_tcp_client(void)
{	
//   uint16_t len=0,i=0;	
  uint16_t ret=0;
  ret=getSn_SR(SOCK_TCPC);//��ȡר����TCP��Socket��״ֵ̬
  printf(" getSn_SR = 0x%X\r\n",ret);
//  printf(" socket״̬ = %s\r\n",Sn_SR_values[ret]);
  switch(ret)								  				         /*�ж�socket��״̬*/
  {
  case SOCK_CLOSED:											        		         /*���socket���ڹر�״̬=0x00*/
    printf(" socket��״̬ = SOCK_CLOSED\r\n");
    socket(SOCK_TCPC,Sn_MR_TCP,local_port,Sn_MR_ND);       //�Ƚ��� ר����TCP��Socket ��ʼ��
    break;
    
  case SOCK_INIT:												/*���socket���ڳ�ʼ��״̬=0x13*/
    printf(" socket��״̬ = SOCK_INIT\r\n");
    getSIPR (local_ip);			
    printf(" W5500 IP��ַ: %d.%d.%d.%d\r\n", local_ip[0], local_ip[1], local_ip[2], local_ip[3]);
    printf(" ������IP��ַ: %d.%d.%d.%d��%d\r\n",remote_ip[0],remote_ip[1],remote_ip[2],remote_ip[3],remote_port); 
    connect(SOCK_TCPC,remote_ip,remote_port);                /*socket���ӷ�����*/ 
    break;
    
  case SOCK_ESTABLISHED: 								/*���socket�������ӽ���״̬=0x17�� ���3��������ɺ󣬲Ż���뽨������״̬*/
    printf(" socket��״̬ = SOCK_ESTABLISHED\r\n"); 
    return 1;
    
  case SOCK_CLOSE_WAIT: 								 /*���socket���ڵȴ��ر�״̬=SOCK_CLOSE_WAIT*/
    printf(" socket��״̬ = SOCK_CLOSE_WAIT\r\n");
    close(SOCK_TCPC); 							   	/**@Q��socket���ڵȴ��ر�״̬�����ǹر�״̬*/
    break;
    
  }
  return 0;
}


void MQTT_ConectPack(void)
{
	int temp, Remaining_len;

	Fixed_len = 1;														  // ���ӱ����У��̶���ͷ������ʱ��=1
	Variable_len = 10;													  // ���ӱ����У��ɱ䱨ͷ����=10
	Payload_len = 2 + ClientID_len + 2 + Username_len + 2 + Passward_len; // ���ӱ����У����س���
	Remaining_len = Variable_len + Payload_len;							  // ʣ�೤��=�ɱ䱨ͷ����+���س���

	temp_buff[0] = 0x10; // �̶���ͷ��1���ֽ� ���̶�0x01
	do
	{										 // ѭ������̶���ͷ�е�ʣ�೤���ֽڣ��ֽ�������ʣ���ֽڵ���ʵ���ȱ仯
		temp = Remaining_len % 128;			 // ʣ�೤��ȡ��128
		Remaining_len = Remaining_len / 128; // ʣ�೤��ȡ��128
		if (Remaining_len > 0)
			temp |= 0x80;			 // ��Э��Ҫ��λ7��λ
		temp_buff[Fixed_len] = temp; // ʣ�೤���ֽڼ�¼һ������
		Fixed_len++;				 // �̶���ͷ�ܳ���+1
	} while (Remaining_len > 0);	 // ���Remaining_len>0�Ļ����ٴν���ѭ��

	temp_buff[Fixed_len + 0] = 0x00; // �ɱ䱨ͷ��1���ֽ� ���̶�0x00
	temp_buff[Fixed_len + 1] = 0x04; // �ɱ䱨ͷ��2���ֽ� ���̶�0x04
	temp_buff[Fixed_len + 2] = 0x4D; // �ɱ䱨ͷ��3���ֽ� ���̶�0x4D
	temp_buff[Fixed_len + 3] = 0x51; // �ɱ䱨ͷ��4���ֽ� ���̶�0x51
	temp_buff[Fixed_len + 4] = 0x54; // �ɱ䱨ͷ��5���ֽ� ���̶�0x54
	temp_buff[Fixed_len + 5] = 0x54; // �ɱ䱨ͷ��6���ֽ� ���̶�0x54
	temp_buff[Fixed_len + 6] = 0x04; // �ɱ䱨ͷ��7���ֽ� ���̶�0x04
	temp_buff[Fixed_len + 7] = 0xC2; // �ɱ䱨ͷ��8���ֽ� ��ʹ���û���������У�飬��ʹ���������������Ự
	temp_buff[Fixed_len + 8] = 0x00; // �ɱ䱨ͷ��9���ֽ� ������ʱ����ֽ� 0x00
	temp_buff[Fixed_len + 9] = 0x64; // �ɱ䱨ͷ��10���ֽڣ�����ʱ����ֽ� 0x64   100s

	/*     CLIENT_ID      */
	temp_buff[Fixed_len + 10] = ClientID_len / 256;				// �ͻ���ID���ȸ��ֽ�
	temp_buff[Fixed_len + 11] = ClientID_len % 256;				// �ͻ���ID���ȵ��ֽ�
	memcpy(&temp_buff[Fixed_len + 12], ClientID, ClientID_len); // ���ƹ����ͻ���ID�ִ�
	/*     �û���        */
	temp_buff[Fixed_len + 12 + ClientID_len] = Username_len / 256;			   // �û������ȸ��ֽ�
	temp_buff[Fixed_len + 13 + ClientID_len] = Username_len % 256;			   // �û������ȵ��ֽ�
	memcpy(&temp_buff[Fixed_len + 14 + ClientID_len], Username, Username_len); // ���ƹ����û����ִ�
	/*      ����        */
	temp_buff[Fixed_len + 14 + ClientID_len + Username_len] = Passward_len / 256;			  // ���볤�ȸ��ֽ�
	temp_buff[Fixed_len + 15 + ClientID_len + Username_len] = Passward_len % 256;			  // ���볤�ȵ��ֽ�
	memcpy(&temp_buff[Fixed_len + 16 + ClientID_len + Username_len], Passward, Passward_len); // ���ƹ��������ִ�

	send(SOCK_TCPC,temp_buff, Fixed_len + Variable_len + Payload_len); // ���뷢�����ݻ�����
}

uint8_t readCONNACK(void)
{
	uint8_t buff[0xff],i=0;	
	uint8_t ret=0xff;
	uint16_t len=0;

	len=getSn_RX_RSR(SOCK_TCPC); 		/*����lenΪ�ѽ������ݵĳ���*/  

	if(len>0)
	{
	memset(buff,0,sizeof(buff));				// ���ڽ��ջ�����0
	recv(SOCK_TCPC,buff,len); 				/*��������Server������*/
	//buff[len]=0x00;  					/*����ַ���������*/
	for(i=0;i<len;i++)
	  printf("%02X-",buff[i]);
	printf("\r\n");
	ret=buff[3];//����ȷ����
	}	
	return ret;
}

void MQTT_Subscribe(char *topic_name, int QoS)
{
	Fixed_len = 2;							  // SUBSCRIBE�����У��̶���ͷ����=2
	Variable_len = 2;						  // SUBSCRIBE�����У��ɱ䱨ͷ����=2
	Payload_len = 2 + strlen(topic_name) + 1; // ������Ч���ɳ��� = 2�ֽ�(topic_name����)+ topic_name�ַ����ĳ��� + 1�ֽڷ���ȼ�

	temp_buff[0] = 0x82;								   // ��1���ֽ� ���̶�0x82
	temp_buff[1] = Variable_len + Payload_len;			   // ��2���ֽ� ���ɱ䱨ͷ+��Ч���ɵĳ���
	temp_buff[2] = 0x00;								   // ��3���ֽ� �����ı�ʶ�����ֽڣ��̶�ʹ��0x00
	temp_buff[3] = 0x01;								   // ��4���ֽ� �����ı�ʶ�����ֽڣ��̶�ʹ��0x01
	temp_buff[4] = strlen(topic_name) / 256;			   // ��5���ֽ� ��topic_name���ȸ��ֽ�
	temp_buff[5] = strlen(topic_name) % 256;			   // ��6���ֽ� ��topic_name���ȵ��ֽ�
	memcpy(&temp_buff[6], topic_name, strlen(topic_name)); // ��7���ֽڿ�ʼ �����ƹ���topic_name�ִ�
	temp_buff[6 + strlen(topic_name)] = QoS;			   // ���1���ֽڣ����ĵȼ�

	send(SOCK_TCPC,temp_buff, Fixed_len + Variable_len + Payload_len); // ���뷢�����ݻ�����
}

void CMDBuf_Deal(unsigned char *data, int size)
{
	memcpy(&MQTT_CMDInPtr[2], data, size); // �������ݵ��������
	MQTT_CMDInPtr[0] = size / 256;		   // ��¼���ݳ���
	MQTT_CMDInPtr[1] = size % 256;		   // ��¼���ݳ���
	MQTT_CMDInPtr[size + 2] = '\0';		   // �����ַ���������
	MQTT_CMDInPtr += CBUFF_UNIT;		   // ָ������
	if (MQTT_CMDInPtr == MQTT_CMDEndPtr)   // ���ָ�뵽������β����
		MQTT_CMDInPtr = MQTT_CMDBuf[0];	   // ָ���λ����������ͷ
}

void MQTT_DealPushdata_Qs0(unsigned char *redata)
{
	int re_len;			// ����һ����������Ž��յ������ܳ���
	int pack_num;		// ����һ�����������������һ�����ʱ���������͵ĸ���
	int temp, temp_len; // ����һ���������ݴ�����
	int totle_len;		// ����һ������������Ѿ�ͳ�Ƶ����͵���������
	int topic_len;		// ����һ���������������������ĳ���
	int cmd_len;		// ����һ����������������а������������ݵĳ���
	int cmd_loca;		// ����һ����������������а������������ʼλ��
	int i;				// ����һ������������forѭ��
	int local, multiplier;
	unsigned char tempbuff[RBUFF_UNIT]; // ��ʱ������
	unsigned char *data;				// redata������ʱ�򣬵�һ���ֽ�������������data����ָ��redata�ĵ�2���ֽڣ����������ݿ�ʼ�ĵط�

	re_len = redata[0] * 256 + redata[1];		// ��ȡ���յ������ܳ���
	data = &redata[2];							// dataָ��redata�ĵ�2���ֽڣ����������ݿ�ʼ��
	pack_num = temp_len = totle_len = temp = 0; // ������������
	local = 1;
	multiplier = 1;
	do
	{
		pack_num++; // ��ʼѭ��ͳ�����͵ĸ�����ÿ��ѭ�����͵ĸ���+1
		do
		{
			temp = data[totle_len + local];
			temp_len += (temp & 127) * multiplier;
			multiplier *= 128;
			local++;
		} while ((temp & 128) != 0);
		totle_len += (temp_len + local); // �ۼ�ͳ�Ƶ��ܵ����͵����ݳ���
		re_len -= (temp_len + local);	 // ���յ������ܳ��� ��ȥ ����ͳ�Ƶ����͵��ܳ���
		local = 1;
		multiplier = 1;
		temp_len = 0;
	} while (re_len != 0);							   // ������յ������ܳ��ȵ���0�ˣ�˵��ͳ�������
	printf("���ν�����%d����������\r\n", pack_num); // ���������Ϣ
	temp_len = totle_len = 0;						   // ������������
	local = 1;
	multiplier = 1;
	for (i = 0; i < pack_num; i++)
	{ // �Ѿ�ͳ�Ƶ��˽��յ����͸�������ʼforѭ����ȡ��ÿ�����͵�����
		do
		{
			temp = data[totle_len + local];
			temp_len += (temp & 127) * multiplier;
			multiplier *= 128;
			local++;
		} while ((temp & 128) != 0);
		topic_len = data[local + totle_len] * 256 + data[local + 1 + totle_len] + 2; // ���㱾����������������ռ�õ�������
		cmd_len = temp_len - topic_len;												 // ���㱾��������������������ռ�õ�������
		cmd_loca = totle_len + local + topic_len;									 // ���㱾�������������������ݿ�ʼ��λ��
		memcpy(tempbuff, &data[cmd_loca], cmd_len);									 // �������ݿ�������
		CMDBuf_Deal(tempbuff, cmd_len);												 // �������������
		totle_len += (temp_len + local);											 // �ۼ��Ѿ�ͳ�Ƶ����͵����ݳ���
		local = 1;
		multiplier = 1;
		temp_len = 0;
	}
}
void MQTT_PublishQs0(char *topic, char *data, int data_len)
{
	int temp, Remaining_len;

	Fixed_len = 1;								// �̶���ͷ������ʱ�ȵ��ڣ�1�ֽ�
	Variable_len = 2 + strlen(topic);			// �ɱ䱨ͷ���ȣ�2�ֽ�(topic����)+ topic�ַ����ĳ���
	Payload_len = data_len;						// ��Ч���ɳ��ȣ�����data_len
	Remaining_len = Variable_len + Payload_len; // ʣ�೤��=�ɱ䱨ͷ����+���س���

	temp_buff[0] = 0x30; // �̶���ͷ��1���ֽ� ���̶�0x30
	do
	{										 // ѭ������̶���ͷ�е�ʣ�೤���ֽڣ��ֽ�������ʣ���ֽڵ���ʵ���ȱ仯
		temp = Remaining_len % 128;			 // ʣ�೤��ȡ��128
		Remaining_len = Remaining_len / 128; // ʣ�೤��ȡ��128
		if (Remaining_len > 0)
			temp |= 0x80;			 // ��Э��Ҫ��λ7��λ
		temp_buff[Fixed_len] = temp; // ʣ�೤���ֽڼ�¼һ������
		Fixed_len++;				 // �̶���ͷ�ܳ���+1
	} while (Remaining_len > 0);	 // ���Remaining_len>0�Ļ����ٴν���ѭ��

	temp_buff[Fixed_len + 0] = strlen(topic) / 256;					   // �ɱ䱨ͷ��1���ֽ�     ��topic���ȸ��ֽ�
	temp_buff[Fixed_len + 1] = strlen(topic) % 256;					   // �ɱ䱨ͷ��2���ֽ�     ��topic���ȵ��ֽ�
	memcpy(&temp_buff[Fixed_len + 2], topic, strlen(topic));		   // �ɱ䱨ͷ��3���ֽڿ�ʼ ������topic�ַ���
	memcpy(&temp_buff[Fixed_len + 2 + strlen(topic)], data, data_len); // ��Ч���ɣ�����data����

	send(SOCK_TCPC,temp_buff, Fixed_len + Variable_len + Payload_len); // ���뷢�����ݻ�����
}

void LED_PubState(unsigned char sta)
{
	char temp[RBUFF_UNIT];
	sprintf(temp, "{\"method\":\"report\",\"clientToke\":\"123\",\"params\":{\"power_switch\":%d}}", sta);
	MQTT_PublishQs0(P_TOPIC_NAME, temp, strlen(temp));
}



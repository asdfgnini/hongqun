
#define  __MQTT_GLOAL     //__MQTT_GLOAL�궨��
#include "stm32f10x.h"    //������Ҫ��ͷ�ļ�
#include "main.h"         //������Ҫ��ͷ�ļ�
#include "mqtt.h"         //������Ҫ��ͷ�ļ�
#include "string.h"       //������Ҫ��ͷ�ļ�
#include "stdio.h"        //������Ҫ��ͷ�ļ�
#include "usart1.h"       //������Ҫ��ͷ�ļ�
#include "utils_hmac.h"   //������Ҫ��ͷ�ļ�
#include "cat1.h"         //������Ҫ��ͷ�ļ�

unsigned char  MQTT_RxDataBuf[R_NUM][RBUFF_UNIT];           //���ݵĽ��ջ�����,���з��������������ݣ�����ڸû�����,��������һ���ֽڴ�����ݳ���
unsigned char *MQTT_RxDataInPtr;                            //ָ����ջ�����������ݵ�λ��
unsigned char *MQTT_RxDataOutPtr;                           //ָ����ջ�������ȡ���ݵ�λ��
unsigned char *MQTT_RxDataEndPtr;                           //ָ����ջ�����������λ��

unsigned char  MQTT_TxDataBuf[T_NUM][TBUFF_UNIT];           //���ݵķ��ͻ�����,���з��������������ݣ�����ڸû�����,��������һ���ֽڴ�����ݳ���
unsigned char *MQTT_TxDataInPtr;                            //ָ���ͻ�����������ݵ�λ��
unsigned char *MQTT_TxDataOutPtr;                           //ָ���ͻ�������ȡ���ݵ�λ��
unsigned char *MQTT_TxDataEndPtr;                           //ָ���ͻ�����������λ��

unsigned char  MQTT_CMDBuf[C_NUM][CBUFF_UNIT];              //�������ݵĽ��ջ�����
unsigned char *MQTT_CMDInPtr;                               //ָ���������������ݵ�λ��
unsigned char *MQTT_CMDOutPtr;                              //ָ�����������ȡ���ݵ�λ��
unsigned char *MQTT_CMDEndPtr;                              //ָ���������������λ��

char ClientID[128];                                          //��ſͻ���ID�Ļ�����
int  ClientID_len;                                           //��ſͻ���ID�ĳ���

char Username[128];                                          //����û����Ļ�����
int  Username_len;											 //����û����ĳ���

char Passward[128];                                          //�������Ļ�����
int  Passward_len;											 //�������ĳ���

char ServerIP[128];                                          //��ŷ�����IP��������
int  ServerPort;                                             //��ŷ������Ķ˿ں�

int   Fixed_len;                       					     //�̶���ͷ����
int   Variable_len;                     					 //�ɱ䱨ͷ����
int   Payload_len;                       					 //��Ч���ɳ���
unsigned char  temp_buff[TBUFF_UNIT];						 //��ʱ������������������

char Stopic_Buff[S_TOPIC_NUM][128]=                                         //�������Ƕ��ĵ������б�
{
	"app_topic"                                         //��Ҫ���ĵ�����  
};
/*----------------------------------------------------------*/
/*����������ʼ������,����,�������ݵ� ������ �Լ���״̬����  */
/*��  ������                                                */
/*����ֵ����                                                */
/*----------------------------------------------------------*/
void MQTT_Buff_Init(void)
{	
	MQTT_RxDataInPtr=MQTT_RxDataBuf[0];               //ָ���ͻ�����������ݵ�ָ���λ
	MQTT_RxDataOutPtr=MQTT_RxDataInPtr;               //ָ���ͻ�������ȡ���ݵ�ָ���λ
    MQTT_RxDataEndPtr=MQTT_RxDataBuf[R_NUM-1];        //ָ���ͻ�����������ָ���λ
	
	MQTT_TxDataInPtr=MQTT_TxDataBuf[0];               //ָ���ͻ�����������ݵ�ָ���λ
	MQTT_TxDataOutPtr=MQTT_TxDataInPtr;               //ָ���ͻ�������ȡ���ݵ�ָ���λ
	MQTT_TxDataEndPtr=MQTT_TxDataBuf[T_NUM-1];        //ָ���ͻ�����������ָ���λ
	
	MQTT_CMDInPtr=MQTT_CMDBuf[0];                     //ָ���������������ݵ�ָ���λ
	MQTT_CMDOutPtr=MQTT_CMDInPtr;                     //ָ�����������ȡ���ݵ�ָ���λ
	MQTT_CMDEndPtr=MQTT_CMDBuf[C_NUM-1];              //ָ���������������ָ���λ

    MQTT_ConectPack();                                //���ͻ�����������ӱ���	
	MQTT_SubscribeQs0();	                          //���ͻ�������Ӷ���topic���ȼ�0	
}
/*----------------------------------------------------------*/
/*���������Ʒ�������ʼ���������õ��ͻ���ID���û���������    */
/*��  ������                                                */
/*����ֵ����                                                */
/*----------------------------------------------------------*/
void IoT_Parameter_Init(void)
{	
	char temp[128];                                                       //������ܵ�ʱ����ʱʹ�õĻ�����

	memset(ClientID,0,128);  //�ͻ���ID�Ļ�����ȫ������
    CreateSysMAC((uint8_t*)temp);    
	sprintf(ClientID,"%x",(unsigned int)temp);  //�����ͻ���ID�������뻺����
	ClientID_len = strlen(ClientID);                                      //����ͻ���ID�ĳ���
	
	memset(Username,0,128);                                               //�û����Ļ�����ȫ������
	sprintf(Username,"%x",(unsigned int)temp);                      //�����û����������뻺����
	Username_len = strlen(Username);                                      //�����û����ĳ���
	
	memset(temp,0,128);                                                                      //��ʱ������ȫ������
	sprintf(temp,"clientId%sdeviceName%sproductKey%s",DEVICENAME,DEVICENAME,PRODUCTKEY);     //��������ʱ������   
	utils_hmac_sha1(temp,strlen(temp),Passward,DEVICESECRE,DEVICESECRE_LEN);                 //��DeviceSecretΪ��Կ��temp�е����ģ�����hmacsha1���ܣ�����������룬�����浽��������
	Passward_len = strlen(Passward);                                                         //�����û����ĳ���
	
	memset(ServerIP,0,128);  
	sprintf(ServerIP,"%s","47.108.144.41");                  //��������������
	ServerPort = 1883;                                                                       //�������˿ں�1883
	
	u1_printf("�� �� ����%s:%d\r\n",ServerIP,ServerPort); //�������������Ϣ
	u1_printf("�ͻ���ID��%s\r\n",ClientID);               //�������������Ϣ
	u1_printf("�� �� ����%s\r\n",Username);               //�������������Ϣ
	u1_printf("��    �룺%s\r\n",Passward);               //�������������Ϣ
}

/*----------------------------------------------------------*/
/*�����������ӷ���������                                    */
/*��  ������                                                */
/*����ֵ����                                                */
/*----------------------------------------------------------*/
void MQTT_ConectPack(void)
{	
	int temp,Remaining_len;
	
	Fixed_len = 1;                                                        //���ӱ����У��̶���ͷ������ʱ��=1
	Variable_len = 10;                                                    //���ӱ����У��ɱ䱨ͷ����=10
	Payload_len = 2 + ClientID_len + 2 + Username_len + 2 + Passward_len; //���ӱ����У����س���      
	Remaining_len = Variable_len + Payload_len;                           //ʣ�೤��=�ɱ䱨ͷ����+���س���
	
	temp_buff[0]=0x10;                       //�̶���ͷ��1���ֽ� ���̶�0x01		
	do{                                      //ѭ������̶���ͷ�е�ʣ�೤���ֽڣ��ֽ�������ʣ���ֽڵ���ʵ���ȱ仯
		temp = Remaining_len%128;            //ʣ�೤��ȡ��128
		Remaining_len = Remaining_len/128;   //ʣ�೤��ȡ��128
		if(Remaining_len>0)               	
			temp |= 0x80;                    //��Э��Ҫ��λ7��λ          
		temp_buff[Fixed_len] = temp;         //ʣ�೤���ֽڼ�¼һ������
		Fixed_len++;	                     //�̶���ͷ�ܳ���+1    
	}while(Remaining_len>0);                 //���Remaining_len>0�Ļ����ٴν���ѭ��
	
	temp_buff[Fixed_len+0]=0x00;    //�ɱ䱨ͷ��1���ֽ� ���̶�0x00	            
	temp_buff[Fixed_len+1]=0x04;    //�ɱ䱨ͷ��2���ֽ� ���̶�0x04
	temp_buff[Fixed_len+2]=0x4D;	//�ɱ䱨ͷ��3���ֽ� ���̶�0x4D
	temp_buff[Fixed_len+3]=0x51;	//�ɱ䱨ͷ��4���ֽ� ���̶�0x51
	temp_buff[Fixed_len+4]=0x54;	//�ɱ䱨ͷ��5���ֽ� ���̶�0x54
	temp_buff[Fixed_len+5]=0x54;	//�ɱ䱨ͷ��6���ֽ� ���̶�0x54
	temp_buff[Fixed_len+6]=0x04;	//�ɱ䱨ͷ��7���ֽ� ���̶�0x04
	temp_buff[Fixed_len+7]=0xC2;	//�ɱ䱨ͷ��8���ֽ� ��ʹ���û���������У�飬��ʹ���������������Ự
	temp_buff[Fixed_len+8]=0x00; 	//�ɱ䱨ͷ��9���ֽ� ������ʱ����ֽ� 0x00
	temp_buff[Fixed_len+9]=0x64;	//�ɱ䱨ͷ��10���ֽڣ�����ʱ����ֽ� 0x64   100s
	
	/*     CLIENT_ID      */
	temp_buff[Fixed_len+10] = ClientID_len/256;                			  			    //�ͻ���ID���ȸ��ֽ�
	temp_buff[Fixed_len+11] = ClientID_len%256;               			  			    //�ͻ���ID���ȵ��ֽ�
	memcpy(&temp_buff[Fixed_len+12],ClientID,ClientID_len);                 			//���ƹ����ͻ���ID�ִ�	
	/*     �û���        */
	temp_buff[Fixed_len+12+ClientID_len] = Username_len/256; 				  		    //�û������ȸ��ֽ�
	temp_buff[Fixed_len+13+ClientID_len] = Username_len%256; 				 		    //�û������ȵ��ֽ�
	memcpy(&temp_buff[Fixed_len+14+ClientID_len],Username,Username_len);                //���ƹ����û����ִ�	
	/*      ����        */
	temp_buff[Fixed_len+14+ClientID_len+Username_len] = Passward_len/256;			    //���볤�ȸ��ֽ�
	temp_buff[Fixed_len+15+ClientID_len+Username_len] = Passward_len%256;			    //���볤�ȵ��ֽ�
	memcpy(&temp_buff[Fixed_len+16+ClientID_len+Username_len],Passward,Passward_len);   //���ƹ��������ִ�

	TxDataBuf_Deal(temp_buff, Fixed_len + Variable_len + Payload_len);                  //���뷢�����ݻ�����
}
/*----------------------------------------------------------*/
/*��������SUBSCRIBE����topic���� �ȼ�0                      */
/*��  ����topic_buff������topic���ĵĻ�����                 */
/*����ֵ����                                                */
/*----------------------------------------------------------*/
void MQTT_SubscribeQs0(void)
{	
	int temp,Remaining_len,i,len;
	
	Fixed_len = 1;                              //�̶���ͷ������ʱ��=1
	Variable_len = 2;                           //�ɱ䱨ͷ����=2
	Payload_len = 0;                            //�����Payload_len����
	for(i=0;i<S_TOPIC_NUM;i++)                  //ѭ��ͳ��topic�ַ�������
		Payload_len += strlen(Stopic_Buff[i]);  //ÿ���ۼ�
	Payload_len += 3*S_TOPIC_NUM;               //���س��Ȳ�������topic�ַ������ȣ����еȼ�0�ı�ʶ�ֽڣ�һ��topicһ�����Լ�2���ֽڵ��ַ������ȱ�ʶ�ֽڣ�һ��topic�����������ټ���3*S_TOPIC_NUM    
	Remaining_len = Variable_len + Payload_len; //ʣ�೤��=�ɱ䱨ͷ����+���س���

	temp_buff[0]=0x82;                          //�̶���ͷ��1���ֽ� ���̶�0x82		
	do{                                         //ѭ������̶���ͷ�е�ʣ�೤���ֽڣ��ֽ�������ʣ���ֽڵ���ʵ���ȱ仯
		temp = Remaining_len%128;               //ʣ�೤��ȡ��128
		Remaining_len = Remaining_len/128;      //ʣ�೤��ȡ��128
		if(Remaining_len>0)               	
			temp |= 0x80;                       //��Э��Ҫ��λ7��λ          
		temp_buff[Fixed_len] = temp;            //ʣ�೤���ֽڼ�¼һ������
		Fixed_len++;	                        //�̶���ͷ�ܳ���+1    
	}while(Remaining_len>0);                    //���Remaining_len>0�Ļ����ٴν���ѭ��
	
	temp_buff[Fixed_len+0]=0x00;                //���ı�ʶ�����ֽڣ��̶�ʹ��0x00
	temp_buff[Fixed_len+1]=0x01;		        //���ı�ʶ�����ֽڣ��̶�ʹ��0x01
	
	len = 0;
	for(i=0;i<S_TOPIC_NUM;i++){                                                     //ѭ�����Ƹ�������		
		temp_buff[Fixed_len+2+len]=strlen(Stopic_Buff[i])/256;                      //topic�ַ��� ���ȸ��ֽ� ��ʶ�ֽ�
		temp_buff[Fixed_len+3+len]=strlen(Stopic_Buff[i])%256;		                //topic�ַ��� ���ȵ��ֽ� ��ʶ�ֽ�
		memcpy(&temp_buff[Fixed_len+4+len],Stopic_Buff[i],strlen(Stopic_Buff[i]));  //����topic�ִ�		
		temp_buff[Fixed_len+4+strlen(Stopic_Buff[i])+len]=0;                        //���ĵȼ�0	
		len += strlen(Stopic_Buff[i]) + 3;                                          //len���ڱ���ѭ������ӵ������� ���� topic�ִ������� + 2���ֽڳ��ȱ�ʶ + 1���ֽڶ��ĵȼ�
	}
		
	TxDataBuf_Deal(temp_buff, Fixed_len + Variable_len + Payload_len);              //���뷢�����ݻ�����
}
/*----------------------------------------------------------*/
/*��������UNSUBSCRIBEȡ������topic����                      */
/*��  ����topic_name������topic��������                     */
/*����ֵ����                                                */
/*----------------------------------------------------------*/
void MQTT_UNSubscribe(char *topic_name)
{	
	Fixed_len = 2;                              //SUBSCRIBE�����У��̶���ͷ����=2
	Variable_len = 2;                           //SUBSCRIBE�����У��ɱ䱨ͷ����=2	
	Payload_len = 2 + strlen(topic_name);       //������Ч���ɳ��� = 2�ֽ�(topic_name����)+ topic_name�ַ����ĳ���
	
	temp_buff[0]=0xA0;                                    //��1���ֽ� ���̶�0xA0                   
	temp_buff[1]=Variable_len + Payload_len;              //��2���ֽ� ���ɱ䱨ͷ+��Ч���ɵĳ���	
	temp_buff[2]=0x00;                                    //��3���ֽ� �����ı�ʶ�����ֽڣ��̶�ʹ��0x00
	temp_buff[3]=0x01;		                              //��4���ֽ� �����ı�ʶ�����ֽڣ��̶�ʹ��0x01
	temp_buff[4]=strlen(topic_name)/256;                  //��5���ֽ� ��topic_name���ȸ��ֽ�
	temp_buff[5]=strlen(topic_name)%256;		          //��6���ֽ� ��topic_name���ȵ��ֽ�
	memcpy(&temp_buff[6],topic_name,strlen(topic_name));  //��7���ֽڿ�ʼ �����ƹ���topic_name�ִ�		
	
	TxDataBuf_Deal(temp_buff, Fixed_len + Variable_len + Payload_len);  //���뷢�����ݻ�����
}
/*----------------------------------------------------------*/
/*��������PING���ģ�������                                  */
/*��  ������                                                */
/*����ֵ����                                                */
/*----------------------------------------------------------*/
void MQTT_PingREQ(void)
{
	temp_buff[0]=0xC0;              //��1���ֽ� ���̶�0xC0                      
	temp_buff[1]=0x00;              //��2���ֽ� ���̶�0x00 

	TxDataBuf_Deal(temp_buff, 2);   //�������ݵ�������
}
/*----------------------------------------------------------*/
/*���������ȼ�0 ������Ϣ����                                */
/*��  ����topic_name��topic����                             */
/*��  ����data������                                        */
/*��  ����data_len�����ݳ���                                */
/*����ֵ����                                                */
/*----------------------------------------------------------*/
void MQTT_PublishQs0(char *topic, char *data, int data_len)
{	
	int temp,Remaining_len;
	
	Fixed_len = 1;                              //�̶���ͷ������ʱ�ȵ��ڣ�1�ֽ�
	Variable_len = 2 + strlen(topic);           //�ɱ䱨ͷ���ȣ�2�ֽ�(topic����)+ topic�ַ����ĳ���
	Payload_len = data_len;                     //��Ч���ɳ��ȣ�����data_len
	Remaining_len = Variable_len + Payload_len; //ʣ�೤��=�ɱ䱨ͷ����+���س���
	
	temp_buff[0]=0x30;                       //�̶���ͷ��1���ֽ� ���̶�0x30   	
	do{                                      //ѭ������̶���ͷ�е�ʣ�೤���ֽڣ��ֽ�������ʣ���ֽڵ���ʵ���ȱ仯
		temp = Remaining_len%128;            //ʣ�೤��ȡ��128
		Remaining_len = Remaining_len/128;   //ʣ�೤��ȡ��128
		if(Remaining_len>0)               	
			temp |= 0x80;                    //��Э��Ҫ��λ7��λ          
		temp_buff[Fixed_len] = temp;         //ʣ�೤���ֽڼ�¼һ������
		Fixed_len++;	                     //�̶���ͷ�ܳ���+1    
	}while(Remaining_len>0);                 //���Remaining_len>0�Ļ����ٴν���ѭ��
		             
	temp_buff[Fixed_len+0]=strlen(topic)/256;                      //�ɱ䱨ͷ��1���ֽ�     ��topic���ȸ��ֽ�
	temp_buff[Fixed_len+1]=strlen(topic)%256;		               //�ɱ䱨ͷ��2���ֽ�     ��topic���ȵ��ֽ�
	memcpy(&temp_buff[Fixed_len+2],topic,strlen(topic));           //�ɱ䱨ͷ��3���ֽڿ�ʼ ������topic�ַ���	
	memcpy(&temp_buff[Fixed_len+2+strlen(topic)],data,data_len);   //��Ч���ɣ�����data����
	
	TxDataBuf_Deal(temp_buff, Fixed_len + Variable_len + Payload_len);  //���뷢�����ݻ�����
}


/*----------------------------------------------------------*/
/*����������������������ĵȼ�0������                       */
/*��  ����redata�����յ�����                                */
/*����ֵ����                                                */
/*----------------------------------------------------------*/
extern unsigned int g_timeJiffies;
void MQTT_DealPushdata_Qs0(unsigned char *redata)
{
	int  re_len;               	           //����һ����������Ž��յ������ܳ���
	int  pack_num;                         //����һ�����������������һ�����ʱ���������͵ĸ���
    int  temp,temp_len;                    //����һ���������ݴ�����
    int  totle_len;                        //����һ������������Ѿ�ͳ�Ƶ����͵���������
	int  topic_len;              	       //����һ���������������������ĳ���
	int  cmd_len;                          //����һ����������������а������������ݵĳ���
	int  cmd_loca;                         //����һ����������������а������������ʼλ��
	int  i;                                //����һ������������forѭ��
	int  local,multiplier;
	unsigned char tempbuff[RBUFF_UNIT];	   //��ʱ������
	unsigned char *data;                   //redata������ʱ�򣬵�һ���ֽ�������������data����ָ��redata�ĵ�2���ֽڣ����������ݿ�ʼ�ĵط�
		
	re_len = redata[0]*256+redata[1];                               //��ȡ���յ������ܳ���		
	data = &redata[2];                                              //dataָ��redata�ĵ�2���ֽڣ����������ݿ�ʼ�� 
	pack_num = temp_len = totle_len = temp = 0;                	    //������������
	local = 1;
	multiplier = 1;
	g_timeJiffies = 0;
	do{
		pack_num++;                                     			//��ʼѭ��ͳ�����͵ĸ�����ÿ��ѭ�����͵ĸ���+1	
		do{
			temp = data[totle_len + local];   
			temp_len += (temp & 127) * multiplier;
			multiplier *= 128;
			local++;
			if (g_timeJiffies > 4) { //  || pack_num > R_NUM
				u1_printf("��������������ĵȼ�0������ʧ��0\r\n");  // Ī���Ĵ�������������ĵȼ�0������ ʧ��
				return;
			}
		}while ((temp & 128) != 0);
		totle_len += (temp_len + local);                          	//�ۼ�ͳ�Ƶ��ܵ����͵����ݳ���
		re_len -= (temp_len + local) ;                              //���յ������ܳ��� ��ȥ ����ͳ�Ƶ����͵��ܳ���      
		local = 1;
		multiplier = 1;
		temp_len = 0;
		if (g_timeJiffies > 4) { //  || pack_num > R_NUM
			u1_printf("��������������ĵȼ�0������ʧ��1\r\n");  // Ī���Ĵ�������������ĵȼ�0������ ʧ��
			return;
		}
	}while(re_len!=0);                                  			//������յ������ܳ��ȵ���0�ˣ�˵��ͳ�������
	u1_printf("-->�յ�%d����������������\r\n",pack_num);//���������Ϣ
	temp_len = totle_len = 0;                		            	//������������
	local = 1;
	multiplier = 1;
	g_timeJiffies = 0;
	for(i=0;i<pack_num;i++){                                        //�Ѿ�ͳ�Ƶ��˽��յ����͸�������ʼforѭ����ȡ��ÿ�����͵����� 		
		do{
			temp = data[totle_len + local];   
			temp_len += (temp & 127) * multiplier;
			multiplier *= 128;
			local++;
			if (g_timeJiffies > 4) { //  || pack_num > R_NUM
				u1_printf("��������������ĵȼ�0������ʧ��2\r\n");  // Ī���Ĵ�������������ĵȼ�0������ ʧ��
				return;
			}
		}while ((temp & 128) != 0);
		u1_printf("-->���%d������������\r\n",pack_num);//���������Ϣ
		topic_len = data[local+totle_len]*256+data[local+1+totle_len] + 2;    //���㱾����������������ռ�õ�������
		cmd_len = temp_len-topic_len;                               //���㱾��������������������ռ�õ�������
		cmd_loca = totle_len + local +  topic_len;                  //���㱾�������������������ݿ�ʼ��λ��
		memcpy(tempbuff,&data[cmd_loca],cmd_len);                   //�������ݿ�������		                 
		CMDBuf_Deal(tempbuff, cmd_len);                             //�������������
		totle_len += (temp_len+local);                              //�ۼ��Ѿ�ͳ�Ƶ����͵����ݳ���
		local = 1;
		multiplier = 1;
		temp_len = 0;
	}
}
/*----------------------------------------------------------*/
/*��������DISCONNECT���ģ��Ͽ�����                          */
/*��  ������                                                */
/*����ֵ����                                                */
/*----------------------------------------------------------*/
void MQTT_DISCONNECT(void)
{
	temp_buff[0]=0xE0;              //��1���ֽ� ���̶�0xE0                      
	temp_buff[1]=0x00;              //��2���ֽ� ���̶�0x00 

	TxDataBuf_Deal(temp_buff, 2);   //�������ݵ�������
}
/*----------------------------------------------------------*/
/*�������������ͻ�����                                    */
/*��  ����data������                                        */
/*��  ����size�����ݳ���                                    */
/*����ֵ����                                                */
/*----------------------------------------------------------*/
void TxDataBuf_Deal(unsigned char *data, int size)
{
	memcpy(&MQTT_TxDataInPtr[2],data,size);      //�������ݵ����ͻ�����	
	MQTT_TxDataInPtr[0] = size/256;              //��¼���ݳ���
	MQTT_TxDataInPtr[1] = size%256;              //��¼���ݳ���
	MQTT_TxDataInPtr+=TBUFF_UNIT;                //ָ������
	if(MQTT_TxDataInPtr==MQTT_TxDataEndPtr)      //���ָ�뵽������β����
		MQTT_TxDataInPtr = MQTT_TxDataBuf[0];    //ָ���λ����������ͷ
}
/*----------------------------------------------------------*/
/*�������������������                                    */
/*��  ����data������                                        */
/*��  ����size�����ݳ���                                    */
/*����ֵ����                                                */
/*----------------------------------------------------------*/
void CMDBuf_Deal(unsigned char *data, int size)
{
	memcpy(&MQTT_CMDInPtr[2],data,size);      //�������ݵ��������
	MQTT_CMDInPtr[0] = size/256;              //��¼���ݳ���
	MQTT_CMDInPtr[1] = size%256;              //��¼���ݳ���
	MQTT_CMDInPtr[size+2] = '\0';             //�����ַ���������
	MQTT_CMDInPtr+=CBUFF_UNIT;                //ָ������
	if(MQTT_CMDInPtr==MQTT_CMDEndPtr)         //���ָ�뵽������β����
		MQTT_CMDInPtr = MQTT_CMDBuf[0];       //ָ���λ����������ͷ
}

uint32_t Get_MCU_ID(uint8_t* pMcuID)
{
	uint32_t CpuID[3] = {0};
		
	//��ȡCPUΨһID
	#if 1//STM32F1ϵ��
	CpuID[0] =*(vu32*)(0x1ffff7e8); //��ȫ�֣�32λ����ȡ
	CpuID[1] =*(vu32*)(0x1ffff7ec);
	CpuID[2] =*(vu32*)(0x1ffff7f0);
//	printf("\r\nоƬ��ΨһIDΪ: %X-%X-%X\r\n",
//	            CpuID[0],CpuID[1],CpuID[2]);
//	printf("\r\nоƬflash������Ϊ: %dK \r\n", *(uint16_t *)(0X1FFF7a22));

	#endif
	
	#if 0//STM32F4ϵ��
	CpuID[0]=*(vu32*)(0x1fff7a10);
	CpuID[1]=*(vu32*)(0x1fff7a14);
	CpuID[2]=*(vu32*)(0x1fff7a18);
//	/* printf the chipid */
//	rt_kprintf("\r\nоƬ��ΨһIDΪ: %X-%X-%X\r\n",
//	            CpuID[0],CpuID[1],CpuID[2]);
//	rt_kprintf("\r\nоƬflash������Ϊ: %dK \r\n", *(rt_uint16_t *)(0X1FFF7a22));
	#endif
	
	//���ֽڣ�8λ����ȡ
	pMcuID[0] = (uint8_t)(CpuID[0] & 0x000000FF);
	pMcuID[1] = (uint8_t)((CpuID[0] & 0xFF00) >>8);
	pMcuID[2] = (uint8_t)((CpuID[0] & 0xFF0000) >>16);
	pMcuID[3] = (uint8_t)((CpuID[0] & 0xFF000000) >>24);
	
	pMcuID[4] = (uint8_t)(CpuID[1] & 0xFF);
	pMcuID[5] = (uint8_t)((CpuID[1] & 0xFF00) >>8);
	pMcuID[6] = (uint8_t)((CpuID[1] & 0xFF0000) >>16);
	pMcuID[7] = (uint8_t)((CpuID[1] & 0xFF000000) >>24);
	
	pMcuID[8] = (uint8_t)(CpuID[2] & 0xFF);
	pMcuID[9] = (uint8_t)((CpuID[2] & 0xFF00) >>8);
	pMcuID[10] = (uint8_t)((CpuID[2] & 0xFF0000) >>16);
	pMcuID[11] = (uint8_t)((CpuID[2] & 0xFF000000) >>24);

	return (CpuID[0]>>1)+(CpuID[1]>>2)+(CpuID[2]>>3);
}


/***********************************************************************************************
* Function Name  : CreateSysMAC
* Description	 : ��ȡMCU��MAC��ַ
* Input 		 : None
* Output		 : MAC
* Return		 : None
***********************************************************************************************/
void CreateSysMAC(uint8_t* pMacBuf)
{
	unsigned long uiMcuId = 0;
	unsigned char McuID[15] = {0};
	int i = 0;

	uiMcuId = Get_MCU_ID(McuID);

	for(i=0; i<12; i++) //��ȡMcuID[12]
	{
		McuID[12] += McuID[i];	
	}
	for(i=0; i<12; i++)	//��ȡMcuID[13]
	{
		McuID[13] ^= McuID[i];	
	}

	pMacBuf[0] = (uint8_t)(uiMcuId & 0xF0);
	pMacBuf[1] = (uint8_t)((uiMcuId & 0xFF00) >>8);
	pMacBuf[2] = (uint8_t)((uiMcuId & 0xFF0000) >>16);
	pMacBuf[3] = (uint8_t)((uiMcuId & 0xFF000000) >>24);
	pMacBuf[4] = McuID[12];
	pMacBuf[5] = McuID[13];
}



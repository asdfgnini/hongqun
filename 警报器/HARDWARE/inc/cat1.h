#ifndef __CAT1_H
#define __CAT1_H

#define RESET_IO(x)       GPIO_WriteBit(GPIOB, GPIO_Pin_2, (BitAction)x)  //PB2���Ƹ�λIO
#define LINKA_STA         GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0)        //PB0,��ȡ��ƽ״̬,�����ж�����A�Ƿ���
#define LINKB_STA         GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1)        //PB1,��ȡ��ƽ״̬,�����ж�����B�Ƿ���

#define CAT1_printf       u3_printf           
#define CAT1_RxCounter    Usart3_RxCounter    
#define CAT1_RX_BUF       Usart3_RxBuff       
#define CAT1_RXBUFF_SIZE  USART3_RXBUFF_SIZE  


void CAT1_GIPO_Init(void);
char CAT1_Reset(int);
char CAT1_ExitTrans(int);
char CAT1_SendCmd(char *, int);
char CAT1_CSQ(int, int *);
char CAT1_SYSINFO(int);
char CAT1_ICCID(int timeout, char *iccid);//SIM�����
char CAT1_IMEI(int timeout, char *imei);  //SIM���޹�
char CAT1_LBS(int timeout, unsigned long *lac, unsigned long *cid);
char CAT1_Connect_IoTServer(int);

#endif



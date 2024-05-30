#ifndef __CAT1_H
#define __CAT1_H

#define RESET_IO(x)       GPIO_WriteBit(GPIOB, GPIO_Pin_2, (BitAction)x)  //PB2控制复位IO
#define LINKA_STA         GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0)        //PB0,读取电平状态,可以判断连接A是否建立
#define LINKB_STA         GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1)        //PB1,读取电平状态,可以判断连接B是否建立

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
char CAT1_ICCID(int timeout, char *iccid);//SIM卡相关
char CAT1_IMEI(int timeout, char *imei);  //SIM卡无关
char CAT1_LBS(int timeout, unsigned long *lac, unsigned long *cid);
char CAT1_Connect_IoTServer(int);

#endif



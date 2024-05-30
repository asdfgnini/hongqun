
#include <stdio.h>
#include <stdlib.h>
#include "stm32f10x.h"  //包含需要的头文件
#include "main.h"         //包含需要的头文件
#include "cat1.h"	      //包含需要的头文件
#include "delay.h"	      //包含需要的头文件
#include "usart1.h"	      //包含需要的头文件
#include "usart3.h"	      //包含需要的头文件
#include "led.h"          //包含需要的头文件
#include "key.h"          //包含需要的头文件
#include "mqtt.h"         //包含需要的头文件
#include "bsp_timer.h"

/*-------------------------------------------------*/
/*函数名：初始化4G模块的IO                         */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void CAT1_GIPO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;                     //定义一个设置GPIO的变量
		
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB, ENABLE);   //使能GPIOB端口时钟
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;                //准备设置PB2
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;        //IO速率50Mhz
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   		 //推免输出方式
	GPIO_Init(GPIOB, &GPIO_InitStructure);            		 //设置PB2
	RESET_IO(0);                                             //复位IO拉低电平
	
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0 | GPIO_Pin_1;  //准备设置PB0 1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;   		 //上拉输入，默认状态是高电平
	GPIO_Init(GPIOB, &GPIO_InitStructure);                   //设置PB0 1
}
/*-------------------------------------------------*/
/*函数名：复位4G模块                               */
/*参  数：timeout： 超时时间（1s的倍数）           */
/*返回值：无                                       */
/*-------------------------------------------------*/
char CAT1_Reset(int timeout)
{	
	RESET_IO(1);                  //复位IO拉高电平
	bsp_DelayMS(500);                //延时
	RESET_IO(0);                  //复位IO拉低电平
	
	while(timeout--){                         //等待超时时间到0
		bsp_DelayMS(1000);                       //延时1s
		if(strstr(CAT1_RX_BUF,"WH-GM5"))      //如果接收到WH-GM5表示成功
			break;       					  //主动跳出while循环
		u1_printf("%d ",timeout);             //串口输出现在的超时时间
	}	
	u1_printf("%s\r\n", CAT1_RX_BUF);
	if(timeout<=0)return 1;                   //超时错误，返回1
	return 0;                                 //正确返回0	
}
/*-------------------------------------------------*/
/*函数名：4G模块退出透传                           */
/*参  数：timeout： 超时时间（100ms的倍数）        */
/*返回值：无                                       */
/*-------------------------------------------------*/
char CAT1_ExitTrans(int timeout)
{	
	CAT1_RxCounter=0;                         //接收数据量变量清零                        
	memset(CAT1_RX_BUF,0,CAT1_RXBUFF_SIZE);   //清空接收缓冲区 
	CAT1_printf("+++");                       //发送指令		
	while(timeout--){                         //等待超时时间到0
		bsp_DelayMS(100);                        //延时100ms
		if(strstr(CAT1_RX_BUF,"a"))           //如果接收到a表示成功
			break;       					  //主动跳出while循环
		u1_printf("%d\r\n",timeout);             //串口输出现在的超时时间
	}
                
	if(timeout<=0)return 1;                   //如果timeout<=0，说明超时时间到了，也没能收到a，返回1
	else{
		CAT1_RxCounter=0;                         //接收数据量变量清零                        
		memset(CAT1_RX_BUF,0,CAT1_RXBUFF_SIZE);   //清空接收缓冲区 
		CAT1_printf("a");                         //发送指令			
		while(timeout--){                         //等待超时时间到0
			bsp_DelayMS(100);                        //延时100ms
			if(strstr(CAT1_RX_BUF,"+ok"))         //如果接收到+ok表示成功
				break;       					  //主动跳出while循环
			u1_printf("%d\r\n",timeout);             //串口输出现在的超时时间
		}
		if(timeout<=0)return 2;                   //如果timeout<=0，说明超时时间到了，也没能收到+ok，返回1	
	}
    return 	0;                                    //正确 返回0
}
/*-------------------------------------------------*/
/*函数名：4G模块发送设置指令                       */
/*参  数：cmd：指令                                */
/*参  数：timeout： 超时时间（100ms的倍数）        */
/*返回值：无                                       */
/*-------------------------------------------------*/
char CAT1_SendCmd(char *cmd, int timeout)
{
	CAT1_RxCounter=0;                           //CAT1接收数据量变量清零                        
	memset(CAT1_RX_BUF,0,CAT1_RXBUFF_SIZE);     //清空CAT1接收缓冲区 
	CAT1_printf("%s\r\n",cmd);                  //发送指令
	while(timeout--){                           //等待超时时间到0
		bsp_DelayMS(100);                          //延时100ms
		if(strstr(CAT1_RX_BUF,"OK"))            //如果接收到OK表示指令成功
			break;       						//主动跳出while循环
		u1_printf("%d\r\n",timeout);               //串口输出现在的超时时间
	}
	// u1_printf("%s\r\n",CAT1_RX_BUF);               //串口输出现在的超时时间
	if(timeout<=0)return 1;                     //如果timeout<=0，说明超时时间到了，也没能收到OK，返回1
	else return 0;		         				//反之，表示正确，说明收到OK，通过break主动跳出while
}
/*-------------------------------------------------*/
/*函数名：4G模块信号检测                           */
/*参  数：timeout： 超时时间（100ms的倍数）        */
/*返回值：无                                       */
/*-------------------------------------------------*/
char CAT1_CSQ(int timeout, int *retRssi)
{
	char temp[20];
	char csq[20];
	char *ptr;
	memset(csq,0,sizeof(csq));

	CAT1_RxCounter=0;                           //CAT1接收数据量变量清零                        
	memset(CAT1_RX_BUF,0,CAT1_RXBUFF_SIZE);     //清空CAT1接收缓冲区 
	CAT1_printf("AT+CSQ\r\n");                  //发送指令
	while(timeout--){                           //等待超时时间到0
		bsp_DelayMS(100);                          //延时100ms
		if(strstr(CAT1_RX_BUF,","))            //如果接收到,表示指令成功
			break;       						//主动跳出while循环
		u1_printf("%d\r\n",timeout);               //串口输出现在的超时时间
	}
	if(timeout<=0)return 1;                     //如果timeout<=0，说明超时时间到了，也没能收到,返回1
	else {
		sscanf(CAT1_RX_BUF,"%[^:]:%[^,],%[^,],%[^\r]",temp,csq,temp,temp);//拆分数据
   		*retRssi = strtod(csq, &ptr);
		// u1_printf("rssi%s\r\n",csq);                                 //串口显示信息
	}
	return 0;		                            //正确 返回0        				
}

char CAT1_ICCID(int timeout, char *iccid)
{
	char temp[20];
	char iccidTmp[25];
	memset(iccidTmp,0,sizeof(iccidTmp));

	CAT1_RxCounter=0;                           //CAT1接收数据量变量清零                        
	memset(CAT1_RX_BUF,0,CAT1_RXBUFF_SIZE);     //清空CAT1接收缓冲区 
	CAT1_printf("AT+ICCID?\r\n");                  //发送指令
	while(timeout--){                           //等待超时时间到0
		bsp_DelayMS(100);                          //延时100ms
		if(strstr(CAT1_RX_BUF,":") && strstr(CAT1_RX_BUF,"OK"))            //如果接收到,表示指令成功
			break;       						//主动跳出while循环
//		u1_printf("%d\r\n",timeout);               //串口输出现在的超时时间
	}
	if(timeout<=0)return 1;                     //如果timeout<=0，说明超时时间到了，也没能收到,返回1
	else {
		sscanf(CAT1_RX_BUF,"%[^:]:%[^\r]",temp,iccidTmp);//拆分数据
		memcpy(iccid,iccidTmp,sizeof(iccidTmp));
	}
	return 0;		                            //正确 返回0        				
}

char CAT1_IMEI(int timeout, char *imei)
{
	char temp[20];
	char iccidTmp[25];
	memset(iccidTmp,0,sizeof(iccidTmp));

	CAT1_RxCounter=0;                           //CAT1接收数据量变量清零                        
	memset(CAT1_RX_BUF,0,CAT1_RXBUFF_SIZE);     //清空CAT1接收缓冲区 
	CAT1_printf("AT+IMEI?\r\n");                  //发送指令
	while(timeout--){                           //等待超时时间到0
		bsp_DelayMS(100);                          //延时100ms
		if(strstr(CAT1_RX_BUF,":") && strstr(CAT1_RX_BUF,"OK"))            //如果接收到,表示指令成功
			break;       						//主动跳出while循环
//		u1_printf("%d\r\n",timeout);               //串口输出现在的超时时间
	}
	if(timeout<=0)return 1;                     //如果timeout<=0，说明超时时间到了，也没能收到,返回1
	else {
		sscanf(CAT1_RX_BUF,"%[^:]:%[^\r]",temp,iccidTmp);//拆分数据
		memcpy(imei,iccidTmp,sizeof(iccidTmp));
	}
	return 0;		                            //正确 返回0        				
}

char CAT1_LBS(int timeout, unsigned long *lac, unsigned long *cid)
{
	char temp[20];
	char lacTmp[10];
	char cidTmp[15];
	memset(lacTmp,0,sizeof(lacTmp));
	memset(cidTmp,0,sizeof(cidTmp));

	CAT1_RxCounter=0;                           //CAT1接收数据量变量清零                        
	memset(CAT1_RX_BUF,0,CAT1_RXBUFF_SIZE);     //清空CAT1接收缓冲区 
	CAT1_printf("AT+LBS?\r\n");                  //发送指令
	while(timeout--){                           //等待超时时间到0
		bsp_DelayMS(100);                          //延时100ms
		if(strstr(CAT1_RX_BUF,":") && strstr(CAT1_RX_BUF,",") &&
			strstr(CAT1_RX_BUF,"OK"))            //如果接收到,表示指令成功
			break;       						//主动跳出while循环
		u1_printf("%d\r\n",timeout);               //串口输出现在的超时时间
	}
	if(timeout<=0)return 1;                     //如果timeout<=0，说明超时时间到了，也没能收到,返回1
	else {
		sscanf(CAT1_RX_BUF,"%[^:]: LAC = %[^,], CID = %[^\r]",temp,lacTmp,cidTmp);//拆分数据
		*lac = strtol(lacTmp,NULL,16);
		*cid = strtol(cidTmp,NULL,16);
	}
	return 0;		                            //正确 返回0        				
}
/*-------------------------------------------------*/
/*函数名：4G模块查询网络信息                       */
/*参  数：timeout： 超时时间（100ms的倍数）        */
/*返回值：无                                       */
/*-------------------------------------------------*/
char CAT1_SYSINFO(int timeout)
{
	char temp[20];
	char info[20];
	
	CAT1_RxCounter=0;                           //CAT1接收数据量变量清零                        
	memset(CAT1_RX_BUF,0,CAT1_RXBUFF_SIZE);     //清空CAT1接收缓冲区 
	CAT1_printf("AT+SYSINFO\r\n");              //发送指令
	while(timeout--){                           //等待超时时间到0
		bsp_DelayMS(100);                          //延时100ms
		if(strstr(CAT1_RX_BUF,","))            //如果接收到,表示指令成功
			break;       						//主动跳出while循环
		u1_printf("%d ",timeout);               //串口输出现在的超时时间
	}
	u1_printf("\r\n");                         
	if(timeout<=0)return 1;                     //如果timeout<=0，说明超时时间到了，也没能收到,返回1
	else {
		sscanf(CAT1_RX_BUF,"%[^:]:%[^,],%[^\r]",temp,temp,info);          //拆分数据
		u1_printf("当前网络形式:%s\r\n",info);                            //串口显示信息
	}
	return 0;		                            //正确 返回0        				
}

char imei[25];
char iccid[25];
/*-------------------------------------------------*/
/*函数名：4G模块连接服务器                         */
/*参  数：timeout： 超时时间（100ms的倍数）        */
/*返回值：0：正确   其他：错误                     */
/*-------------------------------------------------*/
char CAT1_Connect_IoTServer(int timeout)
{			
	char temp[256];
	int retRssi = 0;
	
	u1_printf("准备复位模块\r\n");                     //串口提示数据
	if(CAT1_Reset(20)){                                //复位，1s超时单位，总计20s超时时间
		u1_printf("复位失败，准备重启\r\n");           //返回非0值，进入if，串口提示数据
		return 1;                                      //返回1
	}else u1_printf("复位成功\r\n");                   //串口提示数据

	u1_printf("准备退出透传\r\n");                     //串口提示数据
	if(CAT1_ExitTrans(50)){                            //退出透传，100ms超时单位，总计5s超时时间
		u1_printf("退出透传失败，准备重启\r\n");       //返回非0值，进入if，串口提示数据
		return 2;                                      //返回2
	}else u1_printf("退出透传成功\r\n");               //串口提示数据

	if(CAT1_CSQ(50, &retRssi)){                                //复位，1s超时单位，总计20s超时时间
		u1_printf("查询信号失败，准备重启\r\n");           //返回非0值，进入if，串口提示数据
		return 1;                                      //返回1
	}else u1_printf("查询信号成功\r\n");                   //串口提示数据

	u1_printf("准备设置NET模式\r\n");                  //串口提示数据
	if(CAT1_SendCmd("AT+WKMOD=NET",50)){               //设置NET模式，100ms超时单位，总计5s超时时间
		u1_printf("设置NET模式失败，准备重启\r\n");    //返回非0值，进入if，串口提示数据
		return 5;                                      //返回5
	}else u1_printf("设置NET模式成功\r\n");            //串口提示数据
	
	u1_printf("准备使能链接A\r\n");                    //串口提示数据
	if(CAT1_SendCmd("AT+SOCKAEN=ON",50)){              //使能链接A，100ms超时单位，总计5s超时时间
		u1_printf("使能链接A失败，准备重启\r\n");      //返回非0值，进入if，串口提示数据
		return 6;                                      //返回6
	}else u1_printf("使能链接A成功\r\n");              //串口提示数据	
	
	u1_printf("准备关闭链接B\r\n");                    //串口提示数据
	if(CAT1_SendCmd("AT+SOCKBEN=OFF",50)){             //关闭链接B，100ms超时单位，总计5s超时时间
		u1_printf("关闭链接B失败，准备重启\r\n");      //返回非0值，进入if，串口提示数据
		return 7;                                      //返回7
	}else u1_printf("关闭链接B成功\r\n");              //串口提示数据

	u1_printf("准备设置链接A长连接\r\n");                  //串口提示数据
	if(CAT1_SendCmd("AT+SOCKASL=LONG",50)){                //设置链接A长连接，100ms超时单位，总计5s超时时间
		u1_printf("设置链接A长连接失败，准备重启\r\n");    //返回非0值，进入if，串口提示数据
		return 8;                                          //返回8
	}else u1_printf("设置链接A长连接成功\r\n");            //串口提示数据	
	 
	u1_printf("准备关闭自带心跳功能\r\n");                 //串口提示数据
	if(CAT1_SendCmd("AT+HEARTEN=OFF",50)){                 //关闭自带心跳功能，100ms超时单位，总计5s超时时间
		u1_printf("关闭自带心跳功能失败，准备重启\r\n");   //返回非0值，进入if，串口提示数据
		return 9;                                          //返回9
	}else u1_printf("关闭自带心跳功能成功\r\n");           //串口提示数据		
	
	u1_printf("准备设置链接A链接服务器\r\n");                //串口提示数据
	sprintf(temp,"AT+SOCKA=TCP,%s,%d",ServerIP,ServerPort);  //构建命令
	if(CAT1_SendCmd(temp,50)){                               //设置链接A链接服务器，100ms超时单位，总计5s超时时间
		u1_printf("设置链接A链接服务器失败，准备重启\r\n");  //返回非0值，进入if，串口提示数据
		return 10;                                           //返回10
	}else u1_printf("设置链接A链接服务器成功\r\n");          //串口提示数据				
	
	u1_printf("保存设置，然后重启模块\r\n");                 //串口提示数据
	if(CAT1_SendCmd("AT+S",50)){                             //保存设置，然后重启模块，100ms超时单位，总计5s超时时间
		u1_printf("保存设置失败，准备重启\r\n");             //返回非0值，进入if，串口提示数据
		return 11;                                           //返回11
	}else u1_printf("保存设置成功\r\n");     //串口提示数据
     
	u1_printf("等待重启并连接服务器\r\n");  //串口提示数据
	while(timeout--){                       //等待超时时间到0
		bsp_DelayMS(1000);                     //延时1s
		if(LINKA_STA==1)                    //如LINKA_STA PC1是高电平了，表示连接成功
			break;                          //跳出while循环  
	}

	u1_printf("%d\r\n", timeout);
	if(timeout<=0)return 12;                //超时错误，返回12
	bsp_DelayMS(500);                          //延时
	u1_printf("初始化GM5配置完成\r\n");
	return 0;                               //正确返回0	
}

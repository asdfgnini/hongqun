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
//	printf("网口连接成功\r\n");
	
	bsp_StartAutoTimer(0,1000);
	
	CAT_Connect_IoTServer();
	
	bsp_InitJQ();
	uint8_t _data;
	while (1)
	{
		if(0)
		{
			if(comGetChar(COM3,&_data))//救救我
			{
				if(_data == 'a')
				{
					bsp_JQ_On(2);
					bsp_DelayMS(100);
					bsp_JQ_Off(2);
					printf("%c\r\n",_data);
					u2_printf("AT+MPUB=\"kfb_topic\",0,0,\"help\"\r\n");
				}
				else if(_data == 'b')//傻逼
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
							setSn_IR(SOCK_TCPC, Sn_IR_CON); 			            /*清除接收中断标志位，如果想把 Sn_IR 位清零的话，主机应该将该位置‘1’。*/
						}
						MQTT_Buff_Init();
						//CONNECT
						MQTT_ConectPack();					
						while((getSn_IR(SOCK_TCPC) & Sn_IR_RECV)==0) 				// Sn_IR的RECV位置1，通知W5500收到数据
						{

							printf(" 等待MQTT服务器响应 ...\r\n");
							bsp_DelayMS(100);  
						}	
						setSn_IR(SOCK_TCPC, Sn_IR_RECV);
						Connect_flag = 1;
						
						//CONNACK
						switch(readCONNACK())
						{
						case 0: printf("连接已接受\r\n"); 

						break;//连接已被服务端接受
						case 1: printf("连接已拒绝，不支持的协议版本\r\n"); break;//服务端不支持客户端请求的 MQTT 协议级别
						case 2: printf("连接已拒绝，不合格的客户端标识符\r\n"); break;//客户端标识符是正确的 UTF-8 编码， 但服务端不允许使用
						case 3: printf("连接已拒绝，服务端不可用 \r\n"); break;//网络连接已建立， 但 MQTT 服务不可用
						case 4: printf("连接已拒绝，无效的用户名或密码\r\n"); break;//用户名或密码的数据格式无效
						case 5: printf("连接已拒绝，未授权\r\n"); break;//未授权 客户端未被授权连接到此服务器
						case 0xff: printf("bug!!\r\n"); break;//未授权 客户端未被授权连接到此服务器
						default: printf("保留\r\n"); break;//	
						}

						//订阅相关主题
						MQTT_Subscribe(S_TOPIC_NAME, 0);
					}
					//MQTT数据接收
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
					//MQTT已经连接
					if(Connect_flag == 1)
					{
						count++;
						if (MQTT_RxDataOutPtr != MQTT_RxDataInPtr)
						{ // if成立的话，说明接收缓冲区有数据了
							printf("接收到数据:");

							if (MQTT_RxDataOutPtr[2] == 0x90)
							{
								switch (MQTT_RxDataOutPtr[6])
								{
								case 0x00:
								case 0x01:
									printf("订阅成功\r\n"); // 串口输出信息
									SubcribePack_flag = 1;	   // SubcribePack_flag置1，表示订阅报文成功，其他报文可发送
									TIM3_Init();		   // 启动30s的PING定时器
									ping_open = 1;
									break; // 跳出分支
								default:
									printf("订阅失败，准备重启\r\n"); // 串口输出信息
									NVIC_SystemReset();					 // 重启
									break;								 // 跳出分支
								}
							}
							// if判断，如果一共接收了2个字节，第一个字节是0xD0，表示收到的是PINGRESP报文
							else if (MQTT_RxDataOutPtr[2] == 0xD0)
							{
								printf("PING报文回复\r\n"); // 串口输出信息
							}
							// if判断，如果第一个字节是0x30，表示收到的是服务器发来的推送数据
							// 我们要提取控制命令
							else if ((MQTT_RxDataOutPtr[2] == 0x30))
							{
								printf("服务器等级0推送\r\n");		  // 串口输出信息
								MQTT_DealPushdata_Qs0(MQTT_RxDataOutPtr); // 处理等级0推送数据
							}
							MQTT_RxDataOutPtr += RBUFF_UNIT;			// 指针下移
							if (MQTT_RxDataOutPtr == MQTT_RxDataEndPtr) // 如果指针到缓冲区尾部了
								MQTT_RxDataOutPtr = MQTT_RxDataBuf[0];	// 指针归位到缓冲区开头
						}
						if (MQTT_CMDOutPtr != MQTT_CMDInPtr)
						{												  // if成立的话，说明命令缓冲区有数据了
							printf("命令:%s\r\n", &MQTT_CMDOutPtr[2]); // 串口输出信息
							if (strstr((char *)MQTT_CMDOutPtr + 2, "\"params\":{\"power_switch\":1}"))
							{						 // 如果搜索到"params":{"power_switch":1}说明服务器下发打开开关1
									bsp_LedOn(1);
									LED_PubState(1);	 // 判断开关状态，并发布给服务器
							}
							else if (strstr((char *)MQTT_CMDOutPtr + 2, "\"params\":{\"power_switch\":0}"))
							{						  // 如果搜索到"params":{"power_switch":0}说明服务器下发关闭开关1
									bsp_LedOff(1); // 关闭LED1
									LED_PubState(0);	  // 判断开关状态，并发布给服务器
							}
							MQTT_CMDOutPtr += CBUFF_UNIT;		  // 指针下移
							if (MQTT_CMDOutPtr == MQTT_CMDEndPtr) // 如果指针到缓冲区尾部了
								MQTT_CMDOutPtr = MQTT_CMDBuf[0];  // 指针归位到缓冲区开头
						}

						//进行时间压力流量测试
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
					my_dhcp_retry++; // 失败次数+1
					if (my_dhcp_retry > MY_MAX_DHCP_RETRY)
					{										 // 如果失败次数大于最大次数，进入if
						printf("DHCP失败，准备重启\r\n"); // 串口提示信息
						NVIC_SystemReset();					 // 重启
					}
					break;
			}
		}
	}
	
}





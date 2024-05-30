/*-------------------------------------------------*/
/*            ModuleStar STM32ϵ�п�����              */
/*-------------------------------------------------*/
/*                                                 */
/*              ʵ��spi2���ܵ�Դ�ļ�               */
/*                                                 */
/*-------------------------------------------------*/

#include "stm32f10x.h"  //������Ҫ��ͷ�ļ�
#include "bsp_timer.h"      //������Ҫ��ͷ�ļ�

/*-------------------------------------------------*/
/*������������ӳ��JTAG�ڣ����ó���ͨIO						   */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void JTAGRemapGPIOInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE); 
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);    
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
    GPIO_Init(GPIOB, &GPIO_InitStructure);//PB3
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
    GPIO_Init(GPIOA, &GPIO_InitStructure);//PA15
}

/*-------------------------------------------------*/
/*����������ʼ��SPI�ӿ�                            */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void SPI_GPIO_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;                      //����һ������GPIO�ı���
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOA,ENABLE);   //ʹ��GPIOB/A�˿�ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2,ENABLE);    //ʹ��SPI2ʱ�� 
	
	GPIO_InitStruct.GPIO_Pin =  GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;//׼������PB13 PB14 PB15
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;                      //����50Mhz
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;                        //�����������ģʽ
	GPIO_Init(GPIOB, &GPIO_InitStruct);                                 //����PB13 PB14 PB15
	/*����SCS����*/
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12;        //׼������PB12
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz; //����50Mhz
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;  //�������ģʽ
	GPIO_Init(GPIOB, &GPIO_InitStruct);            //����PB12 
	GPIO_SetBits(GPIOB,GPIO_Pin_12);               //PB12�øߵ�ƽ

	/*����RESET����*/
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_11;					 /*ѡ��Ҫ���Ƶ�GPIO����*/		 
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;		     /*������������Ϊ50MHz */		
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;		     /*��������ģʽΪͨ���������*/	
	GPIO_Init(GPIOA, &GPIO_InitStruct);		 /*���ÿ⺯������ʼ��GPIO*/
	GPIO_SetBits(GPIOA, GPIO_Pin_11);		

	/*����INT����*/	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8;						 /*ѡ��Ҫ���Ƶ�GPIO����*/		 
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;		     /*������������Ϊ50MHz*/		
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;				 /*��������ģʽΪͨ������ģ����������*/		
	GPIO_Init(GPIOA, &GPIO_InitStruct);			 /*���ÿ⺯������ʼ��GPIO*/

	JTAGRemapGPIOInit();
	// ���ϻ���ʾ����δ����TODO
	// GPIO_ResetBits(GPIOA, GPIO_Pin_11);
	// Delay_Ms(1);  
	// GPIO_SetBits(GPIOA, GPIO_Pin_11);
	// Delay_Ms(10);
}

/*-------------------------------------------------*/
/*����������ʼ��SPI����                            */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void SPI_Configuration(void)
{
	SPI_InitTypeDef SPI_InitStruct;
	SPI_GPIO_Configuration();	
	SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
	SPI_InitStruct.SPI_Direction= SPI_Direction_2Lines_FullDuplex;
	SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
	SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStruct.SPI_CRCPolynomial = 7;
	SPI_Init(SPI2,&SPI_InitStruct);	
	SPI_SSOutputCmd(SPI2, ENABLE);
	SPI_Cmd(SPI2, ENABLE);
}
/*-------------------------------------------------*/
/*��������д1�ֽ����ݵ�SPI����                     */
/*��  ����TxData:д�����ߵ�����                    */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void SPI_WriteByte(unsigned char TxData)
{				 
	while((SPI2->SR&SPI_I2S_FLAG_TXE)==0);	//�ȴ���������		  
	SPI2->DR=TxData;	 	  				//����һ��byte 
	while((SPI2->SR&SPI_I2S_FLAG_RXNE)==0); //�ȴ�������һ��byte  
	SPI2->DR;		
}

/*-------------------------------------------------*/
/*����������SPI���߶�ȡ1�ֽ�����                   */
/*��  ������                                       */
/*����ֵ������������                               */
/*-------------------------------------------------*/
unsigned char SPI_ReadByte(void)
{			 
	while((SPI2->SR&SPI_I2S_FLAG_TXE)==0);	//�ȴ���������			  
	SPI2->DR=0xFF;	 	  					//����һ�������ݲ����������ݵ�ʱ�� 
	while((SPI2->SR&SPI_I2S_FLAG_RXNE)==0); //�ȴ�������һ��byte  
	return SPI2->DR;  						    
}
/*-------------------------------------------------*/
/*�������������ٽ���                               */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void SPI_CrisEnter(void)
{
	__set_PRIMASK(1);    //��ֹȫ���ж�
}
/*-------------------------------------------------*/
/*���������˳��ٽ���                               */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void SPI_CrisExit(void)
{
	__set_PRIMASK(0);   //��ȫ���ж�
}
/*-------------------------------------------------*/
/*�������� Ƭѡ�ź�����͵�ƽ                      */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void SPI_CS_Select(void)
{
	GPIO_ResetBits(GPIOB,GPIO_Pin_12);
}

/*-------------------------------------------------*/
/*�������� Ƭѡ�ź�����ߵ�ƽ                      */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void SPI_CS_Deselect(void)
{
	GPIO_SetBits(GPIOB,GPIO_Pin_12);
}

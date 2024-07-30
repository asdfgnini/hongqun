

#include "bsp.h"




/* LED�ڶ�Ӧ��RCCʱ�� */
#define LED_GPIO_CLK_ALLENABLE() {	\
		__HAL_RCC_GPIOB_CLK_ENABLE();	\
	};

#define GPIO_PORT_JQ_1  GPIOB
#define GPIO_PIN_JQ_1	GPIO_PIN_8
	
#define GPIO_PORT_JQ_2  GPIOB
#define GPIO_PIN_JQ_2	GPIO_PIN_9
	
	
	
	
void bsp_InitJQ(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;

	/* ��GPIOʱ�� */
	LED_GPIO_CLK_ALLENABLE();
	
	/*
		�������е�LEDָʾ��GPIOΪ�������ģʽ
		���ڽ�GPIO����Ϊ���ʱ��GPIO����Ĵ�����ֵȱʡ��0����˻�����LED����.
		�����Ҳ�ϣ���ģ�����ڸı�GPIOΪ���ǰ���ȹر�LEDָʾ��
	*/
	bsp_LedOff(1);


	/* ����LED */
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;   		/* ����������� */
	GPIO_InitStruct.Pull = GPIO_NOPULL;                 /* ���������費ʹ�� */
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;  		/* GPIO�ٶȵȼ� */

	GPIO_InitStruct.Pin = GPIO_PIN_JQ_1;
	HAL_GPIO_Init(GPIO_PORT_JQ_1, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = GPIO_PIN_JQ_2;
	HAL_GPIO_Init(GPIO_PORT_JQ_2, &GPIO_InitStruct);

}


void bsp_JQ_On(uint8_t _no)
{
//	_no--;

	if (_no == 1)
	{
		GPIO_PORT_JQ_1->BSRR = GPIO_PIN_JQ_1;
	}
	else if(_no == 2)
	{
		GPIO_PORT_JQ_2->BSRR = GPIO_PIN_JQ_2;

	}

}

/*
*********************************************************************************************************
*	�� �� ��: bsp_LedOff
*	����˵��: Ϩ��ָ����LEDָʾ�ơ�
*	��    ��:  _no : ָʾ����ţ���Χ 1 - 4
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_JQ_Off(uint8_t _no)
{
//	_no--;

	if (_no == 1)
	{
		GPIO_PORT_JQ_1->BSRR = (uint32_t)GPIO_PIN_JQ_1 << 16U;
	}
	else if(_no == 2)
	{
		GPIO_PORT_JQ_2->BSRR = (uint32_t)GPIO_PIN_JQ_2 << 16U;
	}
}

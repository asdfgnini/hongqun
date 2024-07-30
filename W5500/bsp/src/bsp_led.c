

#include "bsp.h"



/* LED�ڶ�Ӧ��RCCʱ�� */
#define LED_GPIO_CLK_ALLENABLE() {	\
		__HAL_RCC_GPIOD_CLK_ENABLE();	\
	};

#define GPIO_PORT_LED_1  GPIOD
#define GPIO_PIN_LED_1	GPIO_PIN_12
	


void bsp_InitLed(void)
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

	GPIO_InitStruct.Pin = GPIO_PIN_LED_1;
	HAL_GPIO_Init(GPIO_PORT_LED_1, &GPIO_InitStruct);

}

/*
*********************************************************************************************************
*	�� �� ��: bsp_LedOn
*	����˵��: ����ָ����LEDָʾ�ơ�
*	��    ��:  _no : ָʾ����ţ���Χ 1 - 4
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_LedOn(uint8_t _no)
{
//	_no--;

	if (_no == 1)
	{
		GPIO_PORT_LED_1->BSRR = GPIO_PIN_LED_1;
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
void bsp_LedOff(uint8_t _no)
{
//	_no--;

	if (_no == 1)
	{
		GPIO_PORT_LED_1->BSRR = (uint32_t)GPIO_PIN_LED_1 << 16U;
	}
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_LedToggle
*	����˵��: ��תָ����LEDָʾ�ơ�
*	��    ��:  _no : ָʾ����ţ���Χ 1 - 4
*	�� �� ֵ: ��������
*********************************************************************************************************
*/
void bsp_LedToggle(uint8_t _no)
{
	if (_no == 1)
	{
		GPIO_PORT_LED_1->ODR ^= GPIO_PIN_LED_1;
	}
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_IsLedOn
*	����˵��: �ж�LEDָʾ���Ƿ��Ѿ�������
*	��    ��:  _no : ָʾ����ţ���Χ 1 - 4
*	�� �� ֵ: 1��ʾ�Ѿ�������0��ʾδ����
*********************************************************************************************************
*/
uint8_t bsp_IsLedOn(uint8_t _no)
{
	if (_no == 1)
	{
		if ((GPIO_PORT_LED_1->ODR & GPIO_PIN_LED_1) == 1)
		{
			return 1;
		}
		return 0;
	}


	return 0;
}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
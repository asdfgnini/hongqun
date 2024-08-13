/*
*********************************************************************************************************
*
*	ģ������ : �ⲿSDRAM����ģ��
*	�ļ����� : bsp_fmc_sdram.c
*	��    �� : V2.4
*	˵    �� : ������STM32-V7����������SDRAM�ͺ�IS42S32800G-6BLI, 32λ����, ����32MB, 6ns�ٶ�(166MHz)
*
*	�޸ļ�¼ :
*		�汾��  ����        ����     ˵��
*		V1.0    2018-05-04 armfly  ��ʽ����
*
*	Copyright (C), 2018-2030, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#include "bsp.h"


/* #define SDRAM_MEMORY_WIDTH            FMC_SDRAM_MEM_BUS_WIDTH_8  */
/* #define SDRAM_MEMORY_WIDTH            FMC_SDRAM_MEM_BUS_WIDTH_16 */
#define SDRAM_MEMORY_WIDTH               FMC_SDRAM_MEM_BUS_WIDTH_32

#define SDCLOCK_PERIOD                   FMC_SDRAM_CLOCK_PERIOD_2
/* #define SDCLOCK_PERIOD                FMC_SDRAM_CLOCK_PERIOD_3 */

#define SDRAM_TIMEOUT                    ((uint32_t)0xFFFF)
#define REFRESH_COUNT                    ((uint32_t)918)    /* SDRAM��ˢ�¼��� */  

/* SDRAM�Ĳ������� */
#define SDRAM_MODEREG_BURST_LENGTH_1             ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_LENGTH_2             ((uint16_t)0x0001)
#define SDRAM_MODEREG_BURST_LENGTH_4             ((uint16_t)0x0002)
#define SDRAM_MODEREG_BURST_LENGTH_8             ((uint16_t)0x0004)
#define SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL      ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_TYPE_INTERLEAVED     ((uint16_t)0x0008)
#define SDRAM_MODEREG_CAS_LATENCY_2              ((uint16_t)0x0020)
#define SDRAM_MODEREG_CAS_LATENCY_3              ((uint16_t)0x0030)
#define SDRAM_MODEREG_OPERATING_MODE_STANDARD    ((uint16_t)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_PROGRAMMED ((uint16_t)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_SINGLE     ((uint16_t)0x0200)

static void SDRAM_GPIOConfig(void);
static void SDRAM_Initialization_Sequence(SDRAM_HandleTypeDef *hsdram, FMC_SDRAM_CommandTypeDef *Command);


/*
*********************************************************************************************************
*	�� �� ��: bsp_InitExtSDRAM
*	����˵��: ���������ⲿSDRAM��GPIO��FMC
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_InitExtSDRAM(void)
{
    SDRAM_HandleTypeDef      hsdram = {0};
	FMC_SDRAM_TimingTypeDef  SDRAM_Timing = {0};
	FMC_SDRAM_CommandTypeDef command = {0};

	
	/* FMC SDRAM���漰��GPIO���� */
	SDRAM_GPIOConfig();
	
	RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Initializes the peripherals clock
  */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_FMC;
    PeriphClkInitStruct.PLL2.PLL2M = 5;
    PeriphClkInitStruct.PLL2.PLL2N = 144;
    PeriphClkInitStruct.PLL2.PLL2P = 2;
    PeriphClkInitStruct.PLL2.PLL2Q = 2;
    PeriphClkInitStruct.PLL2.PLL2R = 3;
    PeriphClkInitStruct.PLL2.PLL2RGE = RCC_PLL2VCIRANGE_2;
    PeriphClkInitStruct.PLL2.PLL2VCOSEL = RCC_PLL2VCOWIDE;
    PeriphClkInitStruct.FmcClockSelection = RCC_FMCCLKSOURCE_PLL2;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      Error_Handler(__FILE__,__LINE__);
    }
	
	/* Peripheral clock enable */
	__HAL_RCC_FMC_CLK_ENABLE();
    

	/* SDRAM���� */
	hsdram.Instance = FMC_SDRAM_DEVICE;

	SDRAM_Timing.LoadToActiveDelay    = 2; /* 20ns, TMRD�������ģʽ�Ĵ����������뼤�������ˢ������֮����ӳ� */
	SDRAM_Timing.ExitSelfRefreshDelay = 9; /* 70ns, TXSR����ӷ�����ˢ�����������������֮����ӳ� */
	SDRAM_Timing.SelfRefreshTime      = 5; /* 50ns, TRAS������̵���ˢ������ */
	SDRAM_Timing.RowCycleDelay        = 8; /* 70ns, TRC����ˢ������ͼ�������֮����ӳ� */
	SDRAM_Timing.WriteRecoveryTime    = 4; /* 20ns, TWR������д�����Ԥ�������֮����ӳ� */
	SDRAM_Timing.RPDelay              = 2; /* 20ns, TRP����Ԥ�����������������֮����ӳ� */
	SDRAM_Timing.RCDDelay             = 2; /* 20ns, TRCD���弤���������/д����֮����ӳ� */

	hsdram.Init.SDBank             = FMC_SDRAM_BANK2;               /* Ӳ��������õ�BANK1 */
	hsdram.Init.ColumnBitsNumber   = FMC_SDRAM_COLUMN_BITS_NUM_9;   /* 9�� */
	hsdram.Init.RowBitsNumber      = FMC_SDRAM_ROW_BITS_NUM_13;     /* 12�� */
	hsdram.Init.MemoryDataWidth    = FMC_SDRAM_MEM_BUS_WIDTH_32;	/* 32λ���� */
	hsdram.Init.InternalBankNumber = FMC_SDRAM_INTERN_BANKS_NUM_4;  /* SDRAM��4��BANK */
	hsdram.Init.CASLatency         = FMC_SDRAM_CAS_LATENCY_3;       /* CAS Latency��������Latency1��2��3��ʵ�ʲ���Latency3�ȶ� */
	hsdram.Init.WriteProtection    = FMC_SDRAM_WRITE_PROTECTION_DISABLE; /* ��ֹд���� */
	hsdram.Init.SDClockPeriod      = SDCLOCK_PERIOD;                /* FMCʱ��200MHz��2��Ƶ���SDRAM����100MHz */
	hsdram.Init.ReadBurst          = FMC_SDRAM_RBURST_ENABLE;       /* ʹ�ܶ�ͻ�� */
	hsdram.Init.ReadPipeDelay      = FMC_SDRAM_RPIPE_DELAY_2;       /* ��λ��CAS��ʱ���Ӻ���ٸ�SDRAMʱ�����ڶ�ȡ���ݣ�ʵ�ʲ��λ�������������ӳ� */

	/* ����SDRAM�������������� */
	if(HAL_SDRAM_Init(&hsdram, &SDRAM_Timing) != HAL_OK)
	{
		/* Initialization Error */
		Error_Handler(__FILE__, __LINE__);
	}

	/* ���SDRAM���г�ʼ�� */
	SDRAM_Initialization_Sequence(&hsdram, &command);
}

/*
*********************************************************************************************************
*	�� �� ��: SDRAM_GPIOConfig
*	����˵��: ���������ⲿSDRAM��GPIO
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/

static void SDRAM_GPIOConfig(void)
{
	/* USER CODE END FMC_MspInit 0 */
	GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOI_CLK_ENABLE();
  __HAL_RCC_GPIOK_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOJ_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
	
	/* GPIO_InitStruct */
	GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_5|GPIO_PIN_4|GPIO_PIN_1
				  |GPIO_PIN_0|GPIO_PIN_7|GPIO_PIN_2|GPIO_PIN_3
				  |GPIO_PIN_9|GPIO_PIN_10;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF12_FMC;

	HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);

	/* GPIO_InitStruct */
	GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_0|GPIO_PIN_10|GPIO_PIN_9
				  |GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_15|GPIO_PIN_8
				  |GPIO_PIN_13|GPIO_PIN_7|GPIO_PIN_14;
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

	/* GPIO_InitStruct */
	GPIO_InitStruct.Pin = GPIO_PIN_15|GPIO_PIN_14|GPIO_PIN_13|GPIO_PIN_10
				  |GPIO_PIN_11|GPIO_PIN_9|GPIO_PIN_12|GPIO_PIN_6
				  |GPIO_PIN_8|GPIO_PIN_7;
	HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);

	/* GPIO_InitStruct */
	GPIO_InitStruct.Pin = GPIO_PIN_15|GPIO_PIN_8|GPIO_PIN_5|GPIO_PIN_4
				  |GPIO_PIN_2|GPIO_PIN_0|GPIO_PIN_1;
	HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

	/* GPIO_InitStruct */
	GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_15|GPIO_PIN_14
				  |GPIO_PIN_10|GPIO_PIN_9|GPIO_PIN_8;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

	/* GPIO_InitStruct */
	GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_1|GPIO_PIN_0|GPIO_PIN_3
						  |GPIO_PIN_5|GPIO_PIN_4|GPIO_PIN_13|GPIO_PIN_14
						  |GPIO_PIN_12|GPIO_PIN_15|GPIO_PIN_11;
	HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

	/* GPIO_InitStruct */
	GPIO_InitStruct.Pin = GPIO_PIN_0;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

}

/*
*********************************************************************************************************
*	�� �� ��: SDRAM��ʼ������
*	����˵��: ���SDRAM���г�ʼ��
*	��    ��: hsdram: SDRAM���
*			  Command: ����ṹ��ָ��
*	�� �� ֵ: None
*********************************************************************************************************
*/
static void SDRAM_Initialization_Sequence(SDRAM_HandleTypeDef *hsdram, FMC_SDRAM_CommandTypeDef *Command)
{
	__IO uint32_t tmpmrd =0;
 
    /*##-1- ʱ��ʹ������ ##################################################*/
	Command->CommandMode = FMC_SDRAM_CMD_CLK_ENABLE;
	Command->CommandTarget = FMC_SDRAM_CMD_TARGET_BANK2;
	Command->AutoRefreshNumber = 1;
	Command->ModeRegisterDefinition = 0;

	/* �������� */
	HAL_SDRAM_SendCommand(hsdram, Command, SDRAM_TIMEOUT);

    /*##-2- �����ӳ٣�����100us ##################################################*/
	HAL_Delay(1);

    /*##-3- ����SDRAMԤ������PALL(precharge all) #############################*/
	Command->CommandMode = FMC_SDRAM_CMD_PALL;
	Command->CommandTarget = FMC_SDRAM_CMD_TARGET_BANK2;
	Command->AutoRefreshNumber = 1;
	Command->ModeRegisterDefinition = 0;

	/* �������� */
	HAL_SDRAM_SendCommand(hsdram, Command, SDRAM_TIMEOUT);

    /*##-4- �Զ�ˢ������ #######################################################*/
	Command->CommandMode = FMC_SDRAM_CMD_AUTOREFRESH_MODE;
	Command->CommandTarget = FMC_SDRAM_CMD_TARGET_BANK2;
	Command->AutoRefreshNumber = 8;
	Command->ModeRegisterDefinition = 0;

	/* �������� */
	HAL_SDRAM_SendCommand(hsdram, Command, SDRAM_TIMEOUT);

    /*##-5- ����SDRAMģʽ�Ĵ��� ###############################################*/
	tmpmrd = (uint32_t)SDRAM_MODEREG_BURST_LENGTH_1          |
					 SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL   |
					 SDRAM_MODEREG_CAS_LATENCY_3           |
					 SDRAM_MODEREG_OPERATING_MODE_STANDARD |
					 SDRAM_MODEREG_WRITEBURST_MODE_SINGLE;

	Command->CommandMode = FMC_SDRAM_CMD_LOAD_MODE;
	Command->CommandTarget = FMC_SDRAM_CMD_TARGET_BANK2;
	Command->AutoRefreshNumber = 1;
	Command->ModeRegisterDefinition = tmpmrd;

	/* �������� */
	HAL_SDRAM_SendCommand(hsdram, Command, SDRAM_TIMEOUT);

    /*##-6- ������ˢ���� ####################################################*/
    /*
		ȡֵ918
    */
	HAL_SDRAM_ProgramRefreshRate(hsdram, REFRESH_COUNT); 
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_TestExtSDRAM
*	����˵��: ɨ������ⲿSDRAM��ȫ����Ԫ��
*	��    ��: ��
*	�� �� ֵ: 0 ��ʾ����ͨ���� ����0��ʾ����Ԫ�ĸ�����
*********************************************************************************************************
*/
uint32_t bsp_TestExtSDRAM1(void)
{
	uint32_t i;
	uint32_t *pSRAM;
	uint8_t *pBytes;
	uint32_t err;
	const uint8_t ByteBuf[4] = {0x55, 0xA5, 0x5A, 0xAA};

	/* дSRAM */
	pSRAM = (uint32_t *)EXT_SDRAM_ADDR;
	for (i = 0; i < EXT_SDRAM_SIZE / 4; i++)
	{
		*pSRAM++ = i;
	}

	/* ��SRAM */
	err = 0;
	pSRAM = (uint32_t *)EXT_SDRAM_ADDR;
	for (i = 0; i < EXT_SDRAM_SIZE / 4; i++)
	{
		if (*pSRAM++ != i)
		{
			err++;
		}
	}

	if (err >  0)
	{
		return  (4 * err);
	}

	/* ��SRAM �������󷴲�д�� */
	pSRAM = (uint32_t *)EXT_SDRAM_ADDR;
	for (i = 0; i < EXT_SDRAM_SIZE / 4; i++)
	{
		*pSRAM = ~*pSRAM;
		pSRAM++;
	}

	/* �ٴαȽ�SDRAM������ */
	err = 0;
	pSRAM = (uint32_t *)EXT_SDRAM_ADDR;
	for (i = 0; i < EXT_SDRAM_SIZE / 4; i++)
	{
		if (*pSRAM++ != (~i))
		{
			err++;
		}
	}

	if (err >  0)
	{
		return (4 * err);
	}

	/* ���԰��ֽڷ�ʽ����, Ŀ������֤ FSMC_NBL0 �� FSMC_NBL1 ���� */
	pBytes = (uint8_t *)EXT_SDRAM_ADDR;
	for (i = 0; i < sizeof(ByteBuf); i++)
	{
		*pBytes++ = ByteBuf[i];
	}

	/* �Ƚ�SDRAM������ */
	err = 0;
	pBytes = (uint8_t *)EXT_SDRAM_ADDR;
	for (i = 0; i < sizeof(ByteBuf); i++)
	{
		if (*pBytes++ != ByteBuf[i])
		{
			err++;
		}
	}
	if (err >  0)
	{
		return err;
	}
	return 0;
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_TestExtSDRAM2
*	����˵��: ɨ������ⲿSDRAM����ɨ��ǰ��4M�ֽڵ��Դ档
*	��    ��: ��
*	�� �� ֵ: 0 ��ʾ����ͨ���� ����0��ʾ����Ԫ�ĸ�����
*********************************************************************************************************
*/
uint32_t bsp_TestExtSDRAM2(void)
{
	uint32_t i;
	uint32_t *pSRAM;
	uint8_t *pBytes;
	uint32_t err;
	const uint8_t ByteBuf[4] = {0x55, 0xA5, 0x5A, 0xAA};

	/* дSRAM */
	pSRAM = (uint32_t *)SDRAM_APP_BUF;
	for (i = 0; i < SDRAM_APP_SIZE / 4; i++)
	{
		*pSRAM++ = i;
	}

	/* ��SRAM */
	err = 0;
	pSRAM = (uint32_t *)SDRAM_APP_BUF;
	for (i = 0; i < SDRAM_APP_SIZE / 4; i++)
	{
		if (*pSRAM++ != i)
		{
			err++;
		}
	}

	if (err >  0)
	{
		return  (4 * err);
	}

#if 0
	/* ��SRAM �������󷴲�д�� */
	pSRAM = (uint32_t *)SDRAM_APP_BUF;
	for (i = 0; i < SDRAM_APP_SIZE / 4; i++)
	{
		*pSRAM = ~*pSRAM;
		pSRAM++;
	}

	/* �ٴαȽ�SDRAM������ */
	err = 0;
	pSRAM = (uint32_t *)SDRAM_APP_BUF;
	for (i = 0; i < SDRAM_APP_SIZE / 4; i++)
	{
		if (*pSRAM++ != (~i))
		{
			err++;
		}
	}

	if (err >  0)
	{
		return (4 * err);
	}
#endif	

	/* ���԰��ֽڷ�ʽ����, Ŀ������֤ FSMC_NBL0 �� FSMC_NBL1 ���� */
	pBytes = (uint8_t *)SDRAM_APP_BUF;
	for (i = 0; i < sizeof(ByteBuf); i++)
	{
		*pBytes++ = ByteBuf[i];
	}

	/* �Ƚ�SDRAM������ */
	err = 0;
	pBytes = (uint8_t *)SDRAM_APP_BUF;
	for (i = 0; i < sizeof(ByteBuf); i++)
	{
		if (*pBytes++ != ByteBuf[i])
		{
			err++;
		}
	}
	if (err >  0)
	{
		return err;
	}
	return 0;
}

void SDRAM_Test(void)
{
	uint32_t err = bsp_TestExtSDRAM1();
	if (err == 0)
	{
		printf("�ⲿSDRAM����ͨ��\r\n");
	}
	else
	{
		printf("�ⲿSDRAM����������Ԫ������%d\r\n", err);
	}
}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
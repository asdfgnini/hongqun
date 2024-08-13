

#include "qspi.h"
extern void SystemClock_Config(void);

/*
*********************************************************************************************************
*	�� �� ��: Init
*	����˵��: Flash��̳�ʼ��
*	��    ��: ��
*	�� �� ֵ: 0 ��ʾʧ�ܣ� 1��ʾ�ɹ�
*********************************************************************************************************
*/
int Init(void)
{   
    int result = 1;
	
	HAL_Init();
 
    /* ϵͳ��ʼ�� */
    SystemInit(); 

    /* ʱ�ӳ�ʼ�� */
    SystemClock_Config();

    /* W25Q256��ʼ�� */
    MX_QUADSPI_Init();
	  //unlock
		uint32_t reg[3] = {0};
		reg[0] =  QSPI_FLASH_ReadStatusReg(1);
		reg[1] = QSPI_FLASH_ReadStatusReg(2);
		reg[2] = QSPI_FLASH_ReadStatusReg(3);
		do{
			GPIO_InitTypeDef GPIO_InitStruct = {0};
			GPIO_InitStruct.Pin = GPIO_PIN_9;
			GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
			GPIO_InitStruct.Pull = GPIO_NOPULL;
			GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
			HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

			GPIO_InitStruct.Pin = GPIO_PIN_7;
			GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
			GPIO_InitStruct.Pull = GPIO_NOPULL;
			GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
			HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

			HAL_GPIO_WritePin(GPIOG, GPIO_PIN_9, 1);
			HAL_GPIO_WritePin(GPIOF, GPIO_PIN_7, 1);

		}while(0);

		QSPI_FLASH_WriteStatusReg(1,0);
		//re init
		HAL_QSPI_MspDeInit(&hqspi);
		HAL_QSPI_MspInit(&hqspi);
		MX_QUADSPI_Init();

		reg[0] =  QSPI_FLASH_ReadStatusReg(1);
		reg[1] = QSPI_FLASH_ReadStatusReg(2);
		reg[2] = QSPI_FLASH_ReadStatusReg(3);
		
		BSP_QSPI_Init();
    
    /* �ڴ�ӳ�� */    
    result = QSPI_EnableMemoryMappedMode(); 
    if (result == 1)
    {
        return 0;
    }

    return 1;
}

/*
*********************************************************************************************************
*	�� �� ��: Write
*	����˵��: д���ݵ�Device
*	��    ��: Address д���ַ
*             Size   д���С����λ�ֽ�
*             buffer Ҫд������ݵ�ַ
*	�� �� ֵ: 1 ��ʾ�ɹ���0��ʾʧ��
*********************************************************************************************************
*/
int Write(uint32_t Address, uint32_t Size, uint8_t* buffer)
{ 
    int result = 0;  

    if (Address < 0x90000000 || Address >= 0x90000000 + W25Q256JV_FLASH_SIZE)
    {
        return 0;
    }
    
    Address -= 0x90000000;

    /* W25Q256��ʼ�� */
    MX_QUADSPI_Init();
		QSPI_FLASH_WriteStatusReg(1,0);
		BSP_QSPI_Init();

    BSP_QSPI_Write((uint8_t*) buffer, (Address & (0x0fffffff)),Size);

    /* �ڴ�ӳ�� */    
    result = QSPI_EnableMemoryMappedMode();
    if (result == 1)
    {
        return 0;
    }

    return (1);  
}

/*
*********************************************************************************************************
*	�� �� ��: SectorErase
*	����˵��: EraseStartAddress ������ʼ��ַ
*             EraseEndAddress   ����������ַ
*	��    ��: adr ������ַ
*	�� �� ֵ: 1 ��ʾ�ɹ���0��ʾʧ��
*********************************************************************************************************
*/
int SectorErase (uint32_t EraseStartAddress ,uint32_t EraseEndAddress)
{
    uint32_t BlockAddr, result;
    
    EraseStartAddress -= 0x90000000;
    EraseEndAddress -= 0x90000000;
    EraseStartAddress = EraseStartAddress -  EraseStartAddress % W25Q256JV_BLOCK_SIZE; 
    
    /* W25Q256��ʼ�� */
    MX_QUADSPI_Init();
		QSPI_FLASH_WriteStatusReg(1,0);
		BSP_QSPI_Init();

		while (EraseEndAddress >= EraseStartAddress)
		{
			BlockAddr = EraseStartAddress & 0x03FFFFFF;

			result = BSP_QSPI_Erase_Block64(BlockAddr);
			if (result == 1)
			{
				QSPI_EnableMemoryMappedMode();
				return 0;
			}

			EraseStartAddress += W25Q256JV_BLOCK_SIZE;
		}

    /* �ڴ�ӳ�� */    
    result = QSPI_EnableMemoryMappedMode();
    if (result == 1)
    {
        return 0;
    }

    return 1; 
}

/*
*********************************************************************************************************
*	�� �� ��: Verify
*	����˵��: У�飬δʹ�ã�STM32CubeProg���Զ�������У��
*	��    ��: MemoryAddr    ���ݵ�ַ
*             RAMBufferAddr RAM���ݻ����ַ
*             Size ��С�Ƚ�
*	�� �� ֵ: 1 ��ʾ�ɹ���0��ʾʧ��
*********************************************************************************************************
*/
//int Verify(uint32_t MemoryAddr, uint32_t RAMBufferAddr, uint32_t Size)
//{   
//    return 1;
//}

/*
*********************************************************************************************************
*	�� �� ��: MassErase
*	����˵��: ����оƬ����
*	��    ��: ��
*	�� �� ֵ: 1 ��ʾ�ɹ���0��ʾʧ��
*********************************************************************************************************
*/
int MassErase(void)
{
    int result = 1;
    
    /* W25Q256��ʼ�� */
    MX_QUADSPI_Init();
		QSPI_FLASH_WriteStatusReg(1,0);
		BSP_QSPI_Init();
                                             
    BSP_QSPI_Erase_Chip();  

    /* �ڴ�ӳ�� */    
    result = QSPI_EnableMemoryMappedMode();
    if (result == 1)
    {
        return 0;
    }
    
    return 1;          
}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/


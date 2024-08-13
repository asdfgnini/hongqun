

#include "qspi.h"
extern void SystemClock_Config(void);

/*
*********************************************************************************************************
*	函 数 名: Init
*	功能说明: Flash编程初始化
*	形    参: 无
*	返 回 值: 0 表示失败， 1表示成功
*********************************************************************************************************
*/
int Init(void)
{   
    int result = 1;
	
	HAL_Init();
 
    /* 系统初始化 */
    SystemInit(); 

    /* 时钟初始化 */
    SystemClock_Config();

    /* W25Q256初始化 */
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
    
    /* 内存映射 */    
    result = QSPI_EnableMemoryMappedMode(); 
    if (result == 1)
    {
        return 0;
    }

    return 1;
}

/*
*********************************************************************************************************
*	函 数 名: Write
*	功能说明: 写数据到Device
*	形    参: Address 写入地址
*             Size   写入大小，单位字节
*             buffer 要写入的数据地址
*	返 回 值: 1 表示成功，0表示失败
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

    /* W25Q256初始化 */
    MX_QUADSPI_Init();
		QSPI_FLASH_WriteStatusReg(1,0);
		BSP_QSPI_Init();

    BSP_QSPI_Write((uint8_t*) buffer, (Address & (0x0fffffff)),Size);

    /* 内存映射 */    
    result = QSPI_EnableMemoryMappedMode();
    if (result == 1)
    {
        return 0;
    }

    return (1);  
}

/*
*********************************************************************************************************
*	函 数 名: SectorErase
*	功能说明: EraseStartAddress 擦除起始地址
*             EraseEndAddress   擦除结束地址
*	形    参: adr 擦除地址
*	返 回 值: 1 表示成功，0表示失败
*********************************************************************************************************
*/
int SectorErase (uint32_t EraseStartAddress ,uint32_t EraseEndAddress)
{
    uint32_t BlockAddr, result;
    
    EraseStartAddress -= 0x90000000;
    EraseEndAddress -= 0x90000000;
    EraseStartAddress = EraseStartAddress -  EraseStartAddress % W25Q256JV_BLOCK_SIZE; 
    
    /* W25Q256初始化 */
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

    /* 内存映射 */    
    result = QSPI_EnableMemoryMappedMode();
    if (result == 1)
    {
        return 0;
    }

    return 1; 
}

/*
*********************************************************************************************************
*	函 数 名: Verify
*	功能说明: 校验，未使用，STM32CubeProg会自动做读回校验
*	形    参: MemoryAddr    数据地址
*             RAMBufferAddr RAM数据缓冲地址
*             Size 大小比较
*	返 回 值: 1 表示成功，0表示失败
*********************************************************************************************************
*/
//int Verify(uint32_t MemoryAddr, uint32_t RAMBufferAddr, uint32_t Size)
//{   
//    return 1;
//}

/*
*********************************************************************************************************
*	函 数 名: MassErase
*	功能说明: 整个芯片擦除
*	形    参: 无
*	返 回 值: 1 表示成功，0表示失败
*********************************************************************************************************
*/
int MassErase(void)
{
    int result = 1;
    
    /* W25Q256初始化 */
    MX_QUADSPI_Init();
		QSPI_FLASH_WriteStatusReg(1,0);
		BSP_QSPI_Init();
                                             
    BSP_QSPI_Erase_Chip();  

    /* 内存映射 */    
    result = QSPI_EnableMemoryMappedMode();
    if (result == 1)
    {
        return 0;
    }
    
    return 1;          
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/


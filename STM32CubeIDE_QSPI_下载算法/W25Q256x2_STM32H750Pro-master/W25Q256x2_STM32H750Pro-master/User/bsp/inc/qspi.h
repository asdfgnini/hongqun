/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    quadspi.h
  * @brief   This file contains all the function prototypes for
  *          the quadspi.c file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __QUADSPI_H__
#define __QUADSPI_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal.h"

/* USER CODE BEGIN Includes */
void Error_Handler(void);
/* USER CODE END Includes */

extern QSPI_HandleTypeDef hqspi;

/* USER CODE BEGIN Private defines */
/* Private typedef -----------------------------------------------------------*/

#define  sFLASH_ID                         0XEF4019     

#define  sDEVICE_ID                         0XEF18     

/* QSPI Error codes */
#define QSPI_OK            ((uint8_t)0x00)
#define QSPI_ERROR         ((uint8_t)0x01)
#define QSPI_BUSY          ((uint8_t)0x02)
#define QSPI_NOT_SUPPORTED ((uint8_t)0x04)
#define QSPI_SUSPENDED     ((uint8_t)0x08)


/* W25Q256JV Micron memory */
/* Size of the flash */
#define QSPI_FLASH_SIZE            24     
#define QSPI_PAGE_SIZE             256

/* QSPI Info */
typedef struct {
  uint32_t FlashSize;          
  uint32_t EraseSectorSize;   
  uint32_t EraseSectorsNumber; 
  uint32_t ProgPageSize;      
  uint32_t ProgPagesNumber;   
} QSPI_Info;

/*????-????????*******************************/
/**
  * @brief  W25Q256JV??
  */
#define W25Q256JV_FLASH_SIZE                  0x4000000 
#define W25Q256JV_BLOCK_SIZE                  0x20000	
#define W25Q256JV_BLOCK_COUNT				          512		
#define W25Q256JV_SECTOR_SIZE                 0x1000  
#define W25Q256JV_PAGE_SIZE                   0x200 

#define W25Q256JV_DUMMY_CYCLES_READ           4
#define W25Q256JV_DUMMY_CYCLES_READ_QUAD      10

#define W25Q256JV_BULK_ERASE_MAX_TIME         250000
#define W25Q256JV_SECTOR_ERASE_MAX_TIME       3000
#define W25Q256JV_SUBSECTOR_ERASE_MAX_TIME    800


/**
  * @brief  W25Q256JV ??
  */
/* ???? */
#define RESET_ENABLE_CMD                     0x66
#define RESET_MEMORY_CMD                     0x99

#define ENTER_QPI_MODE_CMD                   0x38
#define EXIT_QPI_MODE_CMD                    0xFF

/* ???? */
#define READ_ID_CMD                          0x90
#define DUAL_READ_ID_CMD                     0x92
#define QUAD_READ_ID_CMD                     0x94
#define READ_JEDEC_ID_CMD                    0x9F

/* ?????? */
#define READ_CMD_4BYTE                       0x03
#define FAST_READ_CMD                        0x0B
#define DUAL_OUT_FAST_READ_CMD               0x3B
#define DUAL_INOUT_FAST_READ_CMD             0xBB
#define QUAD_OUT_FAST_READ_CMD               0x6B
#define QUAD_INOUT_FAST_READ_CMD             0xEB
#define QUAD_INOUT_FAST_READ_CMD_4BYTE       0xEC

/* ?????? */
#define WRITE_ENABLE_CMD                     0x06
#define WRITE_DISABLE_CMD                    0x04

/* ???????? */
#define READ_STATUS_REG1_CMD                 0x05
#define READ_STATUS_REG2_CMD                 0x35
#define READ_STATUS_REG3_CMD                 0x15

#define WRITE_STATUS_REG1_CMD                0x01
#define WRITE_STATUS_REG2_CMD                0x31
#define WRITE_STATUS_REG3_CMD                0x11
/* ???? */
#define PAGE_PROG_CMD                        0x02
#define QUAD_INPUT_PAGE_PROG_CMD_4BYTE       0x34
#define EXT_QUAD_IN_FAST_PROG_CMD_4BYTE      0x12
#define ENTER_4_BYTE_ADDR_MODE_CMD           0xB7

/* ???? */
#define SECTOR_ERASE_CMD                     0x20    //0xD8?:64K    0x52?:32K     0x20?:4K
#define SECTOR_ERASE_CMD_4BYTE               0x21
#define BLOCK32_ERASE_CMD                    0x52
#define BLOCK64_ERASE_CMD                    0xD8
#define BLOCK64_ERASE_CMD_4BYTE              0xDC
#define CHIP_ERASE_CMD                       0xC7

//#define PROG_ERASE_RESUME_CMD                 0x7A
//#define PROG_ERASE_SUSPEND_CMD                0x75

/* ????????? */
#define W25Q256JV_FSR_BUSY                    ((uint8_t)0x0101)    /*!< busy */
#define W25Q256JV_FSR_WREN                    ((uint8_t)0x0202)    /*!< write enable */
#define W25Q256JV_FSR_QE                      ((uint8_t)0x0202)    /*!< quad enable */
/*????-??*******************************/


/*QSPI????-????????****************************/
#define QSPI_FLASH                         QUADSPI
#define QSPI_FLASH_CLK_ENABLE()            __QSPI_CLK_ENABLE()

#define QSPI_MDMA_CLK_ENABLE()         		__HAL_RCC_MDMA_CLK_ENABLE()

#define QSPI_FORCE_RESET()             		__HAL_RCC_QSPI_FORCE_RESET()
#define QSPI_RELEASE_RESET()           		__HAL_RCC_QSPI_RELEASE_RESET()

#define QSPI_FLASH_CLK_PIN                 GPIO_PIN_2
#define QSPI_FLASH_CLK_GPIO_PORT           GPIOB
#define QSPI_FLASH_CLK_GPIO_ENABLE()       __GPIOB_CLK_ENABLE()
#define QSPI_FLASH_CLK_GPIO_AF             GPIO_AF9_QUADSPI

#define QSPI_FLASH_BK1_IO0_PIN             GPIO_PIN_8
#define QSPI_FLASH_BK1_IO0_PORT            GPIOF
#define QSPI_FLASH_BK1_IO0_CLK_ENABLE()    __GPIOF_CLK_ENABLE()
#define QSPI_FLASH_BK1_IO0_AF              GPIO_AF10_QUADSPI

#define QSPI_FLASH_BK1_IO1_PIN             GPIO_PIN_9
#define QSPI_FLASH_BK1_IO1_PORT            GPIOF
#define QSPI_FLASH_BK1_IO1_CLK_ENABLE()    __GPIOF_CLK_ENABLE()
#define QSPI_FLASH_BK1_IO1_AF              GPIO_AF10_QUADSPI

#define QSPI_FLASH_BK1_IO2_PIN             GPIO_PIN_7
#define QSPI_FLASH_BK1_IO2_PORT            GPIOF
#define QSPI_FLASH_BK1_IO2_CLK_ENABLE()    __GPIOF_CLK_ENABLE()
#define QSPI_FLASH_BK1_IO2_AF              GPIO_AF9_QUADSPI

#define QSPI_FLASH_BK1_IO3_PIN             GPIO_PIN_6
#define QSPI_FLASH_BK1_IO3_PORT            GPIOF
#define QSPI_FLASH_BK1_IO3_CLK_ENABLE()    __GPIOF_CLK_ENABLE()
#define QSPI_FLASH_BK1_IO3_AF              GPIO_AF9_QUADSPI

#define QSPI_FLASH_CS_PIN                 GPIO_PIN_6
#define QSPI_FLASH_CS_GPIO_PORT           GPIOG
#define QSPI_FLASH_CS_GPIO_CLK_ENABLE()   __GPIOG_CLK_ENABLE()
#define QSPI_FLASH_CS_GPIO_AF             GPIO_AF10_QUADSPI

/*QSPI BANK2????-????????****************************/
#define QSPI_FLASH_BK2_IO0_PIN             GPIO_PIN_2
#define QSPI_FLASH_BK2_IO0_PORT            GPIOH
#define QSPI_FLASH_BK2_IO0_CLK_ENABLE()    __GPIOH_CLK_ENABLE()
#define QSPI_FLASH_BK2_IO0_AF              GPIO_AF9_QUADSPI

#define QSPI_FLASH_BK2_IO1_PIN             GPIO_PIN_3
#define QSPI_FLASH_BK2_IO1_PORT            GPIOH
#define QSPI_FLASH_BK2_IO1_CLK_ENABLE()    __GPIOH_CLK_ENABLE()
#define QSPI_FLASH_BK2_IO1_AF              GPIO_AF9_QUADSPI

#define QSPI_FLASH_BK2_IO2_PIN             GPIO_PIN_9
#define QSPI_FLASH_BK2_IO2_PORT            GPIOG
#define QSPI_FLASH_BK2_IO2_CLK_ENABLE()    __GPIOG_CLK_ENABLE()
#define QSPI_FLASH_BK2_IO2_AF              GPIO_AF9_QUADSPI

#define QSPI_FLASH_BK2_IO3_PIN             GPIO_PIN_14
#define QSPI_FLASH_BK2_IO3_PORT            GPIOG
#define QSPI_FLASH_BK2_IO3_CLK_ENABLE()    __GPIOG_CLK_ENABLE()
#define QSPI_FLASH_BK2_IO3_AF              GPIO_AF9_QUADSPI



/* USER CODE END Private defines */

void MX_QUADSPI_Init(void);

/* USER CODE BEGIN Prototypes */
uint8_t BSP_QSPI_Init(void);
uint8_t BSP_QSPI_Erase_Sector(uint32_t SectorAddress);
uint8_t BSP_QSPI_Erase_Block32(uint32_t BlockAddress);
uint8_t BSP_QSPI_Erase_Block64(uint32_t BlockAddress);
uint8_t BSP_QSPI_Erase_Chip(void);
uint8_t BSP_QSPI_Read(uint8_t* pData, uint32_t ReadAddr, uint32_t Size);
uint8_t BSP_QSPI_FastRead(uint8_t* pData, uint32_t ReadAddr, uint32_t Size);
uint8_t BSP_QSPI_Write(uint8_t* pData, uint32_t WriteAddr, uint32_t Size);

uint32_t QSPI_FLASH_ReadDeviceID(void);
uint32_t QSPI_FLASH_ReadID(void);
uint32_t QSPI_FLASH_ReadStatusReg(uint8_t reg);
uint32_t QSPI_FLASH_WriteStatusReg(uint8_t reg,uint8_t regvalue);
void QSPI_Set_WP_High(void);
void QSPI_Set_WP_TO_QSPI_IO(void);
void QSPI_FLASH_Wait_Busy(void);
uint32_t QSPI_EnableMemoryMappedMode(void);

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __QUADSPI_H__ */


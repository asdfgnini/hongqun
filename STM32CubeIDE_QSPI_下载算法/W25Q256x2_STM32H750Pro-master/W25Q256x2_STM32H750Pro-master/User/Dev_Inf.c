
//#include "Dev_Inf.h"
//#include "qspi.h"

///* This structure contains information used by ST-LINK Utility to program and erase the device */
//#if defined (__ICCARM__)
//__root struct StorageInfo const StorageInfo  =  {
//#else
//struct StorageInfo const StorageInfo = {
//#endif
//		"STM32H750Pro_W25Q256x2", 	 	         // Device Name + version number
//		NOR_FLASH,                  		 // Device Type
//		0x90000000,                			 // Device Start Address
//		W25Q256JV_FLASH_SIZE,                 	 // Device Size in Bytes
//		W25Q256JV_PAGE_SIZE,                    // Programming Page Size
//		0xFF,                                // Initial Content of Erased Memory
//		{ { W25Q256JV_BLOCK_COUNT, W25Q256JV_BLOCK_SIZE },       //Sector Size
//				{ 0x00000000, 0x00000000 },
//		}
//};

#include "Dev_Inf.h"


#if defined (__ICCARM__)
__root struct StorageInfo const StorageInfo  =  {
#else
struct StorageInfo const StorageInfo =  {
#endif
    "ZJT_STM32H743Pro_W25Q256x2", /* 算法名，添加算法到STM32CubeProg安装目录会显示此名字 */
    NOR_FLASH,                      /* 设备类型 */
    0x90000000,                     /* Flash起始地址 */
    64 * 1024 * 1024,               /* Flash大小 */
    512,                            /* 编程页大小 */
    0xFF,                           /* 擦除后的数值 */
    512 , 128 * 1024,                /* 块个数和块大小 */
    0x00000000, 0x00000000,
};

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/

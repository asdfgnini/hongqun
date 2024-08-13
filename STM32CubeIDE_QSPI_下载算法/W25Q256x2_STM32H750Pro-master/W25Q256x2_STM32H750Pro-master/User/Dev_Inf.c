
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
    "ZJT_STM32H743Pro_W25Q256x2", /* �㷨��������㷨��STM32CubeProg��װĿ¼����ʾ������ */
    NOR_FLASH,                      /* �豸���� */
    0x90000000,                     /* Flash��ʼ��ַ */
    64 * 1024 * 1024,               /* Flash��С */
    512,                            /* ���ҳ��С */
    0xFF,                           /* ���������ֵ */
    512 , 128 * 1024,                /* ������Ϳ��С */
    0x00000000, 0x00000000,
};

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/

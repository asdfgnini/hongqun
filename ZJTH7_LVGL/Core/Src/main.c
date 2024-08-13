
#include "bsp.h"
#include "lvgl.h"
#include "lv_port_disp_template.h"
#include "lv_port_indev_template.h"
#include "lv_demos.h"



#if USE_FreeRTOS == 1
static void vTaskGUI(void *pvParameters);
static void vTaskStart(void *pvParameters);
static void vTaskLED(void *pvParameters);
static void AppTaskCreate (void);


static TaskHandle_t xHandleTaskLED = NULL;
static TaskHandle_t xHandleTaskStart = NULL;

static void vTaskGUI(void *pvParameters)
{
	lv_init();
	lv_port_disp_init(); 
	lv_port_indev_init();
	
#if LV_USE_DEMO_WIDGETS
    lv_demo_widgets();
#endif
	
	TickType_t xLastWakeTime;
	const TickType_t xFrequency = 5;

	HAL_ResumeTick();
	
	/* 获取当前的系统时间 */
    xLastWakeTime = xTaskGetTickCount();

	while (1) 
	{
		lv_timer_handler();
		
		vTaskDelayUntil(&xLastWakeTime, xFrequency);
	}
}

static void vTaskStart(void *pvParameters)
{
	TickType_t xLastWakeTime;
	const TickType_t xFrequency = 1;

	HAL_ResumeTick();
	
	/* 获取当前的系统时间 */
    xLastWakeTime = xTaskGetTickCount();
	
    while(1)
    {
		/* 需要周期性处理的程序，对应裸机工程调用的SysTick_ISR */
		bsp_ProPer1ms();
		
		/* vTaskDelayUntil是绝对延迟，vTaskDelay是相对延迟。*/
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}

static void vTaskLED(void *pvParameters)
{
    while(1)
    {
		bsp_LedToggle(3);
        vTaskDelay(200);
    }
}

static void AppTaskCreate (void)
{
	xTaskCreate(  vTaskGUI,             /* 任务函数  */
                  "vTaskGUI",           /* 任务名    */
                  8192,                 /* 任务栈大小，单位word，也就是4字节 */
                  NULL,                 /* 任务参数  */
                  1,                    /* 任务优先级*/
                  NULL );               /* 任务句柄  */
//	
//    xTaskCreate( vTaskTaskUserIF,   	/* 任务函数  */
//                 "vTaskUserIF",     	/* 任务名    */
//                 512,               	/* 任务栈大小，单位word，也就是4字节 */
//                 NULL,              	/* 任务参数  */
//                 2,                 	/* 任务优先级*/
//                 &xHandleTaskUserIF );  /* 任务句柄  */
	
	
	xTaskCreate( vTaskLED,    		/* 任务函数  */
                 "vTaskLED",  		/* 任务名    */
                 512,         		/* 任务栈大小，单位word，也就是4字节 */
                 NULL,        		/* 任务参数  */
                 3,           		/* 任务优先级*/
                 &xHandleTaskLED ); /* 任务句柄  */
	
//	xTaskCreate( vTaskMsgPro,     		/* 任务函数  */
//                 "vTaskMsgPro",   		/* 任务名    */
//                 2048,             		/* 任务栈大小，单位word，也就是4字节 */
//                 NULL,           		/* 任务参数  */
//                 4,               		/* 任务优先级*/
//                 &xHandleTaskMsgPro );  /* 任务句柄  */
//	
//	
	xTaskCreate( vTaskStart,     		/* 任务函数  */
                 "vTaskStart",   		/* 任务名    */
                 512,            		/* 任务栈大小，单位word，也就是4字节 */
                 NULL,           		/* 任务参数  */
                 10,              		/* 任务优先级*/
                 &xHandleTaskStart );   /* 任务句柄  */
}
#endif


int main(void)
{
	System_init();
#if USE_FreeRTOS == 1
	HAL_SuspendTick();
	__set_PRIMASK(1); 

#endif
	bsp_Init();
#if USE_FreeRTOS == 1
	AppTaskCreate();
    vTaskStartScheduler();
#endif

#if USE_FreeRTOS == 0
	bsp_StartAutoTimer(0,5);
	bsp_StartAutoTimer(1,500);
#endif
#if USE_FreeRTOS == 1
	while(1);
#endif
	
#if USE_FreeRTOS == 0
	lv_init();
	lv_port_disp_init(); 
	lv_port_indev_init();
	
#if LV_USE_DEMO_WIDGETS
    lv_demo_widgets();
#endif

	while(1)
	{
		bsp_Idle();
		if(bsp_CheckTimer(0))
		{
			lv_timer_handler();
		}
		if(bsp_CheckTimer(1))
		{
			bsp_LedToggle(1);
		}
	}
#endif
}



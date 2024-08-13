/**
 * @file lv_port_disp_templ.c
 *
 */

/*Copy this file as "lv_port_disp.c" and set this value to "1" to enable content*/
#if 1

/*********************
 *      INCLUDES
 *********************/
#include "lv_port_disp_template.h"
#include <stdbool.h>
#include "lvgl.h"

#include "bsp.h"

/*********************
 *      DEFINES
 *********************/
 #ifndef      __MEMORY_AT
  #if     (defined (__CC_ARM))
    #define  __MEMORY_AT(x)     __attribute__((at(x)))
  #elif   (defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050))
    #define  __MEMORY_AT__(x)   __attribute__((section(".bss.ARM.__at_"#x)))
    #define  __MEMORY_AT(x)     __MEMORY_AT__(x)
  #else
    #define  __MEMORY_AT(x)
    #warning Position memory containing __MEMORY_AT macro at absolute address!
  #endif
#endif

#ifndef MY_DISP_HOR_RES
    #define MY_DISP_HOR_RES    800
#endif

#ifndef MY_DISP_VER_RES
    #define MY_DISP_VER_RES    480
#endif

#define CreateOnebuffer
//#define CreateTWObuffer
//#define Doublebuffering

/**********************
 *      TYPEDEFS
 **********************/
static __IO int8_t wTransferState;
extern LTDC_HandleTypeDef   hltdc_F;

/* 注意bsp_tft_h7.c文件使能了行中断和NVIC */
void LTDC_IRQHandler(void) 
{
	LTDC->ICR = (uint32_t)LTDC_IER_LIE;
	wTransferState = 1;
}

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void disp_init(void);
static void disp_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_port_disp_init(void)
{
    /*-------------------------
     * Initialize your display
     * -----------------------*/
    disp_init();

    /*-----------------------------
     * Create a buffer for drawing
     *----------------------------*/

    /**
     * LVGL requires a buffer where it internally draws the widgets.
     * Later this buffer will passed to your display driver's `flush_cb` to copy its content to your display.
     * The buffer has to be greater than 1 display row
     *
     * There are 3 buffering configurations:
     * 1. Create ONE buffer:
     *      LVGL will draw the display's content here and writes it to your display
     *
     * 2. Create TWO buffer:
     *      LVGL will draw the display's content to a buffer and writes it your display.
     *      You should use DMA to write the buffer's content to the display.
     *      It will enable LVGL to draw the next part of the screen to the other buffer while
     *      the data is being sent form the first buffer. It makes rendering and flushing parallel.
     *
     * 3. Double buffering
     *      Set 2 screens sized buffers and set disp_drv.full_refresh = 1.
     *      This way LVGL will always provide the whole rendered screen in `flush_cb`
     *      and you only need to change the frame buffer's address.
     */

    /* Example for 1) */
#ifdef CreateOnebuffer
    static lv_disp_draw_buf_t draw_buf_dsc_1;
    static lv_color_t buf_1[MY_DISP_HOR_RES * 200];                          /*A buffer for 10 rows*/
    lv_disp_draw_buf_init(&draw_buf_dsc_1, buf_1, NULL, MY_DISP_HOR_RES * 200);   /*Initialize the display buffer*/

    /* Example for 2) */
#elif defined CreateTWObuffer
    static lv_disp_draw_buf_t draw_buf_dsc_2;
    static lv_color_t buf_2_1[MY_DISP_HOR_RES * 50];                        /*A buffer for 10 rows*/
    static lv_color_t buf_2_2[MY_DISP_HOR_RES * 50];                        /*An other buffer for 10 rows*/
    lv_disp_draw_buf_init(&draw_buf_dsc_2, buf_2_1, buf_2_2, MY_DISP_HOR_RES * 50);   /*Initialize the display buffer*/

    /* Example for 3) also set disp_drv.full_refresh = 1 below*/
#elif defined Doublebuffering
    static lv_disp_draw_buf_t draw_buf_dsc_3; 
    static lv_color_t buf_3_1[MY_DISP_HOR_RES * MY_DISP_VER_RES] __MEMORY_AT(0xD0000000);  /*A screen sized buffer*/
    static lv_color_t buf_3_2[MY_DISP_HOR_RES * MY_DISP_VER_RES] __MEMORY_AT(0xD00BB800);  /*Another screen sized buffer*/

	lv_disp_draw_buf_init(&draw_buf_dsc_3, buf_3_1, buf_3_2,
							MY_DISP_VER_RES * MY_DISP_HOR_RES);   /*Initialize the display buffer*/
#endif

    /*-----------------------------------
     * Register the display in LVGL
     *----------------------------------*/

    static lv_disp_drv_t disp_drv;                  /*Descriptor of a display driver*/
    lv_disp_drv_init(&disp_drv);                    /*Basic initialization*/

    /*Set up the functions to access to your display*/
	switch (g_LcdType)
	{
		case LCD_43_480X272:		/* 4.3寸 480 * 272 */	
		case LCD_50_480X272:		/* 5.0寸 480 * 272 */
			 /*Set the resolution of the display*/
			disp_drv.hor_res = 480;
			disp_drv.ver_res = 272;
			break;
		
		case LCD_43_800X480:
		case LCD_50_800X480:		/* 5.0寸 800 * 480 */
		case LCD_70_800X480:		/* 7.0寸 800 * 480 */	
			disp_drv.hor_res = 800;
			disp_drv.ver_res = 480;
			break;
		
		default:	
			break;
	}
  
    /*Used to copy the buffer's content to the display*/
    disp_drv.flush_cb = disp_flush;

#ifdef CreateOnebuffer
    /*Set a display buffer*/
    disp_drv.draw_buf = &draw_buf_dsc_1;
#elif defined CreateTWObuffer
    /*Set a display buffer*/
    disp_drv.draw_buf = &draw_buf_dsc_2;	
#elif defined Doublebuffering
    /*Set a display buffer*/
    disp_drv.draw_buf = &draw_buf_dsc_3;	
	/*Required for Example 3)*/
	disp_drv.full_refresh = 1;	
#endif
	
	wTransferState = 0;
	
    /*Finally register the driver*/
    lv_disp_drv_register(&disp_drv);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/*Initialize your display and the required peripherals.*/
static void disp_init(void)
{
}

volatile bool disp_flush_enabled = true;

/* Enable updating the screen (the flushing process) when disp_flush() is called by LVGL
 */
void disp_enable_update(void)
{
    disp_flush_enabled = true;
}

/* Disable updating the screen (the flushing process) when disp_flush() is called by LVGL
 */
void disp_disable_update(void)
{
    disp_flush_enabled = false;
}

#ifndef Doublebuffering
static void _DMA2D_Copy(void * pSrc, 
	                    void * pDst, 
						uint32_t xSize, 
						uint32_t ySize, 
						uint32_t OffLineSrc, 
						uint32_t OffLineDst, 
						uint32_t PixelFormat) 
{

	/* DMA2D采用存储器到存储器模式, 这种模式是前景层作为DMA2D输入 */  
	DMA2D->CR      = 0x00000000UL | (1 << 9);
	DMA2D->FGMAR   = (uint32_t)pSrc;
	DMA2D->OMAR    = (uint32_t)pDst;
	DMA2D->FGOR    = OffLineSrc;
	DMA2D->OOR     = OffLineDst;
	
	/* 前景层和输出区域都采用的RGB565颜色格式 */
	DMA2D->FGPFCCR = LTDC_PIXEL_FORMAT_RGB565;
	DMA2D->OPFCCR  = LTDC_PIXEL_FORMAT_RGB565;
	
	DMA2D->NLR     = (uint32_t)(xSize << 16) | (uint16_t)ySize;

	/* 启动传输 */
	DMA2D->CR   |= DMA2D_CR_START;   

	/* 等待DMA2D传输完成 */
	while (DMA2D->CR & DMA2D_CR_START) {} 
}
#endif

/*Flush the content of the internal buffer the specific area on the display
 *You can use DMA or any hardware acceleration to do this operation in the background but
 *'lv_disp_flush_ready()' has to be called when finished.*/
static void disp_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{

#ifndef  Doublebuffering
	int32_t x;
    int32_t y;
	
	x = area->x2 - area->x1 +1;  
	y = area->y2 - area->y1 +1;
		
	_DMA2D_Copy((void *)(color_p), 
		        (void *)(EXT_SDRAM_ADDR+disp_drv->hor_res*area->y1*2+ area->x1*2), 
				x, 
				y, 
				0, 
				disp_drv->hor_res - x, 
				LTDC_PIXEL_FORMAT_RGB565);
#else
	while (wTransferState== 0){}
	wTransferState = 0;

	__HAL_LTDC_LAYER(&hltdc_F, 0)->CFBAR =(uint32_t)color_p;
	__HAL_LTDC_RELOAD_IMMEDIATE_CONFIG(&hltdc_F);	
#endif
		
    /*IMPORTANT!!!
     *Inform the graphics library that you are ready with the flushing*/
    lv_disp_flush_ready(disp_drv);
}

#else /*Enable this file at the top*/

/*This dummy typedef exists purely to silence -Wpedantic.*/
typedef int keep_pedantic_happy;
#endif

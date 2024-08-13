/*
*********************************************************************************************************
*	                                  
*	模块名称 : GUI界面主函数
*	文件名称 : MainTask.c
*	版    本 : V1.0
*	说    明 : LCD界面
*              
*	修改记录 :
*		版本号   日期         作者          说明
*		V1.0    2020-09-13   Eric2013  	    首版    
*                                     
*	Copyright (C), 2018-2030, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/
#include "MainTask.h"



/*
*********************************************************************************************************
*                                               变量
*********************************************************************************************************
*/
GX_WINDOW 		*pScreen;
GX_WINDOW_ROOT  *root;


/*
*********************************************************************************************************
*	函 数 名: _cbWindow0
*	功能说明: 窗口回调函数
*	形    参: widget 窗口句柄
*	返 回 值: 无
*********************************************************************************************************
*/
VOID _cbWindow0(GX_WINDOW *widget)
{
    GX_RECTANGLE drawto;
    GX_RECTANGLE tempdraw;     
    GX_CANVAS *mycanvas; 
    GX_POINT my_polygon[3] = { { 150, 210 }, { 250, 210 }, { 200, 260 } };
    

    /* 默认的窗口绘制回调函数，即默认界面效果绘制 */
    gx_window_draw(widget);

    /* 定义一个矩形框，后续的2D绘制函数都是在这个矩形范围内绘制的 */
    gx_utility_rectangle_define(&drawto,
                                10, 
                                100,
                                400, 
                                265);
 
    /* 返回窗口对应的canvas画布 */
    gx_widget_canvas_get(widget, &mycanvas);
    
    /* 
      在指定的画布上启动绘图。此功能在内部被延迟绘图算法调用，GUIX在需要画布时自动执行更新。 
      但是允许应用程序绕过延期绘图算法并立即执行。
      首先调用gx_canvas_drawing_inititate在画布上绘画。
      然后调用所需的绘图函数，最后然后调用gx_canvas_drawing_complete即可。
    */
    gx_canvas_drawing_initiate(mycanvas, widget, &drawto);

    /* 设置笔刷画线的颜色值 */
    gx_context_raw_line_color_set(0xffff0000);

    /* 设置笔刷填充的颜色值 */   
    gx_context_raw_fill_color_set(0xff00ff00);

    /* 通过GX_BRUSH_SOLID_FILL使能圆圈，矩形，多边形等绘制为填充效果 */
    gx_context_brush_style_set(GX_BRUSH_SOLID_FILL);

    /* 设置笔刷线宽 */
    gx_context_brush_width_set(1);

    /* 绘制直线 */
    gx_canvas_line_draw(10, 100, 50, 150);
 
    /* 绘制圆圈 */
    gx_canvas_circle_draw(120, 150, 50);

    /* 绘制椭圆 */
    gx_canvas_ellipse_draw(300, 150, 100, 50);

    /* 绘制多边形 */
    gx_canvas_polygon_draw(my_polygon, 3); 

    /* 绘制矩形 */
    tempdraw.gx_rectangle_left = 30; 
    tempdraw.gx_rectangle_top = 210; 
    tempdraw.gx_rectangle_right = 100; 
    tempdraw.gx_rectangle_bottom = 260;
    gx_canvas_rectangle_draw(&tempdraw);

    /* 用于强制立即绘制，注意，务必和gx_canvas_drawing_initiate成对调用 */
    gx_canvas_drawing_complete(mycanvas, GX_TRUE);
}


//UINT _cbEventWindow(GX_WINDOW *widget, GX_EVENT *event_ptr)
//{
//	GX_CHAR *buffer_address;
//	UINT buffer_size;
//	UINT content_size;

//    switch (event_ptr->gx_event_type)
//    {
//        /* 控件显示事件 */
//        case GX_EVENT_SHOW:
//     
//            /* 默认事件处理 */
//            gx_window_event_process(widget, event_ptr);
//            break;
//			
//        /* 按钮1 */
//        case GX_SIGNAL(GUI_ID_BUTTON0, GX_EVENT_CLICKED):
//            gx_single_line_text_input_character_insert(&(window_1.window_1_text_input), (GX_UBYTE *)"1", 1);
//            break;
//		
//        /* 按钮2 */
//        case GX_SIGNAL(GUI_ID_BUTTON1, GX_EVENT_CLICKED):
//            gx_single_line_text_input_character_insert(&(window_1.window_1_text_input), (GX_UBYTE *)"2", 1);
//            break;
//		
//        /* 按钮3 */
//        case GX_SIGNAL(GUI_ID_BUTTON2, GX_EVENT_CLICKED):
//            gx_single_line_text_input_character_insert(&(window_1.window_1_text_input), (GX_UBYTE *)"3", 1);
//            break;

//        /* 清除输入框 */
//        case GX_SIGNAL(GUI_ID_BUTTON3, GX_EVENT_CLICKED):
//            gx_single_line_text_input_buffer_clear(&(window_1.window_1_text_input));
//            break;	

//        /* 按钮4 */
//        case GX_SIGNAL(GUI_ID_BUTTON4, GX_EVENT_CLICKED):
//            gx_single_line_text_input_character_insert(&(window_1.window_1_text_input), (GX_UBYTE *)"4", 1);
//            break;	

//        /* 按钮5 */
//        case GX_SIGNAL(GUI_ID_BUTTON5, GX_EVENT_CLICKED):
//            gx_single_line_text_input_character_insert(&(window_1.window_1_text_input), (GX_UBYTE *)"5", 1);
//            break;

//        /* 按钮6 */
//        case GX_SIGNAL(GUI_ID_BUTTON6, GX_EVENT_CLICKED):
//            gx_single_line_text_input_character_insert(&(window_1.window_1_text_input), (GX_UBYTE *)"6", 1);
//            break;	

//        /* 按钮C */
//        case GX_SIGNAL(GUI_ID_BUTTON7, GX_EVENT_CLICKED):
//			gx_single_line_text_input_left_arrow(&(window_1.window_1_text_input));
//			gx_single_line_text_input_character_delete(&(window_1.window_1_text_input));
//            break;			
//		
//        /* 按钮7 */
//        case GX_SIGNAL(GUI_ID_BUTTON8, GX_EVENT_CLICKED):
//            gx_single_line_text_input_character_insert(&(window_1.window_1_text_input), (GX_UBYTE *)"7", 1);
//            break;

//        /* 按钮8 */
//        case GX_SIGNAL(GUI_ID_BUTTON9, GX_EVENT_CLICKED):
//            gx_single_line_text_input_character_insert(&(window_1.window_1_text_input), (GX_UBYTE *)"8", 1);
//            break;	

//        /* 按钮9 */
//        case GX_SIGNAL(GUI_ID_BUTTON10, GX_EVENT_CLICKED):
//            gx_single_line_text_input_character_insert(&(window_1.window_1_text_input), (GX_UBYTE *)"9", 1);
//            break;	

//        /* 未使用*/
//        case GX_SIGNAL(GUI_ID_BUTTON11, GX_EVENT_CLICKED):
//            break;

//        /* 未使用*/
//        case GX_SIGNAL(GUI_ID_BUTTON12, GX_EVENT_CLICKED):
//            break;	

//        /* 按钮0 */
//        case GX_SIGNAL(GUI_ID_BUTTON13, GX_EVENT_CLICKED):
//			gx_single_line_text_input_character_insert(&(window_1.window_1_text_input), (GX_UBYTE *)"0", 1);
//            break;	

//        /* 未使用 */
//        case GX_SIGNAL(GUI_ID_BUTTON14, GX_EVENT_CLICKED):
//            break;

//        /* 按钮OK */
//        case GX_SIGNAL(GUI_ID_BUTTON15, GX_EVENT_CLICKED):
//			/* 获取文本 */
//			gx_single_line_text_input_buffer_get(&(window_1.window_1_text_input), &buffer_address, &content_size, &buffer_size);
//			gx_prompt_text_set(&(window.window_prompt_1), buffer_address);
//		
//			/* 默认事件处理 */
//            gx_window_event_process(widget, event_ptr);
//            break;			

//        default:
//            return gx_window_event_process(widget, event_ptr);
//    }

//    return 0;
//}


/*
*********************************************************************************************************
*	函 数 名: MainTask
*	功能说明: GUI主函数
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void MainTask(void) 
{
	/*
       触摸校准函数默认是注释掉的，电阻屏需要校准，电容屏无需校准。如果用户需要校准电阻屏的话，执行
	   此函数即可，会将触摸校准参数保存到EEPROM里面，以后系统上电会自动从EEPROM里面加载。
	*/
#if 0
	LCD_SetBackLight(255);
	LCD_InitHard();
    TOUCH_Calibration(2);
#endif
	
	/*初始化配置 */
	gx_initconfig();

	/* 配置显示屏 */
    gx_studio_display_configure(DISPLAY_1, stm32h7_graphics_driver_setup_565rgb,
        LANGUAGE_ENGLISH, DISPLAY_1_THEME_1, &root);
	
    /* 创建窗口 */
//	gx_studio_named_widget_create("window_1", (GX_WIDGET *)root, (GX_WIDGET **)&pScreen);

    gx_studio_named_widget_create("window", (GX_WIDGET *)root, (GX_WIDGET **)&pScreen);

	/* 显示根窗口 */
    gx_widget_show(root);

    /* 启动GUIX */
    gx_system_start();
	
	tx_thread_sleep(100);
	LCD_SetBackLight(255);
	
	while(1)
	{
		tx_thread_sleep(20);
	}
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/

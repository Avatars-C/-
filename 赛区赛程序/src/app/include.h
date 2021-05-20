/******************** (C) COPYRIGHT 2011 野火嵌入式开发工作室 ********************
 * 文件名       ：include.h
 * 描述         ：工程模版头文件
 *
 * 实验平台     ：野火kinetis开发板
 * 库版本       ：
 * 嵌入系统     ：
 *
 * 作者         ：oО殇のＳo 
**********************************************************************************/	

#ifndef __INCLUDE_H__
#define __INCLUDE_H__

#include  "common.h"

/*
 * Include 用户自定义的头文件
 */
#include  "Com_def.h"
#include  "isr.h"
#include  "gpio.h"      //IO口操作
#include  "LED.H"       //流水灯
#include  "OLED.h"
#include  "uart.h"      //串口
#include  "adc.h"       //ADC模块
#include  "FTM.h"       //FTM模块（FTM0：电机控制 / 通用 /PWM     FTM1、2：正交解码 / 通用 /PWM ）
#include  "PIT.h"       //周期中断计时器
#include  "i2c.h"
#include  "lptmr.h"     //低功耗定时器(延时)
#include  "flash.h"    
#include  "exti.h"      //EXTI外部GPIO中断
#include  "var.h"  
#include  "Date_analyse.h"
#include  "MPU6050.h"
#include  "pose.h"
#include  "outputdata.h"


/***************** ucos 专用 *****************/
#define USOC_EN     0u      //0为禁止uC/OS，大于0则启动uC/OS
#if USOC_EN > 0u
#include  "ucos_ii.h"  		//uC/OS-II系统函数头文件
#include  "BSP.h"			//与开发板相关的函数
#include  "app.h"			//用户任务函数


#endif  //if  USOC_EN > 0


#endif  //__INCLUDE_H__

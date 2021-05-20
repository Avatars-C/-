/******************** (C) COPYRIGHT 2011 ********************* ********************
 * 文件名       ：Date_analyse.h
 * 描述         ：电感数据采集与分析
 *
 * 实验平台     ：野火kinetis开发板
 * 库版本       ：
 * 嵌入系统     ：
 *
 * 作者         ：oО殇のＳo 
**********************************************************************************/	
#ifndef _Date_analyse_H
#define _Date_analyse_H


extern int16  AD_valu[4],chazhi;
extern float  AD[4];
extern float  Slope_AD_1,Slope_AD_2; 
extern int16  AD_MAX_NUM;
extern int16  position;
extern int16  Stright_Flag;

void SC_black_Init(void);
void Read_ADC(void);
void Date_analyse();


 
#endif 

/******************** (C) COPYRIGHT 2011 野火嵌入式开发工作室 ********************
 * 文件名       ：main.c
 * 描述         ：工程模版实验
 *
 * 实验平台     ：野火kinetis开发板
 * 库版本       ：
 * 嵌入系统     ：
 *
 * 作者         ：oО殇のＳo 
  

**********************************************************************************/	
#include "include.h"

extern   int16  duoji_duty;;



/*******************函数声明**********************/
void system_init(void);


void main()
{
   system_init();
   EnableInterrupts;		    //开总中断

   for(;;)
   {
  
      
    //SEND((float)Gyro_Z,0, 0,0);
     // delayms(1);	
     //water_lights();
     // 文件 ”关于“  有程序的一些说明  请先查看   “关于”
  // SEND_DATE_LABVIEW(&AD_valu[0],duoji_duty,abs(cesu),Goal_speed);

    }
}
void system_init(void)
{   
    DisableInterrupts; //关总中断  防止系统初始化被打断
    MOTO_disable();
    LED_init();
    adc_init(ADC1,AD9);                      // PB1
    adc_init(ADC1,AD8);                      // PB0
    adc_init(ADC1,AD15);                     // PB11
    adc_init(ADC1,AD14);                     // PB10
    adc_init(ADC1,AD10);                      // PB5
    adc_init(ADC1,AD11);                      // PB6
    LCD_Init();          
    Steer_init();
    MOTO_pwm_init(); 
    Beer_init();
    Stir_init();
    exti_init(PORTB, 19, rising_down);      //PORTC18 干簧管，上升沿触发中断，内部下拉
    FTM_QUAD_init();         //FTM2正交解码
    pit_init(PIT0,500);      //PIT 5MS定时
    flash_init();	     //初始化flash   
    I2C_init(I2C1);          //初始化I2C0
    delayms(50);	     //上电延时		   
    MPU6050_Init();          //初始化MPU6050
    delayms(150);  
    UART_IRQ_EN(FIRE_PORT);                           //串口3  开接收中断
    Steer_adjust();
    SC_black_Init();
    Offest_Init();
    MOTO_enable();    //电机使能
}  
  
  
  
  // lptmr_counter_init(LPT0_ALT1,400,2,LPT_Rising); //初始化脉冲计数器，用LPT0_ALT2，即PTA19输
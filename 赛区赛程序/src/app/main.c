/******************** (C) COPYRIGHT 2011 Ұ��Ƕ��ʽ���������� ********************
 * �ļ���       ��main.c
 * ����         ������ģ��ʵ��
 *
 * ʵ��ƽ̨     ��Ұ��kinetis������
 * ��汾       ��
 * Ƕ��ϵͳ     ��
 *
 * ����         ��o����Σ�o 
  

**********************************************************************************/	
#include "include.h"

extern   int16  duoji_duty;;



/*******************��������**********************/
void system_init(void);


void main()
{
   system_init();
   EnableInterrupts;		    //�����ж�

   for(;;)
   {
  
      
    //SEND((float)Gyro_Z,0, 0,0);
     // delayms(1);	
     //water_lights();
     // �ļ� �����ڡ�  �г����һЩ˵��  ���Ȳ鿴   �����ڡ�
  // SEND_DATE_LABVIEW(&AD_valu[0],duoji_duty,abs(cesu),Goal_speed);

    }
}
void system_init(void)
{   
    DisableInterrupts; //�����ж�  ��ֹϵͳ��ʼ�������
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
    exti_init(PORTB, 19, rising_down);      //PORTC18 �ɻɹܣ������ش����жϣ��ڲ�����
    FTM_QUAD_init();         //FTM2��������
    pit_init(PIT0,500);      //PIT 5MS��ʱ
    flash_init();	     //��ʼ��flash   
    I2C_init(I2C1);          //��ʼ��I2C0
    delayms(50);	     //�ϵ���ʱ		   
    MPU6050_Init();          //��ʼ��MPU6050
    delayms(150);  
    UART_IRQ_EN(FIRE_PORT);                           //����3  �������ж�
    Steer_adjust();
    SC_black_Init();
    Offest_Init();
    MOTO_enable();    //���ʹ��
}  
  
  
  
  // lptmr_counter_init(LPT0_ALT1,400,2,LPT_Rising); //��ʼ���������������LPT0_ALT2����PTA19��
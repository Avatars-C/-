/******************** (C) COPYRIGHT 2011 ******************************
 * �ļ���       ��isr.c
 * ����         ���жϴ���
 *          
 * ʵ��ƽ̨     ��Ұ��kinetis������
 * ��汾       ��
 * Ƕ��ϵͳ     ��
 *
 * ����         ��o����Σ�o 
**********************************************************************************/	
#include "include.h"

int16  cesu,cesu_old,now_speed;
int16  LPT_INT_count,STOP_FLAG,stop_flag_yes;
int32  qipao_count,Allow_stop_flag;
char   Accept[8];
int32  jilu_counter;
int16  yaokong_mode = 0;;


/*************************************************************************
*  �������ƣ�PIT0_IRQHandler
*  ����˵����PIT0 ��ʱ�жϷ�����
*  ����˵������   
*  �������أ���
*  �޸�ʱ�䣺2012-2-18    �Ѳ���
*  ��    ע��
*************************************************************************/
void PIT0_IRQHandler(void)
{
    PIT_Flag_Clear(PIT0);             //���жϱ�־λ
    
    cesu = FTM2_CNT >> 2;
    FTM2_CNT = 0;
    if( cesu > 1000)  cesu -= 16000;  
    now_speed = (cesu + cesu_old) >> 1;    
    cesu_old = cesu;
    
    if(Debug_flag) //�������ģʽ
    {  
      Paramet_Adjust(); 
    }
    else if(!Debug_flag && Straight_Record_Flag)  //�˳�����ģʽ ���ҷ���������
    {
       Straight_Record();
    }
    
    Date_analyse();
    CarVoltageGet();
    Special_road_analyse();
    Steer_conter(chazhi);
    Speed_analyse();
    
    if(!Debug_flag)
    {  
        if(0 == Straight_Record_Flag)  //�˳�����ģʽ �������� δ��⵽������
        {  
            if(0 == yaokong_mode)
            {
               if(STOP_FLAG && stop_flag_yes)  //��⵽������ ����ͣ��  Ҫͣ���� ^_^
               {
                 Stop_car_conter(); 
               } 
               else
               {
                   MOTO_GO_dty(Goal_speed*250);
              //   Speed_conter();  //�ٶȿ���
               }
            }
            ///���ܺ��¼����������  ����һ��ֵ�� �ڿ���ͣ�����/////////// 
            if(!Allow_stop_flag && stop_flag_yes) 
            {
                qipao_count += now_speed;
                if(qipao_count >= CODER*45)
                {
                    qipao_count = CODER*45;
                    Allow_stop_flag = 1;
                }
            }
         }
         if(K5)  //��һ��·��
         {
             jilu_counter += now_speed;   
             if(jilu_counter >= CODER*300)
             {
                jilu_counter = CODER*300;
                STOP_FLAG = 1;
             }
          }
    }
}
void  LPT_Handler(void)
{
    LPTMR0_CSR|=LPTMR_CSR_TCF_MASK;     //���LPTMR�Ƚϱ�־
    LPT_INT_count++;                    //�ж������1
    lptmr_counter_clean();
}

/*************************************************************************
*  �������ƣ�PORTB_IRQHandler
*  ����˵����PORTB�˿��жϷ�����
*  ����˵������   
*  �������أ���
*  �޸�ʱ�䣺2012-1-25    �Ѳ���
*  ��    ע�����ź���Ҫ�����Լ���ʼ�����޸�
*************************************************************************/
void PORTB_IRQHandler()
{
    u8  n=0;      //���ź�   
    n=19;
    water_lights();
    if(PORTB_ISFR & (1<<n))           //PTB1�����ж�
    {
        PORTB_ISFR  |= (1<<n);          //д1���жϱ�־λ
        if(Allow_stop_flag)
        {
          STOP_FLAG = 1;
        }
    } 
    
}
u8 data;
/************************************************************************* 
*  �������ƣ�USART3_IRQHandler
*  ����˵��������3 �ж� ���� ������
*  ����˵������   
*  �������أ���
*  �޸�ʱ�䣺2012-2-14    �Ѳ���
*  ��    ע��
*************************************************************************/
void USART3_IRQHandler(void)
{
    //static u8 i = 0;
    // s8 j;
    
    DisableInterrupts;		    //�����ж�
    data=uart_getchar(UART3);
    
    if(data=='G')
    {
        yaokong_mode =1;
        MOTO_GO_dty(50000);
        MOTO_BAC_dty(0);
    }
    else if(data=='B')
    {
        yaokong_mode = 1;
        MOTO_GO_dty(0);
        MOTO_BAC_dty(40000);
    }
    else if(data=='S')
    {
        yaokong_mode = 0;
        STOP_FLAG = 1;
    }
    else if(data=='N')
    {
        yaokong_mode = 0;
        STOP_FLAG = 0;
    }
   /* Accept[i] = uart_getchar (UART3);              //���յ�һ������
    i++;
    if(i==6)  i = 0;
    
    for(j=0;j<6;j++)
    {
      if(Accept[j] =='P')
      {
         j = (j==5)? -1:j;
         if(Accept[++j] =='I')
         {
            j = (j==5)? -1:j;
            if(Accept[++j] =='D')
            {
               j = (j==5)? -1:j;
               AA=Accept[++j];
               j = (j==5)? -1:j;
               BB=Accept[++j];
               j = (j==5)? -1:j;
               CC=Accept[++j];
               Beer_ON;
            }
         }    
         break;
      }
    }*/
    
    EnableInterrupts;		    //�����ж�
}

/*************************************************************************
*                             Ұ��Ƕ��ʽ����������
*  
*  �������ƣ�SysTick_Handler
*  ����˵����ϵͳ�δ�ʱ���жϷ����� 
*  ����˵������   
*  �������أ���
*  �޸�ʱ�䣺2012-2-18    �Ѳ���
*  ��    ע��ucos���õõ�
*************************************************************************/
void SysTick_Handler(void)
{
    //    OSIntEnter(); 
    //    OSTimeTick(); 
    //    OSIntExit(); 
}

/*************************************************************************
*                             Ұ��Ƕ��ʽ����������
*  
*  �������ƣ�HardFault_Handler
*  ����˵����Ӳ���Ϸ��жϷ�����
*  ����˵������   
*  �������أ���
*  �޸�ʱ�䣺2012-2-4    �Ѳ���
*  ��    ע��������LED��˸��ָʾ������Ӳ���Ϸ�
*************************************************************************/
void HardFault_Handler(void)
{
    while (1)
    {
        printf("\n****Ӳ���Ϸô���!!!*****\r\n\n");
    }
}

/*************************************************************************
*  �������ƣ�FTM1_IRQHandler
*  ����˵����FTM1���벶׽�жϷ�����
*  ����˵������   
*  �������أ���
*  �޸�ʱ�䣺2012-2-25
*  ��    ע�����ź���Ҫ�����Լ���ʼ�����޸ģ��ο����еĴ�������Լ��Ĺ���
*************************************************************************/
void FTM1_IRQHandler()
{
    u8 s=FTM1_STATUS;               //��ȡ��׽�ͱȽ�״̬  All CHnF bits can be checked using only one read of STATUS.
    u8 n;
    FTM1_STATUS=0x00;                 //���жϱ�־λ
    
    n=0;
    if( s & (1<<n) )
    {
        FTM_CnSC_REG(FTM1_BASE_PTR,n) &= ~FTM_CnSC_CHIE_MASK; //��ֹ���벶׽�ж� 
        /*     �û�����       */
        LED_turn(LED1);                             //��תLED1
        printf("\nFTM1�����ж�\n");
        
        /*********************/
        //�����������￪�����벶׽�жϣ�������main�����������Ҫ������
        //ͨ�� CH0��CH1��Ch2��Ch3 ���˲���  
        //FTM_CnSC_REG(FTM1_BASE_PTR,n) |= FTM_CnSC_CHIE_MASK;  //�������벶׽�ж�
        //delayms(10);        //��Ϊ������ź�������̲��ȶ������״���������벶׽�����������ʱ
        //�����ǵ��жϲ�Ӧ�ù�����ʱ�����Կ����벶׽�жϾͷ���main�����������Ҫ������  
    }
    
    n=1;
    if( s & (1<<n) )
    {
        FTM_CnSC_REG(FTM1_BASE_PTR,n) &= ~FTM_CnSC_CHIE_MASK; //��ֹ���벶׽�ж� 
        /*     �û�����       */
        
        
        /*********************/
        //�����������￪�����벶׽�ж�
        //FTM_CnSC_REG(FTM1_BASE_PTR,n) |= FTM_CnSC_CHIE_MASK;  //�������벶׽�ж�
    }
    
}
/*************************************************************************
*                             Ұ��Ƕ��ʽ����������
*  
*  �������ƣ�PendSV_Handler
*  ����˵����PendSV��������ϵͳ���ã��жϷ�����
*  ����˵������   
*  �������أ���
*  �޸�ʱ�䣺2012-2-15    �Ѳ���
*  ��    ע��uC/OS�����л�����
*************************************************************************/
void PendSV_Handler(void)
{
}
/*************************************************************************
*  �������ƣ�PORTA_IRQHandler
*  ����˵����PORTA�˿��жϷ�����
*  ����˵������   
*  �������أ���
*  �޸�ʱ�䣺2012-1-25    �Ѳ���
*  ��    ע�����ź���Ҫ�Լ���ʼ�������
*************************************************************************/
void PORTA_IRQHandler()
{
    u8  n=0;      //���ź�
    
    n=0;
    if(PORTA_ISFR & (1<<n))           //PTA0�����ж�
    {
        PORTA_ISFR  |= (1<<n);          //д1���жϱ�־λ
        /*  �û�����  */
        
    } 
    
    n=26;
    if(PORTA_ISFR & (1<<n))           //PTA16�����ж�
    {
        PORTA_ISFR  |= (1<<n);          //д1���жϱ�־λ
        /*  �û�����  */
        LED_turn(LED1);                 //LED1��ת
    } 
}
/*************************************************************************
*  �������ƣ�FTM0_IRQHandler
*  ����˵����FTM0���벶׽�жϷ�����
*  ����˵������   
*  �������أ���
*  �޸�ʱ�䣺2012-2-25
*  ��    ע�����ź���Ҫ�����Լ���ʼ�����޸ģ��ο����еĴ�������Լ��Ĺ���
*************************************************************************/
void FTM0_IRQHandler()
{
    
    
}
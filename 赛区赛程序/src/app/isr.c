/******************** (C) COPYRIGHT 2011 ******************************
 * 文件名       ：isr.c
 * 描述         ：中断处理
 *          
 * 实验平台     ：野火kinetis开发板
 * 库版本       ：
 * 嵌入系统     ：
 *
 * 作者         ：oО殇のＳo 
**********************************************************************************/	
#include "include.h"

int16  cesu,cesu_old,now_speed;
int16  LPT_INT_count,STOP_FLAG,stop_flag_yes;
int32  qipao_count,Allow_stop_flag;
char   Accept[8];
int32  jilu_counter;
int16  yaokong_mode = 0;;


/*************************************************************************
*  函数名称：PIT0_IRQHandler
*  功能说明：PIT0 定时中断服务函数
*  参数说明：无   
*  函数返回：无
*  修改时间：2012-2-18    已测试
*  备    注：
*************************************************************************/
void PIT0_IRQHandler(void)
{
    PIT_Flag_Clear(PIT0);             //清中断标志位
    
    cesu = FTM2_CNT >> 2;
    FTM2_CNT = 0;
    if( cesu > 1000)  cesu -= 16000;  
    now_speed = (cesu + cesu_old) >> 1;    
    cesu_old = cesu;
    
    if(Debug_flag) //进入调试模式
    {  
      Paramet_Adjust(); 
    }
    else if(!Debug_flag && Straight_Record_Flag)  //退出调试模式 并且非正常起跑
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
        if(0 == Straight_Record_Flag)  //退出调试模式 正常起跑 未检测到起跑线
        {  
            if(0 == yaokong_mode)
            {
               if(STOP_FLAG && stop_flag_yes)  //检测到起跑线 允许停车  要停车了 ^_^
               {
                 Stop_car_conter(); 
               } 
               else
               {
                   MOTO_GO_dty(Goal_speed*250);
              //   Speed_conter();  //速度控制
               }
            }
            ///起跑后记录编码器脉冲  大于一定值后 在开启停车检测/////////// 
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
         if(K5)  //跑一段路程
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
    LPTMR0_CSR|=LPTMR_CSR_TCF_MASK;     //清除LPTMR比较标志
    LPT_INT_count++;                    //中断溢出加1
    lptmr_counter_clean();
}

/*************************************************************************
*  函数名称：PORTB_IRQHandler
*  功能说明：PORTB端口中断服务函数
*  参数说明：无   
*  函数返回：无
*  修改时间：2012-1-25    已测试
*  备    注：引脚号需要根据自己初始化来修改
*************************************************************************/
void PORTB_IRQHandler()
{
    u8  n=0;      //引脚号   
    n=19;
    water_lights();
    if(PORTB_ISFR & (1<<n))           //PTB1触发中断
    {
        PORTB_ISFR  |= (1<<n);          //写1清中断标志位
        if(Allow_stop_flag)
        {
          STOP_FLAG = 1;
        }
    } 
    
}
u8 data;
/************************************************************************* 
*  函数名称：USART3_IRQHandler
*  功能说明：串口3 中断 接收 服务函数
*  参数说明：无   
*  函数返回：无
*  修改时间：2012-2-14    已测试
*  备    注：
*************************************************************************/
void USART3_IRQHandler(void)
{
    //static u8 i = 0;
    // s8 j;
    
    DisableInterrupts;		    //关总中断
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
   /* Accept[i] = uart_getchar (UART3);              //接收到一个数据
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
    
    EnableInterrupts;		    //开总中断
}

/*************************************************************************
*                             野火嵌入式开发工作室
*  
*  函数名称：SysTick_Handler
*  功能说明：系统滴答定时器中断服务函数 
*  参数说明：无   
*  函数返回：无
*  修改时间：2012-2-18    已测试
*  备    注：ucos里用得到
*************************************************************************/
void SysTick_Handler(void)
{
    //    OSIntEnter(); 
    //    OSTimeTick(); 
    //    OSIntExit(); 
}

/*************************************************************************
*                             野火嵌入式开发工作室
*  
*  函数名称：HardFault_Handler
*  功能说明：硬件上访中断服务函数
*  参数说明：无   
*  函数返回：无
*  修改时间：2012-2-4    已测试
*  备    注：可以用LED闪烁来指示发生了硬件上访
*************************************************************************/
void HardFault_Handler(void)
{
    while (1)
    {
        printf("\n****硬件上访错误!!!*****\r\n\n");
    }
}

/*************************************************************************
*  函数名称：FTM1_IRQHandler
*  功能说明：FTM1输入捕捉中断服务函数
*  参数说明：无   
*  函数返回：无
*  修改时间：2012-2-25
*  备    注：引脚号需要根据自己初始化来修改，参考现有的代码添加自己的功能
*************************************************************************/
void FTM1_IRQHandler()
{
    u8 s=FTM1_STATUS;               //读取捕捉和比较状态  All CHnF bits can be checked using only one read of STATUS.
    u8 n;
    FTM1_STATUS=0x00;                 //清中断标志位
    
    n=0;
    if( s & (1<<n) )
    {
        FTM_CnSC_REG(FTM1_BASE_PTR,n) &= ~FTM_CnSC_CHIE_MASK; //禁止输入捕捉中断 
        /*     用户任务       */
        LED_turn(LED1);                             //翻转LED1
        printf("\nFTM1发送中断\n");
        
        /*********************/
        //不建议在这里开启输入捕捉中断，而是在main函数里根据需要来开启
        //通道 CH0、CH1、Ch2、Ch3 有滤波器  
        //FTM_CnSC_REG(FTM1_BASE_PTR,n) |= FTM_CnSC_CHIE_MASK;  //开启输入捕捉中断
        //delayms(10);        //因为输入的信号跳变过程不稳定，容易触发多次输入捕捉，所以添加延时
        //但考虑到中断不应该过长延时，所以开输入捕捉中断就放在main函数里，根据需要来开启  
    }
    
    n=1;
    if( s & (1<<n) )
    {
        FTM_CnSC_REG(FTM1_BASE_PTR,n) &= ~FTM_CnSC_CHIE_MASK; //禁止输入捕捉中断 
        /*     用户任务       */
        
        
        /*********************/
        //不建议在这里开启输入捕捉中断
        //FTM_CnSC_REG(FTM1_BASE_PTR,n) |= FTM_CnSC_CHIE_MASK;  //开启输入捕捉中断
    }
    
}
/*************************************************************************
*                             野火嵌入式开发工作室
*  
*  函数名称：PendSV_Handler
*  功能说明：PendSV（可悬起系统调用）中断服务函数
*  参数说明：无   
*  函数返回：无
*  修改时间：2012-2-15    已测试
*  备    注：uC/OS用来切换任务
*************************************************************************/
void PendSV_Handler(void)
{
}
/*************************************************************************
*  函数名称：PORTA_IRQHandler
*  功能说明：PORTA端口中断服务函数
*  参数说明：无   
*  函数返回：无
*  修改时间：2012-1-25    已测试
*  备    注：引脚号需要自己初始化来清除
*************************************************************************/
void PORTA_IRQHandler()
{
    u8  n=0;      //引脚号
    
    n=0;
    if(PORTA_ISFR & (1<<n))           //PTA0触发中断
    {
        PORTA_ISFR  |= (1<<n);          //写1清中断标志位
        /*  用户任务  */
        
    } 
    
    n=26;
    if(PORTA_ISFR & (1<<n))           //PTA16触发中断
    {
        PORTA_ISFR  |= (1<<n);          //写1清中断标志位
        /*  用户任务  */
        LED_turn(LED1);                 //LED1反转
    } 
}
/*************************************************************************
*  函数名称：FTM0_IRQHandler
*  功能说明：FTM0输入捕捉中断服务函数
*  参数说明：无   
*  函数返回：无
*  修改时间：2012-2-25
*  备    注：引脚号需要根据自己初始化来修改，参考现有的代码添加自己的功能
*************************************************************************/
void FTM0_IRQHandler()
{
    
    
}
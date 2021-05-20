/******************** (C) COPYRIGHT 2011 ********************* ********************
 * 文件名       ：var.c
 * 描述         ：函数板块
 *
 * 实验平台     ：野火kinetis开发板
 * 库版本       ：
 * 嵌入系统     ：
 *
 * 作者         ：oО殇のＳo 
**********************************************************************************/	
#include "include.h"



int16  adc_max;
///////////////////////////////////////////////
int16  Encoder_Sum;         //修改参数 记录脉冲
byte   Debug_flag = 1;      // 调试模
byte   Read_flash_flag = 1; // 读取flash 标志位
int16  Straight_Record_Flag;//高速起跑，起跑完成标志位
int16  Stright_speed, Speed_Max, Speed_Min,Speed_Ramp,Speed_Block;
int16  Err_speed,Err_speed_old,Err_speed_oldest;   //速度PID  参数
int16  Goal_speed;     //目标速度
int32  MOTO_pwm;
int16  duoji_duty;     //舵机占空比  用于发送上位机
int16  Turn_mid;
int16  Straight_Distance_Record,Straight_Distance_num,Straight_Distance_num1;  //记录直道长度

float Angle_P_Array[6]={5.05,5.1,5.16,5.24,5.34};
int16 Angle_D_Array[6]={40,47,55,63,75};

float Angle_P;  //舵机P  D
int16 Angle_D;

int16  Stright_Count,Curve_Count;
int16  Stright_Flag = 1,Stright_Flag_jiwan = 1,Curve_Flag;
int16  Curve_speed_Add,Curve_speed_Add_Set;
float  range,range_old;

int16  UPhill_flag = 0,Dowmhill_flag=0,Hill_possible_flag=0,Hill_maichong,Dowmhill_time;  //坡道
int16  Obstruct_flag=0,Obstruct_time;  //路障

int16 Speed_P_Array[6] = {10,20,30,40,50,60};
uint8 Data_table[4][4]=
{
  // 速度   
  //直 高 低  
   {70,65,55,20}, //保底速度  
   {85,75,55,15}, //中档速度  
   {90,81,61,10}, //较高速度   
   {95,85,65,5}  //冲刺速度  
};
uint8 mode_num = 4;




/*************************************************************************
*  函数名称   Steer_adjus
*  功能说明： 舵机中值调节函数
*  参数说明：          
*  函数返回： 
*  修改时间：
*  备    注：  舵机中值不对了  可以脱机修改舵机中值 
*************************************************************************/
void Steer_adjust(void)
{
   int16 adjust = 0;
   int16 turn_add;
   
   adjust=(K3==1)? 1:0;
   led(LED0,LED_ON);
   while(K3);
   led(LED0,LED_OFF);
   turn_add = flash_read(SECTOR_TURN,0,int16);	
   while(adjust)
   {      
       LCD_Print(29,2,"Steer"); 
       LCD_Print(28,4,"adjustint..."); 
       if(K4)
       {
           delay_ms(1);
           turn_add += 5;
           while(K4);
       }
       if(K2)
       {
           delay_ms(1);
           turn_add -= 5;
           while(K2);
       }
       Turn_mid = TURN_MID + turn_add;
       Steer_duty(Turn_mid);
       LCD_Show_Number(70,3,turn_add);
       if(K3) 
       {
          flash_erase_sector(SECTOR_TURN);       //擦除扇区
          flash_write(SECTOR_TURN,0,turn_add);     //参数写入扇区
          LCD_Print(6,0,"tuichu");
          adjust = 0;
          while(K3);
          LCD_CLS(); 
       }
   }
   turn_add = flash_read(SECTOR_TURN,0,int16);	
   turn_add = flash_read(SECTOR_TURN,0,int16);	
   Turn_mid = TURN_MID + turn_add; 
}
/*************************************************************************
*  函数名称   Paramet_Adjust
*  功能说明： 参数调节
*  参数说明：          
*  函数返回： 
*  修改时间：
*  备    注： 
*************************************************************************/
void Paramet_Adjust()
{
     static int16   site,site_old;
     int16          yf=0;
     static int16   Free_time=0;
     static int16   stright_speed,speed_Max,speed_Min;
     
     if(Read_flash_flag)
     {
         stright_speed = flash_read(SECTOR_NUM,0,int16);	 //读取Flash  
         speed_Max     = flash_read(SECTOR_NUM,4,int16);	
         speed_Min     = flash_read(SECTOR_NUM,8,int16);	
         Speed_Ramp    = flash_read(SECTOR_NUM,12,int16);
         Speed_Block   = flash_read(SECTOR_NUM,16,int16);
         stop_flag_yes = flash_read(SECTOR_NUM,20,uint8);	
         
         Read_flash_flag ++;
         if(Read_flash_flag >=3)  Read_flash_flag =0;     //读取2次Flsah 后标志位清零  不在读取Flash 
     }

         if(K2) {    site++;    delay_ms(20);    while(K2);  }           
         if(K1) {    site--;    delay_ms(20);    while(K1);  }
         if(site<0)  site = 25;
         else if(site>25)  site = 0;
         site %= 12;
         
         if(site <= 5)
         LCD_Draw(0,2+site,4);  //光标显示
         else 
         LCD_Draw(40,2+site-6,4);  //光标显示  
         
         LCD_Print(6,2,"H:");
         LCD_Print(6,4,"L:");
         LCD_Print(6,6,"S:");
         
         Encoder_Sum += now_speed;      //编码器修改数据
         if(Encoder_Sum >= CODER/2)       //正转 100个脉冲 参数+1
         {  
             yf++;
             Encoder_Sum=0;
         }
         if(Encoder_Sum <= -(CODER/2))      //反转100脉冲  参数 -1
         {
            yf--;
            Encoder_Sum=0;
         }
         switch(site)
         {
            case 0:   stright_speed +=yf;     //直道速度
            
                      if(stright_speed < 0)  stright_speed = 0;
                      break;
            case 1:   speed_Max +=yf;         //高速

                      if(speed_Max < 0)     speed_Max = 0;
                      break;
            case 2:   speed_Min +=yf;         //低速
                      
                      if(speed_Min < 0)   speed_Min = 0;
                      break;   
            case 3:   Speed_Ramp += yf;      //坡道速度
            
                      if(Speed_Ramp < 0)  Speed_Ramp = 0;
                      break;
            case 4:   Speed_Block += yf;     //路障 速度
            
                      if(Speed_Block < 0) Speed_Block = 0;
                      break;          
            case 5:   stop_flag_yes += yf;  //停车选择
            
                      if(stop_flag_yes <= 0) stop_flag_yes = 0;
                      else if(stop_flag_yes > 1) stop_flag_yes = 1;
                      break;
            case 6:   mode_num += yf;
            
                      if(mode_num >= 4)       mode_num = 4;
                      else if(mode_num <= 0)  mode_num = 0; 
                      break;
            case 11:  Straight_Record_Flag +=yf;  //起跑模式选择
            
                      if(Straight_Record_Flag < 0) Straight_Record_Flag = 0;
                      else if(Straight_Record_Flag > 2) Straight_Record_Flag = 2;
                      break;
            default :  break;         
               
         }
        
         switch(mode_num)
         {
              case 0:  Stright_speed = Data_table[0][0];
                       Speed_Max     = Data_table[0][1];
                       Speed_Min     = Data_table[0][2];
                       break;
              case 1:  Stright_speed = Data_table[1][0];
                       Speed_Max     = Data_table[1][1];
                       Speed_Min     = Data_table[1][2];
                       break;
              case 2:  Stright_speed = Data_table[2][0];
                       Speed_Max     = Data_table[2][1];
                       Speed_Min     = Data_table[2][2];
                       break;
              case 3:  Stright_speed = Data_table[3][0];
                       Speed_Max     = Data_table[3][1];
                       Speed_Min     = Data_table[3][2];
                       break;
              case 4:  Stright_speed = stright_speed;
                       Speed_Max     = speed_Max;
                       Speed_Min     = speed_Min;
                       break;
              default :  break;          
         }

         LCD_Show_Number(15,2,Stright_speed);         //参数OLED显示
         LCD_Show_Number(15,3,Speed_Max);
         LCD_Show_Number(15,4,Speed_Min);
         LCD_Show_Number(15,5,Speed_Ramp);
         LCD_Show_Number(15,6,Speed_Block);
         LCD_Show_Number(15,7,stop_flag_yes);
         LCD_Show_Number(50,2,mode_num);
         LCD_Show_Number(50,7,Straight_Record_Flag);
         
         
         if(abs(now_speed)<2 && site_old == site)     Free_time++;
         else                Free_time = 0;
         site_old = site;
        
     if(Free_time >= 600)          //确定 退出调试模式 不在写入FLASH  避免因擦写次数过多导致FLASH 损坏  
     {
         Beer_ON;     
         delay_ms(9000);  
          Beer_OFF; 
         Debug_flag = 0;   //调试完毕 调试标志清零  
     }
     if(K4)          //确定 退出调试模式
     {
         Beer_ON;     //蜂鸣器 滴一声  表示正在退出调试模式
         delay_ms(18000);            
         flash_erase_sector(SECTOR_NUM);       //擦除254扇区	
         flash_write(SECTOR_NUM,0,Stright_speed);     //讲所有参数写入扇区
         flash_write(SECTOR_NUM,4,Speed_Max);
         flash_write(SECTOR_NUM,8,Speed_Min);
         flash_write(SECTOR_NUM,12,Speed_Ramp);
         flash_write(SECTOR_NUM,16,Speed_Block);
         flash_write(SECTOR_NUM,20,stop_flag_yes);
         Beer_OFF; 
         Debug_flag = 0;   //调试完毕 调试标志清零  
     }
}
/*************************************************************************
*  函数名称   Straight_Record
*  功能说明： 记录开跑时的 直道长度  全速前进  以便节省时间  
*  参数说明：         
*  函数返回： 无
*  修改时间：
*  备    注： Straight_Record_Flag = 0  正常起跑  
              Straight_Record_Flag = 1  测量直道长度
              Straight_Record_Flag = 2  先高速前进  然后在正常行驶
*************************************************************************/
void Straight_Record()
{
       if(Straight_Record_Flag == 1)           //  测量直道长度
       {
           if(position > 1 && position < 5)                //偏差 较小 检测为弯道
           {
              Straight_Distance_Record += now_speed;     //记录直道 脉冲
              if(Straight_Distance_Record > CODER)
              { 
                 Straight_Distance_Record = 0;
                 Straight_Distance_num++;
              }
           }
           else   //偏差过大  检测到弯道
           {
                Straight_Record_Flag = 0;  
                flash_erase_sector(SECTOR_TAB);       //擦除253扇区	
                flash_write(SECTOR_TAB,0,Straight_Distance_num);   //直道距离写入Flash
           }
           Speed_conter(); 
           LCD_Show_Number(90,6,Straight_Distance_num); 
       }
       else if(Straight_Record_Flag == 2)    //高速起跑
       {
           Straight_Distance_num = flash_read(SECTOR_TAB,0,int16);	
           Straight_Distance_Record += now_speed;    //记录所走的路程
           if(Straight_Distance_Record > CODER)
           { 
               Straight_Distance_Record = 0;
               Straight_Distance_num1++;
           }
           if(Straight_Distance_num1 < (int16)((float)Straight_Distance_num * 0.8) && (position <= 1 || position >= 5))      // 还没有到达弯道  并且 偏差在一范围之内  防止因速度太高 而跑出赛道
           {
                 MOTO_GO_dty( 90000 );
           }
           else       
           {
                Straight_Record_Flag = 0;
           } 
       }
}
/*************************************************************************
*  函数名称   Steer_conter
*  功能说明： 舵机控制
*  参数说明：         
*  函数返回： 无
*  修改时间：
*  备    注：
*************************************************************************/
void Steer_conter(int16 D_value)
{
       static int16 D_value_old = 0;
       uint16 steer_pwmdty;
       int8  speed_tab;
       
       if(Stright_Flag)  //长直道
       {
          Angle_P=7.2;
          Angle_D = 45;
       }
       else
       {     
          if((AD_MAX_NUM ==0 || AD_MAX_NUM == 3) && now_speed > 80 && Stright_Flag_jiwan == 1) //直道入急弯
          {
            Angle_P=8.9+(now_speed - 50)/20;
            Angle_D=300+(now_speed - 50)/3;
          }
          else 
          { 
             speed_tab = (now_speed-50)/10;
             
             if(speed_tab>5)
                   speed_tab=5;    //防溢出
             else if(speed_tab <= 0)
                   speed_tab=0;
                       
             if(now_speed >= 50)
                   Angle_P = Angle_P_Array[speed_tab];  //50+ 
             else 
                   Angle_P = 4.9; 
      
           
          // else  Angle_D = Angle_D_Array[speed_tab];
           
           if(now_speed >= 70) 
                 Angle_D = 85; 
           else if(now_speed < 70 && now_speed >= 50)
                 Angle_D = 65;
           else if(now_speed <50 && now_speed >= 10)
                 Angle_D = 40;
           else 
                 Angle_D = 10;   //低速防抖动
         }
       }
       steer_pwmdty = Turn_mid - ((uint16)(D_value * Angle_P) + (uint16)((D_value - D_value_old) * Angle_D));  
       
       D_value_old = D_value;
       
       if(steer_pwmdty > Turn_mid + TURN_MAX)        steer_pwmdty = Turn_mid + TURN_MAX;
       else if(steer_pwmdty < Turn_mid - TURN_MAX)   steer_pwmdty = Turn_mid - TURN_MAX;
       Steer_duty(steer_pwmdty);
        
       range_old = range;
       range = abs(steer_pwmdty - Turn_mid);  // 传送入  目标速度分析  用于加减速
       
     
}
/*************************************************************************
*  函数名称   Special_road_analyse
*  功能说明： 特殊路况的检测 
*  参数说明：         
*  函数返回： 无
*  修改时间：
*  备    注：  坡道 障碍的检测
*************************************************************************/
void Special_road_analyse(void)
{
    /////////////////////////////////////坡道处理/////////////////////////////////////////
    if(Slope_AD_1 >= 140 && Slope_AD_2 >= 140 && 0 == UPhill_flag && 0 == Dowmhill_flag && 0 == Obstruct_flag)     //中间两个电感 特大  上坡标志位未置1  也不是路障的时候才行 则可能上坡了
    {
        Hill_possible_flag=1;  
    }   
    if(Hill_possible_flag == 1 && Angle_X > 700)   //可能上坡了  X轴角度变大了  则真的是上坡了
    {
       UPhill_flag = 1;   
    }
    if(UPhill_flag == 1 && Angle_X < -700)    //已经上坡了  X轴角度又出现负的了   进行时间累加  
    {
        Dowmhill_flag = 1;
    }
    if(Dowmhill_flag )             //正在下坡  上坡标志位清零呗  速度恢复正常 下坡加速 不会飞车车的   不然按照坡道速度^_^
    {
       UPhill_flag =0;
       
    }
    if(Dowmhill_flag)          //正处于下坡呢   记录编码器脉冲
    {
       Hill_maichong += now_speed;
    }
    if(Hill_maichong >= 7000 )     //大约下坡一米了    下坡标志清零    可能是坡道标志位清零   
    {
       Dowmhill_flag = 0;
       Hill_possible_flag = 0;
       Hill_maichong = 0;
    }  
    if(UPhill_flag == 1 && (position <= 1 || position >= 5))
    {
         Dowmhill_flag = 0;
         Hill_possible_flag = 0;
         Hill_maichong = 0;
         UPhill_flag =0;
    }
 /////////////////////////路障处理//////////////////////   
    if(0 == Hill_possible_flag && 0 == UPhill_flag && Gyro_X > 700)  //不可能上坡  但X轴角加速度 太大了  说明遇见障碍了呗  障碍标志位置1   如果掉轮了  也可能当成路障来检测了
     {
      Obstruct_flag = 1;
    }
     if(Obstruct_flag && Gyro_X < 400)   
     {
          Obstruct_time ++;
     }
    else Obstruct_time = 0;
    
    if(Obstruct_time >= 50)
    {
       Obstruct_flag = 0;
    }
}

/*************************************************************************
*  函数名称   PID_conter
*  功能说明： 电机PID调节
*  参数说明： goal_speed    目标速度    
*             ture_speed    实际速度        
*  函数返回： 无
*  修改时间：
*  备    注：
*************************************************************************/
void PID_conter()
{  
       s16  incpid;
       int16  speed_tab,speed_p;
      
       if(now_speed > 60)        //   速度高于60 时  
       {
            speed_tab = abs(Err_speed) >> 2; 
            if(speed_tab <= 0)  
                    speed_tab = 0;
            else  if(speed_tab > 5)
                    speed_tab = 5;  //限制 防数组溢出
       }
       speed_p = Speed_P + Speed_P_Array[speed_tab];  
        
       incpid = speed_p * Err_speed + Speed_I * (Err_speed - Err_speed_old) + Speed_D * (Err_speed + Err_speed_oldest - 2 * Err_speed_old);
       
       Err_speed_oldest = Err_speed_old;						  
       Err_speed_old = Err_speed;                  //储存误差
       
       MOTO_pwm += incpid;
       if(MOTO_pwm > 90000)   MOTO_pwm = 90000;     //PWM 限制
       else if(MOTO_pwm <= -70000)      MOTO_pwm = -70000; 
       
       if(MOTO_pwm>=0)
       {       
          MOTO_GO_dty(MOTO_pwm);
          MOTO_BAC_dty(0);
       }
       else
       {
          MOTO_GO_dty(0);
          MOTO_BAC_dty(abs(MOTO_pwm)); 
       }
}


/*************************************************************************
*  函数名称   Speed_analyse
*  功能说明： 速度分析
*  参数说明：    
*                  
*  函数返回： 无
*  修改时间：
*  备    注：
*************************************************************************/
void Speed_analyse()
{
   if(position == 3 && Debug_flag == 0)           //偏差小于一定范围 记录编码器脉冲
    {
       Stright_Count += now_speed;
       if(0 == Stright_Flag && Stright_Count > CODER*7)                       //大于1400个脉冲 时 判定为直道
       {
           Stright_Count = CODER*7;
           Stright_Flag = 1;   
       }
       if(Stright_Count > CODER*8 || abs(chazhi) < 20)  //直道脉冲大于1500时 或偏差小于20时  弯道速度增量清零  用于限定连续弯 速度
       {
           Curve_Count = 0;
           Curve_speed_Add = 0;
       }
    }
    else                 
    {   
        Stright_Flag = 0 ;
    
        if(abs(now_speed - Goal_speed) < 4)   //由直道入弯时  速度减下去之后 才可以在加速
        {
            Curve_Count += now_speed;        //记录弯道编码器脉冲数  
            if(Curve_Count > CODER*2)
            {
              Curve_Count = 0;
              Curve_speed_Add ++;           //进弯之后 逐渐加速
              if( Curve_speed_Add > Speed_Min)       //弯道速度限定
                  Curve_speed_Add = Speed_Min;
           }
        }
        if(position < 1 || position > 5)
        {
           Stright_Count = 0;
           Stright_Flag_jiwan = 0;  //长直道入急弯 标志位清零
        }
    }
    if(1 == Stright_Flag)  Stright_Flag_jiwan = 1;  //只跟随置1   不跟随置0
  
   
    if(Stright_Flag == 1)      //直道速度
    {
        Goal_speed=Stright_speed;
    }
    else if(1 == UPhill_flag)  //上坡  速度
    {
        Goal_speed = Speed_Ramp; 
    }
    else if(1 == Obstruct_flag) // 障碍  速度
    {
        Goal_speed = Speed_Block; 
    }
    else                    //普通弯道速度
    {
         if(position == 3)   Goal_speed = Stright_speed - 10;
         else                Goal_speed = Speed_Max - (int16)((float)(abs(chazhi)*(Speed_Max-Speed_Min)) / 100) ;//- (int16)((range - range_old)*now_speed/2000);
          
         if(Goal_speed > Speed_Max)
             Goal_speed = Speed_Max;
         if(Goal_speed < Speed_Min)
             Goal_speed = Speed_Min;
    }
    
   // LCD_Show_Number(70,1,Goal_speed);
    
}
/*************************************************************************
*  函数名称   Speed_conter
*  功能说明： 速度整天控制
*  参数说明：         
*  函数返回： 
*  修改时间：
*  备    注：
*************************************************************************/
void Speed_conter(void)
{ 
     Err_speed = Goal_speed - cesu; 
     if(Err_speed < -35)
     {
            BangBangBack();
      }
     else if(Err_speed > 50)
     {
           BangBangGO();
     }
     else 
     {
          PID_conter();
     }
}
/*************************************************************************
*  函数名称   BangBang
*  功能说明： 棒棒控制
*  参数说明：         
*  函数返回： 
*  修改时间：
*  备    注：
*************************************************************************/
void BangBangGO(void)   //棒棒控制
{
     MOTO_BAC_dty(0);  
     MOTO_GO_dty(85000); 
     MOTO_pwm =  85000;
}
void BangBangBack(void)   //棒棒控制
{
     MOTO_BAC_dty(50000);  
     MOTO_GO_dty(0); 
     MOTO_pwm = - 50000;  //占空比赋值小一些  使PID切换平滑 迅速减下来
}
/*************************************************************************
*  函数名称   top_car_conter
*  功能说明： 停车控制
*  参数说明：         
*  函数返回： 
*  修改时间：
*  备    注：
*************************************************************************/
void Stop_car_conter(void)
{
    Err_speed = -now_speed; 
    if(Err_speed < -7)
    {
       BangBangBack();
    }
    else
    {
       MOTO_BAC_dty(0);  
       MOTO_GO_dty(0); 
    }
}
/*************************************************************************
*  函数名称   abs
*  功能说明： 求绝对值函数
*  参数说明：         
*  函数返回： |X|
*  修改时间：
*  备    注：
*************************************************************************/
uint16 abs(int16 x)
{
    if(x<0)  return - x;
    else     return x;
}

float abs_f(float x)
{
    if(x<0)  return - x;
    else     return x;
}


/*************************************************************************
*  函数名称   SEND_DATE_LABVIEW
*  功能说明： 上位机发送协议
*  参数说明：         
*  函数返回： 
*  修改时间：
*  备    注：
*************************************************************************/
void SEND_DATE_LABVIEW(int16 *P,int16 rudd,int16 sudu,int16 mubiao)
{
    static   u8  puts[]={"AD0000,AD1000,AD2000,AD3000,RUDDER00000,CESU000,Gole000,"};  
    char     table0[]={'0','1','2','3','4','5','6','7','8','9'};
    int16    i,j,date;
    
     j=3;
     for(i=0;i<4;i++)
     {
        date = *P /100;
        puts[j+i] = table0[date];
        date = *P %100/10;
        puts[j+i+1] = table0[date];
        date = *P % 10;
        puts[j+i+2] = table0[date];
        j +=  6;
        P ++;
     }
    
     date = rudd /10000;
     puts[34] = table0[date];
     date = rudd %10000/1000;
     puts[35] = table0[date];
     date = rudd % 1000/100;
     puts[36] = table0[date];  
     date = rudd % 100/10;
     puts[37] = table0[date];  
     date = rudd % 10;
     puts[38] = table0[date];  
     
     date = sudu /100;
     puts[44] = table0[date];  
     date = sudu % 100/10;
     puts[45] = table0[date];  
     date = sudu % 10;
     puts[46] = table0[date];  
     
     date = mubiao /100;
     puts[52] = table0[date];  
     date = mubiao % 100/10;
     puts[53] = table0[date];  
     date = mubiao % 10;
     puts[54] = table0[date];  
     
     uart_sendStr(UART3,puts);

}

void delay_ms(uint32 x)
{
    int i,j;
    for(i=0;i<x;i++)
      for(j=0;j<200;j++);
}

/*************************************************************************
*  函数名称   Cable_survey
*  功能说明： 检测赛道电流
*  参数说明：          
*  函数返回： 
*  修改时间：
*  备    注： 更换赛道后 打开K6  上电 然后左右移动车子检测赛道  
*             电流  然后进行参数转换  检测完毕后记得关上K6
*************************************************************************/
void Cable_survey(void)
{
    int16 num_count;
    if(K6)
     {
       LCD_Print(4,0,"please wait");
       while(1)
       {  
          Read_ADC();
          if(AD_valu[3] > adc_max) 
          {
            adc_max =AD_valu[3];
            num_count = 0;
          }
          num_count++;
          if(num_count > 10000)
          {
             flash_erase_sector(SECTOR_NUM);       //擦除扇区	
             if(0 == flash_write(SECTOR_NUM,0,adc_max) )//写扇区
             {
                LCD_CLS();  //clean OLED
                break;
             }
             else num_count = 0;
          }
       }
     }    
}
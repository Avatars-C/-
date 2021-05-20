/******************** (C) COPYRIGHT 2011 ********************* ********************
 * �ļ���       ��var.c
 * ����         ���������
 *
 * ʵ��ƽ̨     ��Ұ��kinetis������
 * ��汾       ��
 * Ƕ��ϵͳ     ��
 *
 * ����         ��o����Σ�o 
**********************************************************************************/	
#include "include.h"



int16  adc_max;
///////////////////////////////////////////////
int16  Encoder_Sum;         //�޸Ĳ��� ��¼����
byte   Debug_flag = 1;      // ����ģ
byte   Read_flash_flag = 1; // ��ȡflash ��־λ
int16  Straight_Record_Flag;//�������ܣ�������ɱ�־λ
int16  Stright_speed, Speed_Max, Speed_Min,Speed_Ramp,Speed_Block;
int16  Err_speed,Err_speed_old,Err_speed_oldest;   //�ٶ�PID  ����
int16  Goal_speed;     //Ŀ���ٶ�
int32  MOTO_pwm;
int16  duoji_duty;     //���ռ�ձ�  ���ڷ�����λ��
int16  Turn_mid;
int16  Straight_Distance_Record,Straight_Distance_num,Straight_Distance_num1;  //��¼ֱ������

float Angle_P_Array[6]={5.05,5.1,5.16,5.24,5.34};
int16 Angle_D_Array[6]={40,47,55,63,75};

float Angle_P;  //���P  D
int16 Angle_D;

int16  Stright_Count,Curve_Count;
int16  Stright_Flag = 1,Stright_Flag_jiwan = 1,Curve_Flag;
int16  Curve_speed_Add,Curve_speed_Add_Set;
float  range,range_old;

int16  UPhill_flag = 0,Dowmhill_flag=0,Hill_possible_flag=0,Hill_maichong,Dowmhill_time;  //�µ�
int16  Obstruct_flag=0,Obstruct_time;  //·��

int16 Speed_P_Array[6] = {10,20,30,40,50,60};
uint8 Data_table[4][4]=
{
  // �ٶ�   
  //ֱ �� ��  
   {70,65,55,20}, //�����ٶ�  
   {85,75,55,15}, //�е��ٶ�  
   {90,81,61,10}, //�ϸ��ٶ�   
   {95,85,65,5}  //����ٶ�  
};
uint8 mode_num = 4;




/*************************************************************************
*  ��������   Steer_adjus
*  ����˵���� �����ֵ���ں���
*  ����˵����          
*  �������أ� 
*  �޸�ʱ�䣺
*  ��    ע��  �����ֵ������  �����ѻ��޸Ķ����ֵ 
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
          flash_erase_sector(SECTOR_TURN);       //��������
          flash_write(SECTOR_TURN,0,turn_add);     //����д������
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
*  ��������   Paramet_Adjust
*  ����˵���� ��������
*  ����˵����          
*  �������أ� 
*  �޸�ʱ�䣺
*  ��    ע�� 
*************************************************************************/
void Paramet_Adjust()
{
     static int16   site,site_old;
     int16          yf=0;
     static int16   Free_time=0;
     static int16   stright_speed,speed_Max,speed_Min;
     
     if(Read_flash_flag)
     {
         stright_speed = flash_read(SECTOR_NUM,0,int16);	 //��ȡFlash  
         speed_Max     = flash_read(SECTOR_NUM,4,int16);	
         speed_Min     = flash_read(SECTOR_NUM,8,int16);	
         Speed_Ramp    = flash_read(SECTOR_NUM,12,int16);
         Speed_Block   = flash_read(SECTOR_NUM,16,int16);
         stop_flag_yes = flash_read(SECTOR_NUM,20,uint8);	
         
         Read_flash_flag ++;
         if(Read_flash_flag >=3)  Read_flash_flag =0;     //��ȡ2��Flsah ���־λ����  ���ڶ�ȡFlash 
     }

         if(K2) {    site++;    delay_ms(20);    while(K2);  }           
         if(K1) {    site--;    delay_ms(20);    while(K1);  }
         if(site<0)  site = 25;
         else if(site>25)  site = 0;
         site %= 12;
         
         if(site <= 5)
         LCD_Draw(0,2+site,4);  //�����ʾ
         else 
         LCD_Draw(40,2+site-6,4);  //�����ʾ  
         
         LCD_Print(6,2,"H:");
         LCD_Print(6,4,"L:");
         LCD_Print(6,6,"S:");
         
         Encoder_Sum += now_speed;      //�������޸�����
         if(Encoder_Sum >= CODER/2)       //��ת 100������ ����+1
         {  
             yf++;
             Encoder_Sum=0;
         }
         if(Encoder_Sum <= -(CODER/2))      //��ת100����  ���� -1
         {
            yf--;
            Encoder_Sum=0;
         }
         switch(site)
         {
            case 0:   stright_speed +=yf;     //ֱ���ٶ�
            
                      if(stright_speed < 0)  stright_speed = 0;
                      break;
            case 1:   speed_Max +=yf;         //����

                      if(speed_Max < 0)     speed_Max = 0;
                      break;
            case 2:   speed_Min +=yf;         //����
                      
                      if(speed_Min < 0)   speed_Min = 0;
                      break;   
            case 3:   Speed_Ramp += yf;      //�µ��ٶ�
            
                      if(Speed_Ramp < 0)  Speed_Ramp = 0;
                      break;
            case 4:   Speed_Block += yf;     //·�� �ٶ�
            
                      if(Speed_Block < 0) Speed_Block = 0;
                      break;          
            case 5:   stop_flag_yes += yf;  //ͣ��ѡ��
            
                      if(stop_flag_yes <= 0) stop_flag_yes = 0;
                      else if(stop_flag_yes > 1) stop_flag_yes = 1;
                      break;
            case 6:   mode_num += yf;
            
                      if(mode_num >= 4)       mode_num = 4;
                      else if(mode_num <= 0)  mode_num = 0; 
                      break;
            case 11:  Straight_Record_Flag +=yf;  //����ģʽѡ��
            
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

         LCD_Show_Number(15,2,Stright_speed);         //����OLED��ʾ
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
        
     if(Free_time >= 600)          //ȷ�� �˳�����ģʽ ����д��FLASH  �������д�������ർ��FLASH ��  
     {
         Beer_ON;     
         delay_ms(9000);  
          Beer_OFF; 
         Debug_flag = 0;   //������� ���Ա�־����  
     }
     if(K4)          //ȷ�� �˳�����ģʽ
     {
         Beer_ON;     //������ ��һ��  ��ʾ�����˳�����ģʽ
         delay_ms(18000);            
         flash_erase_sector(SECTOR_NUM);       //����254����	
         flash_write(SECTOR_NUM,0,Stright_speed);     //�����в���д������
         flash_write(SECTOR_NUM,4,Speed_Max);
         flash_write(SECTOR_NUM,8,Speed_Min);
         flash_write(SECTOR_NUM,12,Speed_Ramp);
         flash_write(SECTOR_NUM,16,Speed_Block);
         flash_write(SECTOR_NUM,20,stop_flag_yes);
         Beer_OFF; 
         Debug_flag = 0;   //������� ���Ա�־����  
     }
}
/*************************************************************************
*  ��������   Straight_Record
*  ����˵���� ��¼����ʱ�� ֱ������  ȫ��ǰ��  �Ա��ʡʱ��  
*  ����˵����         
*  �������أ� ��
*  �޸�ʱ�䣺
*  ��    ע�� Straight_Record_Flag = 0  ��������  
              Straight_Record_Flag = 1  ����ֱ������
              Straight_Record_Flag = 2  �ȸ���ǰ��  Ȼ����������ʻ
*************************************************************************/
void Straight_Record()
{
       if(Straight_Record_Flag == 1)           //  ����ֱ������
       {
           if(position > 1 && position < 5)                //ƫ�� ��С ���Ϊ���
           {
              Straight_Distance_Record += now_speed;     //��¼ֱ�� ����
              if(Straight_Distance_Record > CODER)
              { 
                 Straight_Distance_Record = 0;
                 Straight_Distance_num++;
              }
           }
           else   //ƫ�����  ��⵽���
           {
                Straight_Record_Flag = 0;  
                flash_erase_sector(SECTOR_TAB);       //����253����	
                flash_write(SECTOR_TAB,0,Straight_Distance_num);   //ֱ������д��Flash
           }
           Speed_conter(); 
           LCD_Show_Number(90,6,Straight_Distance_num); 
       }
       else if(Straight_Record_Flag == 2)    //��������
       {
           Straight_Distance_num = flash_read(SECTOR_TAB,0,int16);	
           Straight_Distance_Record += now_speed;    //��¼���ߵ�·��
           if(Straight_Distance_Record > CODER)
           { 
               Straight_Distance_Record = 0;
               Straight_Distance_num1++;
           }
           if(Straight_Distance_num1 < (int16)((float)Straight_Distance_num * 0.8) && (position <= 1 || position >= 5))      // ��û�е������  ���� ƫ����һ��Χ֮��  ��ֹ���ٶ�̫�� ���ܳ�����
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
*  ��������   Steer_conter
*  ����˵���� �������
*  ����˵����         
*  �������أ� ��
*  �޸�ʱ�䣺
*  ��    ע��
*************************************************************************/
void Steer_conter(int16 D_value)
{
       static int16 D_value_old = 0;
       uint16 steer_pwmdty;
       int8  speed_tab;
       
       if(Stright_Flag)  //��ֱ��
       {
          Angle_P=7.2;
          Angle_D = 45;
       }
       else
       {     
          if((AD_MAX_NUM ==0 || AD_MAX_NUM == 3) && now_speed > 80 && Stright_Flag_jiwan == 1) //ֱ���뼱��
          {
            Angle_P=8.9+(now_speed - 50)/20;
            Angle_D=300+(now_speed - 50)/3;
          }
          else 
          { 
             speed_tab = (now_speed-50)/10;
             
             if(speed_tab>5)
                   speed_tab=5;    //�����
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
                 Angle_D = 10;   //���ٷ�����
         }
       }
       steer_pwmdty = Turn_mid - ((uint16)(D_value * Angle_P) + (uint16)((D_value - D_value_old) * Angle_D));  
       
       D_value_old = D_value;
       
       if(steer_pwmdty > Turn_mid + TURN_MAX)        steer_pwmdty = Turn_mid + TURN_MAX;
       else if(steer_pwmdty < Turn_mid - TURN_MAX)   steer_pwmdty = Turn_mid - TURN_MAX;
       Steer_duty(steer_pwmdty);
        
       range_old = range;
       range = abs(steer_pwmdty - Turn_mid);  // ������  Ŀ���ٶȷ���  ���ڼӼ���
       
     
}
/*************************************************************************
*  ��������   Special_road_analyse
*  ����˵���� ����·���ļ�� 
*  ����˵����         
*  �������أ� ��
*  �޸�ʱ�䣺
*  ��    ע��  �µ� �ϰ��ļ��
*************************************************************************/
void Special_road_analyse(void)
{
    /////////////////////////////////////�µ�����/////////////////////////////////////////
    if(Slope_AD_1 >= 140 && Slope_AD_2 >= 140 && 0 == UPhill_flag && 0 == Dowmhill_flag && 0 == Obstruct_flag)     //�м�������� �ش�  ���±�־λδ��1  Ҳ����·�ϵ�ʱ����� �����������
    {
        Hill_possible_flag=1;  
    }   
    if(Hill_possible_flag == 1 && Angle_X > 700)   //����������  X��Ƕȱ����  �������������
    {
       UPhill_flag = 1;   
    }
    if(UPhill_flag == 1 && Angle_X < -700)    //�Ѿ�������  X��Ƕ��ֳ��ָ�����   ����ʱ���ۼ�  
    {
        Dowmhill_flag = 1;
    }
    if(Dowmhill_flag )             //��������  ���±�־λ������  �ٶȻָ����� ���¼��� ����ɳ�����   ��Ȼ�����µ��ٶ�^_^
    {
       UPhill_flag =0;
       
    }
    if(Dowmhill_flag)          //������������   ��¼����������
    {
       Hill_maichong += now_speed;
    }
    if(Hill_maichong >= 7000 )     //��Լ����һ����    ���±�־����    �������µ���־λ����   
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
 /////////////////////////·�ϴ���//////////////////////   
    if(0 == Hill_possible_flag && 0 == UPhill_flag && Gyro_X > 700)  //����������  ��X��Ǽ��ٶ� ̫����  ˵�������ϰ�����  �ϰ���־λ��1   ���������  Ҳ���ܵ���·���������
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
*  ��������   PID_conter
*  ����˵���� ���PID����
*  ����˵���� goal_speed    Ŀ���ٶ�    
*             ture_speed    ʵ���ٶ�        
*  �������أ� ��
*  �޸�ʱ�䣺
*  ��    ע��
*************************************************************************/
void PID_conter()
{  
       s16  incpid;
       int16  speed_tab,speed_p;
      
       if(now_speed > 60)        //   �ٶȸ���60 ʱ  
       {
            speed_tab = abs(Err_speed) >> 2; 
            if(speed_tab <= 0)  
                    speed_tab = 0;
            else  if(speed_tab > 5)
                    speed_tab = 5;  //���� ���������
       }
       speed_p = Speed_P + Speed_P_Array[speed_tab];  
        
       incpid = speed_p * Err_speed + Speed_I * (Err_speed - Err_speed_old) + Speed_D * (Err_speed + Err_speed_oldest - 2 * Err_speed_old);
       
       Err_speed_oldest = Err_speed_old;						  
       Err_speed_old = Err_speed;                  //�������
       
       MOTO_pwm += incpid;
       if(MOTO_pwm > 90000)   MOTO_pwm = 90000;     //PWM ����
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
*  ��������   Speed_analyse
*  ����˵���� �ٶȷ���
*  ����˵����    
*                  
*  �������أ� ��
*  �޸�ʱ�䣺
*  ��    ע��
*************************************************************************/
void Speed_analyse()
{
   if(position == 3 && Debug_flag == 0)           //ƫ��С��һ����Χ ��¼����������
    {
       Stright_Count += now_speed;
       if(0 == Stright_Flag && Stright_Count > CODER*7)                       //����1400������ ʱ �ж�Ϊֱ��
       {
           Stright_Count = CODER*7;
           Stright_Flag = 1;   
       }
       if(Stright_Count > CODER*8 || abs(chazhi) < 20)  //ֱ���������1500ʱ ��ƫ��С��20ʱ  ����ٶ���������  �����޶������� �ٶ�
       {
           Curve_Count = 0;
           Curve_speed_Add = 0;
       }
    }
    else                 
    {   
        Stright_Flag = 0 ;
    
        if(abs(now_speed - Goal_speed) < 4)   //��ֱ������ʱ  �ٶȼ���ȥ֮�� �ſ����ڼ���
        {
            Curve_Count += now_speed;        //��¼���������������  
            if(Curve_Count > CODER*2)
            {
              Curve_Count = 0;
              Curve_speed_Add ++;           //����֮�� �𽥼���
              if( Curve_speed_Add > Speed_Min)       //����ٶ��޶�
                  Curve_speed_Add = Speed_Min;
           }
        }
        if(position < 1 || position > 5)
        {
           Stright_Count = 0;
           Stright_Flag_jiwan = 0;  //��ֱ���뼱�� ��־λ����
        }
    }
    if(1 == Stright_Flag)  Stright_Flag_jiwan = 1;  //ֻ������1   ��������0
  
   
    if(Stright_Flag == 1)      //ֱ���ٶ�
    {
        Goal_speed=Stright_speed;
    }
    else if(1 == UPhill_flag)  //����  �ٶ�
    {
        Goal_speed = Speed_Ramp; 
    }
    else if(1 == Obstruct_flag) // �ϰ�  �ٶ�
    {
        Goal_speed = Speed_Block; 
    }
    else                    //��ͨ����ٶ�
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
*  ��������   Speed_conter
*  ����˵���� �ٶ��������
*  ����˵����         
*  �������أ� 
*  �޸�ʱ�䣺
*  ��    ע��
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
*  ��������   BangBang
*  ����˵���� ��������
*  ����˵����         
*  �������أ� 
*  �޸�ʱ�䣺
*  ��    ע��
*************************************************************************/
void BangBangGO(void)   //��������
{
     MOTO_BAC_dty(0);  
     MOTO_GO_dty(85000); 
     MOTO_pwm =  85000;
}
void BangBangBack(void)   //��������
{
     MOTO_BAC_dty(50000);  
     MOTO_GO_dty(0); 
     MOTO_pwm = - 50000;  //ռ�ձȸ�ֵСһЩ  ʹPID�л�ƽ�� Ѹ�ټ�����
}
/*************************************************************************
*  ��������   top_car_conter
*  ����˵���� ͣ������
*  ����˵����         
*  �������أ� 
*  �޸�ʱ�䣺
*  ��    ע��
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
*  ��������   abs
*  ����˵���� �����ֵ����
*  ����˵����         
*  �������أ� |X|
*  �޸�ʱ�䣺
*  ��    ע��
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
*  ��������   SEND_DATE_LABVIEW
*  ����˵���� ��λ������Э��
*  ����˵����         
*  �������أ� 
*  �޸�ʱ�䣺
*  ��    ע��
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
*  ��������   Cable_survey
*  ����˵���� �����������
*  ����˵����          
*  �������أ� 
*  �޸�ʱ�䣺
*  ��    ע�� ���������� ��K6  �ϵ� Ȼ�������ƶ����Ӽ������  
*             ����  Ȼ����в���ת��  �����Ϻ�ǵù���K6
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
             flash_erase_sector(SECTOR_NUM);       //��������	
             if(0 == flash_write(SECTOR_NUM,0,adc_max) )//д����
             {
                LCD_CLS();  //clean OLED
                break;
             }
             else num_count = 0;
          }
       }
     }    
}
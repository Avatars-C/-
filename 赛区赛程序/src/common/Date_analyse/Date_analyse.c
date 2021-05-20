/******************** (C) COPYRIGHT 2011 ********************* ********************
 * 文件名       ：Date_analyse.c
 * 描述         ：电感数据采集与分析
 *
 * 实验平台     ：野火kinetis开发板
 * 库版本       ：
 * 嵌入系统     ：
 *
 * 作者         ：oО殇のＳo 
**********************************************************************************/	
#include "include.h"
#define   NM    8

int16  AD_valu[4],AD_V[4][NM],chazhi,chazhi_old;
float  AD[4];
float  Slope_AD_1,Slope_AD_2;  // 用于坡道检测
int16  max_v[4];
int16  min_v[4];
float  sensor_to_one[4];
int16  AD_sum[4]; 
int16  AD_MAX_NUM;   //
int16  position = 3;
float  max_value,AD_1_max,AD_2_max,AD_0_max,AD_3_max;

int16  ADD1,ADD2;

/*************************************************************************
*  函数名称   SC_black_Init
*  功能说明： 最大值采样
*  参数说明：         
*  函数返回： 无
*  修改时间：
*  备    注：
*************************************************************************/
void SC_black_Init(void)
{
   uint16  i,j;
   if(K2)
   {
       LCD_Print(25,2,"Collecting"); 
       LCD_Print(28,4,"samples..."); 
   
       max_v[0] = max_v[1] = max_v[2] = max_v[3] = 0;
       min_v[0] = min_v[1] = min_v[2] = min_v[3] = 7;  
       for(i=0;i<1200;i++) 
       {
           AD_valu[0] = ad_ave(ADC1,AD9,ADC_10bit,6);  //PTC0  通道      
           AD_valu[1] = ad_ave(ADC1,AD8,ADC_10bit,6);  //PTC1  通道     
           AD_valu[2] = ad_ave(ADC1,AD15,ADC_10bit,6); //PTE25 通道     
           AD_valu[3] = ad_ave(ADC1,AD14,ADC_10bit,6); //PTE24 通道 
       
           for(j=0;j<4;j++) 
           {     
               if(AD_valu[j] > max_v[j])  
               max_v[j] = AD_valu[j];
           }
           delayms(1);           //延时	
       }
       flash_erase_sector(SECTOR_ADM);       //擦除254扇区	
       for(i=0; i<4; i++)
       {
           flash_write(SECTOR_ADM,i*4,max_v[i]);     //讲所有参数写入扇区
       }       
   }
   else 
   {
       for(i=0;i<3;i++)
       { 
          for(j=0;j<4;j++)
          {
             max_v[j] = flash_read(SECTOR_ADM,j*4,int16);
          }
          LCD_Print(29,2,"Reading"); 
          LCD_Print(28,4,"samples..."); 
          delayms(10);           
       }
   }
   LCD_CLS();             
   Beer_ON; 
   delayms(25);  
   Beer_OFF;
}

/*************************************************************************
*  函数名称   Read_ADC
*  功能说明： AD采集
*  参数说明：         
*  函数返回： 无
*  修改时间：
*  备    注：
*************************************************************************/
void Read_ADC(void)
{
     int16  i,j,k,temp;
     int16  ad_valu[4][5],ad_valu1[4],ad_sum[4];
     
     for(i=0;i<5;i++)
     {
         ad_valu[0][i]=ad_ave(ADC1,AD9,ADC_10bit,5);     // ADC0 通道 
         ad_valu[1][i]=ad_ave(ADC1,AD8,ADC_10bit,5);     // ADC0 通道 
         ad_valu[2][i]=ad_ave(ADC1,AD15,ADC_10bit,5);     // ADC0 通道
         ad_valu[3][i]=ad_ave(ADC1,AD14,ADC_10bit,5);     // ADC0 通道 
     }
  //////////////////////冒泡排序///////////////////////////////////
     for(i=0;i<4;i++)          
     {
        for(j=0;j<4;j++)
        {
           for(k=0;k<4-j;k++)
           {
              if(ad_valu[i][k] > ad_valu[i][k+1])  //前面的比后面的大  则进行交换
              {
                 temp = ad_valu[i][k+1];
                 ad_valu[i][k+1] = ad_valu[i][k];
                 ad_valu[i][k] = temp;
              }
           }
        }
     }
     for(i=0;i<4;i++)    //求中间三项的和
     {
        ad_sum[i] = ad_valu[i][1] + ad_valu[i][2] + ad_valu[i][3];       
        ad_valu1[i] = ad_sum[i] / 3;
     }
  ////////////////////////滑动平均滤波/////////////////////////////
     for(i = 0;i < NM-1;i ++)
     {
         AD_V[0][i] = AD_V[0][i + 1];
         AD_V[1][i] = AD_V[1][i + 1];
         AD_V[2][i] = AD_V[2][i + 1];
         AD_V[3][i] = AD_V[3][i + 1];
     }
     for(i=0;i<4;i++)
     {
         AD_V[i][NM-1] =  ad_valu1[i];
     }
    
     for(i = 0;i < NM;i ++)
     {
         AD_sum[0] += AD_V[0][i];
         AD_sum[1] += AD_V[1][i];
         AD_sum[2] += AD_V[2][i];
         AD_sum[3] += AD_V[3][i];
     }
     for(i=0;i<4;i++)  //求平均
     {
         AD_valu[i] = AD_sum[i] / NM;
         AD_sum[i] = 0;   
     }  
     
     ADD1 = ad_ave(ADC1,AD10,ADC_10bit,5);     // ADC0 通道
     ADD2 = ad_ave(ADC1,AD11,ADC_10bit,5);     // ADC0 通道 
     
     LCD_Show_Number(64,1,ADD1);
     LCD_Show_Number(96,1,ADD2);
     
     LCD_Show_Number(1, 0,AD_valu[0]);       //OLED显示 AD数据
     LCD_Show_Number(32,0,AD_valu[1]);
     LCD_Show_Number(64,0,AD_valu[2]);
     LCD_Show_Number(96,0,AD_valu[3]);

}

/*************************************************************************
*  函数名称   Date_analyse
*  功能说明： 数据分析
*  参数说明：         
*  函数返回： 无
*  修改时间：
*  备    注：
*************************************************************************/
void Date_analyse()
{
    int16  i,max=0;  
    static int16 max_old = 2,max_crosstalk = 2;
    static int16 position_last = 3;
    float  sensor_1,sensor_2;
    
    Read_ADC();
    
    /*********************归一化处理********************/
    for(i=0;i<4;i++) 
    {
       sensor_to_one[i] = (float)(AD_valu[i] - min_v[i])/(float)(max_v[i] - min_v[i]); 
       if(sensor_to_one[i]<=0.0)  sensor_to_one[i]=0.001;
       if(sensor_to_one[i]>1.0)  sensor_to_one[i]=1.0; 
       
       AD[i] = 100 * sensor_to_one[i];     //AD[i]为归一化后的值  范围为0-100
    }      

     sensor_1 = (float)(AD_valu[1] - min_v[1])/(float)(max_v[1] - min_v[1]);
     if(sensor_1 <= 0.0)  sensor_1 = 0.001;
     Slope_AD_1 = 100 * sensor_1;
     sensor_2 = (float)(AD_valu[2] - min_v[2])/(float)(max_v[2] - min_v[2]);
     if(sensor_2 <= 0.0)  sensor_2 = 0.001;
     Slope_AD_2 = 100 * sensor_2;
  ////////////////////////////////////////////////////////////////////
    
    for(i=0;i<4;i++)                 //找出最强的传感器
    {  
      if(AD[max]<AD[i]-2)
        max=i;
    } 
    max_value=AD[max]; 
    
    if(max_value < 50)            //丢线时最大值取旧值 
    {
      max=max_old;
      max_value=AD[max]; 
    }
    else
      max_old=max;
    
    if(abs(max - max_crosstalk) <= 2)
    {
         max_crosstalk = max;
    }
    else 
         max = max_crosstalk;
   
    AD_MAX_NUM = max;
    LCD_Show_Number(90,7,max); 
     
    if(max==0 && (AD[1] <= 80))  //已经偏离0号传感器     80为偏离0号传感器时1号传感器的值
    {
          if(AD[1] <= 40)  //偏离很多
          {
            position=0;   
          }         
          else  if(AD[1] >= 43)
          {
            position=1;   
          }
    }   
    else if((max==0 && (AD[1] > 84)) || (max==1 && (AD[0] - AD[2]) > 2 ))         //左侧位置  0-1号传感器之间
    {
          position=2;
          AD_0_max=96;//AD[0];//记录下此时的0号传感器的值;   
    }
    else if((max==1 && (AD[2] - AD[0]) > 2) || (max==2 && (AD[1] - AD[3]) > 2))  //中间位置  1-2号传感器之间
    {
          position=3;
    }
    else if((max==3 && (AD[2] > 86)) || (max==2 && (AD[3] - AD[1]) > 2))         //右侧位置  2-3传感器之间
    {
          position=4;
          AD_3_max=96;//AD[3];//记录下此时的3号传感器的值
    }
    else if(max==3 && (AD[2]<=82))  //已经偏离3号传感器    70为偏离3号传感器时2号传感器的值
    {        
          if(AD[2] <= 40)  //偏离很多
          {
            position=6; 
          }          
          else if(AD[2] >= 43)  
          {
            position=5;   
          }
    }
  //////////////在弯道时 有时位置会跳变 过渡点的值就会不能储存  导致接下来转向不足/////////  
    if(position == 1 && position_last == 3)
    {
       AD_0_max = 100;
    }
    if(position == 5 && position_last == 3)
    {
       AD_3_max = 100;
    }
//////////////////////////////////////////////////////////////////////////////////////////    
    position_last = position;
    
    /*
    if(Stright_Flag)
       chazhi=(int16)((AD[1] - AD[2]) * 1.0);
    else
    {
        switch(position) 
        {
          case 0: chazhi=(float)(abs(AD_0_max-AD[0]) + AD_0_max - AD[3]) * 1.6; break;                     
          case 1: chazhi=(float)(abs(AD_0_max-AD[0]) + AD_0_max - AD[3]) * 1.4; break;   
          case 2: chazhi=(AD[0] - AD[3]) * 1.3; break;                          
          case 3: chazhi=(AD[0] - AD[3]) * 1.1; break;  
          case 4: chazhi=(AD[0] - AD[3]) * 1.3 + 8; break;                                  
          case 5: chazhi=(float)(AD[0] - abs(AD_3_max-AD[3]) - AD_3_max) * 1.4; break;   
          case 6: chazhi=(float)(AD[0] - abs(AD_3_max-AD[3]) - AD_3_max) * 1.6; break;                     
          default:break;
        } 
    }
    chazhi = -chazhi;*/

         LCD_Show_Number(90,6,position); 
    
     //////////////////////////////////////////////////////////////////////////////   
         if(max==1 ||max==2)   //1  2  号电感最大时  
         {
             if(Stright_Flag)
               chazhi = (int16)((AD[2] - AD[1])); 
             else 
               chazhi = (int16)((AD[3] - AD[0]) * 1.1); 
  
         }
         else if(max == 0)    //0号电感最大是 用1号电感值算偏差
         {
              chazhi = (int16)((1/(float)(AD[1]+50))*13000 - 57);  
              chazhi = -(int16)(chazhi*1.78);
        
         } 
         else if(max == 3)   //3号电感最大时 用2号电感值算偏差
         {
            chazhi = (int16)((1/(float)(AD[2]+50))*13000-57);
            chazhi = (int16)(chazhi*1.78);
         }
         
         if(UPhill_flag == 1)
         {
              chazhi = (int16)((AD[2] - AD[1]) * 1.1);
         }
        
          LCD_Show_Number(40, 1,abs(chazhi)); 

    
    

     
}
#include "include.h"
#define GYRO_Sensitivity  16.4  //����ѡ����������ȷ����MPU6050��GYRO�ĸ���������250,500,1000,2000��Ӧ��GYRO_Sensitivity�ֱ�Ϊ131,65.5,32.8,16.4.  

int32   Accel_X,Accel_Y,Accel_Z;
int32   Gyro_X,Gyro_Y,Gyro_Z; 
int16   Gyro_X_Offest=0,Gyro_Y_Offest=0,Gyro_Z_Offest=0;
int16   Angle_AX,Angle_AY,Angle_AZ;
int32   Gyro_XY,Gyro_YX,Gyro_ZZ; 
int16   Angle_X,Angle_Y,Angle_Z;
int32   AngleSigma_X,AngleSigma_Y,AngleSigma_Z;
float   arc_r;
int16   arc_count;
int32   Gyro_Zz[3];


void Offest_Init(void)
{
     uint16 i;  
     if(K4)
     {
        LCD_CLS(); 
        LCD_Print(25,2,"Collecting"); 
        LCD_Print(28,4,"offest..."); 
        Gyro_X_Offest=0;Gyro_Y_Offest=0;Gyro_Z_Offest=0;  //��ƫ��0
     
        for(i=0;i<500;i++) //���Ư���벹��
        { 
             Gyro_X = GetData(GYRO_XOUT_H);	   
             Gyro_Y = GetData(GYRO_YOUT_H);	
             Gyro_Z = GetData(GYRO_ZOUT_H);
             delayms(2);           //��ʱ	
             Gyro_X_Offest += Gyro_X;	
             Gyro_Y_Offest += Gyro_Y;	
             Gyro_Z_Offest += Gyro_Z;  
        } 
        Gyro_X_Offest = Gyro_X_Offest/500;	
        Gyro_Y_Offest = Gyro_Y_Offest/500;	
        Gyro_Z_Offest = Gyro_Z_Offest/500; 
     
        AngleSigma_X=0;AngleSigma_Y=0;AngleSigma_Z=0;  //���ֹ�0
     
        flash_erase_sector(SECTOR_TOP);       //��������	
        flash_write(SECTOR_TOP,0,Gyro_X_Offest);  //д����
        flash_write(SECTOR_TOP,4,Gyro_Y_Offest);  //д����
        flash_write(SECTOR_TOP,8,Gyro_Z_Offest);  //д����
     }
     else 
     { 
          Gyro_X_Offest = flash_read(SECTOR_TOP,0,uint16);		
          Gyro_Y_Offest = flash_read(SECTOR_TOP,4,uint16);		
          Gyro_Z_Offest = flash_read(SECTOR_TOP,8,uint16);	
     } 
     LCD_CLS();      //����
     Beer_ON; 
     delayms(25);  //��ʱ	
     Beer_OFF;          
}

void CarVoltageGet(void)
{
      int32   lnDeltaValue;
      
      Accel_X  = GetData(ACCEL_XOUT_H);     //��ȡX����ٶ�
      Accel_Y  = GetData(ACCEL_YOUT_H);
      Accel_Z  = GetData(ACCEL_ZOUT_H);
      Gyro_X = GetData(GYRO_XOUT_H);	   
      Gyro_Y = GetData(GYRO_YOUT_H);	
      Gyro_Z = GetData(GYRO_ZOUT_H);
        
       /*******************X����̬����************************/               
        if(Accel_Y > 32767)     Accel_Y = Accel_Y - 65535;  
        if(Accel_Y <= -18000)   Accel_Y = -18000;      //���Ƽ��ٶȷ���
        Angle_AY = (int32)(Accel_Y * ACCE_Y_RATIO / 100);   //�Ƕȹ�һ��
   
        if(Gyro_X > 32767)      Gyro_X = Gyro_X - 65535;
	Gyro_XY = (int32)(Gyro_X *GYRO_X_RATIO / 100 );    //���ٶȹ�һ��
      
        Angle_X = (int16)(AngleSigma_X / 20);                    //X��Ƕȼ���
        lnDeltaValue = Angle_AY - Angle_X;                          //�Ƕ�У��
        lnDeltaValue = (int32)(lnDeltaValue * ACCE_XY_RATIO / 100); //�Ƕȷ���
        AngleSigma_X += (Gyro_XY + lnDeltaValue);	            //���ٶȻ��� 
        
       /*******************Y����̬����************************/               
        if(Accel_X > 32767)     Accel_X = Accel_X - 65535;  
        if(Accel_X <= -18000)   Accel_X = -18000;      //���Ƽ��ٶȷ���
        Angle_AX = (int32)(Accel_X * ACCE_X_RATIO / 100);   //�Ƕȹ�һ��
   
        if(Gyro_Y > 32767)      Gyro_Y = Gyro_Y - 65535;
        Gyro_Y = - Gyro_Y;       //��ת ʹ������ٶ�һ��
	Gyro_YX = (int32)(Gyro_Y *GYRO_Y_RATIO / 100 );    //���ٶȹ�һ��
      
        Angle_Y = (int16)(AngleSigma_Y / 20);                    //Y��Ƕȼ���
        lnDeltaValue = Angle_AX - Angle_Y;                          //�Ƕ�У��
        lnDeltaValue = (int32)(lnDeltaValue * ACCE_YX_RATIO / 100); //�Ƕȷ���
        AngleSigma_Y += (Gyro_YX + lnDeltaValue);	            //���ٶȻ��� 

        
     //  SEND((float)Accel_X,(float)Angle_X, (float)Gyro_Z,(float)Angle_Z);

       
        /***************************ת�ǿ���*****************************/  
        
        Gyro_Z = Gyro_Z - Gyro_Z_Offest;  
        if(Gyro_Z > 32767)      Gyro_Z = Gyro_Z - 65535;
        Gyro_Zz[2] = Gyro_Zz[1];
        Gyro_Zz[1] = Gyro_Zz[0];
        Gyro_Zz[0] = Gyro_Z;     
        
        Gyro_Z = (Gyro_Zz[2] + Gyro_Zz[1] + Gyro_Zz[0]) / 3;   //������ƽ��
        
        if(now_speed > 30)
           arc_r = now_speed / ((float)abs(Gyro_Z)/ 175);   //����İ뾶���뾶=�ٶ�/���ٶ�(������)      
        else
            arc_r = 2000;
        if(arc_r>=5 && arc_r<=50)  //�����뾶
        {
              arc_count += now_speed;  //״̬�ۼ�
              if(arc_count>=2500)      //7565����/m 
              {
                  arc_count=2500;
              }
        }
        else
        {    
             arc_count=0;
        }
       /* if(arc_r < 4)  Beer_ON;
        else            Beer_OFF;
        */
        
        if(arc_r < 4 && position != 3)    
        {    
              AngleSigma_Z += Gyro_Z / 16.4 * 0.00991; 
        }
        else
        {
             AngleSigma_Z=0; 
        }

        Angle_Z = AngleSigma_Z;
              
        if(abs(Angle_Z)>250)  //С������ת�ǳ���230���ж�Ϊ��ʮ�ֽ���
        {
         // Beer_ON;
           // chushizi_flag=1;
        }
       // else Beer_OFF;
      
              
       
         
}
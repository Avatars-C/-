/*************************** (C) COPYRIGHT 2011 **********************************
 * 文件名       ：Com_def.h
 * 描述         ：通用配置头文件，适合全部的工程
 *
 * 实验平台     ：野火kinetis开发板
 * 库版本       ：
 * 嵌入系统     ：
 *
 * 作者         ： oО殇のＳo 
**********************************************************************************/	

#ifndef _COM_DEF_H_
#define _COM_DEF_H_


/****************舵机参数配置********************/
#define  TURN_MID   9930                    //舵机中值   左大 右小
#define  TURN_MAX   1100

/***************FLASH数据定义********************/
#define	SECTOR_NUM	254		//参数扇区	
#define	SECTOR_TAB	253	        //特殊模式起跑 记录直道扇区      
#define	SECTOR_ADM	252	        //AD标定     扇区
#define	SECTOR_TOP	251	        //陀螺仪标定 扇区
#define SECTOR_TURN     250             //舵机中值 扇区

/****************舵机控制定义********************/
#define  Steer_init()     FTM_PWM_init(FTM1,CH0,50,TURN_MID);      //转向舵机  50HZ     PA9  
#define  Steer_duty(x)    FTM_PWM_Duty(FTM1,CH0,x);

/****************编码器线数********************/
#define  CODER    200

/****************电机控制配置********************/
#define  MOTO_pwm_init()  FTM_PWM_init(FTM0,CH3,10000,0);  \
                          FTM_PWM_init(FTM0,CH4,10000,40000);  \
                          gpio_init(PORTE,24,GPO,0);
                          //电机正转  10KHZ PA6   电机反转  10KHZ    PA7

#define MOTO_GO_dty(x)    FTM_PWM_Duty(FTM0,CH4, x);
#define MOTO_BAC_dty(x)   FTM_PWM_Duty(FTM0,CH3, x);
   
#define MOTO_enable()            gpio_init(PORTE,24,GPO,1);
#define MOTO_disable()           gpio_init(PORTE,24,GPO,0);

/*****************蜂鸣器配置*********************/
#define  Beer_init()      gpio_init(PORTA,12,GPO,0);
#define  Beer_ON          gpio_set(PORTA,12,1)
#define  Beer_OFF         gpio_set(PORTA,12,0)

/****************开关配置********************/
#define  Stir_init()      gpio_init(PORTA,14,GPI_UP,0);   \
                          gpio_init(PORTA,26,GPI_UP,0);   \
                          gpio_init(PORTA,24,GPI_UP,0);   \
                          gpio_init(PORTA,17,GPI_UP,0);   \
                          gpio_init(PORTB,6,GPI_UP,0);   \
                          gpio_init(PORTB,3,GPI_UP,0); 

#define  K1          !gpio_get(PORTA,14)
#define  K2          !gpio_get(PORTA,26)
#define  K3          !gpio_get(PORTA,24)
#define  K4          !gpio_get(PORTA,17)
#define  K5          !gpio_get(PORTB,6)
#define  K6          !gpio_get(PORTB,3)


/****************速度闭环参数配置***************/
#define   Speed_P 440        //350
#define   Speed_I  410
#define   Speed_D  610


/*************互补滤波姿态解算参数配置*************/ 

    //~~~~~~~~~~~~~~~X轴~~~~~~~~~~~/
#define ACCE_Y_RATIO      19     //Y轴加速度归一化系数        
#define GYRO_X_RATIO      50     //X轴陀螺仪归一化系数
#define ACCE_XY_RATIO     10     //XY轴角度反馈系数 0.45

    //~~~~~~~~~~~~~~~Y轴~~~~~~~~~~~/
#define ACCE_X_RATIO      19     //X轴加速度归一化系数        
#define GYRO_Y_RATIO      50     //Y轴陀螺仪归一化系数
#define ACCE_YX_RATIO     10     //YX轴角度反馈系数 0.45

    //~~~~~~~~~~~~~~~Z轴~~~~~~~~~~~/
#define ACCE_Z_RATIO      19     //Z轴加速度归一化系数        
#define GYRO_Z_RATIO      50     //Z轴陀螺仪归一化系数
#define ACCE_ZZ_RATIO     10     //ZZ轴角度反馈系数 0.45


   
#define  Up_cpu_PID  0             //PID上位机调节




#endif   /* _COM_DEF_H_ */

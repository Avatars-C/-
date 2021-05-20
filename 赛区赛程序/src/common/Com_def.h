/*************************** (C) COPYRIGHT 2011 **********************************
 * �ļ���       ��Com_def.h
 * ����         ��ͨ������ͷ�ļ����ʺ�ȫ���Ĺ���
 *
 * ʵ��ƽ̨     ��Ұ��kinetis������
 * ��汾       ��
 * Ƕ��ϵͳ     ��
 *
 * ����         �� o����Σ�o 
**********************************************************************************/	

#ifndef _COM_DEF_H_
#define _COM_DEF_H_


/****************�����������********************/
#define  TURN_MID   9930                    //�����ֵ   ��� ��С
#define  TURN_MAX   1100

/***************FLASH���ݶ���********************/
#define	SECTOR_NUM	254		//��������	
#define	SECTOR_TAB	253	        //����ģʽ���� ��¼ֱ������      
#define	SECTOR_ADM	252	        //AD�궨     ����
#define	SECTOR_TOP	251	        //�����Ǳ궨 ����
#define SECTOR_TURN     250             //�����ֵ ����

/****************������ƶ���********************/
#define  Steer_init()     FTM_PWM_init(FTM1,CH0,50,TURN_MID);      //ת����  50HZ     PA9  
#define  Steer_duty(x)    FTM_PWM_Duty(FTM1,CH0,x);

/****************����������********************/
#define  CODER    200

/****************�����������********************/
#define  MOTO_pwm_init()  FTM_PWM_init(FTM0,CH3,10000,0);  \
                          FTM_PWM_init(FTM0,CH4,10000,40000);  \
                          gpio_init(PORTE,24,GPO,0);
                          //�����ת  10KHZ PA6   �����ת  10KHZ    PA7

#define MOTO_GO_dty(x)    FTM_PWM_Duty(FTM0,CH4, x);
#define MOTO_BAC_dty(x)   FTM_PWM_Duty(FTM0,CH3, x);
   
#define MOTO_enable()            gpio_init(PORTE,24,GPO,1);
#define MOTO_disable()           gpio_init(PORTE,24,GPO,0);

/*****************����������*********************/
#define  Beer_init()      gpio_init(PORTA,12,GPO,0);
#define  Beer_ON          gpio_set(PORTA,12,1)
#define  Beer_OFF         gpio_set(PORTA,12,0)

/****************��������********************/
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


/****************�ٶȱջ���������***************/
#define   Speed_P 440        //350
#define   Speed_I  410
#define   Speed_D  610


/*************�����˲���̬�����������*************/ 

    //~~~~~~~~~~~~~~~X��~~~~~~~~~~~/
#define ACCE_Y_RATIO      19     //Y����ٶȹ�һ��ϵ��        
#define GYRO_X_RATIO      50     //X�������ǹ�һ��ϵ��
#define ACCE_XY_RATIO     10     //XY��Ƕȷ���ϵ�� 0.45

    //~~~~~~~~~~~~~~~Y��~~~~~~~~~~~/
#define ACCE_X_RATIO      19     //X����ٶȹ�һ��ϵ��        
#define GYRO_Y_RATIO      50     //Y�������ǹ�һ��ϵ��
#define ACCE_YX_RATIO     10     //YX��Ƕȷ���ϵ�� 0.45

    //~~~~~~~~~~~~~~~Z��~~~~~~~~~~~/
#define ACCE_Z_RATIO      19     //Z����ٶȹ�һ��ϵ��        
#define GYRO_Z_RATIO      50     //Z�������ǹ�һ��ϵ��
#define ACCE_ZZ_RATIO     10     //ZZ��Ƕȷ���ϵ�� 0.45


   
#define  Up_cpu_PID  0             //PID��λ������




#endif   /* _COM_DEF_H_ */

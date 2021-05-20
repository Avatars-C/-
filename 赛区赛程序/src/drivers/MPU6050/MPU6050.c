/******************** (C) COPYRIGHT 2011 Ұ��Ƕ��ʽ���������� ********************
 * �ļ���       ��gpio.c
 * ����         ��gpio��������
 *
 * ʵ��ƽ̨     ��Ұ��kinetis������
 * ��汾       ��
 * Ƕ��ϵͳ     ��
 *
 * ����         ��Ұ��Ƕ��ʽ����������
 * �Ա���       ��http://firestm32.taobao.com
 * ����֧����̳ ��http://www.ourdev.cn/bbs/bbs_list.jsp?bbs_id=1008
**********************************************************************************/	

#include "include.h"




void MPU6050_Init()
{  
    I2C_WriteAddr(I2C1,SlaveAddress,PWR_MGMT_1,0x00);	//�������״̬
    I2C_WriteAddr(I2C1,SlaveAddress,SMPLRT_DIV,0x07);
    I2C_WriteAddr(I2C1,SlaveAddress,CONFIG,0x06);
    I2C_WriteAddr(I2C1,SlaveAddress,GYRO_CONFIG,0x18);
    I2C_WriteAddr(I2C1,SlaveAddress,ACCEL_CONFIG,0x01);
}

int GetData(uint8 REG_Address)//�ϳ�����
{
    uint8 H,L;
    H=I2C_ReadAddr(I2C1,SlaveAddress,REG_Address);
    Pause();//��ʱ������������ 
    L=I2C_ReadAddr(I2C1,SlaveAddress,REG_Address+1);
    Pause();//��ʱ������������ 
    return (H<<8)+L;   //�ϳ�����
}

/**************************ʵ�ֺ���********************************************
*����ԭ��:          u8 MPU6050_testConnection
*��������:	    ���MPU6050 �Ƿ��Ѿ�����
*******************************************************************************/
u8 MPU6050_testConnection(void) 
{
   
   if(I2C_ReadAddr(I2C1,SlaveAddress,WHO_AM_I) == 0x68)  //0b01101000;
   return 1;
   else return 0;

}
/**************************ʵ�ֺ���********************************************
*����ԭ��:		void MPU6050_setFullScaleAccelRange(uint8_t range)
*��������:	    ����  MPU6050 ���ٶȼƵ��������
*******************************************************************************/

void MPU6050_setFullScaleAccelRange(uint8_t range) 
{
    
  I2C_WriteAddr(I2C1,SlaveAddress, ACCEL_CONFIG, range);

}
int Abs(int x)
{
    if(x<0)
      x=-x;
    else
      x=x;
    return(x);
}
 
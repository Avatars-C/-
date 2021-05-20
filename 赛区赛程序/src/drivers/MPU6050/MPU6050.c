/******************** (C) COPYRIGHT 2011 野火嵌入式开发工作室 ********************
 * 文件名       ：gpio.c
 * 描述         ：gpio驱动函数
 *
 * 实验平台     ：野火kinetis开发板
 * 库版本       ：
 * 嵌入系统     ：
 *
 * 作者         ：野火嵌入式开发工作室
 * 淘宝店       ：http://firestm32.taobao.com
 * 技术支持论坛 ：http://www.ourdev.cn/bbs/bbs_list.jsp?bbs_id=1008
**********************************************************************************/	

#include "include.h"




void MPU6050_Init()
{  
    I2C_WriteAddr(I2C1,SlaveAddress,PWR_MGMT_1,0x00);	//解除休眠状态
    I2C_WriteAddr(I2C1,SlaveAddress,SMPLRT_DIV,0x07);
    I2C_WriteAddr(I2C1,SlaveAddress,CONFIG,0x06);
    I2C_WriteAddr(I2C1,SlaveAddress,GYRO_CONFIG,0x18);
    I2C_WriteAddr(I2C1,SlaveAddress,ACCEL_CONFIG,0x01);
}

int GetData(uint8 REG_Address)//合成数据
{
    uint8 H,L;
    H=I2C_ReadAddr(I2C1,SlaveAddress,REG_Address);
    Pause();//延时，否则无数据 
    L=I2C_ReadAddr(I2C1,SlaveAddress,REG_Address+1);
    Pause();//延时，否则无数据 
    return (H<<8)+L;   //合成数据
}

/**************************实现函数********************************************
*函数原型:          u8 MPU6050_testConnection
*功　　能:	    检测MPU6050 是否已经连接
*******************************************************************************/
u8 MPU6050_testConnection(void) 
{
   
   if(I2C_ReadAddr(I2C1,SlaveAddress,WHO_AM_I) == 0x68)  //0b01101000;
   return 1;
   else return 0;

}
/**************************实现函数********************************************
*函数原型:		void MPU6050_setFullScaleAccelRange(uint8_t range)
*功　　能:	    设置  MPU6050 加速度计的最大量程
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
 
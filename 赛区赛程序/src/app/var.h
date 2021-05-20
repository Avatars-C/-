#ifndef __var_h
#define __var_h

#include "common.h"


extern int16  Goal_speed;
extern byte   Debug_flag;
extern int16  Stright_speed, Speed_Max, Speed_Min;
extern int16  Straight_Record_Flag;
extern int16  Angle_X;
extern int16  UPhill_flag;
extern int16  Stright_Flag_jiwan;

void Cable_survey(void);
void Steer_adjust(void);
void Paramet_Adjust();
void Straight_Record();
void Special_road_analyse(void);
void Steer_conter(int16 D_value);
void PID_conter();
void Speed_conter(void);
void Speed_analyse();
void BangBangGO(void);
void BangBangBack(void);
void Stop_car_conter(void);
uint16 abs(int16 x);
float abs_f(float x);
void SEND_DATE_LABVIEW(int16 *P,int16 rudd,int16 sudu,int16 mubiao);
void delay_ms(uint32 x);

#endif
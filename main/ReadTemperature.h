#ifndef READTEMPERATURE_H
#define READTEMPERATURE_H
//#include <LiquidCrystal.h>
//#include <DallasTemperature.h>
#include <OneWire.h>
#include"Arduino.h"
#include"MyTimer1.h"
#define DQ A4
#define NOP do { __asm__ __volatile__ ("nop"); } while (0)

class readtemperature {
  public:
    readtemperature();//构造函数
    float readT();//读取当前温度
    void setAlarmTemp(float temph, float templ);//设置报警温度
    bool alarm();//温度报警
    void interface();//在主函数中添加计时器
    void Tmode(int M);//同步主函数的mode
  private:
    byte temp[9];//温度寄存器中的内容
    byte address[8];//rom地址
    bool printTip;//控制温度报警的提示输出
    MyTimer timeral;//控制alarm函数中读取温度的频率
    float alarm_temp;//报警温度
    float tempeN;//当前温度（两秒更新一次）
    int mode;
};

#endif

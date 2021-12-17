#ifndef HUMIDITY_H
#define HUMIDITY_H
#include"MyTimer1.h"
#include"Arduino.h"
#define anapin A0
//#define alarmpin 3

class humidity {
  public:
    humidity();//构造函数
    float readHI();//读取湿度
    void setAlarm(int first , int second);//设置湿度高限
    bool alarm();//响铃
    void Hmode(int M);//同步主函数的运行模式
  private:
    float alarmHI;//湿度报警上限
    int mode;//同步主函数的运行模式
    bool printTip;//控制是否输出提示
};
#endif
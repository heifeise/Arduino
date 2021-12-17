#ifndef _CLOCK_H_
#define _CLOCK_H_
#include "MyTimer1.h"
class myClock {
  public:
    myClock();
    void clock_run();//以不同的模式运行
    void work();//正常工作
    void modefy_second();//计时
    void modefy_minute();//当按下按钮时或计时满秒时修改分钟
    void modefy_hour();
    void set_alarm();//设置闹钟
    bool alarm();//响铃
    int setbutton1();//确定按钮A2是否被按下
    int setbutton2();//确定按钮A3是否被按下
    void set_mode(int M) {
      mode = M;
    };//同步主函数中的运行模式
    void disPlay();//时间显示
    void output(int num, int select); //数码管显示
    void interface()//向主函数的终端服务程序中添加计时器
    {
      timer.modefyTime();
      timerb1.modefyTime();
      timerb2.modefyTime();
      timerExt_Bri.modefyTime();
    }
    void PrintTime(int H = -1, int M = -1, int S = -1); //打印当前时间
    void setNTime(int TimeArray[]);//利用输入的命令修改时间
    void setAlarm(int TimeArra[]);//通过命令修改闹钟
    bool extinguish;//全灭
    bool fullbright;//全亮
  private:
    MyTimer timer;//计时器，以秒为单位，修改时间
    MyTimer timerb1;//计时器，用以按钮A2消抖
    MyTimer timerb2;//计时器，用以按钮A3消抖
    MyTimer timerExt_Bri;//计时器，控制全亮和全灭状态的持续时间
    int buttonpin1;//加操作
    int buttonpin2;//减操作
    int laststate1;//A2的上一状态
    int laststate2;//A3的上一状态
    bool flag1;//A2消抖用
    bool flag2;//A3消抖用
    int second;
    int lastsecond;
    int minute;
    int hour;
    unsigned char NUM[11] = {0xc0, 0xf9, 0xa4, 0xb0, 0x99, 0x92, 0x82, 0xf8, 0x80, 0x90, 0xff}; //0-9,全灭
    unsigned char SELECTED[5] = {0x00, 0x01, 0x02, 0x04, 0x08};//片选
    int alarm_minute;//闹钟时间
    int alarm_hour;//闹钟时间
    bool printTip;
    int mode;//运行模式
    const static int SDI = 8; //输入
    const static int SFTCLK = 7; //移位,低电平使能
    const static int LCHCLK = 4; //锁存，低电平使能
};
#endif

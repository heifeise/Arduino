#ifndef _MYTIMER1_
#define _MYTIMER1_
class MyTimer {
  public:
    MyTimer(int _times);//构造函数，设置要求的定时时间，设置运行状态
    MyTimer() {};
    void initialset(int _times);
    bool timerState();//查看定时状态
    void reStart();//重置软定时器
    void modefyTime();//累加计数
    void stopwork();//手动停止
  private:
    int current;//累计计数（计时），以毫秒为单位
    int times;//要求的定时时间
    bool runState;//工作状态,停止工时为TRUE
};
#endif

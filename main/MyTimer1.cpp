#include"MyTimer1.h"
#include"Arduino.h"
MyTimer::MyTimer(int _times)
{
  current = 0;
  times = _times;
  runState = false;
}

void MyTimer::initialset(int _times)
{
  current = 0;
  times = _times;
  runState = false;
}
bool MyTimer::timerState()
{
  return runState;
}

void MyTimer::reStart()
{
  runState = false;
  current = 0;
}

void MyTimer::modefyTime()
{
  if (!runState)
  {
    current++;
    if (times - current < 1)
    {
      runState = true; //停止软计时器工作

    }
  }
}

void MyTimer::stopwork()
{
  current = 0;
  runState = true;
}

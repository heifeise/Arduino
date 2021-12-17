#include"clock.h"
#include"Arduino.h"
myClock::myClock()
{
  int lastsecond = -1;
  int second = 0;
  int minute = 0;
  int hour = 0;
  flag1 = false;
  flag2 = false;
  printTip = true;
  buttonpin1 = A2;
  buttonpin2 = A3;
  alarm_minute = 0;
  alarm_hour = 0;
  extinguish = false;
  fullbright = false;
  pinMode(buttonpin1, INPUT_PULLUP);
  pinMode(buttonpin2, INPUT_PULLUP);
  pinMode(SDI, OUTPUT);
  pinMode(SFTCLK, OUTPUT);
  pinMode(LCHCLK, OUTPUT);
  //  digitalWrite(alarmpin,HIGH);
  //函数外不可初始化，手动设置初始状态
  timer.initialset(1000);
  timerb1.initialset(50);
  timerb2.initialset(50);
  timerExt_Bri.initialset(10000);
}

void myClock::work()
{

  if (timer.timerState())
  {
    modefy_second();
    timer.reStart();
  }
}

void myClock::modefy_second()
{
  second++;
  if (second == 60)
  {
    second = 0;
    modefy_minute();
  }
}

void myClock::modefy_minute()
{

  if (mode != 2)
  {
    minute++;
    if (minute == 60)
    {
      minute = 0;
      modefy_hour();
    }
  }
  else if (mode == 2)
  {
    int a = setbutton1();
    int b = setbutton2();
    minute += a + b;
    if (minute == 60)
    {
      minute = 0;
      modefy_hour();
    }
    else if (minute == -1)
    {
      minute = 59;
    }
  }
}

void myClock::modefy_hour()
{
  if (mode != 1)
  {
    hour++;
    if (hour == 24)
    {
      hour = 0;
    }
  }
  else if (mode == 1)
  {
    int a = setbutton1();
    int b = setbutton2();
    hour += a + b;
    if (hour == 24)
    {
      hour = 0;
    }
    else if (hour == -1)
    {
      hour = 23;
    }
  }
}



int myClock::setbutton1()
{
  int buttonState = digitalRead(buttonpin1);
  if (buttonState == HIGH) //未按下
  {
    laststate1 = buttonState;
    flag1 = false;
  }
  if (buttonState == LOW && laststate1 == HIGH && !flag1) //下降沿
  {
    timerb1.reStart();
    flag1 = true;
  }
  if (buttonState == LOW && laststate1 == HIGH && timerb1.timerState() && flag1) //确定被按下
  {
    laststate1 = buttonState;
    return 1;
  }
  return 0;
}

int myClock::setbutton2()
{
  int buttonstate = digitalRead(buttonpin2);
  if (buttonstate == HIGH)
  {
    laststate2 = HIGH;
    flag2 = false;
  }
  if (buttonstate == LOW && laststate2 == HIGH && !flag2)
  {
    flag2 = true;
    timerb2.reStart();
  }
  if (timerb2.timerState() && buttonstate == LOW && laststate2 == HIGH && flag2)
  {
    laststate2 = buttonstate;
    return -1;
  }
  return 0;
}

void myClock::set_alarm()
{
  if (mode == 3)//设置小时
  {
    int a = setbutton1();
    int b = setbutton2();
    alarm_hour += a + b;
    if (alarm_hour == 24)
      alarm_hour = 0;
    if (alarm_hour == -1)
      alarm_hour = 23;
  }
  if (mode == 4)//设置分钟
  {
    int a = setbutton1();
    int b = setbutton2();
    alarm_minute += a + b;
    if (alarm_minute == 60)
      alarm_minute = 0;
    if (alarm_minute == -1)
      alarm_minute = 59;
  }

}

bool myClock::alarm()
{

  if (mode == 0)
  {
    if (alarm_minute == minute && alarm_hour == hour)
    {
      if (printTip)
      {
        Serial.println("the alarm time has come!");
        printTip = false;
      }
      return true;
    }
    printTip = true;
  }
  return false;
}

void myClock::disPlay()
{
  static bool flag = false;
  if (extinguish)
  {
    if (!flag)
    {
      timerExt_Bri.reStart();
      flag = true;
    }
    output(10, 1);
    output(10, 2);
    output(10, 3);
    output(10, 4);
    if (timerExt_Bri.timerState())
    {
      extinguish = false;
      flag = false;
    }
  }
  else if (fullbright)
  {
    if (!flag)
    {
      timerExt_Bri.reStart();
      flag = true;
    }
    output(8, 1);
    output(8, 2);
    output(8, 3);
    output(8, 4);
    if (timerExt_Bri.timerState())
    {
      fullbright = false;
      flag = false;
    }
  }
  else
  {
    int tem_hour, tem_minute;
    if (mode == 3 || mode == 4)
    {
      tem_hour = alarm_hour;
      tem_minute = alarm_minute;
    }
    else
    {
      tem_hour = hour;
      tem_minute = minute;
    }
    int times[4] = {tem_hour / 10, tem_hour % 10, tem_minute / 10, tem_minute % 10};
    output(times[0], 1);
    output(times[1], 2);
    output(times[2], 3);
    output(times[3], 4);
  }
}

void myClock::output(int num, int sel)
{
  unsigned char number = NUM[num];
  unsigned char Select = SELECTED[sel];
  const static int n = 8;

  for (int i = 0; i < 8; i++)
  {
    bool tem = 0x80 & number;
    digitalWrite(SDI, tem);
    number <<= 1;
    digitalWrite(SFTCLK, 0);
    digitalWrite(SFTCLK, 1);
  }
  for (int i = 0; i < 8; i++)
  {

    bool tem = 0x80 & Select ;
    digitalWrite(SDI, tem);
    Select <<= 1;
    digitalWrite(SFTCLK, 0);
    digitalWrite(SFTCLK, 1);
  }
  digitalWrite(LCHCLK, 0);
  digitalWrite(LCHCLK, 1);
}

void myClock::PrintTime(int H, int M, int S)
{
  if (H == -1 || M == -1 || S == -1)
  {
    H = hour;
    M = minute;
    S = second;
  }
  Serial.println("the time is:");
  if (H < 10)
    Serial.print(0);
  Serial.print(H);
  Serial.print(":");
  if (M < 10)
    Serial.print(0);
  Serial.print(M);
  Serial.print(":");
  if (S < 10)
    Serial.print(0);
  Serial.println(S);
}

void myClock::setNTime(int TimeArray[])//利用输入的命令修改时间
{
  hour = TimeArray[0];
  minute = TimeArray[1];
  second = TimeArray[2];
  Serial.println("success in modefying the time!Current time:");
  PrintTime();
}

void myClock::setAlarm(int TimeArray[])//通过命令修改闹钟
{
  alarm_hour = TimeArray[0];
  alarm_minute = TimeArray[1];
  Serial.println("success in modefying the alarm_time!Alarm_time:");
  PrintTime(alarm_hour, alarm_minute, 0);
}

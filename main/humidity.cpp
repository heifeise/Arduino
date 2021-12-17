#include"humidity.h"
humidity::humidity()//构造函数
{
  alarmHI = 101.0;
  printTip = true;
  pinMode(anapin, INPUT);
}

float humidity::readHI()//读取湿度
{
  return (analogRead(anapin) / 1023.0) * 100; //(返回湿度百分数)
}
void humidity::setAlarm(int first , int second)//设置湿度高限
{
  alarmHI = first + second * 0.01;
  Serial.print("success!");
  Serial.print("the alarm humidity is:");
  Serial.println(alarmHI);
}

bool humidity::alarm()//响铃
{
  if (mode != 0)
    return false;
  if (readHI() - alarmHI > 1e-6)
  {
    if (printTip)
    {
      Serial.println("the humidity is over the limitation!");
      printTip = false;
    }
    return true;
  }
  else
    printTip = true;
  return false;
}

void humidity::Hmode(int M)//同步主函数的运行模式
{
  mode = M;
}
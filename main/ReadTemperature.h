#ifndef READTEMPERATURE_H
#define READTEMPERATURE_H
#include <LiquidCrystal.h>
#include <DallasTemperature.h>
#include <OneWire.h>
#include"Arduino.h"
#include"MyTimer1.h"
#define DQ A4
#define alarmpin 3//蜂鸣器引脚
#define button3 A3
//OneWire G_onewire(DQ);
DallasTemperature G_temp(new OneWire(DQ));
class readtemperature {
  public:
    readtemperature();
    float readT();
    void setAlarmTemp(int temp);
    void alarm();
    bool buttonPressed();//确定按钮被按下
    void interface();
   private:
    int alarm_temp;
    bool alarm_flag;
    int laststate;
    bool flag;
    bool printTip;
    MyTimer timerb;
 };
readtemperature::readtemperature()
{
   G_temp.begin();
   int alarm_temp = 9999;
   alarm_flag = false;
   laststate = HIGH;
   flag = false;
   printTip = false;
   pinMode(alarmpin,OUTPUT);
   pinMode(button3,INPUT_PULLUP);
   digitalWrite(alarmpin,HIGH);
   timerb.initialset(50);
}

bool readtemperature::buttonPressed()//确定A1是否被按下
{
  int buttonState = digitalRead(button3);
  if (buttonState == HIGH) //未按下
  {
    laststate = buttonState;
    flag = false;
  }
  if (buttonState == LOW && laststate == HIGH && !flag) //下降沿
  {
    timerb.reStart();
    flag = true;
  }
  if (buttonState == LOW && laststate == HIGH && timerb.timerState() && flag) //确定被按下
  {
    laststate = buttonState;
    return true;
  }
  return false;
}

void readtemperature::interface()
{
  timerb.modefyTime();
}

float readtemperature::readT()
{
  G_temp.requestTemperatures();
  return G_temp.getTempCByIndex(0);
}

void readtemperature::setAlarmTemp(int temp)
{
  alarm_temp = temp;
}

void readtemperature::alarm()
{
  if(readT()-alarm_temp >= 0&&alarm_flag)
  {
    digitalWrite(alarmpin,LOW);
    if(printTip)
    {
      Serial.println("temperature is over the limitation!");
      printTip = false;
    }
  }
  if(buttonPressed())
  {
    alarm_flag = !alarm_flag;
    printTip = alarm_flag;
  }
}
#endif

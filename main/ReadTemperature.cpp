#include"ReadTemperature.h"
readtemperature::readtemperature()
{
  alarm_flag = false;
  laststate = HIGH;
  flag = false;
  printTip = false;
  alarm_temp = 999;
  tempeN = readT();
  mode = 0;
  pinMode(alarmpin, OUTPUT);
  pinMode(button3, INPUT_PULLUP);
  digitalWrite(alarmpin, HIGH);
  timerb.initialset(50);
  timeral.initialset(2000);
  OneWire DSB(DQ);
  DSB.reset();
  if(!DSB.search(address))//记录rom地址
  {
    Serial.println("not found advice!");
  }
  if(OneWire::crc8(address,7) != address[7])//地址校验
  {
    Serial.println("address error!");
  }
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

void readtemperature::interface()//在主函数中添加计时器
{
  timerb.modefyTime();
  timeral.modefyTime();
}

float readtemperature::readT()//读取当前温度
{
  OneWire DSB(DQ);
  DSB.reset();
  DSB.select(address);
  DSB.write(0x44);//开始转换温度命令(0X44)
  NOP;
  DSB.depower();//寄生电源供电
  NOP;
  DSB.reset();
  DSB.select(address);
  DSB.write(0xBE);//读存储器命令(0XBE)
  for(int i = 0; i < 9; i++)
  {
    temp[i] = DSB.read();
  }
  if(OneWire::crc8(temp,8) != temp[8])
  {
    Serial.println("temperature error!");
    return -273;
  }
  if(temp[1] == 0x01)
    return (temp[0]|temp[1]<<8)*0.0625;
  else
    return 0-(temp[0]|temp[1]<<8)*0.0625;
}

void readtemperature::setAlarmTemp(float temph, float templ)
{
  alarm_temp = temph + templ*0.01;
  Serial.println("success!");
  Serial.print("the alarm temperature is:");
  Serial.println(alarm_temp);
}

void readtemperature::alarm()
{
  if(mode != 0 )
    return;
  if(timeral.timerState())
  {
    tempeN = readT();
    timeral.reStart();
  }
  if (tempeN - alarm_temp >= 1e-6 && alarm_flag)
  {
    digitalWrite(alarmpin, LOW);
    if (printTip)
    {
      Serial.println("temperature is over the limitation!");
      printTip = false;
    }
  }
  if (buttonPressed())
  {
    alarm_flag = !alarm_flag;
    printTip = alarm_flag;
  }
}

void readtemperature::Tmode(int M)//同步主函数的mode
{
  mode = M;
}

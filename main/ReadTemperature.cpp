#include"ReadTemperature.h"
readtemperature::readtemperature()
{
  printTip = true;
  alarm_temp = 999;
  tempeN = readT();
  mode = 0;
  timeral.initialset(2000);
  OneWire DSB(DQ);
  DSB.reset();
  if (!DSB.search(address)) //记录rom地址
  {
    Serial.println("not found advice!");
  }
  if (OneWire::crc8(address, 7) != address[7]) //地址校验
  {
    Serial.println("address error!");
  }
}

void readtemperature::interface()//在主函数中添加计时器
{
  //  timerb.modefyTime();
  timeral.modefyTime();
}

float readtemperature::readT()//读取当前温度
{
  if (timeral.timerState())//两秒读一次温度
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
    for (int i = 0; i < 9; i++)
    {
      temp[i] = DSB.read();
    }
    if (OneWire::crc8(temp, 8) != temp[8])
    {
      Serial.println("temperature error!");
      return -273;
    }
    if (temp[1] == 0x01)
      tempeN = (temp[0] | temp[1] << 8) * 0.0625;
    else
      tempeN =  0 - (temp[0] | temp[1] << 8) * 0.0625;
    timeral.reStart();
  }
  return tempeN;
}

void readtemperature::setAlarmTemp(float temph, float templ)//设置温度报警高限
{
  alarm_temp = temph + templ * 0.01;
  Serial.println("success!");
  Serial.print("the alarm temperature is:");
  Serial.println(alarm_temp);
}

bool readtemperature::alarm()//响铃
{
  if (mode != 0 )
    return false;
  if (readT() - alarm_temp >= 1e-6)
  {
    if (printTip)
    {
      Serial.println("temperature is over the limitation!");
      printTip = false;
    }
    return true;
  }
  printTip = true;
  return false;
}

void readtemperature::Tmode(int M)//同步主函数的mode
{
  mode = M;
}

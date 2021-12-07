#include"command_read.h"
#include"Arduino.h"
command::command()
{
  getcommand = false;//default: false
}

bool command::read_command()
{
  if (Serial.available())
  {
    Serial.readBytes(command_pack, PACK_LENGTH);
    unsigned char tem = 0x00;
    for (int i = 0; i < 5; i++)
      tem ^= command_pack[i];
    Serial.print("\n//------------------\nMark is:");
    Serial.print(tem);
    Serial.print("(decimal)\n//------------------\n");
    if (command_pack[0] == 0xa0 && check_command())
    {
      getcommand = true;
      return true;
    }
  }
  return false;
}

bool command::check_command()
{
  unsigned char result = 0;
  for (int i = 0; i < PACK_LENGTH - 1; i++)
    result ^= command_pack[i];
  if (result == command_pack[PACK_LENGTH - 1])
    return true;
  return false;
}

int command::analyse_command()
{
  if (getcommand)
  {
    getcommand = false;
    switch (command_pack[1])//分析有效命令的类别,在需要时调用内置函数获取命令中的有效数据
    {
      case 0x01: return 1;
      case 0x02: getArray();return 2;
      case 0x03: getArray();return 3;
      case 0x04: return 4;
      case 0x05: return 5;
      case 0x06: return 6;
      case 0x07: getArray();return 7;
      case 0x08: return 8;
      case 0x09: getArray();return 9;
      default: return -1;
    }
  }
}

void command::getArray()//获取有效数据
{
  NumArray[0] = changeH(command_pack[2]);
  NumArray[1] = changeH(command_pack[3]);
  NumArray[2] = changeH(command_pack[4]); 
}

int command::changeH(unsigned char ch)//将两个四位16进制数各自转化为int型的十进制数后，前一个数字乘以10与后一个相加
{
  int j = 3;
  int sum = 0;
  int zero = 0;
  int a = (ch&0xf0)>>4|zero;
  int b = ch&0x0f|zero;
  sum = a*10+b;
  return sum;
}

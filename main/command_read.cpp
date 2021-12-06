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
    switch (command_pack[1])
    {
      case 0x01: return 1;
      case 0x02: get_time_array();return 2;
      case 0x03: get_time_array();return 3;
      case 0x04: return 4;
      case 0x05: return 5;
      case 0x06: return 6;
      default: return -1;
    }
  }
}

void command::get_time_array()
{
  TimeArray[0] = changeB(command_pack[2]);
  TimeArray[1] = changeB(command_pack[3]);
  TimeArray[2] = changeB(command_pack[4]); 
}

int command::changeB(unsigned char ch)
{
  int j = 3;
  int sum = 0;
  int zero = 0;
  int a = (ch&0xf0)>>4|zero;
  //Serial.print("a:");
  //Serial.println(a);
  int b = ch&0x0f|zero;
  sum = a*10+b;
  return sum;
}

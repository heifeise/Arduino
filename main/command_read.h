/*
  读时间
  a0 01 00 00 00 a1
  b0 01 hour minute second ^

  设定时间：
  a0 02 hour minute second ^
  b0 02 true/false 00 00 ^
  设定闹钟：
  a0 03 hour minute second ^
  b0 03 true/false 00 00 ^
  数码管全灭：
  a0 04 00 00 00 a4
  b0 ------
  数码管全亮：
  a0 05 00 00 00 a5
  0b ------
  读取温度：
  a0 06 00 00 00 a6
  0b 06 HIGH LOW 00 ^
  设定温度报警高限：
  0xa0
  读取湿度：
  a0 07 00 00 00 a7

  设置湿度报警高限：
  a0 08 00 00 00 a8

*/
//－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－
#ifndef COMMAND_READ_H
#define COMMAND_READ_H
#define PACK_LENGTH 6
#include"Arduino.h"
class command {
  public:
    command();
    bool read_command();
    int analyse_command();
    bool check_command();
    unsigned char product_check();
    void send_tip();
    void interface()
    {
      Serial.setTimeout(5);
    }
    void work()
    {
      read_command();
      send_tip();
    }
    void get_time_array();
    int changeB(unsigned char ch);
    int TimeArray[3];
  private:
    unsigned char command_pack[50];
    bool getcommand;//default: false
};
#endif

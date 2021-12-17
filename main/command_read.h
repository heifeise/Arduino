/*
  读时间
  0xa0 01 00 00 00 a1
  b0 01 hour minute second ^
  -------------------------
  设定时间：
  a0 02 hour minute second ^
  -------------------------
  设定闹钟：
  a0 03 hour minute second ^
  ------------------------
  数码管全灭：
  a0 04 00 00 00 a4
  ------------------------
  数码管全亮：
  a0 05 00 00 00 a5
  ------------------------
  读取温度：
  a0 06 00 00 00 a6
  ------------------------
  设定温度报警高限：
  a0 07 temH temL 00 ^
  ------------------------
  读取湿度：
  a0 08 00 00 00 a8

  设置湿度报警高限：
  a0 09 HIMDH HIMIDL 00 a9

*/

/*－－－－－－－－－－－－－命令示例－－－－－－－－－－－－－－－－－－－－－－－
  读时间
  0xa0 01 00 00 00 a0
  b0 01 hour minute second ^
  -------------------------
  设定时间：（12:30：23）
  a0 02 12 30 23 a3
  -------------------------
  设定闹钟：（12:31）
  a0 02 12 31 23 a2
  ------------------------
  数码管全灭：(持续10秒)
  a0 04 00 00 00 a4
  ------------------------
  数码管全亮：(持续10秒)
  a0 05 00 00 00 a5
  ------------------------
  读取温度：
  a0 06 00 00 00 a6
  ------------------------
  设定温度报警高限：（20.33）
  a0 07 20 33 00 b4
  ------------------------
  读取湿度：
  a0 08 00 00 00 a8

  设置湿度报警高限：(68.97)
  a0 09 68 97 00 56
  －－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－*/
#ifndef COMMAND_READ_H
#define COMMAND_READ_H
#define PACK_LENGTH 6
#include"Arduino.h"
class command {
  public:
    command();
    bool read_command();//读命令
    int analyse_command();//解析命令
    bool check_command();//校验命令
    void interface()
    {
      Serial.setTimeout(5);
    }
    void getArray();//获取命令中的有效数据
    int changeH(unsigned char ch);//将16进制数转化为十进制
    int NumArray[3];//存储命令中的有效数据（数据包的2,3,4位置）
  private:
    unsigned char command_pack[50];//存储读取到的命令
    bool getcommand;//default: false
};
#endif

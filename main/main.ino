#include <MsTimer2.h>
#include"MyTimer1.h"
#include"clock.h"
#include"command_read.h"
#include "ReadTemperature.h"

#define buttonpin A1//调整运行模式
#define LED1 10//四个指示灯
#define LED2 11
#define LED3 12
#define LED4 13
const static int SDI = 8; //输入
const static int SFTCLK = 7; //移位,低电平使能
const static int LCHCLK = 4; //锁存，低电平使能
unsigned char NUM[13] = {0xc0, 0xf9, 0xa4, 0xb0, 0x99, 0x92, 0x82, 0xf8, 0x80, 0x90, 0xff, 0x7f, 0xc6}; //0-9,全灭
unsigned char SELECTED[5] = {0x00, 0x01, 0x02, 0x04, 0x08};//片选,第一个凑数

bool flag = false;//用以按钮A1消抖
bool tempflag = false;//用于显示温度
bool tempflag1 = false;//用于显示温度
int ledState = HIGH;
int laststate = HIGH;//按钮A1的上一个状态
int MODE = 0;//0 1 2 3 4五种模式
int lastMode = 4;//上一个运行模式

myClock myclock;//时钟类
MyTimer ledTimer(500);//用以led控制
MyTimer timerb(50);//用以按钮A1消抖
MyTimer timertempera(10000);//控制温度持续显示时间

command com;//命令读取
readtemperature readTem;

void setup()
{
  for (int i = LED1; i <= LED4; i++)
  {
    pinMode(i, OUTPUT);
    digitalWrite(i, HIGH);
  }
  pinMode(buttonpin, INPUT_PULLUP);
  pinMode(SDI, OUTPUT);
  pinMode(SFTCLK, OUTPUT);
  pinMode(LCHCLK, OUTPUT);
  MsTimer2::set(1, onTimer);
  MsTimer2::start();
  com.interface();
  Serial.begin(9600);
}

void onTimer()//中断服务程序，每毫秒与自定义计时器同步一次
{
  ledTimer.modefyTime();//用以led控制
  timerb.modefyTime();//用以按钮A1消抖
  timertempera.modefyTime();//控制温度持续显示时间
  myclock.interface();//添加时钟内部定时器
  readTem.interface();//添加温度读取对象内的定时器
}

bool buttonPressed()//确定A1是否被按下
{
  int buttonState = digitalRead(buttonpin);
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


void modefyMODE()//当A1被按下时修改运行模式
{
  if (buttonPressed())
  {
    MODE++;
    MODE %= 5;
  }
}

void RUNMODE()//选择不同模式下的LED灯和时钟的运行方式
{
  switch (MODE)
  {
    case 0 : mode0(); break;
    case 1 : mode1(); myclock.modefy_hour(); break;
    case 2 : mode2(); myclock.modefy_minute(); break;
    case 3 : mode3(); myclock.set_alarm(); break;
    case 4 : mode4(); myclock.set_alarm(); break;
  }

}
void mode0()//LED灯模式0，对应时钟正常计时
{
  digitalWrite(LED2, HIGH);
  digitalWrite(LED3, HIGH);
  digitalWrite(LED4, HIGH);
  if (ledTimer.timerState())
  {
    digitalWrite(LED1, ledState);
    ledState = !ledState;
    ledTimer.reStart();
  }
}

void mode1()//LED灯模式1，对应时钟校时
{
  if (ledTimer.timerState())
  {
    digitalWrite(LED4, ledState);
    digitalWrite(LED1, ledState);
    ledState = !ledState;
    ledTimer.reStart();
  }
  digitalWrite(LED3, LOW);
}

void mode2()//LED灯模式2，对应时钟校分
{
  if (ledTimer.timerState())
  {
    digitalWrite(LED3, ledState);
    digitalWrite(LED1, ledState);
    ledState = !ledState;
    ledTimer.reStart();
  }
  digitalWrite(LED4, LOW);
}

void mode3()//LED灯模式3，对应时钟闹钟调时
{
  if (ledTimer.timerState())
  {
    digitalWrite(LED4, ledState);
    digitalWrite(LED3, ledState);
    digitalWrite(LED1, ledState);
    ledState = !ledState;
    ledTimer.reStart();
  }
}
void mode4()//LED灯模式4，对应时钟闹钟调分
{
  if (ledTimer.timerState())
  {
    digitalWrite(LED4, ledState);
    digitalWrite(LED3, ledState);
    digitalWrite(LED2, ledState);
    digitalWrite(LED1, ledState);
    ledState = !ledState;
    ledTimer.reStart();
  }
}

void tip()//向串口输出提示信息
{
  if (lastMode == 4 && MODE == 0)
  {
    Serial.println("正常运行");
    lastMode = MODE;
  }
  else if (lastMode == 0 && MODE == 1)
  {
    Serial.println("校时");
    lastMode = MODE;
  }
  else if (lastMode == 1 && MODE == 2)
  {
    Serial.println("校分");
    lastMode = MODE;
  }
  else if (lastMode == 2 && MODE == 3)
  {
    Serial.println("闹钟调时,按钮A2开启/关闭闹铃");
    lastMode = MODE;
  }
  else if (lastMode == 3 && MODE == 4)
  {
    Serial.println("闹钟调分,按钮A2开启/关闭闹铃");
    lastMode = MODE;
  }
}

void printTempe()//向串口输出温度
{
  Serial.println("the temperature is:");
  Serial.print(readTem.readT());
  Serial.println(".C");
  tempflag = true;
}

void displaytemp()//数码管显示温度
{
    int temperature = readTem.readT();
    int first = temperature / 10;
    int second = temperature % 10;
    output(first, 1);
    output(second, 2);
    output(11, 3);
    output(12, 4);
}

void output(int num, int sel)
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

void control(int num)
{
  switch (num)
  {
    case 1: myclock.PrintTime(); break;
    case 2: myclock.setNTime(com.NumArray); break;
    case 3: myclock.setAlarm(com.NumArray); break;
    case 4: myclock.extinguish = true; break;
    case 5: myclock.fullbright = true; break;
    case 6: printTempe();
    case 7: readTem.setAlarmTemp(com.NumArray[0]);
    default: break;
  }
}

void loop()
{
  modefyMODE();
  myclock.set_mode(MODE);//同步时钟内置运行模式参数mode
  myclock.work();//时钟计时
  RUNMODE();//选择不同模式下的LED灯和时钟的运行方式
  tip();//提示信息
  
  if (com.read_command()&&MODE == 0)//其他模式下不处理命令
  {
    int ComNum = com.analyse_command();
    control(ComNum);
    readTem.alarm();
  }
  
  if (tempflag)
  {
      timertempera.reStart();
      tempflag = false;
      tempflag1 = true;
  }
  else if(!timertempera.timerState()&&tempflag1)
  {
    displaytemp();//温度显示
  }
  else if(timertempera.timerState())
  {
    tempflag1 = false;
    myclock.disPlay();//时钟显示
  }
}

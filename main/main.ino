#include <MsTimer2.h>
#include"MyTimer1.h"
#include"clock.h"
#include"command_read.h"
#include "ReadTemperature.h"
#include "humidity.h"

#define buttonpin A1//调整运行模式
#define button2pin A2//控制响铃
#define LED1 10//四个指示灯
#define LED2 11
#define LED3 12
#define LED4 13
#define alarmpin 3
const static int SDI = 8; //输入
const static int SFTCLK = 7; //移位,低电平使能
const static int LCHCLK = 4; //锁存，低电平使能
unsigned char NUM[14] = {0xc0, 0xf9, 0xa4, 0xb0, 0x99, 0x92, 0x82, 0xf8, 0x80, 0x90, 0xff, 0x7f, 0xc6,0x8e}; //0-9,全灭,'.','C','F'
unsigned char SELECTED[5] = {0x00, 0x01, 0x02, 0x04, 0x08};//片选,第一个凑数

bool flag = false;//用以按钮A1消抖
bool flag2 = false;//用以按钮A2消抖
int ledState = HIGH;
int ledState2 = HIGH;
int laststate = HIGH;//按钮A1的上一个状态
int laststate2 = HIGH;//按钮A2的上一个状态
bool alarmable = false;
int MODE = 0;//0 1 2 3 4五种模式
int lastMode = 4;//上一个运行模式
//float lastHUMI = 0;

myClock myclock;//时钟类
MyTimer ledTimer(500);//用以led控制
MyTimer timerb(50);//用于按钮A1消抖
MyTimer timerb2(50);//用于按钮A2消抖
MyTimer timertempera(10000);//控制温度持续显示时间
MyTimer timerHI(10000);//控制湿度持续显示时间

command com;//命令读取
readtemperature readTem;//温度读取
humidity humi;//湿度读取（百分数）

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
  pinMode(alarmpin, OUTPUT);
  digitalWrite(alarmpin, HIGH);
  MsTimer2::set(1, onTimer);
  MsTimer2::start();
  com.interface();
  timertempera.stopwork();
  timerHI.stopwork();
  Serial.begin(9600);
}

void onTimer()//中断服务程序，每毫秒与自定义计时器同步一次
{
  ledTimer.modefyTime();//用以led控制
  timerb.modefyTime();//用以按钮A1消抖
  timerb2.modefyTime();//用以按钮A2消抖
  timertempera.modefyTime();//控制温度持续显示时间
  timerHI.modefyTime();//控制湿度持续显示时间
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

bool button2Pressed()//确定A2是否被按下
{
  int button2State = digitalRead(button2pin);
  if (button2State == HIGH) //未按下
  {
    laststate2 = button2State;
    flag2 = false;
  }
  if (button2State == LOW && laststate2 == HIGH && !flag2) //下降沿
  {
    timerb2.reStart();
    flag2 = true;
  }
  if (button2State == LOW && laststate2 == HIGH && timerb2.timerState() && flag2) //确定被按下
  {
    laststate2 = button2State;
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
    Serial.println("work");
    lastMode = MODE;
  }
  else if (lastMode == 0 && MODE == 1)
  {
    Serial.println("modefy hour");
    lastMode = MODE;
  }
  else if (lastMode == 1 && MODE == 2)
  {
    Serial.println("modefy minute");
    lastMode = MODE;
  }
  else if (lastMode == 2 && MODE == 3)
  {
    Serial.println("modefy alarm_hour,press buttonA2 to no/off the alarm");
    lastMode = MODE;
  }
  else if (lastMode == 3 && MODE == 4)
  {
    Serial.println("modefy alarm_minute,press buttonA2 to no/off the alarm");
    lastMode = MODE;
  }
}

void printTempe()//输出温度提示
{
  Serial.println("the temperature is:");
  float tempe = readTem.readT();
  Serial.print(tempe);
  Serial.println(".C");
}

void printHI()//输出湿度提示
{
  Serial.print("the humidity is: ");
  Serial.print(humi.readHI());
  Serial.println("%");
}

void displaytemp()//数码管显示温度
{
  int temp = readTem.readT();
  output(temp / 10, 1);
  output(temp % 10, 2);
  output(11, 3);
  output(12, 4);
}

void displayhumid()//数码管显示湿度
{
  float HUF = humi.readHI();
  int HU = HUF;
  output(13, 1);
  output(11, 2);
  output(HU / 10, 3);
  output(HU % 10, 4);
}

void output(int num, int sel)//单个数码管的显示
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
    case 6: printTempe(); break;
    case 7: readTem.setAlarmTemp(com.NumArray[0], com.NumArray[1]); break;
    case 8: printHI(); break;
    case 9: humi.setAlarm(com.NumArray[0], com.NumArray[1]); break;
    default: break;
  }
}

void loop()
{
  modefyMODE();
  myclock.set_mode(MODE);//同步时钟内置运行模式参数mode
  readTem.Tmode(MODE);//运行模式同步
  humi.Hmode(MODE);//运行模式同步
  myclock.work();//时钟计时
  RUNMODE();//选择不同模式下的LED灯和时钟的运行方式
  tip();//提示信息
  int ComNum = 0;
  if (com.read_command() && MODE == 0) //其他模式下不处理命令
  {
    ComNum = com.analyse_command();//解析命令
    control(ComNum);//响应命令
  }
  if ((myclock.alarm() | readTem.alarm() | humi.alarm() ) && alarmable) //MODE==0时若任意部分请求，则可以响铃
  {
    digitalWrite(alarmpin, LOW);
    //若同时请求响应，按以下优先顺序进行数码管显示
    if (myclock.alarm())
    {
      myclock.disPlay();
    }
    else if (readTem.alarm())
    {
      displaytemp();
    }
    else if (humi.alarm())
    {
      displayhumid();
    }
  }
  else
  {
    digitalWrite(alarmpin, HIGH);
    if (ComNum == 6)//命令显示温度
    {
      timertempera.reStart();
    }
    if (ComNum == 8)//命令显示湿度
    {
      timerHI.reStart();
    }
    //----------------------------------------
    if (!timertempera.timerState())//温度显示
    {
      displaytemp();
    }
    else if (!timerHI.timerState())//湿度显示
    {
      displayhumid();
    }
    else
      myclock.disPlay();//时钟显示
  }
  if (MODE == 0 && button2Pressed())//控制是否响铃
  {
    alarmable = !alarmable;
  }
}

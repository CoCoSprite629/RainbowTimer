/*
项目名称: 彩虹时钟 (RainbowTimer)
简介: 基于ESP32, 搭配LCD1602屏幕显示天气与时间.
      使用VS Code+PlatformIO开发.
      配有5位流水灯及光敏电阻传感器.
      LCD1602使用16c转I2c模块.
功能: 可查看气温及体感温度
      星期和时间.
      拥有定时休眠，靠近唤醒功能.
      配有5颗呼吸灯,可显示温度的区间.
注意事项:
      此项目使用PlatformIO编写，ArduinoIDE无法直接使用
      需用VS code+PlatformIO扩展打开

详情见根目录下的readme.txt
*/

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include "Core.h"
#include "Get_Password.h"
// 导入核心库

#define D_1 23
#define D_2 22
#define D_3 21
#define D_4 19
#define D_5 18
#define D_6 17

// 宏定义五个LED流水灯引脚
// 及光敏电阻传感器的接收引脚
int Now = 0;
int Status = 0;
int Run_time = 0;
//初始化计时器变量

unsigned char list[5] = {D_1, D_2, D_3, D_4, D_5};

void Water_lamp(int n)
{
  for (int i = 0; i < n; i++)
  {
    // 流水灯
    for (int i = 0; i < 5; i++)
    {
      analogWrite(list[i], 230);
      delay(300);
    }
    delay(500);
    for (int i = 5; i >= 0; i--)
    {
      analogWrite(list[i], 0);
      delay(300);
    }
    delay(500);
  }
}

void setup()
{
  Serial.begin(115200);
  Serial.println("__STRAT__");
  Wire.begin(25, 26);
  Waiting();
  WiFi_reset();
  Weather_get();
  Water_lamp(1);
  display(0x27, 16, 2);

  Run_time = millis();
}

void loop()
{
  Weather_get();
  //爬取天气
  display(0x27, 16, 2);
  //在LCD1602上显示天气及时间
  delay(5000);
  Now = millis();
  if (Now - Run_time >= 20000)  //设置息屏时间
  {
    Nolight();
    //执行关闭背光函数
    Serial.println("NoBackLight");
    int R1 = 0;
    while (true)
    {
      R1 = digitalRead(D_6);
      Serial.println(R1);
      /*
      光敏电阻value值判断：
        光线被阻挡：HIGH；
        光线未阻挡：LOW;
      */
      if (R1 == 1)
      {
        break;
      }
    }
    Run_time = Now;
    // 同步计时器
  }
}
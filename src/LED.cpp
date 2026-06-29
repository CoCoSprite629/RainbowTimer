/*
在不同的天气下
点亮不同颜色的呼吸灯
采用模拟型信号输入
不同的温度呼吸灯具有不同的闪烁频率
*/

#include <Arduino.h>


#define D_1 23
#define D_2 22
#define D_3 21
#define D_4 19
#define D_5 18

void Normal()
{
    for (int i = 0; i < 5; i++)
    {
        // 呼吸灯
        for (int i = 0; i <= 255; i++)  //点亮
        {
            delay(10);
            analogWrite(D_3, i);
        }
        delay(200);
        for (int i = 255; i >= 0; i--)  //熄灭
        {
            delay(10);
            analogWrite(D_3, i);
        }
    }
}

void Cold()
{
    for (int i = 0; i < 5; i++)
    {
        // 呼吸灯
        for (int i = 0; i <= 230; i++)
        {
            delay(10);
            analogWrite(D_4, i);
        }
        delay(200);
        for (int i = 230; i >= 0; i--)
        {
            delay(10);
            analogWrite(D_4, i);
        }
    }
}

void VeryCold()
{
    for (int i = 0; i < 5; i++)
    {
        // 呼吸灯
        for (int i = 0; i <= 230; i++)
        {
            delay(10);
            analogWrite(D_5, i);
        }
        delay(500);
        for (int i = 230; i >= 0; i--)
        {
            delay(10);
            analogWrite(D_5, i);
        }
    }
}

void Hot()
{
    for (int i = 0; i < 5; i++)
    {
        // 呼吸灯
        for (int i = 0; i <= 255; i++)
        {
            delay(5);
            analogWrite(D_2, i);
        }
        delay(100);
        for (int i = 255; i >= 0; i--)
        {
            delay(5);
            analogWrite(D_2, i);
        }
    }
}

void VeryHot()
{
    for (int i = 0; i < 5; i++)
    {
        // 呼吸灯
        for (int i = 0; i <= 255; i++)
        {
            delay(2);
            analogWrite(D_1, i);
        }
        delay(50);
        for (int i = 255; i >= 0; i--)
        {
            delay(2);
            analogWrite(D_1, i);
        }
    }
}


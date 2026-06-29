/*
爬取天气
使用HTTPClient库get
使用ArduinoJson库处理返回JSON文件
--------------------------------
包含LCD1602天气字库
使用和风天气API
并使用if elif+switch case进行天气判断
使用NTP服务器同步时间
*/

#include <Arduino.h>
#include <HTTPClient.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <LiquidCrystal_I2C.h>
#include <time.h>
#include "LED.h"

String key = "SFbwub3EAuLHwm1Cp"; // Your private key
// 这一部分填写您心知天气API的私钥
//关于为什么使用心知天气API
/*
和风天气使用了gzip压缩，且现在无法通过添加&gzip=n的方式取消压缩
故转用心知天气API
*/
String URL = "https://api.seniverse.com/v3/weather/now.json?key=" + key + "&location=ningde&language=zh-Hans&unit=c";

const char *ntp_Sever = "time.windows.com";
//NTP服务器地址
const long gmt_Offset_sec = 3600 * 8;
//时区，以中时区为基准，东八区为3600 * 8
const int daylightOffset_sec = 0;
//不使用夏令时

// 天气API
int Weather_code = 0;
int Temp = 0;
int F_Temp = 0;

//------------------------------------------------------------------------------
// 从此开始为字库部分

byte drop[8] = {
    0b00100,
    0b01110,
    0b11111,
    0b11111,
    0b11111,
    0b11111,
    0b01110,
    0b00000,
};

byte bell[8] = {
    0b00100,
    0b01110,
    0b01110,
    0b01110,
    0b11111,
    0b00000,
    0b00100,
    0b00000,
};

byte qing1[8] = {
    0b00000,
    0b00000,
    0b00000,
    0b01111,
    0b01001,
    0b01001,
    0b01001,
    0b01111,
};
byte qing2[8] = {
    0b01001,
    0b01001,
    0b01001,                        
    0b01111,
    0b01001,
    0b01000,
    0b00000,
    0b00000,
};

byte qing3[8] = {
    0b00000,
    0b00000,
    0b01111,
    0b00000,
    0b00111,
    0b00000,
    0b01111,
    0b00000,
};

byte qing4[8] = {
    0b00111,
    0b00100,
    0b00111,
    0b00100,
    0b00111,
    0b00100,
    0b00100,
    0b00100,
};

byte qing5[8] = {
    0b00000,
    0b10000,
    0b11111,
    0b10000,
    0b11110,
    0b10000,
    0b11111,
    0b00000,
};

byte qing6[8] = {
    0b11110,
    0b00010,
    0b11110,
    0b00010,
    0b11110,
    0b00010,
    0b00110,
    0b00100,
};

//************************************************

byte yun1[8] = {
    0b00000,
    0b00000,
    0b00011,
    0b00000,
    0b00000,
    0b00000,
    0b01111,
    0b01111,
};

byte yun2[8] = {
    0b00000,
    0b00000,
    0b00000,
    0b00001,
    0b00011,
    0b00111,
    0b00011,
    0b00000,
};

byte yun3[8] = {
    0b00000,
    0b00000,
    0b11111,
    0b00000,
    0b00000,
    0b00000,
    0b11111,
    0b11111,
};

byte yun4[8] = {
    0b01100,
    0b01000,
    0b10000,
    0b10000,
    0b00000,
    0b11111,
    0b00000,
    0b00000,
};

byte yun5[8] = {
    0b00000,
    0b00000,
    0b11100,
    0b00000,
    0b00000,
    0b00000,
    0b11111,
    0b11111,
};

byte yun6[8] = {
    0b00000,
    0b00000,
    0b10000,
    0b11000,
    0b01100,
    0b11110,
    0b00010,
    0b00000,
};

//************************************************

byte xue1[8] = {
    0b00000,
    0b00111,
    0b00000,
    0b01111,
    0b01000,
    0b01011,
    0b00000,
    0b00011,
};

byte xue2[8] = {
    0b00000,
    0b00111,
    0b00000,
    0b00111,
    0b00000,
    0b00111,
    0b00000,
    0b00000,
};

byte xue3[8] = {
    0b00000,
    0b11111,
    0b00010,
    0b11111,
    0b00010,
    0b11010,
    0b00010,
    0b11010,
};

byte xue4[8] = {
    0b00000,
    0b11111,
    0b00000,
    0b11111,
    0b00000,
    0b11111,
    0b00000,
    0b00000,
};

byte xue5[8] = {
    0b00000,
    0b11110,
    0b00000,
    0b11111,
    0b00001,
    0b11101,
    0b00000,
    0b11100,
};

byte xue6[8] = {
    0b00000,
    0b11110,
    0b00010,
    0b11110,
    0b00010,
    0b11110,
    0b00010,
    0b00000,
};

//************************************************

byte yu1[8] = {
    0b00000,
    0b11111,
    0b00000,
    0b00000,
    0b01111,
    0b01000,
    0b01010,
    0b01001,
};

byte yu2[8] = {
    0b01000,
    0b01010,
    0b01001,
    0b01000,
    0b01000,
    0b01000,
    0b00000,
    0b00000,
};

byte yu3[8] = {
    0b00000,
    0b11111,
    0b00100,
    0b00100,
    0b11111,
    0b00100,
    0b00101,
    0b00100,
};

byte yu4[8] = {
    0b10100,
    0b00101,
    0b00100,
    0b10100,
    0b00100,
    0b00100,
    0b00000,
    0b00000,
};

byte yu5[8] = {
    0b00000,
    0b11111,
    0b00000,
    0b00000,
    0b11110,
    0b00010,
    0b00010,
    0b10010,
};

byte yu6[8] = {
    0b01010,
    0b00010,
    0b10010,
    0b01010,
    0b00010,
    0b11110,
    0b00000,
    0b00000,
};

//************************************************

byte yin1[8] = {
    0b00000,
    0b01111,
    0b01001,
    0b01001,
    0b01001,
    0b01010,
    0b01010,
    0b01010,
};

byte yin2[8] = {
    0b01001,
    0b01001,
    0b01001,
    0b01001,
    0b01111,
    0b01000,
    0b01000,
    0b00000,
};

byte yin3[8] = {
    0b00000,
    0b00111,
    0b00100,
    0b00100,
    0b00100,
    0b00111,
    0b00100,
    0b00100,
};

byte yin4[8] = {
    0b00100,
    0b00111,
    0b00100,
    0b01100,
    0b01000,
    0b11000,
    0b10000,
    0b00000,
};

byte yin5[8] = {
    0b00000,
    0b11111,
    0b00001,
    0b00001,
    0b00001,
    0b11111,
    0b00001,
    0b00001,
};

byte yin6[8] = {
    0b00001,
    0b11111,
    0b00001,
    0b00001,
    0b00001,
    0b00001,
    0b11110,
    0b00000,
};

//************************************************

byte I[8] = {
    0b00000,
    0b10000,
    0b10000,
    0b10000,
    0b10000,
    0b10000,
    0b10000,
    0b00000,
};

byte II[8] = {
    0b00000,
    0b10100,
    0b10100,
    0b10100,
    0b10100,
    0b10100,
    0b10100,
    0b00000,
};

byte III[8] = {
    0b00000,
    0b10101,
    0b10101,
    0b10101,
    0b10101,
    0b10101,
    0b10101,
    0b00000,
};

byte sign[8] = {
    0b00000,
    0b10000,
    0b00111,
    0b01000,
    0b01000,
    0b01000,
    0b00111,
    0b00000,
};

byte sign2[8] = {
    0b00000,
    0b10000,
    0b01111,
    0b01000,
    0b01111,
    0b01000,
    0b01000,
    0b00000,
};

//************************************************

byte wu1[8] = {
    0b00000,
    0b00111,
    0b00000,
    0b01111,
    0b01100,
    0b01111,
    0b00011,
    0b00000,
};

byte wu2[8] = {
    0b00011,
    0b00110,
    0b00000,
    0b00111,
    0b00011,
    0b00000,
    0b00111,
    0b00000,
};

byte wu3[8] = {
    0b00000,
    0b11111,
    0b00010,
    0b11111,
    0b00010,
    0b00011,
    0b11011,
    0b10000,
};

byte wu4[8] = {
    0b11111,
    0b01101,
    0b11111,
    0b00100,
    0b11111,
    0b01000,
    0b10001,
    0b00000,
};

byte wu5[8] = {
    0b00000,
    0b11110,
    0b00000,
    0b11111,
    0b00001,
    0b11001,
    0b11100,
    0b00000,
};

byte wu6[8] = {
    0b11000,
    0b10000,
    0b10000,
    0b01110,
    0b11100,
    0b01100,
    0b11000,
    0b00000,
};

//------------------------------------------------------------------------------

void Weather_get()
{
    HTTPClient http;
    // 初始化http
    http.begin(URL);
    // 发送请求
    int HttpCode = http.GET();
    // 获取状态码
    if (HttpCode > 0)
    {
        Serial.println();
        Serial.printf("httpCode:%d", HttpCode);
        if (HttpCode == HTTP_CODE_OK)
        { // 200为成功接收
            String Weather_result = http.getString();
            //Serial.println(Weather_result); 
            //Debug使用
            DynamicJsonDocument doc(1024);
            // 设置ArduinoJson内存
            deserializeJson(doc, Weather_result);
            // 将结果存入doc
            Weather_code = doc["results"][0]["now"]["code"].as<int>();
            // 用来识别天气
            Temp = doc["results"][0]["now"]["temperature"].as<int>();
            F_Temp = (Temp * 1.8 + 32) / 1;
            // 提取键,并转换为int类型
            Serial.println();
            Serial.printf("Weather_Code: %d", Weather_code);
            Serial.println();
            Serial.printf("Temp: %d", Temp);
            Serial.println();
            Serial.printf("F_Temp: %d", F_Temp);
            Serial.println();
        }
    }
    else
    {
        Serial.println();
        Serial.printf("htttpERROR, code%s", HttpCode);
        // 发生错误，出现其他状态码
    }
    http.end();
}

void display(unsigned char address, unsigned char cols, unsigned char rows)
{
    LiquidCrystal_I2C lcd1602(address, cols, rows);
    lcd1602.backlight();
    lcd1602.init();
    lcd1602.backlight();
    //天气状况码判断
    if (Weather_code >= 0 && Weather_code <= 3) // 晴
    {
        lcd1602.createChar(0, qing1);
        lcd1602.createChar(1, qing2);
        lcd1602.createChar(2, qing3);
        lcd1602.createChar(3, qing4);
        lcd1602.createChar(4, qing5);
        lcd1602.createChar(5, qing6);
        lcd1602.clear();
        lcd1602.setCursor(0, 0);
        lcd1602.write(0);
        lcd1602.setCursor(0, 1);
        lcd1602.write(1);
        lcd1602.setCursor(1, 0);
        lcd1602.write(2);
        lcd1602.setCursor(1, 1);
        lcd1602.write(3);
        lcd1602.setCursor(2, 0);
        lcd1602.write(4);
        lcd1602.setCursor(2, 1);
        lcd1602.write(5);
    }

    else if (Weather_code >= 4 && Weather_code <= 8) // 多云
    {
        lcd1602.createChar(0, yun1);
        lcd1602.createChar(1, yun2);
        lcd1602.createChar(2, yun3);
        lcd1602.createChar(3, yun4);
        lcd1602.createChar(4, yun5);
        lcd1602.createChar(5, yun6);
        lcd1602.clear();
        lcd1602.setCursor(0, 0);
        lcd1602.write(0);
        lcd1602.setCursor(0, 1);
        lcd1602.write(1);
        lcd1602.setCursor(1, 0);
        lcd1602.write(2);
        lcd1602.setCursor(1, 1);
        lcd1602.write(3);
        lcd1602.setCursor(2, 0);
        lcd1602.write(4);
        lcd1602.setCursor(2, 1);
        lcd1602.write(5);
    }

    else if (Weather_code == 9) // 阴
    {
        lcd1602.createChar(0, yin1);
        lcd1602.createChar(1, yin2);
        lcd1602.createChar(2, yin3);
        lcd1602.createChar(3, yin4);
        lcd1602.createChar(4, yin5);
        lcd1602.createChar(5, yin6);
        lcd1602.clear();
        lcd1602.setCursor(0, 0);
        lcd1602.write(0);
        lcd1602.setCursor(0, 1);
        lcd1602.write(1);
        lcd1602.setCursor(1, 0);
        lcd1602.write(2);
        lcd1602.setCursor(1, 1);
        lcd1602.write(3);
        lcd1602.setCursor(2, 0);
        lcd1602.write(4);
        lcd1602.setCursor(2, 1);
        lcd1602.write(5);
    }

    else if (Weather_code >= 10 && Weather_code <= 18) // 雨
    {
        lcd1602.createChar(0, yu1);
        lcd1602.createChar(1, yu2);
        lcd1602.createChar(2, yu3);
        lcd1602.createChar(3, yu4);
        lcd1602.createChar(4, yu5);
        lcd1602.createChar(5, yu6);
        lcd1602.clear();
        lcd1602.setCursor(0, 0);
        lcd1602.write(0);
        lcd1602.setCursor(0, 1);
        lcd1602.write(1);
        lcd1602.setCursor(1, 0);
        lcd1602.write(2);
        lcd1602.setCursor(1, 1);
        lcd1602.write(3);
        lcd1602.setCursor(2, 0);
        lcd1602.write(4);
        lcd1602.setCursor(2, 1);
        lcd1602.write(5);
        switch (Weather_code)
        {
        case 13: // 小雨
            lcd1602.createChar(6, I);
            lcd1602.setCursor(3, 1);
            lcd1602.write(6);
            break;

        case 14: // 中雨
            lcd1602.createChar(6, II);
            lcd1602.setCursor(3, 1);
            lcd1602.write(6);
            break;

        case 15: // 大雨
            lcd1602.createChar(6, III);
            lcd1602.setCursor(3, 1);
            lcd1602.write(6);
            break;

        case 16: // 暴雨
            lcd1602.createChar(6, III);
            lcd1602.setCursor(3, 1);
            lcd1602.write(6);
            break;

        case 17: // 暴雨
            lcd1602.createChar(6, III);
            lcd1602.setCursor(3, 1);
            lcd1602.write(6);
            break;

        case 11: // 雷阵雨
            lcd1602.createChar(6, III);
            lcd1602.setCursor(3, 1);
            lcd1602.write(6);
            break;

        case 18: // 暴雨
            lcd1602.createChar(6, III);
            lcd1602.setCursor(3, 1);
            lcd1602.write(6);
            break;

        default:
            break;
        }
    }

    else if (Weather_code >= 20 && Weather_code <= 25) // 雪
    {
        lcd1602.createChar(0, xue1);
        lcd1602.createChar(1, xue2);
        lcd1602.createChar(2, xue3);
        lcd1602.createChar(3, xue4);
        lcd1602.createChar(4, xue5);
        lcd1602.createChar(5, xue6);
        lcd1602.clear();
        lcd1602.setCursor(0, 0);
        lcd1602.write(0);
        lcd1602.setCursor(0, 1);
        lcd1602.write(1);
        lcd1602.setCursor(1, 0);
        lcd1602.write(2);
        lcd1602.setCursor(1, 1);
        lcd1602.write(3);
        lcd1602.setCursor(2, 0);
        lcd1602.write(4);
        lcd1602.setCursor(2, 1);
        lcd1602.write(5);
        switch (Weather_code)
        {
        case 20: // 雨夹雪
            lcd1602.createChar(6, drop);
            lcd1602.setCursor(3, 1);
            lcd1602.write(6);
            break;

        case 21: // 阵雪
            lcd1602.createChar(6, I);
            lcd1602.setCursor(3, 1);
            lcd1602.write(6);
            break;

        case 22: // 小雪
            lcd1602.createChar(6, I);
            lcd1602.setCursor(3, 1);
            lcd1602.write(6);
            break;

        case 23: // 中雪
            lcd1602.createChar(6, II);
            lcd1602.setCursor(3, 1);
            lcd1602.write(6);
            break;

        case 24: // 大雪
            lcd1602.createChar(6, III);
            lcd1602.setCursor(3, 1);
            lcd1602.write(6);
            break;

        case 25: // 暴雪
            lcd1602.createChar(6, III);
            lcd1602.setCursor(3, 1);
            lcd1602.write(6);
            break;

        default:
            break;
        }
    }

    else if (Weather_code == 30 || Weather_code == 31) // 雾
    {
        lcd1602.createChar(0, wu1);
        lcd1602.createChar(1, wu2);
        lcd1602.createChar(2, wu3);
        lcd1602.createChar(3, wu4);
        lcd1602.createChar(4, wu5);
        lcd1602.createChar(5, wu6);
        lcd1602.clear();
        lcd1602.setCursor(0, 0);
        lcd1602.write(0);
        lcd1602.setCursor(0, 1);
        lcd1602.write(1);
        lcd1602.setCursor(1, 0);
        lcd1602.write(2);
        lcd1602.setCursor(1, 1);
        lcd1602.write(3);
        lcd1602.setCursor(2, 0);
        lcd1602.write(4);
        lcd1602.setCursor(2, 1);
        lcd1602.write(5);
        switch (Weather_code)
        {
        case 30: // 薄雾
            lcd1602.createChar(6, I);
            lcd1602.setCursor(3, 1);
            lcd1602.write(6);
            break;

        case 31: // 浓雾
            lcd1602.createChar(6, III);
            lcd1602.setCursor(3, 1);
            lcd1602.write(6);
            break;

        default:
            break;
        }
    }

    lcd1602.setCursor(4, 0);
    lcd1602.print("T:");
    lcd1602.setCursor(6, 0);
    lcd1602.print(Temp);
    lcd1602.setCursor(8, 0);
    lcd1602.write(0xDF);
    lcd1602.setCursor(10, 0);
    lcd1602.print("FT:");
    lcd1602.setCursor(13, 0);
    if (F_Temp >= 100) {
        lcd1602.print("NA");
    }
    else {
        lcd1602.print(F_Temp);
    }
    //设置华氏度的取值范围
    lcd1602.setCursor(15, 0);
    lcd1602.write(0xDF);

    /*
    设置系统时间
    并且在LCD1602上显示
    */
    /*
    %a获取星期的简称
    %R显示时间
    */
    struct tm timeinfo;
    configTime(gmt_Offset_sec, daylightOffset_sec, ntp_Sever);
    int Watchdog = 15;
    //设置阻塞程序
    //能在大多时候起作用但不排除少数情况
    while (Watchdog >= 0)
    {
        if (getLocalTime(&timeinfo))
        {
            Serial.println("SetTime Succeeded");
            break;
        }
        else {
            configTime(gmt_Offset_sec, daylightOffset_sec, ntp_Sever);
            Serial.println("SetTime_Failed");
        }
        delay(500);
        Watchdog -= 1;
    }
    // 如果获取失败，退出函数
    Serial.println();
    Serial.println(&timeinfo, "%R %a");
    lcd1602.setCursor(6, 1);
    int Hour = timeinfo.tm_hour;
    int Minute = timeinfo.tm_min;
    int temp = timeinfo.tm_wday;
    String Week;
    switch (temp)
    {
    case 0:
        Week = "Sun";
        break;
    case 1:
        Week = "Mon";
        break;
    case 2:
        Week = "Tue";
        break;
    case 3:
        Week = "Wed";
        break;
    case 4:
        Week = "Thur";
        break;
    case 5:
        Week = "Fri";
        break;
    case 6:
        Week = "Sat";
        break;
    default:
        break;
    }
    if (Hour < 10) {
        lcd1602.printf("0%d:", Hour);
    }
    else {
        lcd1602.printf("%d:", Hour);
    }
    if (Minute < 10) {
        lcd1602.printf("0%d", Minute);
    }
    else {
        lcd1602.printf("%d", Minute);
    }
    lcd1602.setCursor(12, 1);
    lcd1602.print(Week);
    delay(200);
    if (Temp > 0 && Temp <= 20)
    {
        Normal();
    }

    else if (Temp > 20 && Temp <= 30)
    {
        Hot();
    }

    else if (Temp > 30 && Temp <= 40)
    {
        VeryHot();
    }

    else if (Temp <= 0 && Temp >= -10)
    {
        Cold();
    }

    else if (Temp < -10 && Temp >= -20)
    {
        VeryCold();
    }
}

void Nolight()
{
    LiquidCrystal_I2C lcd1602(0x27, 16, 2);
    lcd1602.init();
    lcd1602.noBacklight();
    //取消背光
}

void Waiting()
{
    LiquidCrystal_I2C lcd1602(0x27, 16, 2);
    lcd1602.init();
    lcd1602.backlight();
    lcd1602.setCursor(2, 0);
    lcd1602.print("RainbowTimer");
    lcd1602.setCursor(6, 1);
    lcd1602.print("V1.2");
    /*版本说明：
    V1.00使用和风天气API
    v1.10使用心知天气API
    V1.20完善了一些细节   
    */
}

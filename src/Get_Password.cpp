#include <Arduino.h>
#include <BluetoothSerial.h>
#include <WiFi.h>
//#include <array>
String WiFi_Name;       // Your WiFi name
String WiFi_Password; // Your WiFi password
// 这一部分需要填写您自己的WiFi名称和密码
int n = 0;
const char* ssid;
const char* password;

void WiFi_reset() {
    BluetoothSerial BTSerial;
    BTSerial.begin("RainbowTimer");
    //设置蓝牙名称
    Serial.begin(115200);
    Serial.println("Bluetooth is ready to pair!");
    std::array<String, 16> temp;
    //定义array数组存储账号密码数据
    while (n < 2) {
        if (BTSerial.available()) {
            temp[n] = BTSerial.readString();
            //将收到的数据写入数组
            n += 1;
            Serial.println("Received");
        }
        delay(20);
    }

    Serial.println("Bluetooth received");
    WiFi_Name = temp[0];
    WiFi_Password = temp[1];
    //提取数组中的数据
    ssid = WiFi_Name.c_str();
    password = WiFi_Password.c_str();
    //将String转换为const char *指针

    //Serial.println(strlen(ssid));
    //Serial.println(strlen(password));
    //Serial.println(ssid);
    //Serial.println(password);
    //Debug

    Serial.println("Bluetooth OFF");
    BTSerial.end();
    //关闭蓝牙

    Serial.println("WiFi Started");
    Serial.println();
    WiFi.begin(ssid, password);
    // 初始化WiFi
    Serial.println("Connecting");
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        Serial.print(".");
    }
    // 阻塞程序
    // 直到连接成功
    Serial.print("Connected!");
}



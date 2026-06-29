# 🌈 RainbowTimer / 彩虹时钟

> 基于 **ESP32** 的桌面天气时钟装置 ｜ Desktop Weather Clock with Temperature LEDs

**RainbowTimer** 是一个基于 **ESP32** 与 **Arduino 框架**的桌面物联网小物件。它通过 **I2C LCD1602** 显示实时天气、温度与星期时间，并通过 **光敏电阻** 实现自动熄屏 / 靠近唤醒，同时用 **5 颗 PWM 呼吸灯**以不同颜色和频率直观反馈当前温度区间。适合作为电子DIY入门、物联网学习项目或创意桌面摆件。

---

## 功能特性

- ⚡ **ESP32 物联网核心** — 基于 Arduino 框架，通过 PlatformIO 构建
- 🌤 **实时天气获取** — 调用 [心知天气 API](https://www.seniverse.com/) 获取当前天气与温度
- 🌡 **双温显示** — 同时显示摄氏度（°C）与华氏度（°F）
- 🕒 **自动对时** — 通过 `time.windows.com` NTP 服务器同步东八区时间与星期
- 🌈 **温度呼吸灯** — 5 颗 PWM 呼吸灯对应不同温度区间：舒适、热、很热、冷、很冷
- 🔅 **自动熄屏 / 唤醒** — 20 秒无操作后关闭 LCD 背光，光敏电阻被遮挡时自动唤醒
- 📟 **中文天气图标** — 利用 LCD1602 CGRAM 自定义字符显示汉字天气（晴 / 多云 / 阴 / 雨 / 雪 / 雾）
- 🔗 **蓝牙配网** — 通过蓝牙串口发送 Wi-Fi 名称与密码，无需硬编码

---

## 硬件清单

| 组件 | 型号/规格 | 数量 | 说明 |
|------|----------|------|------|
| 主控 | ESP32-DevKitC / 兼容开发板 | 1 | `esp32dev` 目标板 |
| 液晶显示屏 | LCD1602 带 I2C 转接板 | 1 | 默认 I2C 地址 `0x27` |
| LED | 5mm 单色 LED 或 RGB 灯珠 | 5 | 建议红 / 橙 / 绿 / 蓝 / 白 |
| 光敏传感器 | 光敏电阻模块（数字输出） | 1 | 用于检测遮挡唤醒 |
| 电阻 | 220Ω 限流电阻 | 5 | 每颗 LED 一颗 |
| 线材 | 杜邦线 / 面包板 | 若干 | 连接外设 |
| 电源 | Micro-USB 数据线 | 1 | 供电 + 上传程序 |

### 引脚连接

| 功能 | ESP32 引脚 | 说明 |
|------|-----------|------|
| I2C SDA | `GPIO 25` | 连接 LCD1602 I2C 转接板 SDA |
| I2C SCL | `GPIO 26` | 连接 LCD1602 I2C 转接板 SCL |
| 很热 LED | `GPIO 23` | 温度 > 30°C 时亮起（红色） |
| 热 LED | `GPIO 22` | 20°C < 温度 ≤ 30°C 时亮起（橙色） |
| 舒适 LED | `GPIO 21` | 0°C < 温度 ≤ 20°C 时亮起（绿色） |
| 冷 LED | `GPIO 19` | -10°C ≤ 温度 ≤ 0°C 时亮起（蓝色） |
| 很冷 LED | `GPIO 18` | -20°C ≤ 温度 < -10°C 时亮起（白色/紫色） |
| 光敏电阻 | `GPIO 17` | 数字输入，光线被遮挡时返回 `HIGH` |

---

## 项目结构

```text
RainbowTimer/
├── README.md
├── platformio.ini                # PlatformIO 项目配置
├── include/
│   ├── Core.h                    # 天气核心函数声明
│   ├── Get_Password.h            # 蓝牙配网函数声明
│   └── LED.h                     # 呼吸灯函数声明
├── src/
│   ├── main.cpp                  # 主程序入口：setup / loop
│   ├── Weather_Core.cpp          # 天气获取、LCD 显示、NTP 对时、温度灯驱动
│   ├── Get_Password.cpp          # 蓝牙串口接收 Wi-Fi 账号密码
│   └── LED.cpp                   # 五种温度区间的呼吸灯实现
└── test/                         # 测试目录
```

---

## 系统架构

```text
┌─────────────────────────────────────────────┐
│              RainbowTimer                   │
│                                             │
│  ┌──────────────┐      ┌──────────────┐   │
│  │  光敏传感器  │      │  5 颗 LED    │   │
│  │  GPIO 17     │      │  GPIO 18-23  │   │
│  └──────┬───────┘      └──────┬───────┘   │
│         │                     │             │
│  ┌──────▼─────────────────────▼───────┐    │
│  │        ESP32 DevKitC               │    │
│  │  ┌─────────────────────────────┐  │    │
│  │  │  BluetoothSerial (配网)     │  │    │
│  │  │  WiFi + HTTPClient (天气)   │  │    │
│  │  │  NTP (时间同步)             │  │    │
│  │  │  I2C (LCD1602)              │  │    │
│  │  │  PWM (LED 呼吸灯)           │  │    │
│  │  └─────────────────────────────┘  │    │
│  └──────┬────────────────────┬───────┘    │
│         │ I2C                │ UART       │
│  ┌──────▼───────┐   ┌───────▼───────┐   │
│  │   LCD1602    │   │  串口监视器   │   │
│  │  GPIO 25/26  │   │   115200 bps   │   │
│  └──────────────┘   └───────────────┘   │
└─────────────────────────────────────────┘
```

### 交互流程

```text
上电 ──► 开机画面 ──► 蓝牙配网 ──► 连接 WiFi ──► 获取心知天气 ──► NTP 对时
                                                │
                                                ▼
                                    ┌──────────────────────┐
                                    │ LCD 显示天气 + 时间   │
                                    │ LED 显示温度区间      │
                                    └──────────────────────┘
                                                │
                                                ▼
                                    20 秒无操作 ──► 关闭背光
                                    光敏遮挡   ──► 唤醒刷新
```

---

## 快速上手

### 前置要求

- [Visual Studio Code](https://code.visualstudio.com/)
- [PlatformIO IDE 插件](https://platformio.org/install/ide?install=vscode)
- 一条 Micro-USB 数据线

### 编译烧录

```bash
# 1. 克隆仓库
git clone https://github.com/your-username/RainbowTimer.git
cd RainbowTimer

# 2. 修改心知天气 API Key（重要！）
# 编辑 src/Weather_Core.cpp 中的 key 变量，替换为你自己的私钥

# 3. 用 VS Code 打开项目文件夹，点击 PlatformIO 工具栏的 Upload 上传
```

> 也可以在终端中使用 PlatformIO Core CLI：
> ```bash
> pio run --target upload
> ```

---

## 配置说明

### 心知天气 API Key 与城市

打开 `src/Weather_Core.cpp`，必须替换默认的 API Key 与城市：

```cpp
String key = "YOUR_SENIVERSE_PRIVATE_KEY";  // 替换为你的私钥

String URL = "https://api.seniverse.com/v3/weather/now.json?key=" + key
           + "&location=YOUR_CITY&language=zh-Hans&unit=c";
```

> 💡 获取 Key：前往 [心知天气官网](https://www.seniverse.com/) 注册并申请免费 API 私钥。

### 可调参数

| 参数 | 定义位置 | 默认值 | 说明 |
|------|---------|--------|------|
| `key` | `src/Weather_Core.cpp` | 空字符串 | 心知天气 API 私钥 |
| `location` | `src/Weather_Core.cpp` | `ningde` | 城市代码 |
| `ntp_Sever` | `src/Weather_Core.cpp` | `time.windows.com` | NTP 服务器地址 |
| `gmt_Offset_sec` | `src/Weather_Core.cpp` | `3600 * 8` | 东八区时区偏移 |
| 熄屏时间 | `src/main.cpp` | `20000` ms | 无操作后关闭背光 |
| LCD 地址 | `src/main.cpp` / `src/Weather_Core.cpp` | `0x27` | I2C 设备地址 |
| 蓝牙设备名 | `src/Get_Password.cpp` | `RainbowTimer` | 蓝牙串口广播名称 |

---

## 通信协议

### 蓝牙配网格式

设备上电后进入蓝牙等待模式，使用蓝牙串口工具连接名为 `RainbowTimer` 的设备，依次发送：

1. Wi-Fi 名称（SSID）
2. Wi-Fi 密码（Password）

源码中通过 `BluetoothSerial` 接收并保存到全局变量，供后续 `WiFi.begin()` 使用。

### 天气数据解析

ESP32 通过 `HTTPClient` 向心知天气发送 GET 请求，返回 JSON 后使用 `ArduinoJson` 解析：

```json
{
  "results": [
    {
      "now": {
        "code": 0,
        "temperature": 25
      }
    }
  ]
}
```

代码中读取：

```cpp
Weather_code = doc["results"][0]["now"]["code"].as<int>();
Temp = doc["results"][0]["now"]["temperature"].as<int>();
```

### 天气代码对照

| 代码区间 | 天气 | 屏幕显示 |
|---------|------|---------|
| 0 ~ 3 | 晴 | 晴 |
| 4 ~ 8 | 多云 | 多云 |
| 9 | 阴 | 阴 |
| 10 ~ 18 | 雨 | 雨 + 雨强等级 |
| 20 ~ 25 | 雪 | 雪 + 雪强等级 |
| 30 ~ 31 | 雾 | 雾 + 雾浓等级 |

---

## LED 动画参考

| 状态 | 温度区间 | 触发函数 | 颜色 | 效果 |
|------|---------|---------|------|------|
| 🌈 舒适 | 0°C < T ≤ 20°C | `Normal()` | 绿色 | 温和呼吸 |
| 🔥 热 | 20°C < T ≤ 30°C | `Hot()` | 橙色 | 中等频率呼吸 |
| 🌶 很热 | 30°C < T ≤ 40°C | `VeryHot()` | 红色 | 快速呼吸 |
| ❄️ 冷 | -10°C ≤ T ≤ 0°C | `Cold()` | 蓝色 | 缓慢呼吸 |
| 🧊 很冷 | -20°C ≤ T < -10°C | `VeryCold()` | 白色/紫色 | 缓慢呼吸 |

---

## 软件架构

```text
setup()
  ├── 初始化串口 (Serial, 115200)
  ├── 初始化蓝牙串口 (BluetoothSerial)
  ├── 初始化 LCD1602
  ├── 初始化 GPIO 输出
  └── 显示开机画面 RainbowTimer V1.2

loop()
  ├── 光敏电阻检测
  │     ├── 被遮挡 (HIGH) ──► 唤醒 LCD，刷新天气与时间
  │     └── 无遮挡      ──► 检查 20 秒超时熄屏
  ├── 蓝牙串口接收 ──► 更新 WiFi 名称与密码
  ├── WiFi 连接成功
  │     ├── Weather_get()      ──► 获取心知天气 JSON
  │     ├── display()          ──► 解析并显示天气 + 时间
  │     └── LED 温度区间驱动   ──► Normal / Hot / VeryHot / Cold / VeryCold
  └── 循环延迟等待
```

---

## 依赖

| 库 | 版本 | 用途 |
|----|------|------|
| [ArduinoJson](https://arduinojson.org/) | ^6.20.0 | 解析心知天气 JSON 响应 |
| [LiquidCrystal_I2C](https://github.com/marcoschwartz/LiquidCrystal_I2C) | ^1.1.4 | 驱动 I2C 版 LCD1602 |
| Arduino (ESP32) | — | ESP32 框架基础 |
| WiFi / HTTPClient (ESP32 内置) | — | 网络请求与 Wi-Fi 连接 |
| BluetoothSerial (ESP32 内置) | — | 蓝牙配网 |

---

## 开发环境

- **IDE**: VS Code + PlatformIO
- **开发板**: ESP32-DevKitC / 兼容板
- **分区方案**: `huge_app.csv`
- **框架**: Arduino for ESP32
- **串口波特率**: `115200`

---

## 使用场景

1. 将 ESP32 与 LCD1602、LED、光敏电阻按引脚表连接。
2. 使用 VS Code + PlatformIO 编译并上传固件。
3. 在 `src/Weather_Core.cpp` 中填入心知天气 API Key 与城市。
4. 上电后，用手机蓝牙串口连接 `RainbowTimer`，发送 Wi-Fi 名称与密码。
5. 设备连接网络后自动获取天气、同步时间，并显示在屏幕上。
6. 当温度变化时，不同颜色的 LED 会呼吸闪烁，直观展示当前温度区间。
7. 20 秒后 LCD 自动熄灭，用手遮挡光敏传感器即可唤醒刷新。

---

## 版本历史

| 版本 | 说明 |
| --- | --- |
| V1.00 | 使用和风天气 API |
| V1.10 | 迁移至心知天气 API（避免 gzip 压缩问题） |
| V1.20 | 完善细节，优化显示与交互逻辑 |

---

## 注意事项

1. **必须使用 PlatformIO**：项目工程文件 `platformio.ini` 使用了 ESP32 分区表 `huge_app.csv`，Arduino IDE 直接打开无法编译。
2. **API Key 安全**：不要把心知天气私钥上传到公开仓库，提交前请确认已替换为占位符。
3. **蓝牙配网**：发送 SSID 和密码时请确保使用换行或回车作为结束符，并等待串口提示 `"Bluetooth received"`。
4. **光敏传感器逻辑**：代码中光线被遮挡时返回 `HIGH`，如果你的模块逻辑相反，请将 `main.cpp` 中的判断条件取反。
5. **GPIO 电平**：ESP32 大多数 GPIO 支持 PWM，但建议避开 `GPIO 6 ~ 11`（与 Flash 相关）以及 Strapping 引脚（如 `GPIO 0`、`GPIO 12`）。
6. **华氏度上限**：当华氏度超过 100°F 时，屏幕显示 `NA` 以避免溢出。

---

## 开源协议

本项目采用 **GPL2.0** 开源，欢迎 Star、Fork 和 PR！

---

<p align="center">
  <sub>Made with ❤️ and ESP32</sub>
</p>

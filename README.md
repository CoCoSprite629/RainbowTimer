# 🌈 RainbowTimer / 彩虹时钟

基于 **ESP32** 与 **Arduino 框架**的桌面天气时钟小项目。它通过 LCD1602（I2C）显示实时天气、温度与星期时间，并通过光敏电阻实现自动熄屏 / 靠近唤醒，同时用 5 颗呼吸灯以不同颜色和频率直观展示当前温度区间。

> **注意**：本项目使用 [PlatformIO](https://platformio.org/) 工具链开发，直接在 Arduino IDE 中打开可能无法编译，请使用 **VS Code + PlatformIO 扩展**。

---

## 目录

- [项目简介](#项目简介)
- [功能特点](#功能特点)
- [硬件清单](#硬件清单)
- [接线说明](#接线说明)
- [项目结构](#项目结构)
- [软件依赖](#软件依赖)
- [安装与编译](#安装与编译)
- [配置说明](#配置说明)
- [使用说明](#使用说明)
- [版本历史](#版本历史)
- [注意事项](#注意事项)
- [许可证](#许可证)

---

## 项目简介

RainbowTimer 是一个适合初学者的 ESP32 物联网练手项目。它整合了 Wi-Fi 网络请求、JSON 解析、NTP 时间同步、I2C 液晶显示、PWM 呼吸灯以及光敏传感器，实现了一个会“看天气、报时间、自动省电”的桌面摆件。

核心流程：

1. 上电显示开机画面 `RainbowTimer V1.2`。
2. 通过蓝牙串口（BluetoothSerial）接收 Wi-Fi 名称与密码。
3. 连接 Wi-Fi 后获取心知天气（Seniverse）实时天气数据。
4. 通过 NTP 服务器同步东八区时间。
5. 在 LCD1602 上以自定义字符方式显示天气汉字（晴 / 多云 / 阴 / 雨 / 雪 / 雾等），并显示摄氏度、华氏度、时间与星期。
6. 根据温度区间驱动不同颜色的呼吸灯。
7. 20 秒后自动关闭背光，当光敏电阻被遮挡时重新点亮。

---

## 功能特点

| 功能 | 说明 |
| --- | --- |
| 🌤 实时天气 | 调用 [心知天气 API](https://www.seniverse.com/) 获取当前天气代码与温度 |
| 🌡 双温显示 | 同时显示摄氏度（°C）与华氏度（°F） |
| 🕒 自动对时 | 通过 `time.windows.com` NTP 服务器同步时间与星期 |
| 🌈 温度呼吸灯 | 5 颗 PWM 呼吸灯对应不同温度区间：舒适、热、很热、冷、很冷 |
| 🔅 自动熄屏 / 唤醒 | 20 秒无操作后关闭 LCD 背光，光敏电阻被遮挡时唤醒 |
| 📟 中文天气字库 | 利用 LCD1602 CGRAM 自定义字符显示汉字天气图标 |
| 🔗 蓝牙配网 | 通过蓝牙串口发送 Wi-Fi 名称与密码，无需硬编码 |

---

## 硬件清单

- **ESP32-DevKitC** 或兼容开发板（`esp32dev`）× 1
- **LCD1602** 液晶显示屏（带 I2C 转接板，默认地址 `0x27`）× 1
- **5 颗 LED / RGB 灯珠**（建议不同颜色：红、橙、绿、蓝、白）× 5
- **光敏电阻模块** × 1（数字信号输出）
- 杜邦线若干
- USB 数据线（供电 + 上传程序）

---

## 接线说明

### ESP32 引脚分配

| 功能 | GPIO 引脚 | 说明 |
| --- | --- | --- |
| I2C SDA | `GPIO 25` | 连接 LCD1602 I2C 转接板 SDA |
| I2C SCL | `GPIO 26` | 连接 LCD1602 I2C 转接板 SCL |
| 很热 LED | `GPIO 23` | 温度 > 30°C 时亮起 |
| 热 LED | `GPIO 22` | 20°C < 温度 ≤ 30°C 时亮起 |
| 舒适 LED | `GPIO 21` | 0°C < 温度 ≤ 20°C 时亮起 |
| 冷 LED | `GPIO 19` | -10°C ≤ 温度 ≤ 0°C 时亮起 |
| 很冷 LED | `GPIO 18` | -20°C ≤ 温度 < -10°C 时亮起 |
| 光敏电阻 | `GPIO 17` | 数字输入，光线被遮挡时返回 `HIGH` |

### 典型接线示意

```text
ESP32-DevKitC
    │
    ├── GPIO 25 ───── SDA ─── LCD1602 I2C SDA
    ├── GPIO 26 ───── SCL ─── LCD1602 I2C SCL
    ├── 3.3V/5V ───── VCC ─── LCD1602 VCC
    ├── GND ───────── GND ─── LCD1602 GND
    │
    ├── GPIO 23 ───── LED ─── 很热（红）
    ├── GPIO 22 ───── LED ─── 热（橙）
    ├── GPIO 21 ───── LED ─── 舒适（绿）
    ├── GPIO 19 ───── LED ─── 冷（蓝）
    ├── GPIO 18 ───── LED ─── 很冷（白/紫）
    │
    ├── GPIO 17 ───── DO ──── 光敏电阻模块信号
    ├── 3.3V ──────── VCC ─── 光敏电阻模块 VCC
    └── GND ───────── GND ─── 光敏电阻模块 GND
```

> 建议每颗 LED 串联 220Ω 限流电阻，正极接 GPIO，负极接地。

---

## 项目结构

```text
RainbowTimer/
├── include/
│   ├── Core.h            # 天气核心函数声明
│   ├── Get_Password.h    # 蓝牙配网函数声明
│   └── LED.h             # 呼吸灯函数声明
├── src/
│   ├── main.cpp          # 主程序入口：setup / loop
│   ├── Weather_Core.cpp  # 天气获取、LCD 显示、NTP 对时、温度灯驱动
│   ├── Get_Password.cpp  # 蓝牙串口接收 Wi-Fi 账号密码
│   └── LED.cpp           # 五种温度区间的呼吸灯实现
├── platformio.ini        # PlatformIO 工程配置
└── README.md             # 本说明文档
```

---

## 软件依赖

- **框架**：Arduino（ESP32）
- **构建平台**：PlatformIO（`espressif32`）
- **核心库**：
  - [`ArduinoJson`](https://arduinojson.org/) `^6.20.0` — 解析心知天气 JSON 响应
  - [`LiquidCrystal_I2C`](https://github.com/marcoschwartz/LiquidCrystal_I2C) `^1.1.4` — 驱动 I2C 版 LCD1602
- **开发环境**：VS Code + [PlatformIO IDE](https://marketplace.visualstudio.com/items?itemName=platformio.platformio-ide) 扩展

依赖已在 `platformio.ini` 中声明，首次构建时 PlatformIO 会自动下载。

---

## 安装与编译

1. **克隆或下载本仓库**到本地：

   ```bash
   git clone https://github.com/yourname/RainbowTimer.git
   cd RainbowTimer
   ```

2. **使用 VS Code 打开项目文件夹**，并确保已安装 PlatformIO 扩展。

3. **修改心知天气 API 密钥**：

   打开 `src/Weather_Core.cpp`，将你的私钥替换到 `key` 变量：

   ```cpp
   String key = "YOUR_SENIVERSE_PRIVATE_KEY";
   ```

   同时可以修改默认城市（例如 `ningde`）：

   ```cpp
   String URL = "https://api.seniverse.com/v3/weather/now.json?key=" + key
              + "&location=YOUR_CITY&language=zh-Hans&unit=c";
   ```

4. **连接 ESP32 到电脑**，在 PlatformIO 工具栏中点击：

   - **Build**（编译）
   - **Upload**（上传）
   - **Serial Monitor**（查看串口输出，波特率 115200）

5. **首次运行**需要用手机或电脑蓝牙串口工具连接名为 `RainbowTimer` 的设备，依次发送：

   1. 你的 Wi-Fi 名称（SSID）
   2. 你的 Wi-Fi 密码（Password）

   连接成功后屏幕会自动刷新天气与时间。

---

## 配置说明

| 配置项 | 文件位置 | 说明 |
| --- | --- | --- |
| 心知天气 API Key | `src/Weather_Core.cpp` 第 20 行 | 必须替换为你自己的私钥 |
| 城市 | `src/Weather_Core.cpp` 第 27 行 | 默认 `ningde`，按需修改 |
| NTP 服务器 | `src/Weather_Core.cpp` 第 29 行 | 默认 `time.windows.com` |
| 时区 | `src/Weather_Core.cpp` 第 31 行 | 默认东八区 `3600 * 8` |
| 熄屏时间 | `src/main.cpp` 第 83 行 | 默认 20 秒（20000 ms） |
| LCD I2C 地址 | `src/main.cpp` / `src/Weather_Core.cpp` | 默认 `0x27` |
| 蓝牙设备名 | `src/Get_Password.cpp` 第 14 行 | 默认 `RainbowTimer` |

---

## 使用说明

### 开机画面

上电后 LCD 会先显示：

```text
  RainbowTimer
      V1.2
```

然后进入蓝牙配网等待状态。

### 显示内容

天气更新成功后，屏幕显示类似：

```text
晴  T:25° FT:77°
     14:30 Mon
```

- 左侧为天气汉字图标（晴、多云、阴、雨、雪、雾等）。
- `T` 为当前摄氏度，`FT` 为华氏度。
- 下方为当前时间与星期缩写。

### 呼吸灯与温度区间

| 温度区间 | 触发函数 | 对应 GPIO | 效果 |
| --- | --- | --- | --- |
| 0°C < T ≤ 20°C | `Normal()` | GPIO 21 | 舒适绿灯呼吸 |
| 20°C < T ≤ 30°C | `Hot()` | GPIO 22 | 温暖橙色呼吸 |
| 30°C < T ≤ 40°C | `VeryHot()` | GPIO 23 | 高温红色快速呼吸 |
| -10°C ≤ T ≤ 0°C | `Cold()` | GPIO 19 | 寒冷蓝色慢呼吸 |
| -20°C ≤ T < -10°C | `VeryCold()` | GPIO 18 | 极寒白色慢呼吸 |

### 自动省电

每轮刷新后若累计运行超过 20 秒，LCD 背光关闭，进入低功耗等待。当光敏电阻检测到被遮挡（返回 `HIGH`），背光重新点亮并继续刷新。

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

---

## 许可证

本项目采用 [MIT 许可证](LICENSE)。欢迎自由学习、修改与分享。

---

> 如果这个项目对你有帮助，欢迎点个 ⭐ Star！

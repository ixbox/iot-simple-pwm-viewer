# Simple PWM Viewer

Arduino NanoとOLEDディスプレイを使用したPWM信号モニター。

## 機能

- PWM信号の周波数とデューティ比をリアルタイム測定
- OLEDディスプレイに数値と波形を表示
- シリアル出力によるログ機能

## ハードウェア

### 必要な部品

| 部品 | 仕様 |
|------|------|
| マイコンボード | Arduino Nano / UNO R3 または互換品 (ATmega328P) |
| OLEDディスプレイ | SSD1306 128x64 I2C |

### 配線

| Arduino Nano | 接続先 |
|--------------|--------|
| D2 | PWM入力信号 |
| A4 (SDA) | OLED SDA |
| A5 (SCL) | OLED SCL |
| 5V | OLED VCC |
| GND | OLED GND, 信号GND |

## 表示内容

```
+------------------------+
| Freq: 1.00      [PWM]  |
| Duty: 50.0             |
|------------------------|
|  _    _    _           |
| | |  | |  | |          |
| |_|  |_|  |_|          |
+------------------------+
```

- 周波数 (kHz)
- デューティ比 (%)
- 測定波形のグラフィカル表示

## ビルド

PlatformIOを使用:

```bash
# ビルド
pio run

# 書き込み
pio run -t upload

# シリアルモニター
pio device monitor
```

## 仕様

- 測定周波数範囲: 約20Hz〜20kHz
- 測定タイムアウト: 50ms
- 更新間隔: 100ms
- シリアル通信: 115200bps

## ライセンス

MIT License

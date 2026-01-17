#include <Arduino.h>
#include <U8g2lib.h>

// OLED設定
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

// 契約：PWM測定結果
struct PWMReading
{
    float frequency;
    float dutyCycle;
    bool valid;
};

// Physical実装：デジタルピンでPWM測定
struct DigitalPWMReader
{
    uint8_t pin;
    PWMReading current;

    void init()
    {
        pinMode(pin, INPUT);
        current = {0, 0, false};
    }

    bool read()
    {
        unsigned long high = pulseIn(pin, HIGH, 50000);
        if (high == 0)
        {
            current.valid = false;
            return false;
        }

        unsigned long low = pulseIn(pin, LOW, 50000);
        if (low == 0)
        {
            current.valid = false;
            return false;
        }

        unsigned long period = high + low;
        current.frequency = 1000000.0 / period;
        current.dutyCycle = (high * 100.0) / period;
        current.valid = true;
        return true;
    }
};

// Logical：表示
struct PWMDisplay
{
    U8G2_SSD1306_128X64_NONAME_F_HW_I2C *display;

    void init()
    {
        display->begin();
        display->setFont(u8g2_font_7x13B_tr); // monospace bold
    }

    void show(const PWMReading &reading)
    {
        display->clearBuffer();

        // 右上にPWMアイコン
        drawIcon();

        if (!reading.valid)
        {
            // 信号なし時の表示
            display->setCursor(5, 15);
            display->print("Freq: --");

            display->setCursor(5, 30);
            display->print("Duty: --");

            // 区切り線
            display->drawHLine(0, 35, 128);

            // 波形表示（ベースラインのみ）
            drawWaveform(0); // duty=0でベースラインだけ表示

            display->sendBuffer();
            return;
        }

        // 周波数表示
        display->setCursor(5, 15);
        display->print("Freq: ");
        display->print(reading.frequency / 1000.0, 2);
        display->print(" kHz");

        // デューティ表示
        display->setCursor(5, 30);
        display->print("Duty: ");
        display->print(reading.dutyCycle, 1);
        display->print(" %");

        // 区切り線
        display->drawHLine(0, 35, 128);

        // 矩形波表示
        drawWaveform(reading.dutyCycle);

        display->sendBuffer();
    }

private:
    void drawIcon()
    {
        int x = 105;
        int baseY = 20; // ベースライン（LOW）
        int highY = 12; // HIGH位置
        int w = 4;      // パルス幅

        // ベースライン
        display->drawHLine(x, baseY, 20);

        // 2周期分の矩形波
        for (int i = 0; i < 2; i++)
        {
            int cx = x + i * w * 2;
            // 立ち上がり
            display->drawVLine(cx, highY, baseY - highY);
            // HIGH区間
            display->drawHLine(cx, highY, w);
            // 立ち下がり
            display->drawVLine(cx + w, highY, baseY - highY);
        }
    }

    void drawWaveform(float duty)
    {
        int baseY = 55; // ベースライン（LOW）の位置
        int highY = 40; // HIGHの位置
        int cycleWidth = 50;

        // ベースライン（LOW）
        display->drawHLine(0, baseY, 128);

        // 矩形波（2.5周期分）
        for (int cycle = 0; cycle < 3; cycle++)
        {
            int x = cycle * cycleWidth;
            int highWidth = (cycleWidth * duty) / 100;

            if (highWidth > 0)
            {
                // 立ち上がり
                display->drawVLine(x, highY, baseY - highY);
                // HIGH区間
                display->drawHLine(x, highY, highWidth);
                // 立ち下がり
                display->drawVLine(x + highWidth, highY, baseY - highY);
            }

            // LOW区間はベースラインで表現済み
        }
    }
};

// Logical: ログ出力
struct DebugLogger
{
    bool enabled;

    void init(bool enable = true)
    {
        enabled = enable;
        if (enabled)
        {
            Serial.begin(115200);
        }
    }

    void log(const PWMReading &reading)
    {
        if (!enabled)
            return;

        if (reading.valid)
        {
            Serial.print(F("Freq: "));
            Serial.print(reading.frequency / 1000.0, 2);
            Serial.print(F(" kHz, Duty: "));
            Serial.print(reading.dutyCycle, 1);
            Serial.println(F(" %"));
        }
        else
        {
            Serial.println(F("No signal"));
        }
    }
};

// Physical配線
const uint8_t PWM_INPUT_PIN = 2;

// デバイス
DigitalPWMReader pwmReader = {PWM_INPUT_PIN};
PWMDisplay pwmDisplay = {&u8g2};
DebugLogger logger = {true};

// setup関数内
void setup()
{
    // Physical初期化
    logger.init();
    pwmReader.init();
    pwmDisplay.init();

    Serial.println(F("PWM Monitor Ready"));
}

void loop()
{
    // Meaning: PWMを測定して表示する
    pwmReader.read();
    pwmDisplay.show(pwmReader.current);
    logger.log(pwmReader.current);

    delay(100);
}
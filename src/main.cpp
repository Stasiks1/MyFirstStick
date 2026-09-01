#include <M5StickCPlus.h>
#include <driver/i2s.h>
#include <IRsend.h>

#define PIN_CLK  0
#define PIN_DATA 34
#define READ_LEN 128

int16_t micBuffer[READ_LEN]; // Буфер для звуковых точек
int currentMode = 0; // 0 — режим кликера, 1 — режим акселерометра
float accX = 0.0F;
float accY = 0.0F;
float accZ = 0.0F;
int counter = 0;
int seconds = 0;
bool isRunning = false;
unsigned long previousMillis = 0;
IRsend irsend(9);

void runClicker() {
    if (M5.BtnA.wasPressed()) {
        counter++;
        M5.Lcd.fillScreen(BLACK);
        M5.Lcd.setCursor(10, 30);
        M5.Lcd.print(counter);
    }
    if (M5.BtnB.wasPressed()) {
        counter = max(counter - 1, 0);
        M5.Lcd.fillScreen(BLACK);
        M5.Lcd.setCursor(10, 30);
        M5.Lcd.print(counter);
    }
}

void runImu() {
    M5.IMU.getAccelData(&accX, &accY, &accZ);
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setCursor(10, 20);
    M5.Lcd.printf("X: %.2f\nY: %.2f\nZ: %.2f", accX, accY, accZ);
    delay(100);
}

void runStopwatch() {
    if (M5.BtnA.wasPressed()) {
    isRunning = !isRunning;
     M5.Lcd.setCursor(20, 100);
    if (isRunning) {
        M5.Beep.tone(4000, 50);
        M5.Lcd.print("RUN  ");   // Пробелы, чтобы затереть слово PAUSE
    } else {
        M5.Beep.tone(4000, 50);
        M5.Lcd.print("PAUSE");
    }
}
    if (isRunning && M5.BtnB.wasPressed()) {
        seconds = 0;
        M5.Lcd.setCursor(80,50);
        M5.Lcd.printf("%02d", seconds);
    }
    if (isRunning && (millis() - previousMillis >= 1000)) {
        previousMillis = millis();
        seconds++;
        M5.Lcd.setCursor(80, 50);
        M5.Lcd.printf("%02d", seconds);
    }
}
void initMic() {
    i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX | I2S_MODE_PDM),
        .sample_rate = 44100,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = I2S_CHANNEL_FMT_ALL_RIGHT,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 2,
        .dma_buf_len = 128,
    };
    i2s_pin_config_t pin_config = {
        .bck_io_num = I2S_PIN_NO_CHANGE,
        .ws_io_num = PIN_CLK,
        .data_out_num = I2S_PIN_NO_CHANGE,
        .data_in_num = PIN_DATA
    };
    i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
    i2s_set_pin(I2S_NUM_0, &pin_config);
    i2s_set_clk(I2S_NUM_0, 44100, I2S_BITS_PER_SAMPLE_16BIT, I2S_CHANNEL_MONO);
}
void runNoiseMeter() {
    size_t bytesRead = 0;
    // 1. Читаем 128 точек звука из микрофона
    i2s_read(I2S_NUM_0, (char *)micBuffer, READ_LEN * sizeof(int16_t), &bytesRead, portMAX_DELAY);

    // 2. Ищем максимальное и минимальное значение волны
    int16_t maxVal = -32768;
    int16_t minVal = 32767;

    for (int i = 0; i < READ_LEN; i++) {
        if (micBuffer[i] > maxVal) maxVal = micBuffer[i];
        if (micBuffer[i] < minVal) minVal = micBuffer[i];
    }

    int peak = maxVal - minVal; // Размах волны = уровень шума

    // 3. Переводим звук в длину полоски от 0 до 200 пикселей
    int barWidth = map(peak, 50, 4000, 0, 200);
    barWidth = constrain(barWidth, 0, 200); // Ограничиваем, чтобы не вылезать за экран

    // 4. Отрисовка на экране
    M5.Lcd.setCursor(20, 20);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setTextColor(WHITE, BLACK);
    M5.Lcd.printf("NOISE: %04d", peak);

    // Рисуем рамку для полоски
    M5.Lcd.drawRect(20, 60, 202, 30, WHITE);

    // Закрашиваем саму полоску громкости зеленым
    M5.Lcd.fillRect(21, 61, barWidth, 28, GREEN);
    // Очищаем черным оставшуюся пустую часть полоски
    M5.Lcd.fillRect(21 + barWidth, 61, 200 - barWidth, 28, BLACK);

    delay(20); // Небольшая пауза для плавности глаза
}
void runIrRemote() {
    if (M5.BtnA.wasPressed()) {
        M5.Beep.tone(4000, 100); // Звуковой сигнал

        M5.Lcd.setCursor(20, 100);
        M5.Lcd.print("SENDING... ");

        irsend.sendNEC(0x20DF10EF, 32, 2);      // Выстрел для LG
        delay(40);
        irsend.sendSAMSUNG(0xE0E040BF, 32);  // Выстрел для Samsung

        delay(300);
        M5.Lcd.setCursor(20, 100);
        M5.Lcd.print("READY      ");
    }
}

void setup() {
    M5.begin();                // 1. Включаем плату и шины питания
    M5.Lcd.setRotation(3);     // 2. Поворачиваем экран горизонтально (0-3)
    M5.Lcd.fillScreen(BLACK);  // 3. Заливаем экран черным цветом

    M5.Lcd.setTextDatum(MC_DATUM);
    M5.Lcd.setTextColor(YELLOW, BLACK); // Цвет текста и фона
    M5.Lcd.setTextSize(3);       // Размер шрифта

    M5.Lcd.drawString("Hello", 120, 67);
    M5.IMU.Init(); // Запуск чипа датчика
    initMic(); // Инициализация микрофона

    irsend.begin();
}
void updateDisplay() {
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setTextColor(YELLOW);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(10, 30);
    M5.Lcd.print(counter);
}
void loop() {
    M5.Beep.update();
    M5.update(); // Опрос кнопок
    M5.Beep.update();
    if (M5.BtnB.wasReleasefor(700)) {
        currentMode++;
        if (currentMode > 4) {
            currentMode = 0;
        }
        M5.Lcd.fillScreen(BLACK);
        if(currentMode == 0) {
            updateDisplay();
        }else if (currentMode == 2) {
             // Начальный экран секундомера:
            M5.Lcd.setCursor(20, 20);
             M5.Lcd.setTextSize(2);
            M5.Lcd.print("STOPWATCH");
    
            M5.Lcd.setCursor(80, 50);
            M5.Lcd.printf("%02d", seconds);

            M5.Lcd.setCursor(20, 100);
            M5.Lcd.print("PAUSE");
        } else if (currentMode == 3) {
            M5.Lcd.setCursor(20, 20);
            M5.Lcd.setTextSize(2);
            M5.Lcd.print("IR REMOTE");

            M5.Lcd.setCursor(20, 60);
            M5.Lcd.print("BtnA: TV POWER");

            M5.Lcd.setCursor(20, 100);
            M5.Lcd.print("READY");
        }
        }
        switch (currentMode) {
    case 0:
        runClicker();
        break;
    case 1:
        runImu();
        break;
    case 2:
        runStopwatch();
        break;
    case 3:
        runNoiseMeter();
        break;
    case 4:
        runIrRemote();
        break;
    }
}

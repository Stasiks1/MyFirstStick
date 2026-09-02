#include <M5StickCPlus.h>
#include <driver/i2s.h>
#include <IRsend.h>
#include <math.h>

#define PIN_CLK  0
#define PIN_DATA 34
#define READ_LEN 128

int16_t micBuffer[READ_LEN];
int currentMode = 0;
float accX = 0.0F;
float accY = 0.0F;
float accZ = 0.0F;
int counter = 0;
int seconds = 0;
bool isRunning = false;
unsigned long previousMillis = 0;
IRsend irsend(9);
int menuCursor = 1;
int heartProgress = 0;
void drawMenu();
void runMenu();
void runHeart();
void updateDisplay();
const char* getHeartWord(int index);

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

const char* getHeartWord(int index) {
    if (index == 6)  return "MAMA";
    if (index == 22) return "DAD";
    if (index == 14) return "H";
    if (index == 13 || index == 15) return "YOU"; // Короткие слова у кончика, чтобы не наезжать на H!
    if (index % 2 == 0) return "LOVE";
    return "I LOVE YOU";
}

void runHeart() {
    // 1. Постепенное появление (~3.5 сек)
    if (heartProgress < 28) {
        float t = heartProgress * 0.22;
        float x = 16 * pow(sin(t), 3);
        float y = 13 * cos(t) - 5 * cos(2 * t) - 2 * cos(3 * t) - cos(4 * t);

        int screenX = 120 + (x * 3.1);
        int screenY = 56 - (y * 3.1);

        const char* word = getHeartWord(heartProgress);
        int textOffset = strlen(word) * 3;

        M5.Lcd.setTextSize(1);
        if (heartProgress == 6 || heartProgress == 22) {
            M5.Lcd.setTextColor(WHITE, BLACK);
        } else {
            M5.Lcd.setTextColor(CYAN, BLACK);
        }

        // Если это не точка острия — рисуем слово
        if (heartProgress != 14) {
            M5.Lcd.setCursor(screenX - textOffset, screenY);
            M5.Lcd.print(word);
        }

        // Когда дошли до острия — ставим жирную букву H
        if (heartProgress == 14) {
            M5.Lcd.setTextSize(2);
            M5.Lcd.setTextColor(RED, BLACK);
            M5.Lcd.setCursor(120 - 6, 110);
            M5.Lcd.print("H");
        }

        heartProgress++;
        delay(125);
        return;
    }

    // 2. Фаза биения сердца
    M5.Lcd.fillScreen(BLACK);
    float pulse = 3.0 + 0.25 * sin(millis() / 250.0);
    int wordIndex = 0;

    // Сначала рисуем весь контур сердца словами
    for (float t = 0; t < 6.28; t += 0.22) {
        // Пропускаем острие внутри цикла, чтобы не затирать H
        if (wordIndex != 14) {
            float x = 16 * pow(sin(t), 3);
            float y = 13 * cos(t) - 5 * cos(2 * t) - 2 * cos(3 * t) - cos(4 * t);

            int screenX = 120 + (x * pulse);
            int screenY = 56 - (y * pulse);

            const char* word = getHeartWord(wordIndex);
            int textOffset = strlen(word) * 3;

            M5.Lcd.setTextSize(1);
            if (wordIndex == 6 || wordIndex == 22) {
                M5.Lcd.setTextColor(WHITE, BLACK);
            } else {
                M5.Lcd.setTextColor(CYAN, BLACK);
            }

            M5.Lcd.setCursor(screenX - textOffset, screenY);
            M5.Lcd.print(word);
        }
        wordIndex++;
    }

    // 3. Рисуем букву H ПОВЕРХ ВСЕГО в самом конце (ничего не перекроет!)
    uint16_t hColor = ((millis() / 250) % 2 == 0) ? RED : WHITE;
    int tipY = 56 + (17 * pulse) - 4; // Точный кончик острия сердца

    M5.Lcd.setTextSize(2);
    M5.Lcd.setTextColor(hColor, BLACK);
    M5.Lcd.setCursor(120 - 6, tipY);
    M5.Lcd.print("H");

    delay(30);
}

void drawMenu() {
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setTextColor(YELLOW, BLACK);
    M5.Lcd.setCursor(20, 5);
    M5.Lcd.print("--- MENU ---");

    const char* items[6] = {
        "1. CLICKER",
        "2. IMU SENS",
        "3. STOPWATCH",
        "4. NOISE METER",
        "5. IR REMOTE",
        "6. HEART ANIM"
    };

    for (int i = 0; i < 6; i++) {
        int y = 25 + (i * 18);
        M5.Lcd.setCursor(10, y);

        if (menuCursor == (i + 1)) {
            M5.Lcd.setTextColor(GREEN, BLACK);
            M5.Lcd.printf("> %s", items[i]);
        } else {
            M5.Lcd.setTextColor(WHITE, BLACK);
            M5.Lcd.printf("  %s", items[i]);
        }
    }
}

void runMenu() {
    if (M5.BtnB.wasPressed()) {
        menuCursor++;
        if (menuCursor > 6) {
            menuCursor = 1;
        }
        drawMenu();
    }

    if (M5.BtnA.wasPressed()) {
        currentMode = menuCursor;
        M5.Lcd.fillScreen(BLACK);

        if (currentMode == 1) {
            updateDisplay();
        } else if (currentMode == 3) {
            M5.Lcd.setCursor(20, 20);
            M5.Lcd.setTextSize(2);
            M5.Lcd.print("STOPWATCH");
            M5.Lcd.setCursor(80, 50);
            M5.Lcd.printf("%02d", seconds);
            M5.Lcd.setCursor(20, 100);
            M5.Lcd.print("PAUSE");
        } else if (currentMode == 5) {
            M5.Lcd.setCursor(20, 20);
            M5.Lcd.setTextSize(2);
            M5.Lcd.print("IR REMOTE");
            M5.Lcd.setCursor(20, 60);
            M5.Lcd.print("BtnA: TV POWER");
            M5.Lcd.setCursor(20, 100);
            M5.Lcd.print("READY");
        } else if (currentMode == 6) {
            heartProgress = 0; // Сброс для красивой прорисовки сердца
        }
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

    drawMenu();
}
void updateDisplay() {
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setTextColor(YELLOW);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(10, 30);
    M5.Lcd.print(counter);
}
void loop() {
M5.update();        // 1. Опрос кнопок
    M5.Beep.update();   // 2. Обновление звука
    if (currentMode != 0 && M5.BtnB.wasReleasefor(700)) {
        currentMode = 0; // Возвращаемся в меню
        drawMenu();      // Рисуем меню заново
    }
        switch (currentMode) {
    case 0:
        runMenu();
        break;
    case 1:
        runClicker();
        break;
    case 2:
        runImu();
        break;
    case 3:
        runStopwatch();
        break;
    case 4:
        runNoiseMeter();
        break;
    case 5:
        runIrRemote();
        break;
    case 6:
        runHeart();
        break;
    }
}

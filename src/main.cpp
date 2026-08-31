#include <M5StickCPlus.h>
int currentMode = 0; // 0 — режим кликера, 1 — режим акселерометра
float accX = 0.0F;
float accY = 0.0F;
float accZ = 0.0F;
int counter = 0;
int seconds = 0;
bool isRunning = false;
unsigned long previousMillis = 0;

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

void setup() {
    M5.begin();                // 1. Включаем плату и шины питания
    M5.Lcd.setRotation(3);     // 2. Поворачиваем экран горизонтально (0-3)
    M5.Lcd.fillScreen(BLACK);  // 3. Заливаем экран черным цветом

    M5.Lcd.setTextDatum(MC_DATUM);
    M5.Lcd.setTextColor(YELLOW, BLACK); // Цвет текста и фона
    M5.Lcd.setTextSize(3);       // Размер шрифта

    M5.Lcd.drawString("Hello", 120, 67);
    M5.IMU.Init(); // Запуск чипа датчика
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
        if (currentMode > 2) {
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
}
    }

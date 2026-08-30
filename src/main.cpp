#include <M5StickCPlus.h>
int currentMode = 0; // 0 — режим кликера, 1 — режим акселерометра
float accX = 0.0F;
float accY = 0.0F;
float accZ = 0.0F;
int counter = 0;

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

void setup() {
    M5.begin();                // 1. Включаем плату и шины питания
    M5.Lcd.setRotation(3);     // 2. Поворачиваем экран горизонтально (0-3)
    M5.Lcd.fillScreen(BLACK);  // 3. Заливаем экран черным цветом

    M5.Lcd.setTextColor(YELLOW); // Цвет текста
    M5.Lcd.setTextSize(2);       // Размер шрифта
    M5.Lcd.setCursor(10, 30);    // Координаты X, Y в пикселях

    M5.Lcd.println("Hello, Stas!");
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
    M5.update(); // Опрос кнопок
    if (M5.BtnB.wasReleasefor(700)) {
      currentMode = 1 - currentMode;
      M5.Lcd.fillScreen(BLACK);
    }
    if (currentMode == 0) {
        runClicker();
    } else {
        runImu();
    }
  }
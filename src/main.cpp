#include <M5StickCPlus.h>
int counter = 0;

void setup() {
    M5.begin();                // 1. Включаем плату и шины питания
    M5.Lcd.setRotation(3);     // 2. Поворачиваем экран горизонтально (0-3)
    M5.Lcd.fillScreen(BLACK);  // 3. Заливаем экран черным цветом

    M5.Lcd.setTextColor(YELLOW); // Цвет текста
    M5.Lcd.setTextSize(2);       // Размер шрифта
    M5.Lcd.setCursor(10, 30);    // Координаты X, Y в пикселях

    M5.Lcd.println("Hello, Stas!");
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
    if (M5.BtnA.wasPressed()) {
    counter = counter + 1; // Увеличиваем на 1
    // Твои команды очистки экрана и вывода counter
    updateDisplay();
    } else if (M5.BtnB.wasPressed()) {
    counter = counter - 1;
    counter = max(counter, 0);
     updateDisplay();
    }
}
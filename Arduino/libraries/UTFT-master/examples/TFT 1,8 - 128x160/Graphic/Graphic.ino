//Цветной графический дисплей 1,8 TFT 128x160     http://iarduino.ru/shop/Displei/cvetnoy-graficheskiy-tft-ekran-128-10---1-8-rdquo.html

#include <UTFT.h>                              // подключаем библиотеку UTFT
UTFT myGLCD(TFT18SHLD,5,4,8,7,6);              // объявляем объект myGLCD класса библиотеки UTFT указывая тип дисплея TFT18SHLD и номера выводов Arduino к которым подключён дисплей: SDA, SCK, CS, RESET, A0. Можно использовать любые выводы Arduino.
                                               //
void setup(){                                  //
   myGLCD.InitLCD();                           // инициируем дисплей
}                                              //
                                               //
void loop(){                                   //
   myGLCD.clrScr();                            // стираем всю информацию с дисплея
                                               //
   myGLCD.setColor(VGA_RED);                   // Устанавливаем красный цвет
   myGLCD.drawLine(5,5,100,5);                 // Рисуем линию (через точки с координатами 5x5 - 100x5)
                                               //
   myGLCD.setColor(VGA_GREEN);                 // Устанавливаем зелёный цвет
   myGLCD.drawRect(5,10,100,45);               // Рисуем прямоугольник (с противоположными углами в координатах 5x10 - 100x45)
                                               //
   myGLCD.setColor(VGA_BLUE);                  // Устанавливаем синий цвет
   myGLCD.drawRoundRect(5,50,100,85);          // Рисуем прямоугольник со скруглёнными углами (с противоположными углами в координатах 5x50 - 100x85)
                                               //
   myGLCD.setColor(VGA_LIME);                  // Устанавливаем лаймовый цвет
   myGLCD.fillRect(5,90,100,125);              // Рисуем закрашенный прямоугольник (с противоположными углами в координатах 5x90 - 100x125)
                                               //
   myGLCD.setColor(VGA_PURPLE);                // Устанавливаем фиолетовый цвет
   myGLCD.drawCircle(130,35,25);               // Рисуем окружность (с центром в точке 130x35 и радиусом 25)
   myGLCD.fillCircle(130,100,25);              // Рисуем закрашенную окружность (с центром в точке 130x100 и радиусом 25)
                                               //
   delay(20000);                               // ждём 20 секунд
}                                              //

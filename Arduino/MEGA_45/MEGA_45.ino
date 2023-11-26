//=========================================================================+
//                               MEGA 45                                   |
//-------------------------------------------------------------------------+
#include <SoftwareSerial.h>
#include <Wire.h>              // Подключаем библиотеку Wire
#include "Adafruit_TCS34725.h" // Подключаем библиотеку Adafruit_TCS34725

#define PIN_MOTOR_DOOR_DIE_ROOM_OPEN 22
#define PIN_MOTOR_DOOR_DIE_ROOM_CLOSE 23
#define PIN_GERCON_DOOR_DIE_ROOM_OPEN A0
#define PIN_GERCON_DOOR_DIE_ROOM_CLOSE A4

#define PIN_MOTOR_WALL_DIE_ROOM_OPEN 24
#define PIN_MOTOR_WALL_DIE_ROOM_CLOSE 25
#define PIN_GERCON_WALL_DIE_ROOM_OPEN A8
#define PIN_GERCON_WALL_DIE_ROOM_CLOSE A12
#define PIN_GERCON_WALL_DIE_ROOM_AVAR A1

#define PIN_GERCON_PAZL_1 A5
#define PIN_GERCON_PAZL_2 A9
#define PIN_GERCON_PAZL_3 A13
#define PIN_GERCON_PAZL_4 A2
#define PIN_GERCON_PAZL_5 A6
#define PIN_GERCON_PAZL_6 A10
#define PIN_GERCON_PAZL_7 A14

#define PIN_DOMOFON_DOOR 26
#define PIN_GERKON_5_DOOR A7
#define PIN_FAKEL 2
#define PIN_4_STOP_BUTTON A3

#define PIN_GERKON_PALKA_1 28
#define PIN_GERKON_PALKA_2 29
#define PIN_DOMOFON_BOX_HINT_1 30
#define PIN_DOMOFON_BOX_HINT_2 31
#define PIN_GERKON_BOX_HINT_1 32
#define PIN_GERKON_BOX_HINT_2 33
#define PIN_DOMOFON_BOX_CAT 34
#define PIN_GERKON_BOX_CAT 36

#define PIN_GERKON_MOON_1 38
#define PIN_GERKON_MOON_2 40
#define PIN_GERKON_MOON_3 42
#define PIN_GERKON_MOON_4 14
#define PIN_GERKON_MOON_5 46
#define PIN_GERKON_MOON_6 48
#define PIN_LED_MOON_1 50
#define PIN_LED_MOON_2 52
#define PIN_LED_MOON_3 35
#define PIN_DOMOFON_BOX_MOON 37
#define PIN_GERKON_BOX_MOON 39

#define PIN_LED_RED_RUBIN 41
#define PIN_LED_RUBIN 43
#define PIN_FOTO_RUBIN A11

#define PIN_FAKEL_1 7

#define PIN_5_STOP_BUTTON 47

#define PIN_DOMOFON_EXIT 51
#define PIN_GERKON_EXIT 49
// ###############################

#define PIN_RX_RO 12
#define PIN_TX_DI 11
#define PIN_RS485_CONTROL 10

//=========================================================================+
//                               ПЕРЕМЕННЫЕ                                |
//-------------------------------------------------------------------------+

//=========================================================================+
//                               ОБЪЕКТЫ                                   |
//-------------------------------------------------------------------------+
// Создаем объект и передаем данные о настройке усиления и время преобразования
SoftwareSerial RS485(PIN_RX_RO, PIN_TX_DI); // RX, TX
Adafruit_TCS34725 colorSensor = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_16X);
// Adafruit_TCS34725 colorSensor;

//=========================================================================+
//                               ФУНКЦИИ                                   |
//-------------------------------------------------------------------------+
void PRINT(String Str)
{
    digitalWrite(PIN_RS485_CONTROL, HIGH);
    // delay(100);
    Serial.println(Str);
    RS485.print(Str);
    digitalWrite(PIN_RS485_CONTROL, LOW);
    // delay(500);
}
bool CHECK_ERROR_SYMBOL(String Str)
{
    for (int i = 0; i < Str.length(); i++)
        if (int(Str[i]) > 255 or int(Str[i]) < 0)
            return true;
    return false;
}

//=========================================================================+
//                                 СТАРТ                                   |
//-------------------------------------------------------------------------+
void setup()
{
    Serial.begin(9600);
    RS485.begin(57600);
    RS485.setTimeout(200);
    pinMode(PIN_RS485_CONTROL, OUTPUT);
    digitalWrite(PIN_RS485_CONTROL, LOW);
    PRINT("\n");
    // colorSensor.begin();

    pinMode(PIN_MOTOR_DOOR_DIE_ROOM_OPEN, OUTPUT);
    digitalWrite(PIN_MOTOR_DOOR_DIE_ROOM_OPEN, HIGH);
    pinMode(PIN_MOTOR_DOOR_DIE_ROOM_CLOSE, OUTPUT);
    digitalWrite(PIN_MOTOR_DOOR_DIE_ROOM_CLOSE, HIGH);
    pinMode(PIN_GERCON_DOOR_DIE_ROOM_OPEN, INPUT_PULLUP);
    pinMode(PIN_GERCON_DOOR_DIE_ROOM_CLOSE, INPUT_PULLUP);

    pinMode(PIN_MOTOR_WALL_DIE_ROOM_OPEN, OUTPUT);
    digitalWrite(PIN_MOTOR_WALL_DIE_ROOM_OPEN, HIGH);
    pinMode(PIN_MOTOR_WALL_DIE_ROOM_CLOSE, OUTPUT);
    digitalWrite(PIN_MOTOR_WALL_DIE_ROOM_CLOSE, HIGH);
    pinMode(PIN_GERCON_WALL_DIE_ROOM_OPEN, INPUT_PULLUP);
    pinMode(PIN_GERCON_WALL_DIE_ROOM_CLOSE, INPUT_PULLUP);
    pinMode(PIN_GERCON_WALL_DIE_ROOM_AVAR, INPUT_PULLUP);

    pinMode(PIN_GERCON_PAZL_1, INPUT_PULLUP);
    pinMode(PIN_GERCON_PAZL_2, INPUT_PULLUP);
    pinMode(PIN_GERCON_PAZL_3, INPUT_PULLUP);
    pinMode(PIN_GERCON_PAZL_4, INPUT_PULLUP);
    pinMode(PIN_GERCON_PAZL_5, INPUT_PULLUP);
    pinMode(PIN_GERCON_PAZL_6, INPUT_PULLUP);
    pinMode(PIN_GERCON_PAZL_7, INPUT_PULLUP);

    pinMode(PIN_FAKEL, OUTPUT);
    digitalWrite(PIN_FAKEL, LOW);
    pinMode(PIN_DOMOFON_DOOR, OUTPUT);
    digitalWrite(PIN_DOMOFON_DOOR, HIGH);
    pinMode(PIN_GERKON_5_DOOR, INPUT_PULLUP);
    pinMode(PIN_4_STOP_BUTTON, INPUT_PULLUP);

    pinMode(PIN_GERKON_PALKA_1, INPUT_PULLUP);
    pinMode(PIN_GERKON_PALKA_2, INPUT_PULLUP);
    pinMode(PIN_DOMOFON_BOX_HINT_1, OUTPUT);
    digitalWrite(PIN_DOMOFON_BOX_HINT_1, HIGH);
    pinMode(PIN_DOMOFON_BOX_HINT_2, OUTPUT);
    digitalWrite(PIN_DOMOFON_BOX_HINT_2, HIGH);
    pinMode(PIN_GERKON_BOX_HINT_1, INPUT_PULLUP);
    pinMode(PIN_GERKON_BOX_HINT_2, INPUT_PULLUP);
    pinMode(PIN_DOMOFON_BOX_CAT, OUTPUT);
    digitalWrite(PIN_DOMOFON_BOX_CAT, HIGH);
    pinMode(PIN_GERKON_BOX_CAT, INPUT_PULLUP);

    pinMode(PIN_GERKON_MOON_1, INPUT_PULLUP);
    pinMode(PIN_GERKON_MOON_2, INPUT_PULLUP);
    pinMode(PIN_GERKON_MOON_3, INPUT_PULLUP);
    pinMode(PIN_GERKON_MOON_4, INPUT_PULLUP);
    pinMode(PIN_GERKON_MOON_5, INPUT_PULLUP);
    pinMode(PIN_GERKON_MOON_6, INPUT_PULLUP);
    pinMode(PIN_LED_MOON_1, OUTPUT); // red
    pinMode(PIN_LED_MOON_2, OUTPUT); // green
    pinMode(PIN_LED_MOON_3, OUTPUT); // blue
    pinMode(PIN_DOMOFON_BOX_MOON, OUTPUT);
    digitalWrite(PIN_DOMOFON_BOX_MOON, HIGH);
    pinMode(PIN_GERKON_BOX_MOON, INPUT_PULLUP);

    pinMode(PIN_LED_RED_RUBIN, OUTPUT);
    pinMode(PIN_LED_RUBIN, OUTPUT);

    pinMode(PIN_FAKEL_1, OUTPUT); // digitalWrite(PIN_FAKEL_1, HIGH);

    pinMode(PIN_5_STOP_BUTTON, INPUT_PULLUP);

    pinMode(PIN_DOMOFON_EXIT, OUTPUT);
    digitalWrite(PIN_DOMOFON_EXIT, HIGH);
    pinMode(PIN_GERKON_EXIT, INPUT_PULLUP);
}

//=========================================================================+
//                                  ЦИКЛ                                   |
//-------------------------------------------------------------------------+
void loop()
{

    // Дверь в комнату смерти
    static bool TR_Door_RoomDie_On = false;
    if (TR_Door_RoomDie_On)
    {
        if (!digitalRead(PIN_MOTOR_DOOR_DIE_ROOM_OPEN) and !digitalRead(PIN_GERCON_DOOR_DIE_ROOM_OPEN))
        {
            digitalWrite(PIN_MOTOR_DOOR_DIE_ROOM_OPEN, HIGH);
            TR_Door_RoomDie_On = false;
        }
        else if (!digitalRead(PIN_MOTOR_DOOR_DIE_ROOM_CLOSE) and !digitalRead(PIN_GERCON_DOOR_DIE_ROOM_CLOSE))
        {
            digitalWrite(PIN_MOTOR_DOOR_DIE_ROOM_CLOSE, HIGH);
            TR_Door_RoomDie_On = false;
        }
    }
    // Стена смерти
    static String TR_Door_WallDie_On = "Stop";
    if (TR_Door_WallDie_On != "Stop")
    {
        if (!digitalRead(PIN_GERCON_WALL_DIE_ROOM_AVAR))
        {
            digitalWrite(PIN_MOTOR_WALL_DIE_ROOM_OPEN, HIGH);
            digitalWrite(PIN_MOTOR_WALL_DIE_ROOM_CLOSE, HIGH);
        }
        if (digitalRead(PIN_GERCON_WALL_DIE_ROOM_AVAR))
        {
            if (TR_Door_WallDie_On == "Close")
                digitalWrite(PIN_MOTOR_WALL_DIE_ROOM_CLOSE, LOW);
            else if (TR_Door_WallDie_On == "Open")
                digitalWrite(PIN_MOTOR_WALL_DIE_ROOM_OPEN, LOW);
        }
        if (!digitalRead(PIN_MOTOR_WALL_DIE_ROOM_OPEN) and !digitalRead(PIN_GERCON_WALL_DIE_ROOM_OPEN))
        {
            digitalWrite(PIN_MOTOR_WALL_DIE_ROOM_OPEN, HIGH);
            TR_Door_WallDie_On = "Stop";
        }
        else if (!digitalRead(PIN_MOTOR_WALL_DIE_ROOM_CLOSE) and !digitalRead(PIN_GERCON_WALL_DIE_ROOM_CLOSE))
        {
            digitalWrite(PIN_MOTOR_WALL_DIE_ROOM_CLOSE, HIGH);
            TR_Door_WallDie_On = "Stop";
        }
    }

    // Мерцание факелов
    static bool TR_Fakel_1;
    static unsigned long TM_Blink_Fakel;
    static bool TR_Fakels_Blink;
    if (TR_Fakel_1 and !TR_Fakels_Blink and millis() - TM_Blink_Fakel > 100)
    {
        TM_Blink_Fakel = millis();
        analogWrite(PIN_FAKEL_1, 25 + random(-10, 10));
    }
    if (TR_Fakel_1 and TR_Fakels_Blink and millis() - TM_Blink_Fakel > random(20, 40))
    {
        TM_Blink_Fakel = millis();
        analogWrite(PIN_FAKEL_1, random(0, 50));
    }

    // Отправляем информацию на сервер
    static unsigned long NUM;
    NUM += 1;
    static String Last_Command;
    static unsigned long TM_Status_Message;
    if (millis() - TM_Status_Message > 500)
    {
        static int Message_Num;
        switch (Message_Num)
        {
        case 0:
        {
            PRINT("{s.d.n:" + String(NUM) + "}");
            Message_Num++;
            break;
        }
        case 1:
        {
            PRINT("{s.d.t:" + String(millis()) + "}");
            Message_Num++;
            break;
        }
        case 2:
        {
            PRINT("{s.d.c:" + String(Last_Command) + "}");
            Message_Num++;
            break;
        }
        case 3:
        {
            PRINT("{i.b.n:MEGA_45}");
            Message_Num++;
            break;
        }

        case 4:
        {
            PRINT("{s.dd.s:" + String(!digitalRead(PIN_GERCON_DOOR_DIE_ROOM_OPEN)) + String(!digitalRead(PIN_GERCON_DOOR_DIE_ROOM_CLOSE)) + "}");
            Message_Num++;
            break;
        }
        case 5:
        {
            PRINT("{s.dw.s:" + String(!digitalRead(PIN_GERCON_WALL_DIE_ROOM_OPEN)) + String(!digitalRead(PIN_GERCON_WALL_DIE_ROOM_CLOSE)) + "}");
            Message_Num++;
            break;
        }
        case 6:
        {
            PRINT("{s.sp.s:" + String(!digitalRead(PIN_GERCON_PAZL_1)) + String(!digitalRead(PIN_GERCON_PAZL_2)) + String(!digitalRead(PIN_GERCON_PAZL_3)) + String(!digitalRead(PIN_GERCON_PAZL_4)) + String(!digitalRead(PIN_GERCON_PAZL_5)) + String(!digitalRead(PIN_GERCON_PAZL_6)) + String(!digitalRead(PIN_GERCON_PAZL_7)) + "}");
            Message_Num++;
            break;
        }

        case 7:
        {
            PRINT("{s.td.s:" + String(!digitalRead(PIN_GERKON_5_DOOR)) + "}");
            Message_Num++;
            break;
        }
        case 8:
        {
            PRINT("{s.ph.s:" + String(!digitalRead(PIN_GERKON_PALKA_1)) + String(!digitalRead(PIN_GERKON_PALKA_2)) + "}");
            Message_Num++;
            break;
        }
        case 9:
        {
            PRINT("{s.h.s:" + String(!digitalRead(PIN_GERKON_BOX_HINT_1)) + String(!digitalRead(PIN_GERKON_BOX_HINT_2)) + "}");
            Message_Num++;
            break;
        }
        case 10:
        {
            PRINT("{s.cb.s:" + String(!digitalRead(PIN_GERKON_BOX_CAT)) + "}");
            Message_Num++;
            break;
        }
        case 11:
        {
            PRINT("{s.mr.s:" + String(!digitalRead(PIN_GERKON_MOON_1)) + String(!digitalRead(PIN_GERKON_MOON_2)) + String(!digitalRead(PIN_GERKON_MOON_3)) + String(!digitalRead(PIN_GERKON_MOON_4)) + String(!digitalRead(PIN_GERKON_MOON_5)) + String(!digitalRead(PIN_GERKON_MOON_6)) + "}");
            Message_Num++;
            break;
        }
        case 12:
        {
            PRINT("{s.md.s:" + String(!digitalRead(PIN_GERKON_BOX_MOON)) + "}");
            Message_Num++;
            break;
        }

        // case 13: {  float r,g,b; colorSensor.getRGB(&r, &g, &b); PRINT("{s.r.c:"+ String(int(r)) +"}");    Message_Num++; break; }
        case 13:
        {
            PRINT("{s.r.c:" + String(analogRead(PIN_FOTO_RUBIN)) + "}");
            Message_Num++;
            break;
        }

        case 14:
        {
            PRINT("{s.btn.s:");
            if (digitalRead(PIN_4_STOP_BUTTON) or digitalRead(PIN_5_STOP_BUTTON))
                PRINT("1}");
            else
                PRINT("0}");
            Message_Num++;
            break;
        }
        case 15:
        {
            PRINT("{s.fd.s:" + String(!digitalRead(PIN_GERKON_EXIT)) + "}");
            Message_Num++;
            break;
        }

        case 16:
        {
            PRINT("\n");
            Message_Num = 0;
            TM_Status_Message = millis();
        }
        }
    }

    // Принимаем команды
    if (RS485.available() > 0)
    {
        while (1)
        {
            String command = RS485.readStringUntil(']'); // Отдельная команда
            String commandValue = "";                    // Значение, необязательно
            if (command.length() == 0)
            {
                break;
            } // Если команды не поступило, прерываем цикл
            if (command.substring(0, 1) != "[")
            {
                continue;
            }                               // Если команда начинается не с [, пропускаем
            command = command.substring(1); // Команда без начально [
            Last_Command = command;
            int delimeterIndex = command.lastIndexOf("="); // Serial.println(command);
            if (delimeterIndex >= 0)
            {
                commandValue = command.substring(delimeterIndex + 1);
                command = command.substring(0, delimeterIndex);
            }
            if (CHECK_ERROR_SYMBOL(command) or CHECK_ERROR_SYMBOL(commandValue.c_str()))
                Last_Command = "ERR";

            // ДОМОФОННЫЕ МАМГНИТЫ
            if (command == "dm_fvdr")
            {
                digitalWrite(PIN_DOMOFON_DOOR, !bool(commandValue.toInt()));
            } // Магнит двери в 5ю комнату
            else if (command == "dm_hnt1")
            {
                digitalWrite(PIN_DOMOFON_BOX_HINT_1, !bool(commandValue.toInt()));
            } // Магнит ниши с подстказкой 1
            else if (command == "dm_hnt2")
            {
                digitalWrite(PIN_DOMOFON_BOX_HINT_2, !bool(commandValue.toInt()));
            } // Магнит ниши с подстказкой 2
            else if (command == "dm_ctbx")
            {
                digitalWrite(PIN_DOMOFON_BOX_CAT, !bool(commandValue.toInt()));
            } // Магнит ящика с кошкой
            else if (command == "dm_mnbx")
            {
                digitalWrite(PIN_DOMOFON_BOX_MOON, !bool(commandValue.toInt()));
            } // Магнит ящика после луны
            else if (command == "dm_exdr")
            {
                digitalWrite(PIN_DOMOFON_EXIT, !bool(commandValue.toInt()));
            } // Магнит двери выхода
            // ОСВЕЩЕНИЕ
            else if (command == "ld_mnrd")
            { // Подсветка лунной загадки
                digitalWrite(PIN_LED_MOON_1, LOW);
                digitalWrite(PIN_LED_MOON_2, LOW);
                digitalWrite(PIN_LED_MOON_3, LOW);
                if (commandValue.toInt() == 1)
                    digitalWrite(PIN_LED_MOON_1, HIGH);
                else if (commandValue.toInt() == 2)
                    digitalWrite(PIN_LED_MOON_2, HIGH);
                else if (commandValue.toInt() == 3)
                    digitalWrite(PIN_LED_MOON_3, HIGH);
            }
            else if (command == "ld_45rm")
            {
                TR_Fakel_1 = bool(commandValue.toInt());
                if (!TR_Fakel_1)
                    digitalWrite(PIN_FAKEL_1, LOW);
            } // Факела 4-5 комната
            else if (command == "ld_fkbl")
            {
                TR_Fakels_Blink = bool(commandValue.toInt());
            }
            else if (command == "ld_phrb")
            {
                digitalWrite(PIN_LED_RUBIN, bool(commandValue.toInt()));
            } // Лампа над рубином
            else if (command == "ld_pdts")
            {
                digitalWrite(PIN_LED_RED_RUBIN, commandValue.toInt());
            } // Подсветка тумбы с рубином кнасная
            // ДЫМОВУХИ
            // ДВЕРИ НА ПРИВОДЕ
            else if (command == "md_derm")
            {
                if (!bool(commandValue.toInt()) and digitalRead(PIN_GERCON_DOOR_DIE_ROOM_CLOSE))
                {
                    digitalWrite(PIN_MOTOR_DOOR_DIE_ROOM_OPEN, HIGH);
                    digitalWrite(PIN_MOTOR_DOOR_DIE_ROOM_CLOSE, LOW);
                    TR_Door_RoomDie_On = true;
                }
                else if (bool(commandValue.toInt()) and digitalRead(PIN_GERCON_DOOR_DIE_ROOM_OPEN))
                {
                    digitalWrite(PIN_MOTOR_DOOR_DIE_ROOM_OPEN, LOW);
                    digitalWrite(PIN_MOTOR_DOOR_DIE_ROOM_CLOSE, HIGH);
                    TR_Door_RoomDie_On = true;
                }
            }
            else if (command == "md_derm_st")
            {
                digitalWrite(PIN_MOTOR_DOOR_DIE_ROOM_OPEN, HIGH);
                digitalWrite(PIN_MOTOR_DOOR_DIE_ROOM_CLOSE, HIGH);
                TR_Door_RoomDie_On = false;
            } // Дверь в комнату смерти
            else if (command == "md_dewl")
            {
                if (!bool(commandValue.toInt()) and digitalRead(PIN_GERCON_WALL_DIE_ROOM_CLOSE))
                {
                    digitalWrite(PIN_MOTOR_WALL_DIE_ROOM_OPEN, HIGH);
                    digitalWrite(PIN_MOTOR_WALL_DIE_ROOM_CLOSE, LOW);
                    TR_Door_WallDie_On = "Close";
                }
                else if (bool(commandValue.toInt()) and digitalRead(PIN_GERCON_WALL_DIE_ROOM_OPEN))
                {
                    digitalWrite(PIN_MOTOR_WALL_DIE_ROOM_OPEN, LOW);
                    digitalWrite(PIN_MOTOR_WALL_DIE_ROOM_CLOSE, HIGH);
                    TR_Door_WallDie_On = "Open";
                }
            }
            else if (command == "md_dewl_st")
            {
                digitalWrite(PIN_MOTOR_WALL_DIE_ROOM_OPEN, HIGH);
                digitalWrite(PIN_MOTOR_WALL_DIE_ROOM_CLOSE, HIGH);
                TR_Door_WallDie_On = "Stop";
            } // стена в комнату смерти
            // ОШИБКА
            else
            {
                PRINT("{r.c." + Last_Command + ":not_exist}\n");
                break;
            }
            PRINT("{r.c." + Last_Command + ":completed}\n");
        }
    }
}
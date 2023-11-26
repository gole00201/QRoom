//=========================================================================+
//                                 AUDIO                                   |
//-------------------------------------------------------------------------+
#include "GyverButton.h"
#include "SPI.h"      // библиотека для протокола SPI
#include "nRF24L01.h" // библиотека для nRF24L01+
#include "RF24.h"     // библиотека для радио модуля
#include "printf.h"

#define PIN_LED 3
#define PIN_BUT 2

//=========================================================================+
//                               ПЕРЕМЕННЫЕ                                |
//-------------------------------------------------------------------------+
struct { char Whom[10]=""; char Command[10]=""; char Parametr[5]=""; } MESSAGE;
struct { char From[5]="Kaska"; float Power=0; char Status[20]="123"; } KASKA;

bool TR_Led_Lock=false;

const uint8_t num_channels = 126; uint8_t values[num_channels];                                     //Для проверки радио
const int num_reps = 100; bool constCarrierMode = 0;                                                //Для проверки радио

//=========================================================================+
//                               ОБЪЕКТЫ                                   |
//-------------------------------------------------------------------------+
RF24 radio(9,10); // Для MEGA2560 замените на RF24 radio(9,53);
//GButton butt1(PIN_BUT);


//=========================================================================+
//                               ФУНКЦИИ                                   |
//-------------------------------------------------------------------------+
void PRINT(String Str) {
  //digitalWrite(PIN_RS485_CONTROL, HIGH);
  //delay(100);
  Serial.print(Str);
  //RS485.print(Str);
  //digitalWrite(PIN_RS485_CONTROL, LOW);
  //delay(500);
}


//=========================================================================+
//                                 СТАРТ                                   |
//-------------------------------------------------------------------------+
void setup() {
  Serial.begin(57600); Serial.println("\n");  
  
  printf_begin();

  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_BUT, INPUT_PULLUP);
   

}


//=========================================================================+
//                                  ЦИКЛ                                   |
//-------------------------------------------------------------------------+
void loop() {

  //Ручное управление каской
  if (!digitalRead(PIN_BUT) and !TR_Led_Lock) { delay(200); if (digitalRead(PIN_BUT)) digitalWrite(PIN_LED, !digitalRead(PIN_LED)); }

  //Мерцание
  static unsigned long TM_Blink_Work; static int Blink_Iter;
  if (Blink_Iter!=0 and millis()-TM_Blink_Work>50) { Blink_Iter--; TM_Blink_Work=millis(); analogWrite(PIN_LED, 125+random(-125,125)); if (Blink_Iter==0) digitalWrite(PIN_LED, LOW); }

  //Проверка радио
  static unsigned long TM_Up_Radio;
  if (millis()-TM_Up_Radio>1000){
    TM_Up_Radio=millis();
    //Инициализация радиомодуля
    radio.begin(); // включаем радио модуль                                             // Инициируем работу nRF24L01+
    //Serial.println(radio.begin());
    radio.setChannel      (100);                                // Указываем канал передачи данных (от 0 до 125), 27 - значит передача данных осуществляется на частоте 2,427 ГГц.
    radio.setDataRate     (RF24_1MBPS);                        // Указываем скорость передачи данных (RF24_250KBPS, RF24_1MBPS, RF24_2MBPS), RF24_1MBPS - 1Мбит/сек.
    radio.setPALevel      (RF24_PA_MAX);                       // Указываем мощность передатчика (RF24_PA_MIN=-18dBm, RF24_PA_LOW=-12dBm, RF24_PA_HIGH=-6dBm, RF24_PA_MAX=0dBm).

    radio.enableAckPayload();                                  // Указываем что в пакетах подтверждения приёма есть блок с пользовательскими данными.
    radio.openReadingPipe (1, 0xAABBCCDD22LL);                 // Открываем 1 трубу с адресом 0xAABBCCDD11, для приема данных.
    radio.startListening  ();                                  // Включаем приемник, начинаем прослушивать открытые трубы.
    radio.writeAckPayload (1, &KASKA, sizeof(KASKA) );     // Помещаем данные всего массива ackData в буфер FIFO. Как только будут получены любые данные от передатчика на 1 трубе, то данные из буфера FIFO будут отправлены этому передатчику вместе с пакетом подтверждения приёма его данных.
  }


  
  if(radio.available()){     
    //Serial.println(123);     
    //Serial.println(MESSAGE.Whom);                          
    radio.read(&MESSAGE, sizeof(MESSAGE)); 
    if (String(MESSAGE.Whom) == "To_Kaska"){
      Serial.println(MESSAGE.Whom);
      if (String(MESSAGE.Command) == "Set_Led") {
        if      (String(MESSAGE.Parametr) == "ON") { digitalWrite(PIN_LED, HIGH); TR_Led_Lock=false; }
        else if (String(MESSAGE.Parametr) == "BLLC" and Blink_Iter==0) { Blink_Iter=5; TR_Led_Lock=true; }
        else if (String(MESSAGE.Parametr) == "BL" and Blink_Iter==0) { Blink_Iter=5; TR_Led_Lock=false; }
      }
    }
  }

  //Принимаем команды
  static String Last_Command;
  if (Serial.available() > 0) {
    while (1) {
      String command = Serial.readStringUntil(']');                             // Отдельная команда
      String commandValue = "";                                                 // Значение, необязательно
      if (command.length() == 0) { break; }                                     // Если команды не поступило, прерываем цикл
      if (command.substring(0, 1) != "[") { continue; }                         // Если команда начинается не с [, пропускаем
      command = command.substring(1);                                           // Команда без начально [
      Last_Command = command; int delimeterIndex = command.lastIndexOf("=");    //Serial.println(command);
      if (delimeterIndex >= 0) { commandValue=command.substring(delimeterIndex+1); command=command.substring(0,delimeterIndex); }

      //ДРУГОЕ
      if (command == "ot_ckrd") {
        //Serial.println(F("\n\rRF24/examples/scanner/"));
        radio.begin();
        radio.setAutoAck(false);
        radio.startListening();
        radio.stopListening();
        radio.printDetails();
        
        int iiiii=0; while (iiiii < num_channels) { Serial.print(iiiii >> 4, HEX); ++iiiii; } Serial.println();
        iiiii=0;     while (iiiii < num_channels) { Serial.print(iiiii & 0xf, HEX); ++iiiii; } Serial.println();
        for (int ii=0;ii<5;ii++){
          // Configure the channel and power level below
          if (Serial.available()) {
            char c = Serial.read();
            if (c == 'g') { constCarrierMode = 1; radio.stopListening(); delay(2); Serial.println("Starting Carrier Out"); radio.startConstCarrier(RF24_PA_LOW, 40); } 
            else if (c == 'e') { constCarrierMode = 0; radio.stopConstCarrier(); Serial.println("Stopping Carrier Out"); }
          }
          if (constCarrierMode == 0) {
            memset(values, 0, sizeof(values));
             int rep_counter = num_reps;
            while (rep_counter--) { int i=num_channels; while (i--) { radio.setChannel(i); radio.startListening(); delayMicroseconds(128); radio.stopListening(); if (radio.testCarrier()) { ++values[i]; } } }
             int i = 0;
            while (i < num_channels) { if (values[i]) Serial.print(min(0xf, values[i]), HEX); else Serial.print(F("-")); ++i; } 
            Serial.println();
          } 
        }
        
      }
    //}
      //ОШИБКА
      else { PRINT("{r.c." + Last_Command + ":not_exist}\n"); break; } 
      PRINT("{r.c." + Last_Command + ":completed}\n");
    }
  }

  
}

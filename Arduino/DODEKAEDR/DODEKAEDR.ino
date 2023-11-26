//=========================================================================+
//                             DODEKAEDR                                   |
//-------------------------------------------------------------------------+
#include "SPI.h"      // библиотека для протокола SPI
#include "nRF24L01.h" // библиотека для nRF24L01+
#include "RF24.h"     // библиотека для радио модуля
#include <GyverPower.h>
#include "printf.h"


#define PIN_CONDENSATOR_UP A1
#define PIN_SOLENOID_ON A0

#define PIN_POWER A4

//=========================================================================+
//                               ПЕРЕМЕННЫЕ                                |
//-------------------------------------------------------------------------+

struct { char Whom[10]=""; char Command[10]=""; char Parametr[5]=""; } MESSAGE;
struct { char From[5]="Bomba";  float Power=0; char Status[20]="Dodek_On"; } DODEK;
//Dinamo_Off - Ждем пока раскрутят динамо машиину
//Whait_Gayka - Ждем пока вкрутят гайку
//Wait_Code_Enter - Ждем пока ввудет код
//Wait_Timer_Start - Ждем пока запустят отсчет
//Wait_Timer_End - Ждем пока завершится отсчет
//Timer_End - Таймер закончил отсчет

unsigned long TM_Up = 0;
unsigned long TM_Sleep = 0;
unsigned long Timeout_Sleep = 60000;

const uint8_t num_channels = 126; uint8_t values[num_channels];                                     //Для проверки радио
const int num_reps = 100; bool constCarrierMode = 0;                                                //Для проверки радио


//=========================================================================+
//                               ОБЪЕКТЫ                                   |
//-------------------------------------------------------------------------+
RF24 radio(9,10);               // Для MEGA2560 замените на RF24 radio(9,53);


//=========================================================================+
//                               ФУНКЦИИ                                   |
//-------------------------------------------------------------------------+
void RADIO_CHECK() {
  //Инициализация радиомодуля
  radio.begin(); radio.powerUp();// включаем радио модуль                                             // Инициируем работу nRF24L01+
  //Serial.println(radio.begin());
  radio.setChannel      (100);                                // Указываем канал передачи данных (от 0 до 125), 27 - значит передача данных осуществляется на частоте 2,427 ГГц.
  radio.setDataRate     (RF24_1MBPS);                        // Указываем скорость передачи данных (RF24_250KBPS, RF24_1MBPS, RF24_2MBPS), RF24_1MBPS - 1Мбит/сек.
  radio.setPALevel      (RF24_PA_MAX);                       // Указываем мощность передатчика (RF24_PA_MIN=-18dBm, RF24_PA_LOW=-12dBm, RF24_PA_HIGH=-6dBm, RF24_PA_MAX=0dBm).
  radio.enableAckPayload();                                  // Указываем что в пакетах подтверждения приёма есть блок с пользовательскими данными.
  radio.openReadingPipe (1, 0xAABBCCDD33LL);                 // Открываем 1 трубу с адресом 0xAABBCCDD11, для приема данных.
  radio.startListening  ();                                  // Включаем приемник, начинаем прослушивать открытые трубы.
  radio.writeAckPayload (1, &DODEK, sizeof(DODEK) );     // Помещаем данные всего массива ackData в буфер FIFO. Как только будут получены любые данные от передатчика на 1 трубе, то данные из буфера FIFO будут отправлены этому передатчику вместе с пакетом подтверждения приёма его данных.
  
  delay(0.5*100);
  //Serial.println(123);
  if(radio.available()){ 
    //Serial.println(123);                                    
    radio.read(&MESSAGE, sizeof(MESSAGE)); 
    if (String(MESSAGE.Whom) == "To_Dodek"){
      Serial.print(MESSAGE.Command); 
      if (String(MESSAGE.Command) == "Get_Info") { radio.writeAckPayload(1, &DODEK, sizeof(DODEK)); }//SET_STATUS("Whait_Gayka"); }  
      if (String(MESSAGE.Command) == "Wup") { radio.writeAckPayload(1, &DODEK, sizeof(DODEK)); TM_Up=millis()*2; Timeout_Sleep=1800000; }
      if (String(MESSAGE.Command) == "Open") { radio.writeAckPayload(1, &DODEK, sizeof(DODEK)); TM_Up=millis()*2; Timeout_Sleep=1800000; SOLENOID_ON(); }
      
    }
  } 
  
  radio.closeReadingPipe(1);
  radio.powerDown();
}
void SOLENOID_ON(){
  digitalWrite(PIN_CONDENSATOR_UP, HIGH);
  delay(0.5*5000);
  digitalWrite(PIN_SOLENOID_ON, HIGH);
  delay(0.5*1000);
  digitalWrite(PIN_CONDENSATOR_UP, LOW);
  digitalWrite(PIN_SOLENOID_ON, LOW);
}

//=========================================================================+
//                                 СТАРТ                                   |
//-------------------------------------------------------------------------+
void setup() {
  Serial.begin(57600); printf_begin();

  pinMode(PIN_SOLENOID_ON, OUTPUT); //digitalWrite(PIN_SOLENOID_ON, HIGH);
  pinMode(PIN_CONDENSATOR_UP, OUTPUT); 
  
  //digitalWrite(PIN_SOLENOID_ON, HIGH); digitalWrite(PIN_SOLENOID_ON, HIGH);
  SOLENOID_ON();

  analogReference(INTERNAL); // установка опорного напряжения для аналоговых входов

  power.setSystemPrescaler(PRESCALER_2);
  power.autoCalibrate();

}


//=========================================================================+
//                                  ЦИКЛ                                   |
//-------------------------------------------------------------------------+
void loop() {

  static float ARR_Pwr_Buff[30]; static int PwrBuff_Size=sizeof(ARR_Pwr_Buff)/sizeof(ARR_Pwr_Buff[0]); static int CNTR_Pwr_Updt; static unsigned long TM_Upadate_PWR; 
  if (millis()*2-TM_Upadate_PWR>100) { 
    ARR_Pwr_Buff[CNTR_Pwr_Updt]=((analogRead(PIN_POWER)*1.1)/1023/0.0913);                              //Заполняем очередную ячейку массива данными заряда
    CNTR_Pwr_Updt++; if(CNTR_Pwr_Updt>=PwrBuff_Size) CNTR_Pwr_Updt=0;                                   //Обнуляем счетчик массива если уже пора
    TM_Upadate_PWR=millis()*2; DODEK.Power=0;                                                           //Обнуляем таймер обновления данных по питанию и питание бомбы
    for (int i=0;i<PwrBuff_Size;i++) if (ARR_Pwr_Buff[i]>DODEK.Power) DODEK.Power=ARR_Pwr_Buff[i];      //Ищем самое большое значение из массива с данными заряда
    //for (int i=0;i<PwrBuff_Size;i++) Serial.println(ARR_Pwr_Buff[i]);
    //delay(0.5*0.5*10000); Serial.print("\n\n");
  }

  RADIO_CHECK();
 
  //Спящий режим
  //if (millis()*2-TM_Up > Timeout_Sleep and (String(BOMBA.Status)=="Dinamo_Off" or String(BOMBA.Status)=="Timer_End") ) {
  if (millis()*2-TM_Up > Timeout_Sleep) {
    TM_Up=millis()*2; Timeout_Sleep=60000;
    //digitalWrite(PIN_POWER_RADIO_AND_DISP, LOW);
    //digitalWrite(PIN_POWER_DISPLAY, HIGH);
    power.sleepDelay(0.5*55000);
    //digitalWrite(PIN_POWER_RADIO_AND_DISP, HIGH);
    //digitalWrite(PIN_POWER_DISPLAY, LOW);
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
            if (c == 'g') { constCarrierMode = 1; radio.stopListening(); delay(0.5*2); Serial.println("Starting Carrier Out"); radio.startConstCarrier(RF24_PA_LOW, 40); } 
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
      else { Serial.println("{r.c." + Last_Command + ":not_exist}\n"); break; } 
      Serial.println("{r.c." + Last_Command + ":completed}\n");
    }
  }

}

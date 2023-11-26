//=========================================================================+
//                                 AUDIO                                   |
//-------------------------------------------------------------------------+
#include <SoftwareSerial.h>
#include <DFPlayer_Mini_Mp3.h>
#include "SPI.h"             
#include "nRF24L01.h"        
#include "RF24.h"  
#include "printf.h"


#define PIN_PLAYER_STATUS_1 A3
#define PIN_PLAYER_STATUS_2 A2
#define PIN_PLAYER_STATUS_3 A1
#define PIN_PLAYER_STATUS_4 A0

#define CE_PIN 10
#define CSN_PIN A4

//=========================================================================+
//                               ПЕРЕМЕННЫЕ                                |
//-------------------------------------------------------------------------+
int ARR_MP3_NAME[4] = {0,0,0,0};

struct { char Whom[10]="To_Bomba"; char Command[10]="Get_Info"; char Parametr[5]="DO"; } MESSAGE;    //Сообщение для устройств
struct { char From[5]=""; float Power=0; char Status[20]="Zero"; } BOMBA;                      //Объект бомбы
struct { char From[5]=""; float Power=0; char Status[20]="Zero"; } KASKA;                      //Объект бомбы
struct { char From[5]=""; float Power=0; char Status[20]="Zero"; } DODEK;                      //Объект бомбы

const uint8_t num_channels = 126; uint8_t values[num_channels];                                     //Для проверки радио
const int num_reps = 100; bool constCarrierMode = 0;                                                //Для проверки радио

bool TR_Player_1_loop = false;

//=========================================================================+
//                               ОБЪЕКТЫ                                   |
//-------------------------------------------------------------------------+
RF24 radio(CE_PIN, CSN_PIN); //Объект радиомодуля Для MEGA2560 замените на RF24 radio(9,53);
SoftwareSerial Serial_mp3_4(3, 2); // RX, TX
SoftwareSerial Serial_mp3_3(5, 4); // RX, TX
SoftwareSerial Serial_mp3_2(7, 6); // RX, TX
SoftwareSerial Serial_mp3_1(9, 8); // RX, TX


//=========================================================================+
//                               ФУНКЦИИ                                   |
//-------------------------------------------------------------------------+
int CONVERT_AUDIO_NAME(int Num){
  switch(Num){
    case 1001: return 1; break;
    case 1002: return 2; break;
    case 1003: return 3; break;
    case 1004: return 4; break;
    case 1005: return 5; break;
    case 1006: return 6; break;
    case 1007: return 7; break;
    case 2001: return 8; break;
    case 2002: return 9; break;
    case 3001: return 10; break;
    case 3002: return 11; break;
    case 3003: return 12; break;
    case 3004: return 13; break;
    case 3005: return 14; break;
    case 3006: return 15; break;
    case 3007: return 16; break;
    case 3008: return 17; break;
    case 3009: return 18; break;
    case 3010: return 19; break;
    case 3011: return 20; break;
    case 3012: return 21; break;
    case 3013: return 22; break;
    case 3014: return 23; break;
    case 3015: return 24; break;
    case 3016: return 25; break;
    case 3017: return 26; break;
    case 3018: return 27; break;
    case 3019: return 28; break;
    case 3020: return 29; break;
    case 3021: return 30; break;
    case 3022: return 31; break;
    case 3023: return 32; break;
    case 3024: return 33; break;
    case 3025: return 34; break;
    case 3026: return 35; break;
    case 3027: return 36; break;
    case 3028: return 37; break;
    case 3029: return 38; break;
    case 3030: return 39; break;
    case 3031: return 40; break;
    case 3032: return 41; break;
    case 3033: return 42; break;
    case 3034: return 43; break;
    case 3035: return 44; break;
    case 3036: return 45; break;
    case 3037: return 46; break;
    case 3038: return 47; break;
    case 3039: return 48; break;
    
    case 3050: return 49; break;
    case 3051: return 50; break;
    case 3052: return 51; break;
    case 3053: return 52; break;
    case 3054: return 53; break;
    case 3055: return 54; break;
    case 3056: return 55; break;
    case 3057: return 56; break;
    case 3058: return 57; break;
    case 3059: return 58; break;
    case 3060: return 59; break;
    case 3061: return 60; break;
    case 3062: return 61; break;
    case 3063: return 62; break;
    case 3064: return 63; break;
    case 3065: return 64; break;
    case 3066: return 65; break;
    case 3067: return 66; break;
    case 3068: return 67; break;
    case 3069: return 68; break;
    case 3070: return 69; break;
    case 3071: return 70; break;
    case 3072: return 71; break;
    case 3073: return 72; break;
    case 3074: return 73; break;
    case 3075: return 74; break;
    case 3076: return 75; break;
    case 3077: return 76; break;
    case 3078: return 77; break;
    case 3079: return 78; break;
    case 3080: return 79; break;
    case 3081: return 80; break;
    case 3082: return 81; break;
    case 3083: return 82; break;
    case 3084: return 83; break;
    case 3085: return 84; break;
    case 3086: return 85; break;
    case 3087: return 86; break;
    case 3088: return 87; break;
    case 3089: return 88; break;
    case 3090: return 89; break;
    case 3091: return 90; break;
    case 3092: return 91; break;
    case 3093: return 92; break;

    case 4001: return 93; break;
    case 4002: return 94; break;
    case 4003: return 95; break;
    case 4004: return 96; break;
    case 4005: return 97; break;
    case 4006: return 98; break;
    case 4007: return 99; break;
    case 4008: return 100; break;
    case 4009: return 101; break;
    case 4010: return 102; break;
    case 4011: return 103; break;
    case 4012: return 104; break;
    case 4014: return 105; break;
    case 4015: return 106; break;
    case 4016: return 107; break;
    case 4017: return 108; break;
    case 4018: return 109; break;
    case 4019: return 110; break;
    case 4020: return 111; break;
    case 4021: return 112; break;
    case 4022: return 113; break;
    case 4023: return 114; break;
    case 4024: return 115; break;
    case 4025: return 116; break;
    case 4026: return 117; break;
    case 4027: return 118; break;
    case 4028: return 119; break;
    case 4029: return 120; break;
    case 5003: return 121; break;
    case 5004: return 122; break;
  }
}
void MP3_PLAY(int Player_Num, int Mp3_Num){
  switch (Player_Num) {
    case 1: mp3_set_serial (Serial_mp3_1); TR_Player_1_loop=true; break;
    case 2: mp3_set_serial (Serial_mp3_2); break;
    case 3: mp3_set_serial (Serial_mp3_3); break;
    case 4: mp3_set_serial (Serial_mp3_4); break;
  }
  mp3_play(CONVERT_AUDIO_NAME(Mp3_Num));
  delay (1000);
  ARR_MP3_NAME[Player_Num-1] = Mp3_Num;
}
void MP3_VOLUME(int Player_Num, int Vol){
  switch (Player_Num) {
    case 1: mp3_set_serial (Serial_mp3_1); break;
    case 2: mp3_set_serial (Serial_mp3_2); break;
    case 3: mp3_set_serial (Serial_mp3_3); break;
    case 4: mp3_set_serial (Serial_mp3_4); break;
  }
  mp3_set_volume(Vol);
  delay (100);
}
void MP3_STOP(int Player_Num){
  switch (Player_Num) {
    case 1: mp3_set_serial (Serial_mp3_1); TR_Player_1_loop=false; break;
    case 2: mp3_set_serial (Serial_mp3_2); break;
    case 3: mp3_set_serial (Serial_mp3_3); break;
    case 4: mp3_set_serial (Serial_mp3_4); break;
  }
  mp3_stop();
  delay (100);
  ARR_MP3_NAME[Player_Num-1] = 0;
}
void SM(String Str, bool TR) {
  static unsigned long NUM; NUM+=1;
  static String CMD=""; 
  unsigned long TM_After_Start = millis() / 1000ul; String Time;
  int H = (TM_After_Start / 3600ul); 
  int M = (TM_After_Start % 3600ul)/60ul;  
  int S = (TM_After_Start % 3600ul)%60ul;
  if (H>9) Time+=String(H); else Time+="0"+String(H); Time+=":";
  if (M>9) Time+=String(M); else Time+="0"+String(M); Time+=":";
  if (S>9) Time+=String(S); else Time+="0"+String(S); 

  Serial.print("{s.d.c:" + String(CMD) + "}");
  Serial.print("{s.d.n:" + String(NUM) + "}");
  Serial.print("{s.d.t:" + String(millis()) + "}");
  //Serial.print("["+Time+"]["+String(NUM)+"]["+CMD+"]---");
  CMD=Str; if (TR) Serial.println();
}
void PRINT(String Str) {
  //digitalWrite(PIN_RS485_CONTROL, HIGH);
  //delay(100);
  Serial.print(Str);
  //RS485.print(Str);
  //digitalWrite(PIN_RS485_CONTROL, LOW);
  //delay(500);
} 
String ADD_HASH(String Message){

  uint8_t Hash = 0;                          
  for (int i=0; i<Message.length(); i++) { Hash += Message[i]; 
    //Serial.print("\n"+ String(Message[i]) +" = "+ String(byte(Message[i])) +" | Hash + "+ String(Message[i]) +" = "+ String(Hash)); 
  }

  char Hash_Synbol = Hash; //Serial.print("\n\n" +String(Hash_Synbol)+ " = " +String(Hash)+ " | Hash = " +String(Hash_Synbol));
  Message += Hash_Synbol; //Serial.print("\n"+ Message); delay(60000);
  return Message;
  
}


//=========================================================================+
//                                 СТАРТ                                   |
//-------------------------------------------------------------------------+
void setup() {
  Serial.begin(57600); Serial.setTimeout(200); PRINT("\n"); 
  printf_begin();

  Serial_mp3_1.begin(9600); Serial_mp3_2.begin(9600); Serial_mp3_3.begin(9600); Serial_mp3_4.begin(9600);
  pinMode(PIN_PLAYER_STATUS_1, INPUT); pinMode(PIN_PLAYER_STATUS_2, INPUT); pinMode(PIN_PLAYER_STATUS_3, INPUT); pinMode(PIN_PLAYER_STATUS_4, INPUT);
  MP3_VOLUME(1,30); MP3_VOLUME(2,30); MP3_VOLUME(3,30); MP3_VOLUME(4,30);
  MP3_STOP(1); MP3_STOP(2); MP3_STOP(3); MP3_STOP(4);
}


//=========================================================================+
//                                  ЦИКЛ                                   |
//-------------------------------------------------------------------------+
void loop() {  
  //ADD_HASH("Hello_Жопа");

  //Первый плеер становится тише, когда второй играет
  static bool TR_2_Fone_On; if (TR_2_Fone_On and ARR_MP3_NAME[1]==0) { TR_2_Fone_On=false; MP3_VOLUME(1, 30);}

  


  //Отправляем информацию на сервер 
  static unsigned long NUM; NUM+=1; static String Last_Command; static unsigned long TM_Status_Message; 
  static String TR_Bomba_Stat; static String TR_Bomba_Pass; static String TR_Bomba_Timer; static bool TR_Bomba_On; static bool TR_Bomba_Wup;
  if (millis()-TM_Status_Message>500) {
    static int Message_Num;
    bool ARR[4] = {!digitalRead(PIN_PLAYER_STATUS_1), !digitalRead(PIN_PLAYER_STATUS_2), !digitalRead(PIN_PLAYER_STATUS_3), !digitalRead(PIN_PLAYER_STATUS_4)};
    
    switch (Message_Num) {
      case 0: { PRINT("{s.d.n:" + String(NUM) + "}");    Message_Num++; break; }
      case 1: { PRINT("{s.d.t:" + String(millis()) + "}");    Message_Num++; break; }
      case 2: { PRINT("{s.d.c:" + String(Last_Command) + "}");    Message_Num++; break; }
      case 3: { PRINT("{i.b.n:AUDIO}");    Message_Num++; break; }

      case 4: { PRINT("{s.r.o:" + String(radio.begin()) + "}");    Message_Num++; break; }
      case 5: { 
        strcpy(MESSAGE.Whom, "To_Bomba"); 
        if      (TR_Bomba_Stat!="") { strcpy(MESSAGE.Command, "Set_Stat"); strcpy(MESSAGE.Parametr, TR_Bomba_Stat.c_str()); }
        else if (TR_Bomba_Pass!="") { strcpy(MESSAGE.Command, "Set_Pass"); strcpy(MESSAGE.Parametr, TR_Bomba_Pass.c_str()); }
        else if (TR_Bomba_Timer!="") { strcpy(MESSAGE.Command, "Set_Timer"); strcpy(MESSAGE.Parametr, TR_Bomba_Timer.c_str()); }
        else strcpy(MESSAGE.Command, "Get_Info"); 

        radio.setChannel(100); // выбираем канал (от 0 до 127)
        radio.setDataRate(RF24_1MBPS); // скорость: RF24_250KBPS, RF24_1MBPS или RF24_2MBPS
        radio.setPALevel(RF24_PA_HIGH); // мощность: RF24_PA_MIN=-18dBm, RF24_PA_LOW=-12dBm, RF24_PA_MED=-6dBM
        radio.enableAckPayload(); // Указываем что в пакетах подтверждения приёма есть блок с пользовательскими данными.
        radio.openWritingPipe(0xAABBCCDD11LL); // Открываем трубу с адресом 0xAABBCCDD11 для передачи данных (передатчик может одновременно вещать только по одной трубе).
        delay(100);
        radio.write(&MESSAGE, sizeof(MESSAGE)); 
        if (radio.isAckPayloadAvailable()) { 
          PRINT("{s.b.o:1}"); radio.read(&BOMBA, sizeof(BOMBA)); 
          if (TR_Bomba_Stat!="") TR_Bomba_Stat=""; 
          else if (TR_Bomba_Pass!="") TR_Bomba_Pass=""; 
          else if (TR_Bomba_Timer!="") TR_Bomba_Timer=""; 
        } else PRINT("{s.b.o:0}");
        Message_Num++; break;
      }
      case 6: { PRINT("{s.b.v:" + String(BOMBA.Power) + "}");    Message_Num++; break; }
      case 7: { PRINT("{s.b.s:" + String(BOMBA.Status) + "}");    Message_Num++; break; }
      case 8: { 
        strcpy(MESSAGE.Whom, "To_Dodek"); 
        if  (TR_Bomba_On) { strcpy(MESSAGE.Command, "Open"); }
        else if  (TR_Bomba_Wup) { strcpy(MESSAGE.Command, "Wup"); }
        else strcpy(MESSAGE.Command, "Get_Info"); 

        radio.setChannel(100); // выбираем канал (от 0 до 127)
        radio.setDataRate(RF24_1MBPS); // скорость: RF24_250KBPS, RF24_1MBPS или RF24_2MBPS
        radio.setPALevel(RF24_PA_HIGH); // мощность: RF24_PA_MIN=-18dBm, RF24_PA_LOW=-12dBm, RF24_PA_MED=-6dBM
        radio.enableAckPayload(); // Указываем что в пакетах подтверждения приёма есть блок с пользовательскими данными.
        radio.openWritingPipe(0xAABBCCDD33LL); // Открываем трубу с адресом 0xAABBCCDD11 для передачи данных (передатчик может одновременно вещать только по одной трубе).
        delay(100);
        radio.write(&MESSAGE, sizeof(MESSAGE)); 
        if (radio.isAckPayloadAvailable()) { 
          PRINT("{s.d.o:1}"); radio.read(&DODEK, sizeof(DODEK)); 
          TR_Bomba_On=false; TR_Bomba_Wup=false;
        } else PRINT("{s.d.o:0}");
        Message_Num++; break;
      }
      case 9: { PRINT("{s.d.v:" + String(DODEK.Power) + "}");    Message_Num++; break; }

      case 10: { 
        PRINT("{s.p.s:");
        for (int i=0; i<sizeof(ARR_MP3_NAME)/sizeof(ARR_MP3_NAME[0]); i++) {
          if (ARR_MP3_NAME[i]!=0 and !ARR[i]) {
            if (TR_Player_1_loop and i==0) MP3_PLAY(1, ARR_MP3_NAME[i]);
            else ARR_MP3_NAME[i]=0;
          }
          PRINT(String(ARR_MP3_NAME[i]));
          if (i!=sizeof(ARR_MP3_NAME)/sizeof(ARR_MP3_NAME[0])-1) PRINT("/");
        }
        PRINT("}");    Message_Num++; break; 
      }

      case 11: { PRINT("\n"); Message_Num=0; TM_Status_Message=millis(); }
    }
    
  }


  //Принимаем команды
  if (Serial.available() > 0) {
    while (1) {
      String command = Serial.readStringUntil(']');                             // Отдельная команда
      String commandValue = "";                                                 // Значение, необязательно
      if (command.length() == 0) { break; }                                     // Если команды не поступило, прерываем цикл
      if (command.substring(0, 1) != "[") { continue; }                         // Если команда начинается не с [, пропускаем
      command = command.substring(1);                                           // Команда без начально [
      Last_Command = command; int delimeterIndex = command.lastIndexOf("=");    //Serial.println(command);
      if (delimeterIndex >= 0) { commandValue=command.substring(delimeterIndex+1); command=command.substring(0,delimeterIndex); }

      //ВОСПРОИЗВЕДЕНИЕ
      if      (command == "pl_1") { if(ARR_MP3_NAME[0]!=commandValue.toInt()) MP3_PLAY(1, commandValue.toInt()); } 
      else if (command == "pl_2") { if(ARR_MP3_NAME[1]!=commandValue.toInt()) MP3_PLAY(2, commandValue.toInt()); TR_2_Fone_On=true; MP3_VOLUME(1, 20); }
      else if (command == "pl_3") { if(ARR_MP3_NAME[2]!=commandValue.toInt()) MP3_PLAY(3, commandValue.toInt()); }
      else if (command == "pl_4") { if(ARR_MP3_NAME[3]!=commandValue.toInt()) MP3_PLAY(4, commandValue.toInt()); }
      //ГРОМКОСТЬ
      else if (command == "vl_1") MP3_VOLUME(1, commandValue.toInt()); 
      else if (command == "vl_2") MP3_VOLUME(2, commandValue.toInt());
      else if (command == "vl_3") MP3_VOLUME(3, commandValue.toInt());
      else if (command == "vl_4") MP3_VOLUME(4, commandValue.toInt());
      //ОСТАНОВКА
      else if (command == "st_1") MP3_STOP(1); 
      else if (command == "st_2") MP3_STOP(2);
      else if (command == "st_3") MP3_STOP(3);
      else if (command == "st_4") MP3_STOP(4);
      //ДРУГОЕ
      else if (command == "ot_ksk") {
        strcpy(MESSAGE.Whom, "To_Kaska"); strcpy(MESSAGE.Command, "Set_Led"); strcpy(MESSAGE.Parametr, commandValue.c_str() );
        for (int i=0;i<10;i++){
          radio.begin();
          radio.setChannel(100); // выбираем канал (от 0 до 127)
          radio.setDataRate(RF24_1MBPS); // скорость: RF24_250KBPS, RF24_1MBPS или RF24_2MBPS
          radio.setPALevel(RF24_PA_HIGH); // мощность: RF24_PA_MIN=-18dBm, RF24_PA_LOW=-12dBm, RF24_PA_MED=-6dBM
          radio.enableAckPayload(); // Указываем что в пакетах подтверждения приёма есть блок с пользовательскими данными.
          radio.openWritingPipe(0xAABBCCDD22LL); // Открываем трубу с адресом 0xAABBCCDD11 для передачи данных (передатчик может одновременно вещать только по одной трубе).
          //strcpy(MESSAGE.Whom, "To_Kaska"); strcpy(MESSAGE.Command, "Set_Led"); strcpy(MESSAGE.Parametr, "On");
          delay(100);
          radio.write(&MESSAGE, sizeof(MESSAGE)); 
          if (radio.isAckPayloadAvailable()) radio.read(&KASKA, sizeof(KASKA)); 
        }
      }
      else if (command == "ot_bmss") TR_Bomba_Stat=commandValue.c_str();
      else if (command == "ot_bmsp") TR_Bomba_Pass=commandValue.c_str();
      else if (command == "ot_bmst") TR_Bomba_Timer=commandValue.c_str();
      else if (command == "ot_opn") TR_Bomba_On=true;
      else if (command == "ot_wup") TR_Bomba_Wup=true;

      
      /*{ 
        strcpy(MESSAGE.Command, "Set_Stat"); strcpy(MESSAGE.Parametr, commandValue.c_str());
        radio.write(&MESSAGE, sizeof(MESSAGE));
        if (radio.isAckPayloadAvailable() ){ radio.read(&BOMBA, sizeof(BOMBA)); };
      }*/
      else if (command == "ot_ckrd") {
        Serial.println(F("\n\rRF24/examples/scanner/"));
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


/*
  static unsigned long NUM; NUM+=1; static String Last_Command;
  bool ARR[4] = {!digitalRead(PIN_PLAYER_STATUS_1),!digitalRead(PIN_PLAYER_STATUS_2),!digitalRead(PIN_PLAYER_STATUS_3),!digitalRead(PIN_PLAYER_STATUS_4)};

  Serial.print("{s.d.n:" + String(NUM) + "}");
  Serial.print("{s.d.t:" + String(millis()) + "}");
  Serial.print("{s.d.c:" + String(Last_Command) + "}");
  Serial.print("{i.b.n:AUDIO}");
  //Serial.print("{i.a.p:"+String(!digitalRead(PIN_PLAYER_STATUS_1))+String(!digitalRead(PIN_PLAYER_STATUS_2))+String(!digitalRead(PIN_PLAYER_STATUS_3))+String(!digitalRead(PIN_PLAYER_STATUS_4))+"}");

  Serial.print("{s.p.s:");
  for (int i=0; i<sizeof(ARR_MP3_NAME)/sizeof(ARR_MP3_NAME[0]); i++) {
    if (ARR_MP3_NAME[i]!=0 and !ARR[i]) ARR_MP3_NAME[i]=0;
    Serial.print(String(ARR_MP3_NAME[i]));
    if (i!=sizeof(ARR_MP3_NAME)/sizeof(ARR_MP3_NAME[0])-1) Serial.print("/");
  }
  Serial.print("}");
  Serial.println();

  */
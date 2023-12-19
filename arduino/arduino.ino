#include "types.h"

void rs_receive_cfg(CFG_MSG_t* data){
    while (Serial.available() < sizeof(CFG_MSG_t)){}
    Serial.readBytes((char*)data, sizeof(CFG_MSG_t));
}

void rs_receive_msg(MSG_t* data){
    while (Serial.available() < sizeof(MSG_t)){}
    Serial.readBytes((char*)data, sizeof(MSG_t));
}

void rs_send_msg(MSG_t* data){
    Serial.write((uint8_t*)data, sizeof(MSG_t));
}

void cfg_pin(BOARD_STATE_t* cntx, uint16_t pin_cfg, bool analog){
    if(analog){
        return;
    } else {
        uint8_t pin_n     = (pin_cfg >> 8) & 0xFF;
        uint8_t mode      = (pin_cfg & 0x1) >> 0;
        uint8_t def_state = (pin_cfg & 0x2) >> 1;
        pinMode(pin_n, mode);
        digitalWrite(pin_n, 0);
    }
}

uint8_t brd_analize_cfg(CFG_MSG_t* data, BOARD_STATE_t* cntx){
    if(data->crc_f){
        /*
        TODO
        */
    }
    if(data->start_f != MSG_START){
        return 0x01; /* СДЕЛАТЬ ENUM C ОШИБКАМИ */
    }
    for(size_t i = 0; i < d_PINS_CNT; ++i){
        cfg_pin(cntx, data->d_pins_cfg[i], false);
    }
    return 0x00;
}
void brd_analize_msg(MSG_t* data){
    /*
    TODO
    */
}

void setup()
{
	Serial.begin(9600);
}

void loop()
{
    BOARD_STATE_t cntx;
    cntx.addres = 0xFF; /* Чтение адреса?*/
    CFG_MSG_t brd_cfg = {0};
	rs_receive_cfg(&brd_cfg);
    brd_analize_cfg(&brd_cfg, &cntx);
}

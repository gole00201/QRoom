#include "types.h"

void rs_receive_cfg(CFG_MSG_t* data){
    while (Serial.available() < sizeof(CFG_MSG_t)){}
    Serial.readBytes((char*)&data, sizeof(CFG_MSG_t));
}

void rs_receive_msg(MSG_t* data){
    while (Serial.available() < sizeof(MSG_t)){}
    Serial.readBytes((char*)&data, sizeof(MSG_t));
}

void rs_send_msg(MSG_t* data){
    Serial.write((uint8_t*) &data, sizeof(MSG_t));
}

void cfg_pin(BOARD_STATE_t* cntx, uint16_t pin_cfg){
    cntx->pins_data[(pin_cfg >> 8) & 0XFF] = pin_cfg;

}

uint8_t brd_analize_cfg(CFG_MSG_t* data, BOARD_STATE_t* cntx){
    if(data->crc_f){
        /*
        TODO
        */
    }
    if(data->start_f != MSG_START){
        return 0xFA; /* СДЕЛАТЬ ENUM C ОШИБКАМИ */
    }
    if(data->pins_cnt > d_PINS_CNT + a_PINS_CNT){
        return 0xFF; /* СДЕЛАТЬ ENUM C ОШИБКАМИ */
    }
    for(size_t i = 0; i < d_PINS_CNT; ++i){
        cfg_pin(cntx, data->d_pins_cfg[i], i);
    }
    for(size_t i = 0; i < a_PINS_CNT; ++i){
        cfg_pin(cntx, data->a_pins_cfg[i], i);
    }
}
void brd_analize_msg(MSG_t* data){
    /*
    TODO
    */
}



void setup()
{
	Serial.begin(9600);
    BOARD_STATE_t cntx;
    cntx.addres = 0xFF; /* Чтение адреса?*/
}

void loop()
{
    CFG_MSG_t brd_cfg = {0};
	rs_receive_cfg(&brd_cfg);
}

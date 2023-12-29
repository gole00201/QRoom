#include "brd.h"

void rs_get_cfg(CFG_PACK* cfgPack){
    while (Serial.available() < 1) {}
    cfgPack->st_f = Serial.read();

    while (Serial.available() < 1) {}
    cfgPack->addr = Serial.read();
    while (Serial.available() < 1) {}
    cfgPack->pin_cnt = Serial.read();
    cfgPack->pins_cfg = new CFG_PIN_MSG[cfgPack->pin_cnt];

    while (Serial.available() < cfgPack->pin_cnt * sizeof(CFG_PIN_MSG)) {}
    Serial.readBytes((uint8_t *)cfgPack->pins_cfg,
                     cfgPack->pin_cnt * sizeof(CFG_PIN_MSG));

    while (Serial.available() < sizeof(uint16_t)) {}
    Serial.readBytes((uint8_t *)&cfgPack->crc,
                     sizeof(uint16_t));

    while (Serial.available() < 1) {}
    cfgPack->en_f = Serial.read();
}

void brd_cfg_pin(CFG_PIN_MSG cfg, BRD_STATE* brd, size_t i){
    brd->pins[i].cfg = cfg;
    PIN_STATE *pin = &brd->pins[i];
    if(pin->cfg.pin_t == 0x0D){
        /* Цифровой */
        if(pin->cfg.pin_mode == 0xFF){
            /*Тестовая конфигурация, нужна таблица*/
            pinMode(pin->cfg.pin_n, OUTPUT);
            pin->action = digital_write_action;
            pin->write = 0;
        }
        if(pin->cfg.pin_mode == 0xAA){
            pinMode(pin->cfg.pin_n, INPUT);
            pin->action = digital_read_action;
            pin->write = 0;
        }
    }
    if(pin->cfg.pin_t == 0x0A){
        /* Аналоговый */
        if(pin->cfg.pin_mode == 0xFF){
            /* analogWrite()*/
            /* Указатель на юнион с типами?*/
        }
    }
}

uint8_t brd_parse_cfg(CFG_PACK cfg, BRD_STATE* brd){
    if (cfg.st_f != ST_F){
        return 0x01;
    }
    if (cfg.en_f != EN_F){
        return 0x02;
    }
    if (cfg.addr != 0x01){
        return 0x03;
    }
    brd->addr = cfg.addr;
    brd->pins_cnt = cfg.pin_cnt;
    brd->pins = new PIN_STATE[brd->pins_cnt];
    for (size_t i = 0; i < cfg.pin_cnt; ++i){
        brd_cfg_pin(cfg.pins_cfg[i], brd, i);
    }
    return 0x00;
}

void rs_send_state(BRD_STATE cntx){
    Serial.write(ST_F);
    Serial.write(cntx.addr);
    Serial.write(cntx.pins_cnt);
    for(size_t i = 0; i < cntx.pins_cnt; ++i){
        Serial.write(cntx.pins[i].cfg.pin_n);
        Serial.write(cntx.pins[i].read);
        Serial.write(cntx.pins[i].write);
    }
    Serial.write(0xFF); // CRC - 16
    Serial.write(0xFF);
    Serial.write(EN_F);
}

int digital_write_action(uint8_t pin_n, int data){
    if(data > 0){
        digitalWrite(pin_n, HIGH);
    } else{
        digitalWrite(pin_n, LOW);
    }
    return data;
}

int digital_read_action(uint8_t pin_n, int data){
    return digitalRead(pin_n);
}

int analog_read_action(uint8_t pin_n, int data){
    return analogRead(pin_n);
}

int abalog_write_action(uint8_t pin_n, int data){
    analogWrite(pin_n, data);
    return data;
}

void rs_get_check_msg(CHANGE_MSG* data){
    Serial.readBytesUntil(0xaf, (uint8_t *)data, sizeof(CHANGE_MSG));
}
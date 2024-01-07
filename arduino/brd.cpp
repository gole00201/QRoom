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

    while (Serial.available() < sizeof(uint8_t)) {}
    Serial.readBytes((uint8_t *)&cfgPack->crc, sizeof(uint8_t));
    while (Serial.available() < 1) {}
    cfgPack->en_f = Serial.read();
}

void brd_cfg_pin(CFG_PIN_MSG cfg, BRD_STATE* brd, size_t i){
    brd->pins[i].cfg = cfg;
    PIN_STATE *pin = &brd->pins[i];
    if(pin->cfg.pin_t == 0x0D){
        /* Цифровой 1/0*/
        if(pin->cfg.pin_mode == WRITE){
            pinMode(pin->cfg.pin_n, OUTPUT);
            pin->action = digital_write_action;
            pin->write = 0;
        } else if(pin->cfg.pin_mode == READ){
            pinMode(pin->cfg.pin_n, INPUT);
            pin->action = digital_read_action;
            pin->write = 0;
        } else if(pin->cfg.pin_mode == PWM){
            pinMode(pin->cfg.pin_n, OUTPUT);
            pin->action = analog_write_action;
            pin->write = 0;
        } else if (pin->cfg.pin_mode == BLINKER){
            pinMode(pin->cfg.pin_n, OUTPUT);
            pin->action = light_blink_action;
            pin->write = 0;
        } else if (pin->cfg.pin_mode == RFID){
            pin->rfid = OneWire(pin->cfg.pin_n);
            pin->action = NULL;
            pin->rfid_action = rfid_action;
            pin->write = 0;
        } else{
            pin->action = NULL;
        }
    }
    if(pin->cfg.pin_t == ANALOG){
        /* Аналоговый */
        if(pin->cfg.pin_mode == READ){
           pinMode(pin->cfg.pin_n, INPUT);
           pin->action = analog_read_action;
           pin->write = 0;
        } else {
            pin->action = NULL;
        }
    }
}

uint8_t brd_parse_cfg(CFG_PACK cfg, BRD_STATE* brd){
    if (cfg.st_f != ST_F){
        Serial.write(0xFA);
        Serial.write(0x01);
        Serial.write(0xAF);
        return 0x01;
    }
    if (cfg.addr != 0x01){
        Serial.write(0xFA);
        Serial.write(0x03);
        Serial.write(0xAF);
        return 0x03;
    }
    uint8_t crc_val = crc8((uint8_t*) cfg.pins_cfg,
                            cfg.pin_cnt * sizeof(CFG_PIN_MSG));
    if(crc_val != cfg.crc){
        Serial.write(0xFA);
        Serial.write(0x04);
        Serial.write(0xAF);
        return 0x04;
    }
    if (cfg.en_f != EN_F){
        Serial.write(0xFA);
        Serial.write(0x02);
        Serial.write(0xAF);
        return 0x02;
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
    uint8_t crc = 0;
    Serial.write(ST_F);
    Serial.write(cntx.addr);
    Serial.write(cntx.pins_cnt);
    for(size_t i = 0; i < cntx.pins_cnt; ++i){
        Serial.write(cntx.pins[i].cfg.pin_n);
        crc8_dynamic(&crc, cntx.pins[i].cfg.pin_n);

        if(cntx.pins[i].cfg.pin_mode == 0x0C){
            uint8_t* rfid_read = (uint8_t*) &cntx.pins[i].read_rfid;
            for(int i = sizeof(uint64_t) - 1; i >= 0; i--){
                Serial.write(rfid_read[i]);
                crc8_dynamic(&crc, rfid_read[i]);
            }
        } else {
            Serial.write((cntx.pins[i].read >> 8) & 0xFF);
            crc8_dynamic(&crc, (cntx.pins[i].read >> 8) & 0xFF);
            Serial.write(cntx.pins[i].read & 0xFF);
            crc8_dynamic(&crc, cntx.pins[i].read & 0xFF);
        }


        Serial.write((cntx.pins[i].write >> 8) & 0xFF);
        crc8_dynamic(&crc, (cntx.pins[i].write >> 8) & 0xFF);

        Serial.write(cntx.pins[i].write & 0xFF);
        crc8_dynamic(&crc, cntx.pins[i].write & 0xFF);
    }
    Serial.write(crc);
    Serial.write(EN_F);
}

uint16_t digital_write_action(PIN_STATE *pin){
    if (pin->write_change){
        digitalWrite(pin->cfg.pin_n, pin->write);
        pin->write_change = false;
    }
    return pin->write;
}

uint16_t digital_read_action(PIN_STATE *pin){
    return digitalRead(pin->cfg.pin_n);
}

uint16_t analog_read_action(PIN_STATE *pin){
    return analogRead(pin->cfg.pin_n);
}


uint16_t analog_write_action(PIN_STATE *pin){
    if (pin->write_change){
        analogWrite(pin->cfg.pin_n, pin->write);
        pin->write_change = false;
    }
    return pin->write;
}

uint16_t light_blink_action(PIN_STATE *pin){
    if(pin->write){
        if(millis() % 10 == 0){
            analogWrite(pin->cfg.pin_n, (25 + random(-10, 100)));
        }
    } else {
        analogWrite(pin->cfg.pin_n, 0);
    }
    return pin->write;
}

uint64_t rfid_action(OneWire rfid){
    uint8_t addr[8];
    if (rfid.reset()) {
        rfid.write(0x33);
        // delay(2);
        for (int i = 0; i < 8; i++) {
            addr[i] = rfid.read();
        }
        uint64_t rfidAddress = 0;
        for (int i = 0; i < 8; i++) {
            rfidAddress |= static_cast<uint64_t>(addr[i]) << (i * 8);
        }
        return rfidAddress;
    } else {
        return 0;
    }
}

void rs_get_check_msg(CHANGE_MSG* data){
    while (Serial.available() < sizeof(CHANGE_MSG)) {}
    data->st_f = Serial.read();
    data->addr = Serial.read();
    data->comm = Serial.read();
    data->pin_n = Serial.read();
    data->write |= Serial.read() << 8;
    data->write = Serial.read();
    data->en_f = Serial.read();
    // Serial.readBytesUntil(0xaf, (uint8_t *)data, sizeof(CHANGE_MSG));
}

void brd_change_outs(CHANGE_MSG data, BRD_STATE* cntx){
    for (size_t i = 0; i < cntx->pins_cnt; ++i){
        if (cntx->pins[i].cfg.pin_n == data.pin_n){
            cntx->pins[i].write = data.write;
            cntx->pins[i].write_change = true;
        }
    }
}

uint8_t crc8(uint8_t* buffer, size_t size){
    uint8_t crc = 0;
    for (size_t i = 0; i < size; ++i) {
        uint8_t data = buffer[i];
        for (int j = 8; j > 0; --j) {
            if ((crc ^ data) & 1) {
                crc = (crc >> 1) ^ 0x8C;
            } else {
                crc >>= 1;
            }
            data >>= 1;
        }
    }
    return crc;
}

void crc8_dynamic(uint8_t* crc, uint8_t data) {
    for (int i = 0; i < 8; ++i) {
        if ((*crc ^ data) & 1) {
            *crc = (*crc >> 1) ^ 0x8C;
        } else {
            *crc >>= 1;
        }
        data >>= 1;
    }
}
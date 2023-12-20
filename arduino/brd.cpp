#include "brd.h"

void rs_get_cfg(CFG_PACK* cfgPack){
    while (Serial.available() < 1) {}
    cfgPack->st_f = Serial.read();

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
            pinMode(pin->cfg.pin_n, OUTPUT);
            pin->action = digitalWrite;
            pin->val = 0;
        }
    }
    if(pin->cfg.pin_t == 0x0A){
        /* Аналоговый */
        if(pin->cfg.pin_mode == 0xFF){
            /* analogWrite()*/
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
    brd->pins_cnt = cfg.pin_cnt;
    brd->pins = new PIN_STATE[brd->pins_cnt];
    for (size_t i = 0; i < cfg.pin_cnt; ++i){
        brd_cfg_pin(cfg.pins_cfg[i], brd, i);
    }
    return 0x00;
}
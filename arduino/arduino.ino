#include "brd.h"

void setup(){
    Serial.begin(9600);
}

void loop(){
    CFG_PACK cfg;
    BRD_STATE cntx;
    do{
        rs_get_cfg(&cfg);
    } while(brd_parse_cfg(cfg, &cntx));
    while (1)
    {
        for (size_t i = 0; i < cntx.pins_cnt; ++i){
            PIN_STATE pin = cntx.pins[i];
            pin.action(pin.cfg.pin_n, pin.val);
        }
        delay(500);
    }
}
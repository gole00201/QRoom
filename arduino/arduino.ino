#include "brd.h"

void setup(){
    Serial.begin(57600);
}

void loop(){
    CFG_PACK cfg;
    BRD_STATE cntx;
    CHANGE_MSG msg = {0};
    do{
        /* Принимаем конфигурацию*/
        rs_get_cfg(&cfg);
    } while(brd_parse_cfg(cfg, &cntx));
    while (1)
    {
        if (Serial.available()){
            /* Обрабатываем сообщение об изменении состояния*/
            rs_get_check_msg(&msg);
            brd_change_outs(msg, &cntx);
        }
        for (size_t i = 0; i < cntx.pins_cnt; ++i){
            /* Обновляем состояние каждого пина */
            PIN_STATE pin = cntx.pins[i];
            cntx.pins[i].read = pin.action(cntx.pins[i].cfg.pin_n, cntx.pins[i].write);
        }
        if (Serial.availableForWrite()){
            /* Отправляем актуальное состояние*/
            rs_send_state(cntx);
        }
    }
}

#include "brd.h"

void(* reboot) (void) = 0;

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
    Serial.write(0xFA);
    Serial.write(0xAA);
    Serial.write(0xAF);
    while (1)
    {
        if (Serial.available()){
            /* Обрабатываем сообщение об изменении состояния*/
            rs_get_check_msg(&msg);
            if (msg.comm != 0){
                if(msg.comm == TALK){
                    cntx.talk = 1;
                } else if (msg.comm == DONT_TALK){
                    cntx.talk = 0;
                } else if (msg.comm == REBOOT){
                    reboot();
                }
            } else {
                brd_change_outs(msg, &cntx);
            }
        }
        for (size_t i = 0; i < cntx.pins_cnt; ++i){
            /* Обновляем состояние каждого пина */
            PIN_STATE *pin = &cntx.pins[i];
            if(pin->action != NULL){
                cntx.pins[i].read = pin->action(pin);
            } else {
                cntx.pins[i].read_rfid = pin->rfid_action(cntx.pins[i].rfid);
            }
        }
        if (Serial.availableForWrite() && cntx.talk){
            /* Отправляем актуальное состояние*/
            rs_send_state(cntx);
        }
    }
}

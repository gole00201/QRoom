#define ST_F 0xFA
#define EN_F 0xAF
#include <stdint.h>
#include <Arduino.h>

typedef void (*FP)(uint8_t, uint8_t);

typedef struct CFG_PIN_MSG{
    uint8_t pin_n;
    uint8_t pin_t;
    uint8_t pin_mode;
}CFG_PIN_MSG;

typedef struct CFG_PACK{
    uint8_t st_f;
    uint8_t pin_cnt;
    CFG_PIN_MSG *pins_cfg;
    uint16_t crc;
    uint8_t en_f;
}CFG_PACK;

typedef struct PIN_STATE{
    CFG_PIN_MSG cfg;
    FP action;
    uint16_t val;
}PIN_STATE;

typedef struct BRD_STATE{
    uint8_t addr;
    uint8_t pins_cnt;
    PIN_STATE* pins;
}BRD_STATE;


void rs_get_cfg(CFG_PACK* cfg);

uint8_t brd_parse_cfg(CFG_PACK cfg, BRD_STATE* brd);

void brd_cfg_pin(CFG_PIN_MSG cfg, BRD_STATE* brd, size_t i);
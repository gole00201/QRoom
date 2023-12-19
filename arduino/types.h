#define d_PINS_CNT 13
#define a_PINS_CNT 5
#define MSG_START 0xFA

typedef void (*function_ptr)(int);

#pragma pack(1)
typedef struct CFG_MSG_t
{
    uint8_t start_f;
    uint16_t d_pins_cfg[d_PINS_CNT];
    uint16_t crc_f;
} CFG_MSG_t;

typedef struct MSG_t
{
    uint8_t start_f;
    uint8_t pin_n[d_PINS_CNT];
    uint8_t data;
    uint16_t crc;
} MSG_t;
#pragma pack()

typedef struct PIN_CFG_t {
    uint8_t pin_num;
    uint8_t pin_type;
    function_ptr f;
    uint32_t pins_data;
} PIN_CFG_t;

typedef struct BOARD_STATE_t
{
    uint8_t addres;
} BOARD_STATE_t;

void rs_receive_cfg(CFG_MSG_t* data);
uint8_t brd_analize_cfg(CFG_MSG_t* data, BOARD_STATE_t* cntx);

void rs_receive_msg(MSG_t* data);
void brd_analize_msg(MSG_t* data);

void rs_send_msg(MSG_t* data);

void cfg_pin(BOARD_STATE_t* cntx, uint16_t pin_cfg, bool analog);
#define ST_F 0xFA
#define EN_F 0xAF
#include <stdint.h>
#include <Arduino.h>

typedef uint16_t (*FP)(uint8_t, uint16_t);

/* Сообщение конфигурации пина*/
typedef struct CFG_PIN_MSG{
    uint8_t pin_n;
    uint8_t pin_t;
    uint8_t pin_mode;
}CFG_PIN_MSG;

/* Сообщение конфигурации контроллера*/
typedef struct CFG_PACK{
    uint8_t st_f;
    uint8_t addr;
    uint8_t pin_cnt;
    CFG_PIN_MSG *pins_cfg;
    uint16_t crc;
    uint8_t en_f;
}CFG_PACK;

/* Состояние пина*/
typedef struct PIN_STATE{
    CFG_PIN_MSG cfg;
    FP action;
    uint16_t read;
    uint16_t write;
}PIN_STATE;

/* Состояние контроллера*/
typedef struct BRD_STATE{
    uint8_t addr;
    uint8_t pins_cnt;
    PIN_STATE* pins;
}BRD_STATE;

/*Сообщение о смене состояния пина выхода*/
typedef struct CHANGE_MSG
{
    uint8_t st_f;
    uint8_t addr;
    uint8_t pin_n;
    uint8_t write; // !CRC 16 BITS
    uint8_t en_f;
};


void rs_send_state(BRD_STATE);

/**
 * @brief Принимает сообщение конфигурации от сервера
 *
 * @param cfg указатель на структуру сообщения
 */
void rs_get_cfg(CFG_PACK* cfg);

/**
 * @brief Разбирает сообщение конфигурации и заполняет состояние платы
 *
 * @param cfg сообщение конфигурации
 * @param brd состояние платы
 * @return uint8_t состояние ошибки TODO ENUM с ошибками
 */
uint8_t brd_parse_cfg(CFG_PACK cfg, BRD_STATE* brd);


/**
 * @brief Конфигурирует заданный пин и присваевает ему функцию
 *
 * @param cfg сообщение конфигурации
 * @param brd состояние платы
 * @param i номер пина (условный)
 */
void brd_cfg_pin(CFG_PIN_MSG cfg, BRD_STATE* brd, size_t i);

/**
 * @brief Шаблон функции цифровой записи
 *
 * @param pin_n номер пина
 * @param data данные под запись
 * @return int data
 */
uint16_t digital_write_action(uint8_t pin_n, uint16_t data);

/**
 * @brief Шаблон функции цифрового чтения
 *
 * @param pin_n номер пина
 * @param data не валидный аргумент
 * @return uint16_t прочитанные данные
 */
uint16_t digital_read_action(uint8_t pin_n, uint16_t data);

/**
 * @brief Шаблон функции аналогового чтения
 *
 * @param pin_n номер пина
 * @param data не валидный аргумент
 * @return int прочитанные данные
 */
uint16_t analog_read_action(uint8_t pin_n, uint16_t data);

/**
 * @brief Шаблон функции аналоговой записи
 *
 * @param pin_n номер пина
 * @param data данные под запись
 * @return int data
 */
uint16_t analog_write_action(uint8_t pin_n, uint16_t data);

/**
 * @brief Функция чтения сообщения об изменении состояния выхода
 *
 * @param data созданное на стеке сообщение
 */
void rs_get_check_msg(CHANGE_MSG* data);

void brd_change_outs(CHANGE_MSG data, BRD_STATE* cntx);
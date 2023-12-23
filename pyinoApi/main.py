#!/bin/python3
import struct
import serial
import serial.tools.list_ports
import json
import re
import time
BAUD = 9600
ST_F = 0xFA
EN_F = 0xAF

# TODO CRC-16 COM_MSG


class BrdConn:
    """Подключение конкретной платы (условный класс, его стоит убрать)"""
    def __init__(self):
        ports = serial.tools.list_ports.comports()
        for port in ports:
            if re.match(r'/dev/(cu\.usbserial-.+|ttyACM.+)', port.device):
                self.ser = serial.Serial(port.device, BAUD,
                                         stopbits=1, parity='N')
                time.sleep(2)
                break

    def send(self, data):
        self.ser.write(data)


class BrdPin:
    "Состояние пина"
    def __init__(self) -> None:
        self.id = ""
        self.type = ""
        self.name = ""
        self.mode = ""


class Brd:
    """Плата Arduino"""
    def __init__(self, brd_dict):
        self.type = brd_dict["boardType"]
        self.id = brd_dict["boardID"]
        self.list_of_pins = []
        self.conn = BrdConn()
        self.__configurate_pins(brd_dict["pins"])

    def __configurate_pins(self, pins_dict):
        """Создает строку форматирования для struct"""
        for pin in pins_dict:
            pin_t = BrdPin()
            pin_t.name = pin["pinName"]
            pin_t.type = pin["pinType"]
            pin_t.id = pin["pinID"]
            pin_t.mode = pin["pinMode"]
            self.list_of_pins.append(pin_t)
        self.pin_c = len(self.list_of_pins)
        # /* Сообщение конфигурации контроллера*/
        # typedef struct CFG_PACK{
        #     uint8_t st_f; - байт
        #     uint8_t pin_cnt; - байт
        #     CFG_PIN_MSG *pins_cfg; - байт * 3 * pin_c
        #     uint16_t crc; - 2 * байт
        #     uint8_t en_f; - байт
        # }CFG_PACK;
        fmt = f"!BB{'BBB' * self.pin_c}BBB"
        pin_cfg_list = []
        # TODO Общая таблица для конфигурации
        for pin in self.list_of_pins:
            p_mode = p_id = p_type = 0x00
            p_id = pin.id
            if pin.type == "digital":
                p_type = 0x0D
                if pin.mode == "write":
                    p_mode = 0xFF
            pin_cfg_list.append([int(p_id), int(p_type), int(p_mode)])
        pin_cfg_list = [i[0] for i in pin_cfg_list] + \
                       [i[1] for i in pin_cfg_list] + \
                       [i[2] for i in pin_cfg_list]
        self.cfg_str = struct.pack(fmt, ST_F,
                                   self.pin_c,
                                   *pin_cfg_list,
                                   0xFF,
                                   0xFF,
                                   EN_F)

    def configurate(self):
        self.conn.send(self.cfg_str)


class CfgParser:
    """Парсер конфигурации (условный класс, стоит его убрать)"""
    def __init__(self, board_path, omap_path):
        self.brds_cfg = self.__read_json(board_path)
        self.omap_cfg = self.__read_json(omap_path)

    def __read_json(self, path):
        with open(path) as f:
            data = f.read()
        return json.loads(data)


if __name__ == "__main__":
    cfg = CfgParser("../cfg/witcher_game/board.cfg.json",
                    "../cfg/witcher_game/omap.cfg.json")
    list_of_brd = []
    for brd in cfg.brds_cfg["boards"]:
        brd = Brd(brd)
        brd.configurate()
    while 1:
        pass

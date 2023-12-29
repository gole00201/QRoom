#!/bin/python3
import struct
import serial
import serial.tools.list_ports
import json
import re
import time
BAUD = 57600
ST_F = 0xFA
EN_F = 0xAF
EN_F_S = b'\xaf'

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
        self.read = 0
        self.write = 0


class Brd:
    """Плата Arduino"""
    def __init__(self, brd_dict):
        self.type = brd_dict["boardType"]
        self.id = brd_dict["boardID"]
        self.list_of_pins = []
        self.conn = BrdConn()
        self.state = {}
        self.pin_names = {}
        self.__configurate_pins(brd_dict["pins"])

    def __configurate_pins(self, pins_dict):
        """Создает строку форматирования для struct"""
        for pin in pins_dict:
            pin_t = BrdPin()
            pin_t.name = pin["pinName"]
            pin_t.type = pin["pinType"]
            pin_t.id = pin["pinID"]
            pin_t.mode = pin["pinMode"]
            self.state[pin_t.name] = pin_t
            self.pin_names[pin_t.id] = pin_t.name
            self.list_of_pins.append(pin_t)
        self.pin_c = len(self.list_of_pins)
        self.cfg_fmt = f"!BBB{'BBB' * self.pin_c}BBB"
        self.state_fmt = f"!BBB{'BBB' * self.pin_c}BBB"
        pin_cfg_list = []
        # TODO Общая таблица для конфигурации
        for pin in self.list_of_pins:
            p_mode = p_id = p_type = 0x00
            p_id = pin.id
            if pin.type == "digital":
                p_type = 0x0D
                if pin.mode == "write":
                    p_mode = 0xFF
                if pin.mode == "read":
                    p_mode = 0xAA
            pin_cfg_list += [int(p_id), int(p_type), int(p_mode)]
        self.cfg_str = struct.pack(self.cfg_fmt, ST_F, self.id,
                                   self.pin_c,
                                   *pin_cfg_list,
                                   0xFF,
                                   0xFF,
                                   EN_F)

    def get_state(self):
        read = self.conn.ser.read_until(EN_F_S)
        try:
            data = struct.unpack(self.state_fmt,
                                 read)
            addr = data[1]
            pin_c = data[2]
            pins = [(data[i], data[i + 1], data[i + 2])
                    for i in range(3, pin_c * 3, 3)]
            # TODO Ловить ошибки
            if addr != self.id:
                return "ERROR"
            for num, read, _ in pins:
                self.state[self.pin_names[str(num)]].read = read
        except struct.error:
            pass

    def check(self, pin_name):
        return self.state[pin_name].read

    def change(self, pin_name, data):
        self.state[pin_name].write = data
        pin_n = int(self.state[pin_name].id)
        fmt = "!BBBHB"
        msg = struct.pack(fmt, ST_F, self.id, pin_n, data, EN_F)
        self.conn.send(msg)

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
                    "../cfg/witcher_game/room.cfg.json")
    list_of_brd = []
    for brd in cfg.brds_cfg["boards"]:
        brd = Brd(brd)
        brd.configurate()
    k = 0
    s_t = 0
    while 1:
        brd.get_state()
        if time.time() - s_t > 0.03:
            brd.change("test_led", k % 2)
            brd.change("rel_1", k % 2)
            k += 1
            s_t = time.time()

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
DIGITAL = 0x0D
WRITE = 0xFF
READ = 0xAA
PWM = 0xF0
ANALOG = 0x0A

# TODO CRC-16 COM_MSG


class BrdConn:
    """Подключение конкретной платы (условный класс, его стоит убрать)"""
    def __init__(self):
        ports = serial.tools.list_ports.comports()
        for port in ports:
            # На данный момент с RS-485 не было отладки
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
        self.configurate_pins(brd_dict["pins"])
        self.cfg = f"!BBB{'BBB' * self.pin_c}BB"
        self.state_fmt = f"!BBB{'BHH' * self.pin_c}BBB"
        self.change_fmt = "!BBBBBB"
        self.cfg_with_crc = struct.pack(self.cfg, ST_F, self.id,
                                        self.pin_c,
                                        *self.pin_cfg_list,
                                        0xFF,
                                        EN_F)

    def crc8(self, buffer):
        crc = 0
        for data in buffer:
            for _ in range(8, 0, -1):
                if (crc ^ data) & 1:
                    crc = (crc >> 1) ^ 0x8C
                else:
                    crc >>= 1
                data >>= 1
        return crc & 0xFF

    def configurate_pins(self, pins_dict):
        """Парсинг конфига пинов"""
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
        self.pin_cfg_list = []
        for pin in self.list_of_pins:
            p_mode = p_id = p_type = 0x00
            p_id = pin.id
            if pin.type == "digital":
                p_type = DIGITAL
                if pin.mode == "write":
                    p_mode = WRITE
                elif pin.mode == "read":
                    p_mode = READ
                elif pin.mode == "pwm":
                    p_mode = PWM
            elif pin.type == "analog":
                p_type = ANALOG
                if pin.mode == "read":
                    p_mode = READ
                elif pin.mode == "write":
                    p_mode = WRITE
            self.pin_cfg_list += [int(p_id), int(p_type), int(p_mode)]

    def get_state(self):
        """Прием текущего состояния от контроллера"""
        read = self.conn.ser.read_until(EN_F_S)
        try:
            # TODO CRC и проверка ошибок
            data = struct.unpack(self.state_fmt, read)
            pin_c = data[2]
            pins = [(data[i], data[i + 1])
                    for i in range(3, pin_c * 3 + 1, 3)]
            for num, read in pins:
                self.state[self.pin_names[str(num)]].read = read
        except struct.error:
            pass

    def check(self, pin_name):
        """Обращение к текущему состоянию пина с которого читаем"""
        return self.state[pin_name].read

    def change(self, pin_name, data):
        """Запись в пин"""
        if data == self.state[pin_name].write:
            return
        self.state[pin_name].write = data
        pin_n = int(self.state[pin_name].id)
        msg = struct.pack(self.change_fmt,
                          ST_F, self.id,
                          pin_n, data, 0xFF,
                          EN_F)
        self.conn.send(msg)

    def configurate(self):
        self.conn.send(self.cfg_with_crc)


class CfgParser:
    """Парсер конфигурации (условный класс, стоит его убрать)"""
    def __init__(self, board_path, omap_path):
        self.brds_cfg = self.__read_json(board_path)
        self.omap_cfg = self.__read_json(omap_path)

    def __read_json(self, path):
        with open(path) as f:
            data = f.read()
        return json.loads(data)

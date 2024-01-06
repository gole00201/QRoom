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
BLINKER = 0x0F
RFID = 0x0C


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
        self.conn = BrdConn()
        self.list_of_pins = []
        self.state = {}
        self.pin_names = {}
        self.configurate_pins(brd_dict["pins"])
        self.configure_state_fmt()
        self.change_fmt = "!BBBHB"
        crc = self.crc8(self.pin_cfg_list)
        self.cfg_with_crc = struct.pack(self.cfg, ST_F, self.id,
                                        self.pin_c,
                                        *self.pin_cfg_list,
                                        crc,
                                        EN_F)

    def crc8(self, buffer):
        crc = 0x00
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
            if pin_t.id in list(self.pin_names.keys()):
                print(f"ERROR: pin id duplicate {pin_t.id}")
                self.conn.ser.close()
                exit(1)
            if pin_t.name in list(self.pin_names.values()):
                print(f"ERROR: pin name duplicate {pin_t.name}")
                self.conn.ser.close()
                exit(1)
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
                elif pin.mode == "blinker":
                    p_mode = BLINKER
                elif pin.mode == "rfid":
                    p_mode = RFID
            elif pin.type == "analog":
                p_type = ANALOG
                if pin.mode == "read":
                    p_mode = READ
                elif pin.mode == "write":
                    p_mode = WRITE
            self.pin_cfg_list += [int(p_id), int(p_type), int(p_mode)]
        self.cfg = f"!BBB{'BBB' * self.pin_c}BB"

    def configure_state_fmt(self):
        self.state_fmt = "!BBB"
        for pin in self.list_of_pins:
            if pin.mode == "rfid":
                self.state_fmt += "BQH"
            else:
                self.state_fmt += "BHH"
        self.state_fmt += "BB"

    def get_state(self):
        """Прием текущего состояния от контроллера"""
        read = self.conn.ser.read_until(EN_F_S)
        try:
            data = struct.unpack(self.state_fmt, read)
            pin_c = data[2]
            print(data)
            crc = self.crc8(read[3: -2])
            if crc != data[-2]:
                print("STATE: BROKEN_CRC")
                return
            pins = [(data[i], data[i + 1])
                    for i in range(3, pin_c * 3 + 1, 3)]
            for num, read in pins:
                self.state[self.pin_names[str(num)]].read = read
        except struct.error:
            print("STATE: BROKEN_PACK")

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
                          pin_n, data,
                          EN_F)
        self.conn.send(msg)

    def configurate(self):
        self.conn.send(self.cfg_with_crc)


class CfgParser:
    """Парсер конфигурации (условный класс, стоит его убрать)"""
    def __init__(self, board_path):
        self.brds_cfg = self.__read_json(board_path)

    def __read_json(self, path):
        with open(path) as f:
            data = f.read()
        return json.loads(data)

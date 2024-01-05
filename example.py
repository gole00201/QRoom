#!/bin/python3

# Это пример взаимодействия с API, api ещё в работе. Но большая
# часть функционала уже есть.
# !ВАЖНО
# В основном цикле почти нет предела тому как часто, мы
# меняем состояние выхода. Но лучше не делать это чаще чем в
# 5мс.


# По API осталось доделать CRC hash,
# проверку адресов. Ну и перенести все на RS-485.
# просто из-за НГ до сих пор не пришли конверторы =(
from pyinoApi.api import CfgParser, Brd
import time

if __name__ == "__main__":
    cfg = CfgParser("./cfg/witcher_game/board.cfg.json",
                    "./cfg/witcher_game/room.cfg.json")
    # Плата только одна (пока что)
    brd = Brd(cfg.brds_cfg["boards"][0])
    brd.configurate()
    print(f"ПЛАТА: {brd.type} {brd.id} сконфигурировна")
    s_t = time.time()
    k = 0
    brd.change("light_1", 1)
    while 1:
        k += 1
        brd.get_state()  # Каждый тик мы обновляем состояние контроллера
        brd.change("led_1", k % 2)
        print(k)
        # Основной цикл
        # Тут стадии игр обращение к БД, да в принципе что угодно
        # Теперь можно отреагировать на воздействия
        pass

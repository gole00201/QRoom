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

if __name__ == "__main__":
    cfg = CfgParser("./cfg/witcher_game/board.cfg.json",
                    "./cfg/witcher_game/room.cfg.json")
    # Плата только одна (пока что)
    brd = Brd(cfg.brds_cfg["boards"][0])
    brd.configurate()
    print(f"ПЛАТА: {brd.type} {brd.id} сконфигурировна")
    while 1:
        # Основной цикл
        # Тут стадии игр обращение к БД, да в принципе что угодно
        brd.get_state()  # Каждый тик мы обновляем состояние контроллера
        # Теперь можно отреагировать на воздействия
        if not brd.check("gerkon_1"):
            # И дать управляющие воздействия
            brd.change("rel_1", 1)
        else:
            brd.change("rel_1", 0)
        if not brd.check("gerkon_2"):
            brd.change("rel_2", 1)
            brd.change("test_led", 1)
        else:
            brd.change("rel_2", 0)
            brd.change("test_led", 0)

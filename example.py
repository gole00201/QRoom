#!/bin/python3

# Это пример взаимодействия с API, api ещё в работе. Но большая
# часть функционала уже есть.
# !ВАЖНО
# В основном цикле почти нет предела тому как часто, мы
# меняем состояние выхода. Но лучше не делать это чаще чем в
# 5мс.


# Ну и перенести все на RS-485.
from pyinoApi.api import CfgParser, Brd, TALK


if __name__ == "__main__":
    cfg = CfgParser("./cfg/witcher_game/board.cfg.json")
    brd = Brd(cfg.brds_cfg["boards"][0])
    brd.configurate()
    brd.comm(TALK)
    k = 1
    brd.change("smoke_heat", 1)
    while 1:
        k += 1
        brd.get_state()  # Каждый тик мы обновляем состояние контроллера
        brd.change("margnet_frog", k % 2)
        brd.change("megnet_chest", k % 2)
        # Основной цикл
        # Тут стадии игр обращение к БД, да в принципе что угодно
        # Теперь можно отреагировать на воздействия
        pass

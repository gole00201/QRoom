#!/bin/python3

import json


class Cfg:
    def __init__(self,
                 game_cfg_file_path,
                 omap_cfg_file_path,
                 board_cfg_file_path):

        self.imp_table = self.parse_omap(omap_cfg_file_path)
        self.board_table = self.parse_boards(board_cfg_file_path)
        self.game_table = self.parse_game(game_cfg_file_path)

    def parse_boards(self, path):
        with open(path) as json_file:
            data = json.load(json_file)
        return data

    def parse_omap(self, path):
        with open(path) as json_file:
            data = json.load(json_file)
        return data

    def parse_game(self, path):
        with open(path) as json_file:
            data = json.load(json_file)
        return data


class Pherepals:
    def __init__(self):
        pass

    def outs(self):
        pass

    def ins(self):
        pass


class Game:
    def __init__(self,
                 game_cfg_file_path,
                 omap_cfg_file_path,
                 board_cfg_file_path):
        self.cfg = Cfg(game_cfg_file_path,
                       omap_cfg_file_path,
                       board_cfg_file_path)
        self.current_state_id = 0
        self.current_expr_str = ""
        self.iter_state()

    def iter_state(self):
        for state in self.cfg.game_table["gameStates"]:
            while True:
                pass


if __name__ == "__main__":
    Game("./game.cfg.json", "./omap.cfg.json", "./board.cfg.json")

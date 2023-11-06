from abc import ABC, abstractmethod
from get_opening_moves import get_opening_moves
from solve import solve
import random

class Player(ABC):
    def __init__(self):
        self.open_turn = -1
        self.rearrange_turns = 0
        self.opened = False
        self.hand = []
        self.draw_count = 0

    @abstractmethod
    def _make_move(self, board):
        pass

    def make_move(self, board):
        move = self._make_move(board)
        if move != 'DRAW' and not self.opened:
            self.opened = True

        return move


class GreedyPlayer(Player):
    def _make_move(self, board):
        if self.opened:
            new_sets, unused_tiles = solve(board, self.hand)

            if not new_sets:
                self.draw_count += 1
                return 'DRAW'
            
            self.hand = unused_tiles

            for set_ in board:
                if set_ not in new_sets:
                    self.rearrange_turns += 1
                    break
            return new_sets
        else:
            moves = get_opening_moves(self.hand)
            if not moves:
                self.draw_count += 1
                return 'DRAW'

            move = max(moves, key=lambda x: sum([tile.number for set_ in x for tile in set_]))

            for set_ in move:
                for tile in set_:
                    self.hand.remove(tile)
            
            board.extend(move)
            return board

class CRFPlayer(Player):
    def _get_moves(self, board):
        moves = solve(board, self.hand)

        return moves

    def _make_move(self, board):
        if self.opened:
            moves = self._get_moves(board)
            shuffled_moves = random.sample(moves, len(moves))
            for move in shuffled_moves:
                new_sets, unused_tiles = move
                if not new_sets:
                    continue

                self.hand = unused_tiles

                for set_ in board:
                    if set_ not in new_sets:
                        self.rearrange_turns += 1
                        break

                return new_sets
            
            self.draw_count += 1
            return 'DRAW'
        else:
            moves = get_opening_moves(self.hand)
            if not moves:
                self.draw_count += 1
                return 'DRAW'

            move = max(moves, key=lambda x: sum([tile.number for set_ in x for tile in set_]))

            for set_ in move:
                for tile in set_:
                    self.hand.remove(tile)
            
            board.extend(move)
            return board

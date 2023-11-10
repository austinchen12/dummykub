import random
from models.tile import Tile
from constants import DUPLICATE_TILES, HAND_SIZE, UNIQUE_TILE_COUNT

class Rummikub:
    def __init__(self, players):
        self.history = []
        self.players = players
        self.board = []
        self.pool = [Tile(i) for i in range(UNIQUE_TILE_COUNT)
                     ] * DUPLICATE_TILES
        self.turn = 0
        self.turn_count = 0

    def _deal_hands(self):
        for _ in range(HAND_SIZE):
            for player in self.players:
                player.hand.append(self.pool.pop())

        for player in self.players:
            player.hand.sort(key=lambda x: (x.number, x.color))

    def _is_game_over(self):
        for player in self.players:
            if len(player.hand) == 0:
                return True
        
        return len(self.pool) == 0

    def start(self):
        random.shuffle(self.pool)
        self._deal_hands()

        while (not self._is_game_over()):
            # original_board = copy.deepcopy(self.board)
            self.turn_count += 1

            player = self.players[self.turn]
            print('START:', self.turn, self.board, player.hand)
            result = player.make_move(self.board)
            if result == 'DRAW':
                player.hand.append(self.pool.pop())
            else:
                if player.open_turn == -1:
                    player.open_turn = self.turn_count
                self.board = result

            # self.history.append((self.turn_count, self.turn, [copy.deepcopy(player.hand) for player in self.players], original_board, self.board))
            self.turn = (self.turn + 1) % len(self.players)

            print('END:', self.board, player.hand, '\n')

        runs, groups, tiles_played = 0, 0, 0
        for set_ in self.board:
            if set_[0].color == set_[1].color:
                groups += 1
            else:
                runs += 1
            tiles_played += len(set_)

        # for turn in self.history:
        #     print(turn)

        avg_tiles_leftover, avg_score, avg_draw_count, avg_open_turn, avg_rearrange_turns = 0, 0, 0, 0, 0
        for player in self.players:
            avg_tiles_leftover += len(player.hand)
            avg_score += len(player.hand)
            avg_draw_count += player.draw_count
            avg_open_turn += player.open_turn
            avg_rearrange_turns += player.rearrange_turns

        avg_tiles_leftover /= len(self.players)
        avg_score /= len(self.players)
        avg_draw_count /= len(self.players)
        avg_open_turn /= len(self.players)
        avg_rearrange_turns /= len(self.players)


        return {
            'total_tiles_played': tiles_played,
            'turn_count': self.turn_count,
            'first_turn_open': sum((self.players[i].open_turn == i + 1) for i in range(len(self.players))) / len(self.players),
            'avg_open_turn': avg_open_turn,
            'avg_rearrange_turns': avg_rearrange_turns,
            'avg_draw_count': avg_draw_count,
            'set_count': len(self.board),
            'run_count': runs,
            'group_count': groups,
            'avg_tiles_leftover': avg_tiles_leftover,
            'avg_score': avg_score,
        }
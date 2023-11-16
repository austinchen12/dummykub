import copy
import random
from models.tile import Tile
from constants import COLORS, DUPLICATE_TILES, HAND_SIZE, NUM_COLORS, NUMBERS_PER_COLOR, UNIQUE_TILE_COUNT

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

    def is_game_over(self):
        for player in self.players:
            if len(player.hand) == 0:
                return True
        
        return len(self.pool) < len(self.players)

    def clone_and_randomize(self, observer):
        st = copy.deepcopy(self)
        counts = [[0 for n in range(NUMBERS_PER_COLOR)] for k in range(NUM_COLORS)]
        for tile in st.players[observer].hand + [tile for set_ in st.board for tile in set_]:
            counts[COLORS.index(tile.color)][tile.number - 1] += 1

        unseen_tiles = []
        for color in range(NUM_COLORS):
            for number in range(NUMBERS_PER_COLOR):
                for _ in range(DUPLICATE_TILES - counts[color][number]):
                    unseen_tiles.append(Tile(number + 1, COLORS[color]))

        # Deal the unseen cards to the other players
        random.shuffle(unseen_tiles)
        for p in range(len(self.players)):
            if p != observer:
                # Deal cards to player p
                # Store the size of player p's hand
                num_cards = len(st.players[p].hand)
                # Give player p the first numCards unseen cards
                st.players[p].hand = unseen_tiles[:num_cards]
                # Remove those cards from unseenCards
                unseen_tiles = unseen_tiles[num_cards:]
        
        st.pool = unseen_tiles
        return st


    def start(self):
        random.shuffle(self.pool)
        self._deal_hands()

        while (not self.is_game_over()):
            # original_board = copy.deepcopy(self.board)
            self.turn_count += 1

            player = self.players[self.turn]
            print('START:', self.turn, self.board, player.hand)
            result = player.make_move(self)
            if result == 'DRAW':
                player.hand.append(self.pool.pop())
            else:
                if player.open_turn == -1:
                    player.open_turn = self.turn_count
                board, hand = result
                self.board = board
                player.hand = hand

            # self.history.append((self.turn_count, self.turn, [copy.deepcopy(player.hand) for player in self.players], original_board, self.board))
            self.turn = (self.turn + 1) % len(self.players)

            print('END  :', self.board, player.hand, '\n')
        print('GAME OVER', len(self.pool), [sum(tile.number for tile in player.hand) for player in self.players])
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

        # Compute winner, person with empty hand or person with the least score in hand
        if len(self.pool) < len(self.players):
            winners = []
            scores = [sum(tile.number for tile in player.hand) for player in self.players]
            min_score = min(scores)
            for i in range(len(scores)):
                if scores[i] == min_score:
                    winners.append(self.players[i])
        else:
            winners = [player for player in self.players if len(player.hand) == 0]

        return {
            'winner': '_'.join([str(winner.id) for winner in winners]),
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
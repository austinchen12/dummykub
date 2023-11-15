from abc import ABC, abstractmethod
import copy
from models.node import Node
from get_opening_moves import get_opening_moves
from solve import solve
import random

class Player(ABC):
    def __init__(self, id):
        self.id = id
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


class RandomPlayer(Player):
    def __init__(self, id):
        super().__init__(id)
    
    def _make_move(self, state):
        formatted_moves = []
        if self.opened:
            moves = solve(state.board, self.hand)
            for score, new_sets, unused_tiles in moves:
                if not new_sets:
                    continue

                formatted_moves.append((new_sets, unused_tiles))
        else:
            moves = get_opening_moves(self.hand)
            for move in moves:
                board = copy.deepcopy(state.board)
                hand = copy.deepcopy(self.hand)
                for set_ in move:
                    for tile in set_:
                        hand.remove(tile)
                hand.sort()
                
                board.extend(move)
                formatted_moves.append((board, hand))

        formatted_moves.append('DRAW')
        move = random.choice(formatted_moves)
        # print('COUNT2', len(formatted_moves))
        # for idk in formatted_moves:
        #     print("MOVES2", idk)
        return move
            

class GreedyPlayer(Player):
    def __init__(self, id):
        super().__init__(id)

    def _make_move(self, state):
        board = state.board
        if self.opened:
            moves = solve(board, self.hand)
            if not moves:
                self.draw_count += 1
                return 'DRAW'

            score, new_sets, unused_tiles = max(moves, key=lambda x: x[0])

            if not new_sets:
                self.draw_count += 1
                return 'DRAW'

            for set_ in board:
                if set_ not in new_sets:
                    self.rearrange_turns += 1
                    break

            return new_sets, unused_tiles
        else:
            moves = get_opening_moves(self.hand)
            if not moves:
                self.draw_count += 1
                return 'DRAW'

            move = max(moves, key=lambda x: sum([tile.number for set_ in x for tile in set_]))

            hand = copy.deepcopy(self.hand)
            for set_ in move:
                for tile in set_:
                    hand.remove(tile)
            hand.sort()
            
            board.extend(move)
            return board, hand


class ISMCTSPlayer(Player):
    def __init__(self, id, iterations):
        self.iterations = iterations
        self.count = 0
        super().__init__(id)

    def _get_moves(self, board):
        formatted_moves = []
        if self.opened:
            moves = solve(board, self.hand)
            for score, new_sets, unused_tiles in moves:
                if not new_sets:
                    continue

                formatted_moves.append((new_sets, unused_tiles))
        else:
            moves = get_opening_moves(self.hand)
            for move in moves:
                board_copy = copy.deepcopy(board)
                hand = copy.deepcopy(self.hand)
                for set_ in move:
                    for tile in set_:
                        hand.remove(tile)
                hand.sort()
                
                board_copy.extend(move)
                formatted_moves.append((board_copy, hand))

        formatted_moves.append('DRAW')
        return formatted_moves

    def _do_move(self, state, move):
        if move == 'DRAW':
            state.players[state.turn].hand.append(state.pool.pop())
        else:
            board, hand = move
            state.board = copy.deepcopy(board)
            state.players[state.turn].hand = copy.deepcopy(hand)
            if not state.players[state.turn].opened:
                state.players[state.turn].opened = True
        # print('ISMCTS', state.board, state.players[state.turn].hand)
        state.turn = (state.turn + 1) % len(state.players)
        # print('BEFOR2', state.board, state.players[state.turn].hand)

        # Do other players moves
        # TODO: does this break with 2 ISMCTS players?
        for _ in range(len(state.players) - 1):
            player = state.players[state.turn]
            result = player.make_move(state)
            if result == 'DRAW':
                player.hand.append(state.pool.pop())
            else:
                if player.open_turn == -1:
                    player.open_turn = state.turn_count
                board, hand = result
                state.board = board
                player.hand = hand
            # print('RANDOM', state.board, state.players[state.turn].hand)
            state.turn = (state.turn + 1) % len(state.players)
        # print()
                
    def _make_move(self, state):
        # if self.count == 1:
        #     return self._make_random_move(state)

        root = Node()
        root_player = state.players[state.turn]
        root_moves = root_player._get_moves(state.board)
        if len(root_moves) == 1:
            # print('SKIPPED')
            return root_moves[0]
        # print('START:', state.board, root_player.hand, len(root_moves), '\n')
        
        for _ in range(100):
            node = root

            st = state.clone_and_randomize(state.turn)
            player = st.players[st.turn]
            moves = copy.deepcopy(root_moves)

            # Select
            while not st.is_game_over() and node.get_untried_moves(moves) == []:
                node = node.ucb(moves)
                # print('SEELCT', node.move)
                player._do_move(st, node.move)
                moves = player._get_moves(st.board)
            
            # Expand
            untried_moves = [] if st.is_game_over() else node.get_untried_moves(player._get_moves(st.board))
            # print('board', st.board)
            # for move in untried_moves:
                # print('untried', move)
            if untried_moves != []:
                move = random.choice(untried_moves)
                player._do_move(st, move)
                node = node.add_child(move=move, turn=st.turn)
                # print('EXPAND', node.move)
            
            # print('START')
            # Simulate
            simulate_count = 0
            while not st.is_game_over():
                simulate_count += 1
                player = st.players[st.turn]
                if st.turn == player.id:
                    # print('BEFOR1', st.board, player.hand)
                    moves = player._get_moves(st.board)
                    # print('COUNT1', len(moves))
                    # for idk in moves:
                    #     print('MOVES1', idk)
                    move = random.choice(moves)
                    player._do_move(st, move)
                # print()

            # Backpropagate
            while node != None:
                # print('update', node.move)
                node.update(st)
                node = node.parent_node
            # print(root.children_to_string())
            # print('END', '\n', '\n')
        
        ismcts_move = max(root.child_nodes, key=lambda child: child.visits).move
        # print('MOVE :', len(root.child_nodes), 'DRAW' if ismcts_move == 'DRAW' else 'MOVE')
        if ismcts_move != 'DRAW':
            self.count += 1
        return ismcts_move

    def _make_random_move(self, state):
        formatted_moves = []
        if self.opened:
            moves = solve(state.board, self.hand)
            for score, new_sets, unused_tiles in moves:
                if not new_sets:
                    continue

                formatted_moves.append((new_sets, unused_tiles))
        else:
            moves = get_opening_moves(self.hand)
            for move in moves:
                board = copy.deepcopy(state.board)
                hand = copy.deepcopy(self.hand)
                for set_ in move:
                    for tile in set_:
                        hand.remove(tile)
                hand.sort()
                
                board.extend(move)
                formatted_moves.append((board, hand))

        formatted_moves.append('DRAW')
        move = random.choice(formatted_moves)
        # print('COUNT2', len(formatted_moves))
        # for idk in formatted_moves:
        #     print("MOVES2", idk)
        return move
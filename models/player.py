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
        return random.choice(formatted_moves)
            

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
        super().__init__(id)

    def _get_moves(self, board):
        formatted_moves = []
        if self.opened:
            moves = solve(board, self.hand)
            if moves:
                for score, new_sets, unused_tiles in moves:
                    if not new_sets:
                        continue

                    for set_ in board:
                        if set_ not in new_sets:
                            self.rearrange_turns += 1
                            break

                formatted_moves.append((new_sets, unused_tiles))
        else:
            moves = get_opening_moves(self.hand)
            if moves:
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
    
    def _make_move(self, state):
        root = Node(turn=state.turn)

        for _ in range(self.iterations):
            node = root

            st = state.clone_and_randomize(state.turn)
            rootplayer = st.players[st.turn]

            # Select
            while not st.is_game_over() and (moves := rootplayer._get_moves(st.board)) != [] and node.get_untried_moves(moves) == []:
                node = node.ucb(moves)
                rootplayer._do_move(st, node.move)
            
            # Expand
            untried_moves = [] if st.is_game_over() else node.get_untried_moves(rootplayer._get_moves(st.board))
            if untried_moves != []:
                move = random.choice(untried_moves)
                rootplayer._do_move(st, move)
                node = node.add_child(move=move, turn=st.turn)
            

            # Simulate
            simulate_count = 0
            while not st.is_game_over():
                simulate_count += 1
                player = st.players[st.turn]
                if st.turn == rootplayer.id:
                    moves = player._get_moves(st.board)
                    move = random.choice(moves)
                    player._do_move(st, move)
                else:
                    move = player.make_move(st)
                    if move == 'DRAW':
                        player.hand.append(st.pool.pop())
                    else:
                        if player.open_turn == -1:
                            player.open_turn = st.turn_count
                        board, hand = move
                        st.board = board
                        player.hand = hand
                
                st.turn = (st.turn + 1) % len(st.players)

            # Backpropagate
            while node != None:
                node.update(st)
                node = node.parent_node
        
        ismcts_move = max(root.child_nodes, key=lambda child: child.visits).move
        print('MOVE :', len(root.child_nodes), 'DRAW' if ismcts_move == 'DRAW' else 'MOVE')
        return ismcts_move

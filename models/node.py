from math import sqrt, log


class Node:
    def __init__(self, move=None, parent=None, turn=None):
        self.move = move
        self.parent_node = parent 
        self.child_nodes = []
        self.score = 0
        self.visits = 0
        self.avails = 1
        self.turn = turn

    def get_untried_moves(self, legal_moves):
        # print('legal_moves', legal_moves)
        tried_moves = [child.move for child in self.child_nodes]
        # print('tried_moves', tried_moves)
        untried_moves = [move for move in legal_moves if move not in tried_moves]
        # print('untried_moves', untried_moves)

        return untried_moves

    def ucb(self, legal_moves, exploration=2):
        legal_children = [child for child in self.child_nodes if child.move in legal_moves]
        # print('children', legal_children)
        # print('ucb1', [float(child.score) / float(child.visits) for child in legal_children])
        # print('ucb2', [exploration * sqrt(log(child.avails) / float(child.visits)) for child in legal_children])
        # print('ucb3', [float(child.score) / float(child.visits) + exploration * sqrt(log(child.avails) / float(child.visits)) for child in legal_children])
        selected_child = max(
            legal_children,
            key=lambda child: float(child.score) / float(child.visits)
            + exploration * sqrt(log(child.avails) / float(child.visits)),
        )

        for child in legal_children:
            child.avails += 1

        return selected_child

    def add_child(self, move, turn):
        node = Node(move=move, parent=self, turn=turn)
        self.child_nodes.append(node)
        return node

    def update(self, end_state):
        self.visits += 1
        if self.turn is not None:
            hand_sums = [sum(tile.number for tile in player.hand) for player in end_state.players]
            
            min_hand_sum = min(hand_sums)
            computed_scores, losers_score = [], 0
            for hand_sum in hand_sums:
                if hand_sum == min_hand_sum:
                    computed_scores.append(None)
                else:
                    score = -1 * (hand_sum - min_hand_sum)
                    losers_score += score
                    computed_scores.append(score)

            for i in range(len(computed_scores)):
                if computed_scores[i] == None:
                    computed_scores[i] = -1 * losers_score
            # want to scale score so absolute max value is equal to 1 but must also consider negative numbers
            abs_max = max(abs(score) for score in computed_scores)

            self.score += computed_scores[self.turn] if abs_max == 0 else (computed_scores[self.turn] / abs_max)

    def tree_to_string(self, indent):
        s = "\n"
        for _ in range(1, indent + 1):
            s += "| "
        s += str(self)
        for c in self.child_nodes:
            s += c.TreeToString(indent + 1)
        return s

    def children_to_string(self):
        s = ""
        for c in self.child_nodes:
            s += str(c) + "\n"
        return s

    def __repr__(self):
        return "[S/V/A: %4i/%4i/%4i\tM:%s ]" % (
            self.score,
            self.visits,
            self.avails,
            self.move,
        )
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

    def ucb(self, legal_moves, exploration=0.7):
        legal_children = [child for child in self.child_nodes if child.move in legal_moves]

        selected_child = max(
            legal_children,
            key=lambda child: float(child.score) / float(child.visits)
            + exploration * sqrt(log(child.avails) / float(child.visits)),
        )

        for selected_child in legal_children:
            selected_child.avails += 1

        return selected_child

    def add_child(self, move, turn):
        node = Node(move=move, parent=self, turn=turn)
        self.child_nodes.append(node)
        return node

    def update(self, end_state):
        self.visits += 1
        if self.turn is not None:
            if len(end_state.players[self.turn].hand):
                self.score += sum(tile.number for player in end_state.players for tile in player.hand if player.id != self.turn)
            else:
                self.score -= sum(tile.number for tile in end_state.players[self.turn].hand)

    def TreeToString(self, indent):
        s = self.IndentString(indent) + str(self)
        for c in self.child_nodes:
            s += c.TreeToString(indent + 1)
        return s

    def IndentString(self, indent):
        s = "\n"
        for i in range(1, indent + 1):
            s += "| "
        return s

    def ChildrenToString(self):
        s = ""
        for c in self.child_nodes:
            s += str(c) + "\n"
        return s

    def __repr__(self):
        return "[M:%s W/V/A: %4i/%4i/%4i]" % (
            self.move,
            self.score,
            self.visits,
            self.avails,
        )
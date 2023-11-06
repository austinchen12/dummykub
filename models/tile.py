from constants import COLORS, NUM_COLORS, NUMBERS_PER_COLOR, UNIQUE_TILE_COUNT


class Tile:
    def __init__(self, *args):
        if len(args) == 1:
            id = args[0]

            for i in range(NUM_COLORS):
                if (i * UNIQUE_TILE_COUNT / NUM_COLORS) <= id and (id < UNIQUE_TILE_COUNT * (i + 1) / NUM_COLORS):
                    self.color = COLORS[i]
                    break

            self.number = id % NUMBERS_PER_COLOR + 1
        elif len(args) == 2:
            self.number = args[0]
            self.color = args[1].upper()

    def __repr__(self):
        return str(self.number) + self.color

    def __eq__(self, other):
        return str(self.number) + self.color == str(other.number) + other.color

    def __hash__(self):
        return hash(str(self.number) + self.color)
    
    def __gt__(self, other):
        if self.number == other.number:
            return self.color > other.color
        return self.number > other.number

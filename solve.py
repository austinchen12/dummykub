from constants import COLORS, NUM_COLORS, NUMBERS_PER_COLOR

from utils import tiles_from_string
import subprocess

def solve(board, hand):
    board_tiles = [tile for set_ in board for tile in set_] if board else []
    command = ['./rummikub.out'] + [str(tile.number) + { 'BLUE': 'b', 'BLACK': 'g', 'RED': 'r', 'YELLOW': 'y' }[tile.color] for tile in board_tiles] + ['-'] + [str(tile.number) + { 'BLUE': 'b', 'BLACK': 'g', 'RED': 'r', 'YELLOW': 'y' }[tile.color] for tile in hand]

    process = subprocess.run(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE, stdin=subprocess.PIPE, text=True)

    if process.returncode == 0:
        result = process.stdout.split('\n\n')
        score = int(result[0])
        print('result', result)
        if score == 0:
            sets, unused_tiles = [], hand
        else:
            sets = [tiles_from_string(line) for line in result[1].split('\n')]
            unused_tiles = tiles_from_string(result[2])
    else:
        print('ERROR:', process.stderr)
        return None, None
    
    # At least one additional tile was played
    if sum(len(set_) for set_ in sets) <= len(board_tiles):
        return None, None
    
    # Assert no tiles were removed from the board
    board_count = [[0 for _ in range(NUMBERS_PER_COLOR + 1)] for _ in range(NUM_COLORS)]
    for tile in board_tiles:
        board_count[COLORS.index(tile.color)][tile.number] += 1
    for set_ in sets:
        for tile in set_:
            board_count[COLORS.index(tile.color)][tile.number] -= 1
    for c in COLORS:
        for value in range(1, NUMBERS_PER_COLOR + 1):
            if board_count[COLORS.index(c)][value] > 0:
                print('ERROR: tile removed from board')
                print('board', board)
                print('hand', hand)
                print('sets', sets)
                print('unused', unused_tiles)
                print()
            
    old_board_count = [[0 for _ in range(NUMBERS_PER_COLOR + 1)] for _ in range(NUM_COLORS)]
    for tile in board_tiles:
        old_board_count[COLORS.index(tile.color)][tile.number] += 1

    new_board_count = [[0 for _ in range(NUMBERS_PER_COLOR + 1)] for _ in range(NUM_COLORS)]
    for set_ in sets:
        for tile in set_:
            new_board_count[COLORS.index(tile.color)][tile.number] += 1

    # Number of tiles in the new sets can't be less than the number of tiles in the old board
    for c in COLORS:
        for value in range(1, NUMBERS_PER_COLOR + 1):
            if new_board_count[COLORS.index(c)][value] < old_board_count[COLORS.index(c)][value]:
                return None, None

    return sets, unused_tiles

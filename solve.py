from constants import COLORS, NUM_COLORS, NUMBERS_PER_COLOR

from utils import tiles_from_string
import subprocess

def handle_result(chunk, board_tiles, board, hand):
    result = chunk.split("\n\n")
    try:
        score = int(result[0])
    except Exception as e:
        print('ERROR: invalid score')
        print('board', board)
        print('hand', hand)
        raise e
    
    if score == 0:
        return None

    sets = [tiles_from_string(line) for line in result[1].split('\n')]
    unused_tiles = tiles_from_string(result[2])

    # At least one additional tile was played
    if sum(len(set_) for set_ in sets) <= len(board_tiles):
        return None
    
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
                return None

    # sort sets and unused_tiles
    sets = sorted(sets, key=lambda x: (sum([tile.number for tile in x]), x[0]))
    unused_tiles = sorted(unused_tiles)

    return (score, sets, unused_tiles)

def solve(board, hand):
    board_tiles = [tile for set_ in board for tile in set_] if board else []
    command = ['./rummikub.out'] + [str(tile.number) + { 'BLUE': 'b', 'BLACK': 'g', 'RED': 'r', 'YELLOW': 'y' }[tile.color] for tile in board_tiles] + ['-'] + [str(tile.number) + { 'BLUE': 'b', 'BLACK': 'g', 'RED': 'r', 'YELLOW': 'y' }[tile.color] for tile in hand]

    process = subprocess.run(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE, stdin=subprocess.PIPE, text=True)

    if process.returncode == 0:
        moves = {}
        results = process.stdout.strip().split('\n\n\n')
        for chunk in results:
            move = handle_result(chunk, board_tiles, board, hand)
            if move:
                score, sets, unused_tiles = move
                if score in moves:
                    assert(moves[score][0] == score)
                    assert(moves[score][1] == sets)
                    assert(moves[score][2] == unused_tiles)
                moves[tuple(sorted(unused_tiles))] = (score, sets, unused_tiles)
        return moves.values()
    else:
        print('ERROR:', process.stderr)
        return []
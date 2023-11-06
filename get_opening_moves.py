from itertools import combinations
from constants import COLORS, MAX_SET_SIZE, MIN_SET_SIZE, NUMBERS_PER_COLOR, TARGET
from utils import enough_duplicate_tiles, remove_duplicates_by_hash


def get_sorted_hand_dicts(tiles):
    tiles_sorted_by_number = {number: []
                              for number in range(1, NUMBERS_PER_COLOR + 1)}
    tiles_sorted_by_color = {color: [] for color in COLORS}
    for tile in tiles:
        tiles_sorted_by_number[tile.number].append(tile)
        tiles_sorted_by_color[tile.color].append(tile)

    return tiles_sorted_by_number, tiles_sorted_by_color


def get_largest_sets(tiles):
    # Returns greedily calculated groups and runs
    tiles = sorted(remove_duplicates_by_hash(
        tiles), key=lambda x: (x.number, x.color))
    groups, runs = [], []

    tiles_sorted_by_number, tiles_sorted_by_color = get_sorted_hand_dicts(
        tiles)

    for number in tiles_sorted_by_number:
        arr = tiles_sorted_by_number[number]
        if MIN_SET_SIZE <= len(arr) and len(arr) <= MAX_SET_SIZE:
            groups.append(arr)

    for color in tiles_sorted_by_color:
        arr = sorted(tiles_sorted_by_color[color], key=lambda x: x.number)
        if len(arr) == 0:
            continue
        current_sequence = [arr[0]]
        if MIN_SET_SIZE <= len(arr) and len(arr) <= MAX_SET_SIZE:
            for i in range(1, len(arr)):
                if arr[i].number == arr[i - 1].number + 1:
                    current_sequence.append(arr[i])
                else:
                    if len(current_sequence) >= 3:
                        runs.append(current_sequence)
                    current_sequence = [arr[i]]

            # Append the last sequence if it's at least length 3
            if len(current_sequence) >= 3:
                runs.append(current_sequence)

    return groups, runs


def get_opening_moves(hand, target=TARGET):
    groups, runs = get_largest_sets(hand)

    # Since a valid move can use less than the greedily fetched amount, we need to check all possible combinations of MIN_SET_SIZE
    group_permutations = []
    for group in groups:
        group = remove_duplicates_by_hash(group)
        for r in range(MIN_SET_SIZE, len(group) + 1):
            group_permutations.extend(combinations(group, r))

    run_permutations = []
    for run in runs:
        run = remove_duplicates_by_hash(run)
        for r in range(MIN_SET_SIZE, len(run) + 1):
            run_permutations.extend([tuple(run[i:i+r])
                                     for i in range(len(run) - r + 1)])

    possible_moves = []

    joint = group_permutations + run_permutations
    tile_count = {tile: hand.count(tile) for tile in hand}
    for r in range(1, len(joint) + 1):
        for combo in combinations(joint, r):
            possible_set = []
            for item in combo:
                possible_set.extend(item)

            if sum([tile.number for tile in possible_set]) >= target:
                # Check no duplicates or if there are enough duplicate tiles
                if not len(set(possible_set)) == len(possible_set) and not enough_duplicate_tiles(tile_count, possible_set):
                    continue

                possible_moves.append(combo)

    return possible_moves

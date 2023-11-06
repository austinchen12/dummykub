import re

from models.tile import Tile


def remove_duplicates_by_hash(items):
    seen, result = set(), []
    hashed_items = [hash(tuple(item) if isinstance(
        item, list) else item) for item in items]

    for item, hashed_item in zip(items, hashed_items):
        if hashed_item not in seen:
            seen.add(hashed_item)
            result.append(item)

    return result


def tiles_from_string(output):
    # Output is in the form of [1BLACK, 1YELLOW, 3BLUE, 4BLUE, 5BLACK, 6YELLOW, 7BLACK, 7BLUE, 7RED, 9RED, 10BLACK, 10BLACK, 10BLUE, 13BLUE]
    if output == '[]':
        return []

    output = output[1: -1]
    tiles = output.split(', ')

    result = []
    regex = re.compile(r'(\d+)(\w+)')
    seen = set()
    for tile in tiles:
        if tile in seen:
            continue
        else:
            number, color = regex.match(tile).groups()
            result.append(Tile(int(number), color))

    return result


def enough_duplicate_tiles(tile_count, possible_set):
    possible_set_count = {tile: possible_set.count(
        tile) for tile in possible_set}

    for tile in possible_set_count:
        if tile_count[tile] < possible_set_count[tile]:
            return False

    return True

def subtract_hands(a, b):
    a_count = {tile: a.count(tile) for tile in a}
    b_count = {tile: b.count(tile) for tile in b}

    result = []
    for tile in a_count:
        if tile not in b_count:
            result.append(tile)
        elif a_count[tile] > b_count[tile]:
            result.extend([tile] * (a_count[tile] - b_count[tile]))
            
    return result

def get_score(tiles):
    return sum([tile.number for tile in tiles])
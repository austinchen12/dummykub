import unittest
from constants import COLORS, DUPLICATE_TILES, NUMBERS_PER_COLOR, UNIQUE_TILE_COUNT
from get_opening_moves import get_opening_moves
from models.tile import Tile

from utils import enough_duplicate_tiles, tiles_from_string, remove_duplicates_by_hash


class TileTesting(unittest.TestCase):
    def test_constructor(self):
        tiles = []
        for _ in range(DUPLICATE_TILES):
            for i in range(UNIQUE_TILE_COUNT):
                tile = Tile(i)
                tiles.append(tile)

        # Check creation by number and color is correct
        i = 0
        for _ in range(DUPLICATE_TILES):
            for color in COLORS:
                for number in range(1, NUMBERS_PER_COLOR + 1):
                    tile = Tile(number, color)
                    fetched_tile = tiles[i]
                    self.assertEqual(tile.number, fetched_tile.number)
                    self.assertEqual(tile.color, fetched_tile.color)
                    i += 1


class UtilsTesting(unittest.TestCase):
    def test_hand_from_console_output(self):
        output = '[1BLACK, 1BLACK, 3BLUE, 4BLUE, 5BLACK, 6YELLOW, 7BLACK, 7BLUE, 7RED, 9RED, 10BLACK, 10YELLOW, 10BLUE, 13BLUE]'
        hand = tiles_from_string(output)

        self.assertEqual(output, str(hand))

    def test_remove_duplicates_by_hash_tiles(self):
        output = '[1BLACK, 1BLACK, 3BLUE, 4BLUE, 5BLACK, 6YELLOW, 7BLACK, 7BLUE, 7RED, 9RED, 10BLACK, 10YELLOW, 10BLUE, 13BLUE]'
        hand = remove_duplicates_by_hash(tiles_from_string(output))

        self.assertEqual(
            '[1BLACK, 3BLUE, 4BLUE, 5BLACK, 6YELLOW, 7BLACK, 7BLUE, 7RED, 9RED, 10BLACK, 10YELLOW, 10BLUE, 13BLUE]', str(hand))

    def test_remove_duplicates_by_hash_list_of_tiles(self):
        outputs = [
            '[1BLACK, 1BLACK, 3BLUE, 4BLUE, 5BLACK, 6YELLOW, 7BLACK, 7BLUE, 7RED, 9RED, 10BLACK, 10YELLOW, 10BLUE, 13BLUE]',
            '[1BLACK, 1BLACK, 3BLUE, 4BLUE, 5BLACK, 6YELLOW, 7BLACK, 7BLUE, 7RED, 9RED, 10BLACK, 10YELLOW, 10BLUE, 13BLUE]',
            '[1BLACK, 2BLACK, 3BLUE, 4BLUE, 5BLACK, 6YELLOW, 7BLACK, 7BLUE, 7RED, 9RED, 10BLACK, 10YELLOW, 10BLUE, 13BLUE]',
        ]
        hands = remove_duplicates_by_hash(
            [tiles_from_string(output) for output in outputs])

        self.assertEqual(len(hands), 2)
        self.assertEqual(str(hands[0]), outputs[0])
        self.assertEqual(str(hands[1]), outputs[2])

    def test_enough_duplicate_tiles(self):
        output = '[1BLACK, 1BLACK, 1BLUE, 4BLUE, 5BLACK, 6YELLOW, 7BLACK, 7BLUE, 7RED, 9RED, 10BLACK, 10YELLOW, 10BLUE, 13BLUE]'
        hand = tiles_from_string(output)

        tile_count = {tile: hand.count(tile) for tile in hand}

        possible_set_1 = [Tile(1, 'BLACK'), Tile(1, 'BLACK'), Tile(1, 'BLUE')]
        self.assertTrue(enough_duplicate_tiles(tile_count, possible_set_1))

        possible_set_2 = [Tile(1, 'BLACK'), Tile(1, 'BLACK'), Tile(1, 'BLACK')]
        self.assertFalse(enough_duplicate_tiles(tile_count, possible_set_2))


class GetOpeningMovesTesting(unittest.TestCase):
    def test_single_group(self):
        hand = tiles_from_string(
            '[2BLUE, 3BLUE, 3RED, 4YELLOW, 4BLACK, 5BLUE, 5RED, 6BLUE, 7RED, 7YELLOW, 9BLACK, 13BLACK, 13BLUE, 13YELLOW]')
        result = get_opening_moves(hand)
        expected = [(Tile(13, 'BLACK'), Tile(13, 'BLUE'), Tile(13, 'YELLOW'))]

        self.assertEqual(len(result), 1)
        self.assertEqual(result[0], expected)

    def test_multiple_groups(self):
        hand = tiles_from_string(
            '[2BLUE, 3BLUE, 3RED, 5YELLOW, 6BLACK, 6BLUE, 6RED, 7BLUE, 7RED, 7YELLOW, 9BLACK, 11YELLOW, 13BLUE, 13YELLOW]')
        result = get_opening_moves(hand)
        expected = [(Tile(6, 'BLACK'), Tile(6, 'BLUE'), Tile(6, 'RED')),
                    (Tile(7, 'BLUE'), Tile(7, 'RED'), Tile(7, 'YELLOW'))]

        self.assertEqual(len(result), 1)
        self.assertEqual(result[0], expected)

    def test_single_set(self):
        hand = tiles_from_string(
            '[2BLUE, 3BLUE, 3RED, 4YELLOW, 4BLACK, 5BLUE, 5RED, 6BLUE, 7RED, 7YELLOW, 10BLUE, 11BLACK, 12BLACK, 13BLACK]')
        result = get_opening_moves(hand)
        expected = [(Tile(11, 'BLACK'), Tile(12, 'BLACK'), Tile(13, 'BLACK'))]

        self.assertEqual(len(result), 1)
        self.assertEqual(result[0], expected)

    def test_multiple_sets(self):
        hand = tiles_from_string(
            '[1BLUE, 2BLUE, 3BLUE, 5BLUE, 6BLUE, 7BLUE, 7RED, 8BLUE, 9BLACK, 10RED, 11YELLOW, 13BLUE, 13YELLOW]')
        result = get_opening_moves(hand)
        expected = [(Tile(1, 'BLUE'), Tile(2, 'BLUE'), Tile(3, 'BLUE')), (Tile(
            5, 'BLUE'), Tile(6, 'BLUE'), Tile(7, 'BLUE'), Tile(8, 'BLUE'))]

        self.assertEqual(len(result), 1)
        self.assertEqual(result[0], expected)

    def test_mixed(self):
        hand = tiles_from_string(
            '[1BLUE, 3BLUE, 3YELLOW, 4BLUE, 5BLUE, 6BLACK, 6RED, 7BLUE, 7RED, 7YELLOW, 9BLACK, 11YELLOW, 13BLUE, 13YELLOW]')
        result = get_opening_moves(hand)
        expected = [(Tile(7, 'BLUE'), Tile(7, 'RED'), Tile(7, 'YELLOW')),
                    (Tile(3, 'BLUE'), Tile(4, 'BLUE'), Tile(5, 'BLUE'))]

        self.assertEqual(len(result), 1)
        self.assertEqual(result[0], expected)

    def test_valid_duplicates(self):
        hand = tiles_from_string(
            '[1BLUE, 2YELLOW, 3BLUE, 3BLUE, 4RED, 6YELLOW, 7YELLOW, 6RED, 8BLUE, 8RED, 8YELLOW, 8YELLOW, 9RED, 13YELLOW]')
        result = get_opening_moves(hand)
        expected = [(Tile(8, 'BLUE'), Tile(8, 'RED'), Tile(8, 'YELLOW')),
                    (Tile(6, 'YELLOW'), Tile(7, 'YELLOW'), Tile(8, 'YELLOW'))]

        self.assertEqual(len(result), 1)
        self.assertEqual(result[0], expected)

    def test_complex_1(self):
        hand = tiles_from_string(
            '[1BLUE, 3BLUE, 3RED, 3YELLOW, 5BLACK, 5BLUE, 5RED, 7BLUE, 7RED, 7YELLOW, 9BLACK, 11YELLOW, 13BLUE, 13YELLOW]')
        result = get_opening_moves(hand)
        expected = [[(Tile(3, 'BLUE'), Tile(3, 'RED'), Tile(3, 'YELLOW')), (Tile(7, 'BLUE'), Tile(7, 'RED'), Tile(7, 'YELLOW'))],
                    [(Tile(5, 'BLACK'), Tile(5, 'BLUE'), Tile(5, 'RED')),
                     (Tile(7, 'BLUE'), Tile(7, 'RED'), Tile(7, 'YELLOW'))],
                    [(Tile(3, 'BLUE'), Tile(3, 'RED'), Tile(3, 'YELLOW')), (Tile(5, 'BLACK'), Tile(5, 'BLUE'), Tile(5, 'RED')), (Tile(7, 'BLUE'), Tile(7, 'RED'), Tile(7, 'YELLOW'))]]

        self.assertEqual(len(result), len(expected))
        for i in range(len(result)):
            self.assertEqual(result[i], expected[i])

    def test_complex_2(self):
        hand = tiles_from_string(
            '[2YELLOW, 3BLUE, 3BLUE, 4RED, 6YELLOW, 7YELLOW, 6RED, 8BLUE, 8RED, 8YELLOW, 8YELLOW, 10BLUE, 10RED, 10YELLOW]')
        result = get_opening_moves(hand)
        expected = [
            [(Tile(10, 'BLUE'), Tile(10, 'RED'), Tile(10, 'YELLOW'))],
            [(Tile(8, 'BLUE'), Tile(8, 'RED'), Tile(8, 'YELLOW')),
             (Tile(10, 'BLUE'), Tile(10, 'RED'), Tile(10, 'YELLOW'))],
            [(Tile(8, 'BLUE'), Tile(8, 'RED'), Tile(8, 'YELLOW')),
             (Tile(6, 'YELLOW'), Tile(7, 'YELLOW'), Tile(8, 'YELLOW'))],
            [(Tile(10, 'BLUE'), Tile(10, 'RED'), Tile(10, 'YELLOW')),
             (Tile(6, 'YELLOW'), Tile(7, 'YELLOW'), Tile(8, 'YELLOW'))],
            [(Tile(8, 'BLUE'), Tile(8, 'RED'), Tile(8, 'YELLOW')), (Tile(10, 'BLUE'), Tile(
                10, 'RED'), Tile(10, 'YELLOW')), (Tile(6, 'YELLOW'), Tile(7, 'YELLOW'), Tile(8, 'YELLOW'))]
        ]

        self.assertEqual(len(result), len(expected))
        for i in range(len(result)):
            self.assertEqual(result[i], expected[i])


class GetMovesTesting(unittest.TestCase):
    def test_get_moves(self):
        hand = tiles_from_string(
            '[1BLUE, 2BLUE, 3BLUE, 5BLUE, 6BLUE, 7BLUE, 7RED, 8BLUE, 9BLACK, 10RED, 11YELLOW, 13BLUE, 13YELLOW]')
        board = [(Tile(10, 'BLACK'), Tile(10, 'BLUE'), Tile(10, 'YELLOW'))]


if __name__ == '__main__':
    unittest.main()

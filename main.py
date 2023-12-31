

import argparse
import sys

from models.player import ISMCTSPlayer, GreedyPlayer, RandomPlayer
from rummikub import Rummikub
import multiprocessing


def main(args):
    iterations, player_types, stats, exploration = args
    for _ in range(iterations):
        players = []
        for i, player_type in enumerate(player_types):
            if player_type == "greedy":
                players.append(GreedyPlayer(i))
            elif player_type == "random":
                players.append(RandomPlayer(i))
            elif player_type == "ismcts-100":
                players.append(ISMCTSPlayer(i, 100, exploration))
            elif player_type == "ismcts-1000":
                players.append(ISMCTSPlayer(i, 1000, exploration))
            else:
                raise ValueError("Unknown player type: {}".format(player_type))

        rummikub = Rummikub(players)
        new_stats = rummikub.start()
        stats.append(new_stats)
   


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--players", type=str, nargs="*",
                        help="Player types, e.g. random, greedy, ismcts-100, ismcts-1000", default=["greedy", "greedy"])
    parser.add_argument("--multiprocessing", "-m", type=int, default=0)
    parser.add_argument("--exploration", "-e", type=float, default=2.0)
    parser.add_argument("--iterations", "-i", type=int, default=1)
    args = parser.parse_args()

    with multiprocessing.Manager() as manager:
        individual_stats = manager.list()
        stats = manager.dict({
            'total_tiles_played': 0,
            'turn_count': 0,
            'first_turn_open': 0,
            'avg_open_turn': 0,
            'avg_rearrange_turns': 0,
            'avg_draw_count': 0,
            'set_count': 0,
            'run_count': 0,
            'group_count': 0,
            'avg_tiles_leftover': 0,
            'avg_score': 0,
        })
        for i in range(len(args.players)):
            stats[i] = 0

        if args.multiprocessing != 0:
            cores = args.multiprocessing
            with multiprocessing.Pool(processes=cores) as p:
                chunks = args.iterations // cores
                main_args = [(chunks, args.players, individual_stats, args.exploration)] * cores
                p.map(main, main_args)
            print('MULTI')
        else:
            main((args.iterations, args.players, individual_stats, args.exploration))
            print('NOTMULTI')

        for stat in individual_stats:
            for key in stat.keys():
                if key == 'winner':
                    for winner in stat[key].split('_'):
                        stats[int(winner)] += 1
                else:
                    stats[key] += stat[key]
    
        for key in stats.keys():
            if type(key) == int:
                percentage = "{:.2f}".format(stats[key] / args.iterations * 100)
                print(f"{key}: {stats[key]} ({percentage})%")
                continue

            stats[key] /= args.iterations
            if key == 'avg_rearrange_turns' or key == 'avg_draw_count':
                percentage = "{:.2f}".format(stats[key] / stats['turn_count'] * 100)
                print(f"{key}: {stats[key]} ({percentage}%)")
            elif key == 'run_count' or key == 'group_count':
                percentage = "{:.2f}".format(stats[key] / stats['set_count'] * 100)
                print(f"{key}: {stats[key]} ({percentage}%)")
            elif key == 'first_turn_open':
                percentage = "{:.2f}".format(stats[key] * 100)
                print(f"{key}: {percentage}%")
            else:
                print(f"{key}: {stats[key]}")
        

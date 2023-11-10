// @author Frank Takes - Leiden University - ftakes@liacs.nl
// Rummikub, tiles=N, colors=K, duplicates=M, minrunlength=3, mingroupsize=K-1

// WARNING: you may need to set ulimit -s 64000 to not get segfaults

#include <cmath>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <array>
#include <tuple>
using namespace std;

template <int const base, unsigned int const exponent>
struct Power
{
	enum
	{
		value = base * Power<base, exponent - 1>::value
	};
};
template <int const base>
struct Power<base, 0>
{
	enum
	{
		value = 1
	};
};
struct Tile
{
	int number;
	std::string color;
	Tile(int num, const std::string &col) : number(num), color(col) {}
};

const int N = 7;
const int K = 3;
const int M = 2;
const int MIN_SET_SIZE = 3;

const array<std::string, K> COLORS = {"BLUE", "RED", "YELLOW"};

int minn, maxn;
int original_board[K][N + 1];
int hand[K][N + 1];
map<std::string, int> score_dp;
int next_array[N + 1][Power<MIN_SET_SIZE + 1, K * M>::value];
int used_dp[N + 1][Power<MIN_SET_SIZE + 1, K * M>::value][K][N + 1]; // current value, current inlengths, target color, target number

int mp(const int *inlengths)
{ // map runlengths array to dp array index
	int sum = 0;
	for (int i = 0; i < K * M; i++)
		sum += inlengths[i] * pow(K + 1, i);

	return sum;
} // mp

int go(int, int *, std::string); // header for go-function

void rmp(int in, int *result)
{ // reverse of mp
	for (int i = 0; i < K * M; i++)
	{
		result[i] = in % (K + 1);
		in /= (K + 1);
	}
} // rmp

map<pair<tuple<int, int>, tuple<int, int>>, int> rest = {
	{make_pair(make_tuple(0, 1), make_tuple(0, 2)), 2},
	{make_pair(make_tuple(0, 1), make_tuple(1, 2)), 3},
	{make_pair(make_tuple(0, 2), make_tuple(0, 3)), 2},
	{make_pair(make_tuple(0, 2), make_tuple(1, 3)), 3},
	{make_pair(make_tuple(0, 3), make_tuple(0, 3)), 2},
	{make_pair(make_tuple(0, 3), make_tuple(1, 3)), 3},
	{make_pair(make_tuple(1, 0), make_tuple(2, 0)), 1},
	{make_pair(make_tuple(1, 0), make_tuple(2, 1)), 3},
	{make_pair(make_tuple(2, 0), make_tuple(3, 0)), 1},
	{make_pair(make_tuple(2, 0), make_tuple(3, 1)), 3},
	{make_pair(make_tuple(3, 0), make_tuple(3, 0)), 1},
	{make_pair(make_tuple(3, 0), make_tuple(3, 1)), 3},
	{make_pair(make_tuple(1, 1), make_tuple(2, 0)), 1},
	{make_pair(make_tuple(1, 1), make_tuple(0, 2)), 2},
	{make_pair(make_tuple(1, 1), make_tuple(2, 2)), 3},
	{make_pair(make_tuple(1, 1), make_tuple(2, 1)), 3},
	{make_pair(make_tuple(1, 1), make_tuple(1, 2)), 3},
	{make_pair(make_tuple(1, 2), make_tuple(2, 0)), 1},
	{make_pair(make_tuple(1, 2), make_tuple(0, 3)), 2},
	{make_pair(make_tuple(1, 2), make_tuple(2, 3)), 3},
	{make_pair(make_tuple(1, 2), make_tuple(2, 1)), 3},
	{make_pair(make_tuple(1, 2), make_tuple(1, 3)), 3},
	{make_pair(make_tuple(1, 3), make_tuple(2, 0)), 1},
	{make_pair(make_tuple(1, 3), make_tuple(0, 3)), 2},
	{make_pair(make_tuple(1, 3), make_tuple(2, 3)), 3},
	{make_pair(make_tuple(1, 3), make_tuple(2, 1)), 3},
	{make_pair(make_tuple(1, 3), make_tuple(1, 3)), 3},
	{make_pair(make_tuple(2, 1), make_tuple(3, 0)), 1},
	{make_pair(make_tuple(2, 1), make_tuple(0, 2)), 2},
	{make_pair(make_tuple(2, 1), make_tuple(3, 2)), 3},
	{make_pair(make_tuple(2, 1), make_tuple(3, 1)), 3},
	{make_pair(make_tuple(2, 1), make_tuple(1, 2)), 3},
	{make_pair(make_tuple(2, 2), make_tuple(3, 0)), 1},
	{make_pair(make_tuple(2, 2), make_tuple(0, 3)), 2},
	{make_pair(make_tuple(2, 2), make_tuple(3, 3)), 3},
	{make_pair(make_tuple(2, 2), make_tuple(3, 1)), 3},
	{make_pair(make_tuple(2, 2), make_tuple(1, 3)), 3},
	{make_pair(make_tuple(2, 3), make_tuple(3, 0)), 1},
	{make_pair(make_tuple(2, 3), make_tuple(0, 3)), 2},
	{make_pair(make_tuple(2, 3), make_tuple(3, 3)), 3},
	{make_pair(make_tuple(2, 3), make_tuple(3, 1)), 3},
	{make_pair(make_tuple(2, 3), make_tuple(1, 3)), 3},
	{make_pair(make_tuple(3, 1), make_tuple(3, 0)), 1},
	{make_pair(make_tuple(3, 1), make_tuple(0, 2)), 2},
	{make_pair(make_tuple(3, 1), make_tuple(3, 2)), 3},
	{make_pair(make_tuple(3, 1), make_tuple(3, 1)), 3},
	{make_pair(make_tuple(3, 1), make_tuple(1, 2)), 3},
	{make_pair(make_tuple(3, 2), make_tuple(3, 0)), 1},
	{make_pair(make_tuple(3, 2), make_tuple(0, 3)), 2},
	{make_pair(make_tuple(3, 2), make_tuple(3, 3)), 3},
	{make_pair(make_tuple(3, 2), make_tuple(3, 1)), 3},
	{make_pair(make_tuple(3, 2), make_tuple(1, 3)), 3},
	{make_pair(make_tuple(3, 3), make_tuple(3, 0)), 1},
	{make_pair(make_tuple(3, 3), make_tuple(0, 3)), 2},
	{make_pair(make_tuple(3, 3), make_tuple(3, 3)), 3},
	{make_pair(make_tuple(3, 3), make_tuple(3, 1)), 3},
	{make_pair(make_tuple(3, 3), make_tuple(1, 3)), 3},
};

int get_tiles_placed_from_lengths(tuple<int, int> prev_lengths, tuple<int, int> curr_lengths)
{
	if (curr_lengths == make_tuple(0, 0))
	{
		return 0;
	}
	else if (curr_lengths == make_tuple(1, 0))
	{
		return 1;
	}
	else if (curr_lengths == make_tuple(0, 1))
	{
		return 2;
	}
	else if (curr_lengths == make_tuple(1, 1))
	{
		return 3;
	}

	auto it = rest.find(make_pair(prev_lengths, curr_lengths));
	if (it != rest.end())
	{
		return it->second;
	}

	return -1;
}

void show()
{
	for (auto const &dp_pair : score_dp)
	{
		if (dp_pair.first.length() == 4 * (maxn - minn + 1) && dp_pair.second >= 0)
		{
			int score = 0;
			int prev_arr[K * M] = {0};
			int curr_arr[K * M] = {0};
			int current = 0;

			int true_lengths[K * M] = {0};
			vector<vector<Tile>> sets;
			// cout << "KEY: " << dp_pair.first << endl;
			for (int value = minn; value <= maxn; value++)
			{
				int mp_index = value - minn;
				std::string mp_str = dp_pair.first.substr(0, 4 * (mp_index + 1));
				score += score_dp[mp_str];
				current = stoi(mp_str.substr(4 * mp_index, 4));
				rmp(current, curr_arr);
				// cout << value << ", " << mp_str << ", " << score_dp[mp_str] << endl;
				// cout << "inlengths: ";
				// for (int i = 0; i < K * M; i++)
				// {
				// 	cout << curr_arr[i] << " ";
				// }
				// cout << endl;
				for (int k = 0; k < K; ++k)
				{
					int i1 = k, i2 = K + k;
					int prev1_length = prev_arr[i1];
					int curr1_length = curr_arr[i1];
					int prev2_length = prev_arr[i2];
					int curr2_length = curr_arr[i2];
					int count = get_tiles_placed_from_lengths(tuple<int, int>(prev1_length, prev2_length), tuple<int, int>(curr1_length, curr2_length));
					assert(count != -1);

					if (prev1_length >= 3 && curr1_length == 0)
					{
						vector<Tile> run;
						for (int i = value - true_lengths[i1]; i < value; ++i)
						{
							run.push_back(Tile(i, COLORS[k]));
						}
						sets.push_back(run);
					}
					if (prev2_length >= 3 && curr2_length == 0)
					{
						vector<Tile> run;
						for (int i = value - true_lengths[i2]; i < value; ++i)
						{
							run.push_back(Tile(i, COLORS[k]));
						}
						sets.push_back(run);
					}

					if (curr1_length == 0)
					{
						true_lengths[i1] = 0;
					}
					if (curr2_length == 0)
					{
						true_lengths[i2] = 0;
					}

					if (count == 1)
					{
						true_lengths[i1] += 1;
					}
					else if (count == 2)
					{
						true_lengths[i2] += 1;
					}
					else if (count == 3)
					{
						true_lengths[i1] += 1;
						true_lengths[i2] += 1;
					}
				}

				memcpy(prev_arr, curr_arr, sizeof(prev_arr));
				memset(curr_arr, 0, sizeof(curr_arr));
			}

			// leftovers
			for (int k = 0; k < K; k++)
			{
				for (int m = 0; m < M; m++)
				{
					if (true_lengths[(m * K) + k] > 0)
					{
						vector<Tile> run;
						for (int i = maxn + 1 - true_lengths[(m * K) + k]; i < maxn + 1; ++i)
						{
							run.push_back(Tile(i, COLORS[k]));
						}
						sets.push_back(run);
					}
				}
			}

			int tile_counts[K][N + 1];
			memcpy(tile_counts, hand, sizeof(tile_counts));

			for (const vector<Tile> &run : sets)
			{
				for (const Tile &tile : run)
				{
					int color_index = distance(COLORS.begin(), find(COLORS.begin(), COLORS.end(), tile.color));
					tile_counts[color_index][tile.number] -= 1;
				}
			}

			vector<Tile> unused_tiles;
			for (int i = minn; i <= maxn; ++i)
			{
				int grouparray[K] = {0};
				for (int k = 0; k < K; ++k)
				{
					grouparray[k] = tile_counts[k][i];
				}

				vector<std::pair<int, int>> ones, twos;
				for (int k = 0; k < K; ++k)
				{
					if (grouparray[k] >= 1)
					{
						ones.push_back(make_pair(i, k));
					}
					if (grouparray[k] == 2)
					{
						twos.push_back(make_pair(i, k));
					}
				}

				if (ones.size() >= 3 && twos.size() >= 3)
				{
					vector<Tile> group1, group2;
					for (const auto &tile : ones)
					{
						group1.push_back(Tile(tile.first, COLORS[tile.second]));
					}
					for (const auto &tile : twos)
					{
						group2.push_back(Tile(tile.first, COLORS[tile.second]));
					}
					sets.push_back(group1);
					sets.push_back(group2);

					for (const auto &tile : ones)
					{
						int color_index = tile.second;
						int number = tile.first;
						tile_counts[color_index][number] -= 1;
					}
					for (const auto &tile : twos)
					{
						int color_index = tile.second;
						int number = tile.first;
						tile_counts[color_index][number] -= 1;
					}
				}
				else if (ones.size() == 4 && twos.size() == 2)
				{
					// find the first tile that is in ones but not in twos
					vector<Tile> group1, group2;
					bool found = false;
					for (const auto &tile : ones)
					{
						if (!found && find(twos.begin(), twos.end(), tile) == twos.end())
						{
							found = true;
							group2.push_back(Tile(tile.first, COLORS[tile.second]));
						}
						else
						{
							group1.push_back(Tile(tile.first, COLORS[tile.second]));
						}
					}
					for (const auto &tile : twos)
					{
						group2.push_back(Tile(tile.first, COLORS[tile.second]));
					}
					sets.push_back(group1);
					sets.push_back(group2);

					for (const auto &tile : ones)
					{
						int color_index = tile.second;
						int number = tile.first;
						tile_counts[color_index][number] -= 1;
					}
					for (const auto &tile : twos)
					{
						int color_index = tile.second;
						int number = tile.first;
						tile_counts[color_index][number] -= 1;
					}
				}
				else if (ones.size() >= 3)
				{
					vector<Tile> group1;
					for (const auto &tile : ones)
					{
						group1.push_back(Tile(tile.first, COLORS[tile.second]));
					}
					sets.push_back(group1);

					for (const auto &tile : ones)
					{
						int color_index = tile.second;
						int number = tile.first;
						tile_counts[color_index][number] -= 1;
					}
				}
			}

			for (int k = 0; k < K; ++k)
			{
				for (int i = 1; i <= N; ++i)
				{
					for (int _ = 0; _ < tile_counts[k][i]; ++_)
					{
						unused_tiles.push_back(Tile(i, COLORS[k]));
					}
				}
			}

			cout << score << endl
				 << endl;
			for (const auto &group : sets)
			{
				cout << '[';
				for (const auto &tile : group)
				{
					cout << tile.number << tile.color;
					if (&tile != &group.back())
					{
						cout << ", ";
					}
				}
				cout << ']' << endl;
			}
			cout << endl;

			cout << '[';
			for (const auto &tile : unused_tiles)
			{
				cout << tile.number << tile.color;
				if (&tile != &unused_tiles.back())
				{
					cout << ", ";
				}
			}
			cout << ']';
			cout << endl
				 << endl;
			cout << endl;
		}
	}
}

void makegroups(int value, int *inlengths, int *runlengths, int *runscores, std::string in_key)
{
	int used[K];
	memset(used, 0, sizeof(used));
	int scoreOfRuns = 0;
	for (int i = 0; i < M * K; i++)
	{
		if (runlengths[i] > 0)
		{
			used[i % K]++;
			scoreOfRuns += (runlengths[i] < MIN_SET_SIZE && (value == maxn)) ? INT_MIN : runscores[i];
		}
	}

	int scoreOfGroups = 0, grouparray[K] = {0};
	for (int i = 0; i < K; i++)
		grouparray[i] = hand[i][value];
	for (int i = 0; i < M * K; i++)
	{
		if (runlengths[i])
			grouparray[i % K]--;
	}
	int ones = 0, twos = 0;
	for (int i = 0; i < K; i++)
	{
		if (grouparray[i] >= 1)
			ones++;
		if (grouparray[i] == 2)
			twos++;
	}

	if (ones == 4 && twos == 4)
	{
		scoreOfGroups = 8;
		for (int i = 0; i < K; i++)
			if (grouparray[i] > 0)
				used[i] += grouparray[i];
	}
	else if (ones == 4 && twos == 3)
	{
		scoreOfGroups = 7;
		for (int i = 0; i < K; i++)
			if (grouparray[i] > 0)
				used[i] += grouparray[i];
	}
	else if (ones == 3 && twos == 3)
	{
		scoreOfGroups = 6;
		for (int i = 0; i < K; i++)
			if (grouparray[i] > 0)
				used[i] += grouparray[i];
	}
	else if (ones == 4 && twos == 2)
	{
		scoreOfGroups = 6;
		for (int i = 0; i < K; i++)
			if (grouparray[i] > 0)
				used[i] += grouparray[i];
	}
	else if (ones == 4 && twos < 3)
	{
		scoreOfGroups = 4;
		for (int i = 0; i < K; i++)
			if (grouparray[i] > 0)
				used[i] += 1;
	}
	else if (ones == 3 && twos < 3)
	{
		scoreOfGroups = 3;
		for (int i = 0; i < K; i++)
			if (grouparray[i] > 0)
				used[i] += 1;
	}

	for (int i = 0; i < K; i++)
	{
		if (used[i] < original_board[i][value])
			return;
	}

	for (int i = 0; i < K; i++)
	{
		bool firstRunEnded = (inlengths[i] > 0 && inlengths[i] < MIN_SET_SIZE) && runlengths[i] == 0;
		bool secondRunEnded = (inlengths[i + K] > 0 && inlengths[i + K] < MIN_SET_SIZE) && runlengths[i + K] == 0;
		if (firstRunEnded && secondRunEnded)
		{
			for (int j = value - 1; j > 0 && (j > value - MIN_SET_SIZE); j--)
			{
				if (original_board[i][j] > used_dp[value][mp(inlengths)][i][j] - 2)
					return;
				used_dp[value][mp(inlengths)][i][j] -= 2;
			}
		}
		else if (firstRunEnded || secondRunEnded)
		{
			for (int j = value - 1; j > 0 && (j > value - MIN_SET_SIZE); j--)
			{
				if (original_board[i][j] > used_dp[value][mp(inlengths)][i][j] - 1)
					return;
				used_dp[value][mp(inlengths)][i][j] -= 1;
			}
		}
	}

	for (int i = 0; i < K; i++)
	{
		used_dp[value][mp(inlengths)][i][value] = used[i];
	}

	std::string mp_str = std::to_string(mp(runlengths));
	std::string padded_mp = std::string(4 - mp_str.length(), '0') + mp_str;
	std::string new_key = in_key + padded_mp;
	int score = scoreOfRuns + (scoreOfGroups * value);
	if (value + 1 > maxn)
	{
		for (int i = 0; i < K; i++)
		{
			bool firstRunEnded = (runlengths[i] > 0 && runlengths[i] < MIN_SET_SIZE);
			bool secondRunEnded = (runlengths[i + K] > 0 && runlengths[i + K] < MIN_SET_SIZE);
			if (firstRunEnded && secondRunEnded)
			{
				for (int j = value; j > 0 && (j > value + 1 - MIN_SET_SIZE); j--)
				{
					if (original_board[i][j] > used_dp[value][mp(inlengths)][i][j] - 2)
						return;
				}
			}
			else if (firstRunEnded || secondRunEnded)
			{
				for (int j = value; j > 0 && (j > value + 1 - MIN_SET_SIZE); j--)
				{
					if (original_board[i][j] > used_dp[value][mp(inlengths)][i][j] - 1)
						return;
				}
			}
		}
	}
	else
	{
		for (int k = 0; k < K; k++)
		{
			for (int i = 1; i <= N; i++)
			{
				used_dp[value + 1][mp(runlengths)][k][i] = used_dp[value][mp(inlengths)][k][i];
			}
		}
		go(value + 1, runlengths, new_key);
	}
	score_dp[new_key] = score;
} // makegroups

// for each color k, compute the (added) score of a run up to that value
void runs(int value, int *inlengths, int *runlengths, int *runscores, int k, std::string key)
{
	if (k == K)
	{ // we have processed all K colors, so now try to make groups
		makegroups(value, inlengths, runlengths, runscores, key);
		return;
	}
	int runscores2[(M * K) + 1], runlengths2[M * K];

	if (hand[k][value] == 2)
	{ // two runs can be continued
		memcpy(runscores2, runscores, sizeof(runscores2));
		memcpy(runlengths2, runlengths, sizeof(runlengths2));
		for (int m = 0; m < M; m++)
		{
			runlengths2[(m * K) + k]++;
			if (runlengths2[(m * K) + k] == 4)
			{
				runscores2[(m * K) + k] += value;
				runlengths2[(m * K) + k] = 3;
			} // if
			else if (runlengths2[(m * K) + k] == 3)
				runscores2[(m * K) + k] = (value * 3) - 1 - 2;
		} // for
		runs(value, inlengths, runlengths2, runscores2, k + 1, key);
	} // if

	if (hand[k][value] >= 1)
	{ // one run can be continued, try forall M
		// int runscores2[(M*K)+1], runlengths2[M*K];
		for (int m = 0; m < M; m++)
		{
			memcpy(runscores2, runscores, sizeof(runscores2));
			memcpy(runlengths2, runlengths, sizeof(runlengths2));
			runlengths2[(m * K) + k]++;
			if (runlengths2[(m * K) + k] == 4)
			{
				runscores2[(m * K) + k] += value;
				runlengths2[(m * K) + k] = 3;
			} // if
			else if (runlengths2[(m * K) + k] == 3)
				runscores2[(m * K) + k] = (value * 3) - 1 - 2;
			runlengths2[(((m + 1) % 2) * K) + k] = 0;
			runscores2[(((m + 1) % 2) * K) + k] = 0;
			runs(value, inlengths, runlengths2, runscores2, k + 1, key);

			// Hardcoded for M = 2
			// Skip duplicates
			if ((runlengths[k] == runlengths[K + k]) || (runlengths[K + k] == 0))
				break;
		} // for
	}	  // if

	// no run continuing
	memcpy(runscores2, runscores, sizeof(runscores2));
	memcpy(runlengths2, runlengths, sizeof(runlengths2));
	runlengths2[(0 * K) + k] = 0;
	runlengths2[(1 * K) + k] = 0;
	runs(value, inlengths, runlengths2, runscores2, k + 1, key);
} // runs

// recursive function, compute/dp-get added score at this value given runlengths
int go(int value, int *inlengths, std::string key)
{
	int runlengths[K * M], runscores[(K * M) + 1] = {0};
	memcpy(runlengths, inlengths, sizeof(runlengths));	   // TODO: assignment could be done directly from inlengths in runs()-function
	runs(value, inlengths, runlengths, runscores, 0, key); // start recursion
} // go

int main(int argc, char *argv[])
{
	memset(original_board, 0, sizeof(original_board));
	memset(hand, 0, sizeof(hand));
	for (int i = 0; i < N + 1; i++)
	{
		for (int j = 0; j < pow(MIN_SET_SIZE + 1, K * M); j++)
		{
			next_array[i][j] = -1;
		}
	}
	memset(used_dp, 0, sizeof(used_dp));
	maxn = 0;
	minn = N + 1;

	bool seen_dash = false;
	for (int i = 1; i < argc; i++)
	{
		std::string tile = argv[i];
		if (tile[0] == '-')
		{
			seen_dash = true;
			continue;
		}

		char c = tile[tile.length() - 1];
		int value = atoi(tile.substr(0, tile.length() - 1).c_str());

		maxn = max(maxn, value);
		minn = min(minn, value);
		switch (c)
		{
		case 'b':
			hand[0][value]++;
			if (!seen_dash)
				original_board[0][value]++;
			break;
		case 'r':
			hand[1][value]++;
			if (!seen_dash)
				original_board[1][value]++;
			break;
		case 'y':
			hand[2][value]++;
			if (!seen_dash)
				original_board[2][value]++;
			break;
		default:
			break;
		} // switch
	}	  // for

	int inlengths[K * M] = {0};
	go(minn, inlengths, "");
	show();
	return 0;
} // main

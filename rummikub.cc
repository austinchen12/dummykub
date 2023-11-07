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

const int N = 13;
const int K = 4;
const int M = 2;
const int KIJK = 10;
const int MIN_SET_SIZE = 3;

const array<std::string, K> COLORS = {"BLUE", "BLACK", "RED", "YELLOW"};

int minn, maxn;
int original_board[K][N + 1];
int hand[K][N + 1];
int dp[N + 1][Power<K, K * M>::value];
int next_array[N + 1][Power<K, K * M>::value];
int dp2[N + 1][Power<K, K * M>::value][K][N + 1]; // current value, current inlengths, target color, target number

int mp(const int *inlengths)
{ // map runlengths array to dp array index
	int newlengths[K * M];
	for (int i = 0; i < K; i++)
	{
		newlengths[i] = inlengths[i];
		newlengths[i + K] = inlengths[i + K];
	}

	int sum = 0;
	for (int i = 0; i < K * M; i++)
		sum += newlengths[i] * pow(K, i);

	return sum;
	return newlengths[0] + K * newlengths[1] + K * K * newlengths[2] + K * K * K * newlengths[3] + K * K * K * K * newlengths[4] + K * K * K * K * K * newlengths[5] + K * K * K * K * K * K * newlengths[6] + K * K * K * K * K * K * K * newlengths[7]; // now for K=4, should be made for any K
} // mp

int go(int, int *); // header for go-function

void rmp(int in, int *result)
{ // reverse mapping from int to runlengt-array
	int i = 0;
	while (in > 0)
	{
		result[i] = in % K;
		in = in / K;
		i++;
	} // while
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

void show(int value, int *inlengths)
{
	int prev_arr[K * M] = {0};
	int curr_arr[K * M] = {0};
	memcpy(curr_arr, inlengths, sizeof(curr_arr));
	int current = mp(inlengths);

	int true_lengths[K * M] = {0};
	vector<vector<Tile>> sets;

	while (current >= 0)
	{
		memcpy(prev_arr, curr_arr, sizeof(prev_arr));
		memset(curr_arr, 0, sizeof(curr_arr));
		current = next_array[value][current];
		rmp(current, curr_arr);
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
		value += 1;
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
	for (int i = 1; i <= N; ++i)
	{
		int grouparray[K] = {0};
		for (int k = 0; k < K; ++k)
		{
			grouparray[k] = tile_counts[k][i];
		}

		vector<pair<int, int>> ones, twos;
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
}

void makegroups(int value, int *inlengths, int *runlengths, int *runscores)
{
	int used[K];
	memset(used, 0, sizeof(used));
	int scoreOfRuns = 0;
	for (int i = 0; i < M * K; i++)
	{
		used[i % K] += runlengths[i] > 0 ? 1 : 0;
		scoreOfRuns += runscores[i];
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
				if (original_board[i][j] > dp2[value][mp(inlengths)][i][j] - 2)
					return;
			}
		}
		else if (firstRunEnded || secondRunEnded)
		{
			for (int j = value - 1; j > 0 && (j > value - MIN_SET_SIZE); j--)
			{
				if (original_board[i][j] > dp2[value][mp(inlengths)][i][j] - 1)
					return;
			}
		}
	}

	for (int i = 0; i < K; i++)
	{
		dp2[value][mp(inlengths)][i][value] = used[i];
	}

	for (int k = 0; k < K; k++)
	{
		for (int i = 1; i <= N; i++)
		{
			dp2[value + 1][mp(runlengths)][k][i] = dp2[value][mp(inlengths)][k][i];
		}
	}

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
					if (original_board[i][j] > dp2[value + 1][mp(runlengths)][i][j] - 2)
						return;
				}
			}
			else if (firstRunEnded || secondRunEnded)
			{
				for (int j = value - 1; j > 0 && (j > value + 1 - MIN_SET_SIZE); j--)
				{
					if (original_board[i][j] > dp2[value + 1][mp(runlengths)][i][j] - 1)
						return;
				}
			}
		}
	}

	if (value + 1 <= maxn) // stop recursion when we pass the largest tile value
	{
		int future_score = go(value + 1, runlengths);
		if (future_score < 0)
			return;

		score += future_score;
	}
	if (score > dp[value][mp(inlengths)])
	{
		next_array[value][mp(inlengths)] = mp(runlengths);
		dp[value][mp(inlengths)] = score;
	} // if
} // makegroups

// for each color k, compute the (added) score of a run up to that value
void runs(int value, int *inlengths, int *runlengths, int *runscores, int k)
{
	if (k == K)
	{ // we have processed all K colors, so now try to make groups
		makegroups(value, inlengths, runlengths, runscores);
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
		runs(value, inlengths, runlengths2, runscores2, k + 1);
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
			runs(value, inlengths, runlengths2, runscores2, k + 1);
		} // for
	}	  // if

	// no run continuing
	memcpy(runscores2, runscores, sizeof(runscores2));
	memcpy(runlengths2, runlengths, sizeof(runlengths2));
	runlengths2[(0 * K) + k] = 0;
	runlengths2[(1 * K) + k] = 0;
	runs(value, inlengths, runlengths2, runscores2, k + 1);

} // runs

// recursive function, compute/dp-get added score at this value given runlengths
int go(int value, int *inlengths)
{
	if (dp[value][mp(inlengths)] > -1) // return value if we already know it
		return dp[value][mp(inlengths)];
	int runlengths[K * M], runscores[(K * M) + 1] = {0};
	memcpy(runlengths, inlengths, sizeof(runlengths)); // TODO: assignment could be done directly from inlengths in runs()-function
	runs(value, inlengths, runlengths, runscores, 0);  // start recursion
	return dp[value][mp(inlengths)];
} // go

int main(int argc, char *argv[])
{
	memset(original_board, 0, sizeof(original_board));
	memset(hand, 0, sizeof(hand));
	for (int i = 0; i < N + 1; i++)
	{
		for (int j = 0; j < pow(K, K * M); j++)
		{
			dp[i][j] = -1;
			next_array[i][j] = -1;
		}
	}
	memset(dp2, 0, sizeof(dp2));
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
		case 'g':
			hand[1][value]++;
			if (!seen_dash)
				original_board[1][value]++;
			break;
		case 'r':
			hand[2][value]++;
			if (!seen_dash)
				original_board[2][value]++;
			break;
		case 'y':
			hand[3][value]++;
			if (!seen_dash)
				original_board[3][value]++;
			break;
		default:
			break;
		} // switch
	}	  // for

	int inlengths[K * M] = {0};
	cout << go(minn, inlengths) << endl
		 << endl;
	show(minn, inlengths);
	return 0;
} // main

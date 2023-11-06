// @author Frank Takes - Leiden University - ftakes@liacs.nl
// Rummikub, tiles=N, colors=K, duplicates=M, minrunlength=3, mingroupsize=K-1

// WARNING: you may need to set ulimit -s 64000 to not get segfaults

#include <cmath>
#include <cstdlib>
#include <cstring>
#include <iostream>
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

const int N = 100;
const int K = 4;
const int M = 2;
const int KIJK = 10;

int minn, maxn;
int hand[K][N + 1];
int dp[N + 1][Power<K, K * M>::value];
int next_array[N + 1][Power<K, K * M>::value];

int mp(const int *inlengths)
{ // map runlengths array to dp array index
	int newlengths[K * M];
	for (int i = 0; i < K; i++)
	{
		if (inlengths[i] >= inlengths[K + i])
		{
			newlengths[i] = inlengths[i];
			newlengths[i + K] = inlengths[i + K];
		}
		else
		{
			newlengths[i] = inlengths[i + K];
			newlengths[i + K] = inlengths[i];
		}
	}
	return newlengths[0] + K * newlengths[1] + K * K * newlengths[2] + K * K * K * newlengths[3] + K * K * K * K * newlengths[4] + K * K * K * K * K * newlengths[5] + K * K * K * K * K * K * newlengths[6] + K * K * K * K * K * K * K * newlengths[7]; // now for K=4, should be made for any K
} // mp

int go(int, int *); // header for go-function

/*
int mp(const int * inlengths) { // map runlengths array to dp array index - SLOW
	int sum = 0;
	for(int i=0; i<K*M; i++)
		sum += pow(K, i) * inlengths[i];
	return sum;
} // mp, 15%-3000% slower
*/

/*
void rmp(int in, int * result) { // reverse mapping from int to runlengt-array
	int i = 0;
	while(in > 0) {
		result[i] = in % K;
		in = in / K;
		i++;
	} // while
} // rmp

void show(int val, int * inlengths) {  // prints the solution of the problem
	int current = mp(inlengths);
	int c[K*M] = {0};
	while(current >= 0) {
		memset(c, 0, sizeof(c));
		rmp(current, c);
		////cerr<< val-1 << "\t" << current << "\t" << next_array[val][current] <<  "\t";
		for(int i=0; i<K; i++)
			 ////cerr<< i << ": " << c[i] << "-" << c[i+K] << "\t";
		////cerr<< dp[val][current] << endl;
		current = next_array[val][current];
		val++;
	} // while
} // show
*/

void makegroups(int value, int *inlengths, int *runlengths, int *runscores)
{
	int scoreOfRuns = 0;
	for (int i = 0; i < M * K; i++)
	{
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
		scoreOfGroups = 8;
	else if (ones == 4 && twos == 3)
		scoreOfGroups = 7;
	else if (ones == 3 && twos == 3)
		scoreOfGroups = 6;
	else if (ones == 4 && twos == 2)
		scoreOfGroups = 6;
	else if (ones == 4 && twos < 3)
		scoreOfGroups = 4;
	else if (ones == 3 && twos < 3)
		scoreOfGroups = 3;
	int score = scoreOfRuns + (scoreOfGroups * value);
	score += go(value + 1, runlengths);
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
	if (value > maxn) // end recursion when we pass the largest tile value
		return 0;
	if (dp[value][mp(inlengths)] > -1) // return value if we already know it
		return dp[value][mp(inlengths)];
	int runlengths[K * M], runscores[(K * M) + 1] = {0};
	memcpy(runlengths, inlengths, sizeof(runlengths)); // TODO: assignment could be done directly from inlengths in runs()-function
	runs(value, inlengths, runlengths, runscores, 0);  // start recursion
	return dp[value][mp(inlengths)];
} // go

int main(int argc, char *argv[])
{
	int runs, n, value;
	char c;
	cin >> runs;
	while (runs--)
	{
		memset(hand, 0, sizeof(hand));
		memset(dp, -1, sizeof(dp));
		memset(next_array, -1, sizeof(dp));
		maxn = 0;
		minn = N + 1;
		cin >> n;
		for (int i = 0; i < n; i++)
		{
			cin >> value >> c;
			maxn = max(maxn, value);
			minn = min(minn, value);
			switch (c)
			{
			case 'b':
				hand[0][value]++;
				break;
			case 'g':
				hand[1][value]++;
				break;
			case 'r':
				hand[2][value]++;
				break;
			case 'y':
				hand[3][value]++;
				break;
			default:
				break;
			} // switch
		}	  // for
		int inlengths[K * M] = {0};
		cout << go(minn, inlengths) << endl;
		// show(minn, inlengths);
	} // while
	return 0;
} // main

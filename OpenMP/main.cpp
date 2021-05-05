#include <iostream>
#include <chrono>
#include <atomic>
#include <thread>
#include <mutex>

#include "CrazyAlgorithms.h"

using namespace std;

#define N 8

vector <BigInt> factorize(BigInt number)
{
	vector <BigInt> factors;
	vector <pair<pair<int, int>, BigInt> > tasksQ; // {{task type, isDone}, number}
	// types : 0 is Prime, 1 factorize
	
	tasksQ.push_back({ {0, 0}, number });

	while (tasksQ.size() > 0)
	{
		cout << "Start trial\n";

		for (auto it : tasksQ)
		{
			cout << it.first.first << " " << it.first.second << " " << it.second.toString() << "\n";
		}

		vector <int> tasksI;
		vector <pair<BigInt, BigInt> > tasksRes(N);

		{// make tasks
			vector <int> primes, factorize;

			for (int i = 0; i < tasksQ.size(); i++)
			{
				if (tasksQ[i].first.first == 0)
				{
					primes.push_back(i);
				}
				else
				{
					factorize.push_back(i);
				}
			}

			for (auto it : primes)
			{
				if (tasksI.size() < N)
				{
					tasksI.push_back(it);
				}
			}
			if (factorize.size() > 0)
			{
				while (tasksI.size() < N)
				{
					for (auto it : factorize)
					{
						if (tasksI.size() < N)
						{
							tasksI.push_back(it);
						}
					}
				}
			}
			while (tasksI.size() < N)
			{
				tasksI.push_back(-1);
			}
		}

		{
#pragma omp parallel num_threads(N)
			{
#pragma omp for
				for (int i = 0; i < N; i++)
				{
					int taskI = tasksI[i];

					if (taskI != -1)
					{
						if (tasksQ[taskI].first.first == 0)
						{
							bool isPrime = CrazyAlgorithms::millerRabinTest(tasksQ[taskI].second);

							if (isPrime)
							{
								tasksRes[i] = { 1, 1 };
							}
							else
							{
								tasksRes[i] = { 0, 0 };
							}
						}
						else
						{
							BigInt divider = CrazyAlgorithms::lenstraAlgorithm(tasksQ[taskI].second);
							if (divider == 1 || divider == tasksQ[taskI].second || divider == BigInt("-1"))
							{
								tasksRes[i] = { BigInt(-1), BigInt(-1) };
							}
							else
							{
								tasksRes[i] = { divider, tasksQ[taskI].second / divider };
							}
						}
					}
				}
			}
		}

		vector <pair<pair<int, int>, BigInt> > newTasksQ;

		for (int i = 0; i < N; i++)
		{
			int ii = tasksI[i];

			if (ii == -1)
			{
				continue;
			}

			if (tasksQ[ii].first.first == 0)
			{
				tasksQ[ii].first.second = 1;

				if (tasksRes[i].first == BigInt("0"))
				{
					newTasksQ.push_back({ {1, 0}, tasksQ[ii].second });
				}
				else
				{
					factors.push_back(tasksQ[ii].second);
				}
			}
			else
			{
				if (tasksQ[ii].first.second == 0 && tasksRes[i].first != BigInt(-1))
				{
					tasksQ[ii].first.second = 1;

					newTasksQ.push_back({ {0, 0}, tasksRes[i].first });
					newTasksQ.push_back({ {0, 0}, tasksRes[i].second });
				}
			}
		}

		for (auto it : tasksQ)
		{
			if (it.first.second != 1)
			{
				newTasksQ.push_back(it);
			}
		}
		tasksQ = newTasksQ;
	}

	return factors;
}

int main()
{
	//BigInt number = BigInt("17") * BigInt("19");
	//BigInt number = BigInt("1000000007") * BigInt("1000000009");
	BigInt number = BigInt("11037271757") * BigInt("11037271769") * BigInt("1000000007") * BigInt("1000000009");

	chrono::high_resolution_clock::time_point t1 = chrono::high_resolution_clock::now();

	vector <BigInt> factors = factorize(number);

	chrono::high_resolution_clock::time_point t2 = chrono::high_resolution_clock::now();
	chrono::duration<double> time = chrono::duration_cast<chrono::duration<double>>(t2 - t1);

	cout << "Time: " << time.count() << "\n";

	cout << "Factors of " << number.toString() << " are:\n";
	for (auto it : factors)
	{
		cout << it.toString() << "\n";
	}

	getchar();
	getchar();

	return 0;
}
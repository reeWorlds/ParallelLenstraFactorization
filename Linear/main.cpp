#include <iostream>
#include <chrono>
#include <time.h>

#include "CrazyAlgorithms.h"

using namespace std;

vector <BigInt> factorize(BigInt number)
{
	vector <BigInt> factors;
	vector <pair<int, BigInt> > tasksQ; // {{task type, isDone}, number}
	// types : 0 is Prime, 1 factorize

	tasksQ.push_back({0, number });

	while (tasksQ.size() > 0)
	{
		pair<int, BigInt> task = tasksQ.back();
		tasksQ.pop_back();

		cout << task.first << " " << task.second.toString() << "\n";

		if (task.first == 0)
		{
			bool isPrime = CrazyAlgorithms::millerRabinTest(task.second);

			if (isPrime)
			{
				factors.push_back(task.second);
			}
			else
			{
				tasksQ.push_back({ 1, task.second });
			}
		}
		else
		{
			BigInt divider = CrazyAlgorithms::lenstraAlgorithm(task.second);
			if (divider == 1 || divider == task.second || divider == BigInt("-1"))
			{
				tasksQ.push_back({ 1, task.second });
			}
			else
			{
				cout << "Find divider " << divider.toString() << "\n";
				tasksQ.push_back({ 0, divider });
				tasksQ.push_back({ 0, task.second / divider });
			}
		}
	}

	return factors;
}

int main()
{
	BigInt number = BigInt("1000000007") * BigInt("1000000009");
	//BigInt number = BigInt("11037271757") * BigInt("11037271769") * BigInt("1000000007") * BigInt("1000000009");

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
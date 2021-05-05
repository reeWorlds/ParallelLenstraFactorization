#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <set>
#include <map>
#include <unordered_map>
#include <queue>
#include <deque>
#include <cmath>
#include <math.h>
#include <string>
#include <time.h>
#include <random>
#include <memory.h>
#include <bitset>

#include <thread>
#include <mutex>
#include <atomic>

#include <mpi.h>

#include "CrazyAlgorithms.h"

using namespace std;

class TasksQueue
{
public:

	TasksQueue();

	~TasksQueue();

	void addTask(int type, BigInt number);

	void run(int workersCount = 8);

	void stop();

	void worker();

	bool workDone() const;

	void showFactorization();

private:

	static mt19937 rng;

	vector <int> m_initQueue;
	mutable mutex m_initMutex;

	atomic_bool workFinished;
	vector<pair<int, pair<int, BigInt> > > m_workQueue; // {id, {type, number} }, type: 1 facorize, 2 isPrime 
	set<int> activeTasks;
	vector <BigInt> primeFactors;
	mutable mutex m_queueMutex;

	vector<thread> m_workers;

	atomic_bool m_cancel;
};
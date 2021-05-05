#include "TasksQueue.h"

mt19937 TasksQueue::rng = mt19937(time(0));

TasksQueue::TasksQueue()
{
	m_cancel = false;
	workFinished = false;
}

TasksQueue::~TasksQueue()
{
	stop();
}

void TasksQueue::addTask(int type, BigInt number)
{
	//lock_guard<mutex> lock(m_queueMutex);

	int id = rng();
	activeTasks.insert(id);
	m_workQueue.push_back({ id, {type, number} });
}

void TasksQueue::run(int workersCount)
{
	stop();

	m_cancel = false;
	workFinished = false;

	for (int i = 1; i <= workersCount; i++)
	{
		m_initQueue.push_back(i);
	}

	for (int i = 0; i < workersCount; i++)
	{
		m_workers.push_back(thread(&TasksQueue::worker, this));

		this_thread::sleep_for(chrono::milliseconds(10));
	}
}

void TasksQueue::stop()
{
	workFinished = true;
	m_cancel = true;

	for (size_t i = 0; i < m_workers.size(); i++)
	{
		if (m_workers[i].joinable())
			m_workers[i].join();
	}

	m_workers.clear();
}

void TasksQueue::worker()
{
	int processI;
	
	{
		std::lock_guard<std::mutex> lock(m_initMutex);

		processI = m_initQueue.back();
		m_initQueue.pop_back();
	}

	while (true)
	{
		int id, type;
		BigInt number;

		while (true)
		{
			bool nWorkDone;

			{
				nWorkDone = !workDone();
			}

			if (nWorkDone)
			{
				std::lock_guard<std::mutex> lock(m_queueMutex);

				if (!m_workQueue.empty())
				{
					id = m_workQueue.back().first;
					type = m_workQueue.back().second.first;
					number = m_workQueue.back().second.second;

					if (type == 2)
					{
						activeTasks.erase(id);
						m_workQueue.pop_back();
					}

					break;
				}
			}
			else
			{
				int commandSize = 20;
				char command[20] = "stop";

				MPI_Send(command, commandSize, MPI_CHAR, processI, processI, MPI_COMM_WORLD);

				return;
			}

			this_thread::sleep_for(chrono::milliseconds(1));
		}

		// Work using data

		if (type == 1)
		{
			int commandSize = 20;
			char command[20] = "factorize";
			MPI_Send(command, commandSize, MPI_CHAR, processI, processI, MPI_COMM_WORLD);

			string numberS = number.toString();
			int numberCharSize = 200;
			char numberChar[200];
			strcpy_s(numberChar, numberS.c_str());
			MPI_Send(numberChar, numberCharSize, MPI_CHAR, processI, processI, MPI_COMM_WORLD);

			int answerSize = 100;
			char answer[100];
			MPI_Recv(answer, answerSize, MPI_CHAR, processI, processI, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);

			if (strcmp(answer, "-1") == 0)
			{
				// to do nothing, iteration failed

				cout << "Thread #0_" << processI << " factorize but iteration unlucky -1\n";
				cout.flush();
			}
			else
			{
				std::lock_guard<std::mutex> lock(m_queueMutex);

				if (activeTasks.find(id) != activeTasks.end())
				{
					activeTasks.erase(id);

					for (int i = 0; i < m_workQueue.size(); i++)
					{
						if (m_workQueue[i].first == id)
						{
							m_workQueue.erase(m_workQueue.begin() + i);

							break;
						}
					}

					string numberS(answer);
					BigInt a(numberS);
					BigInt b = number / a;

					cout << "Thread #0_" << processI << "factorize " << number.toString() << " = " << a.toString() << " * " << b.toString() << "\n";
					cout.flush();

					addTask(2, a);
					addTask(2, b);
				}
				else
				{
					cout << "Thread #0_" << processI << " factorize " << numberS << " but it is too late\n";
					cout.flush();
				}
			}
		}
		else
		{
			int commandSize = 20;
			char command[20] = "isPrime";
			MPI_Send(command, commandSize, MPI_CHAR, processI, processI, MPI_COMM_WORLD);

			string numberS = number.toString();
			int numberCharSize = 200;
			char numberChar[200];
			strcpy_s(numberChar, numberS.c_str());
			MPI_Send(numberChar, numberCharSize, MPI_CHAR, processI, processI, MPI_COMM_WORLD);

			int answerSize = 10;
			char answer[10];
			MPI_Recv(answer, answerSize, MPI_CHAR, processI, processI, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);

			if (strcmp(answer, "prime") == 0)
			{
				std::lock_guard<std::mutex> lock(m_queueMutex);

				cout << "Thread #0_" << processI << " told that: " << numberS << " is prime\n";
				cout.flush();

				primeFactors.push_back(number);

				if (activeTasks.size() == 0)
				{
					workFinished = true;
				}
			}
			else
			{
				std::lock_guard<std::mutex> lock(m_queueMutex);

				cout << "Thread #0_" << processI << " told that: " << numberS << " is not prime\n";
				cout.flush();

				addTask(1, number);
			}
		}
	}
}

bool TasksQueue::workDone() const
{
	lock_guard<std::mutex> lock(m_queueMutex);
	return m_workQueue.empty() && workFinished;
}

void TasksQueue::showFactorization()
{
	cout << "number = ";
	for (int i = 0; i < primeFactors.size(); i++)
	{
		cout << primeFactors[i].toString() << "*\n"[i + 1 == primeFactors.size()];
	}
	cout << "\n";
	cout.flush();
}
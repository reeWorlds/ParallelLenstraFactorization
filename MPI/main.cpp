#define  _CRT_SECURE_NO_WARNINGS

#include <mpi.h>
#include <string>
#include <iostream>

#include "CrazyAlgorithms.h"
#include "TasksQueue.h"

using namespace std;

void processMainThread(int processI, int totalWorkers)
{
	TasksQueue q;

	//BigInt number = BigInt("1000000007") * BigInt("1000000009");
	//BigInt number = BigInt("4");

	BigInt number = BigInt("11037271757") * BigInt("11037271769") * BigInt("1000000007") * BigInt("1000000009");
	//BigInt number = BigInt("64") * BigInt("2750475");

	q.addTask(2, number);

	q.run(totalWorkers - 1);

	while (!q.workDone())
	{
		this_thread::sleep_for(chrono::milliseconds(1));
	}

	q.stop();

	q.showFactorization();
}

void processWorker(int processI, int mainThreadI)
{
	while (true)
	{
		int commandSize = 20;
		char command[20];
		MPI_Recv(command, commandSize, MPI_CHAR, mainThreadI, processI, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);

		if (strcmp(command, "stop") == 0)
		{
			cout << "Thread #" << processI << " recieve command: " << command << "\n";
			cout << "Process #" << processI << " finish it's work\n";
			cout.flush();

			return;
		}
		else if (strcmp(command, "factorize") == 0)
		{
			int numberCharSize = 200;
			char numberChar[200];
			MPI_Recv(numberChar, numberCharSize, MPI_CHAR, mainThreadI, processI, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);

			cout << "Thread #" << processI << " recieve command: " << command << " " << numberChar << "\n";
			cout.flush();

			string numberS(numberChar);
			BigInt number(numberS);

			BigInt divider = CrazyAlgorithms::lenstraAlgorithm(number);
			if (divider == 1 || divider == number) { divider = -1; }
			string dividerS = divider.toString();

			int answerSize = 100;
			char answer[100];
			strcpy_s(answer, dividerS.c_str());
			MPI_Send(answer, answerSize, MPI_CHAR, 0, processI, MPI_COMM_WORLD);
		}
		else if (strcmp(command, "isPrime") == 0)
		{
			int numberCharSize = 200;
			char numberChar[200];
			MPI_Recv(numberChar, numberCharSize, MPI_CHAR, mainThreadI, processI, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);

			cout << "Thread #" << processI << " recieve command: " << command << " " << numberChar << "\n";
			cout.flush();

			string numberS(numberChar);
			BigInt number(numberS);

			bool isPrime = CrazyAlgorithms::millerRabinTest(number);
			string isPrimeS;
			if (isPrime == true)
			{
				isPrimeS = "prime";
			}
			else
			{
				isPrimeS = "nonprime";
			}

			int answerSize = 10;
			char answer[10];
			strcpy(answer, isPrimeS.c_str());
			MPI_Send(answer, answerSize, MPI_CHAR, 0, processI, MPI_COMM_WORLD);
		}
		else
		{
			cout << "Unknown command in thread #" << processI << "\n";
			cout << "Process #" << processI << " finish it's work with fail\n";
			cout.flush();

			return;
		}
	}
}

int main(int argc, char* argv[])
{
	// processN - total number of processes, processI - index of current process
	int processN;
	int processI;

	chrono::high_resolution_clock::time_point t1 = chrono::high_resolution_clock::now();

	// init MPI with MPI_THREAD_MULTIPLE thread safe type
	int provided;
	MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);

	if (provided < MPI_THREAD_MULTIPLE)
	{
		cout << "ERROR: The MPI library does not have full thread support\n";
		MPI_Abort(MPI_COMM_WORLD, 1);
	}

	// init processN and processI
	MPI_Comm_size(MPI_COMM_WORLD, &processN);
	MPI_Comm_rank(MPI_COMM_WORLD, &processI);

	// processI == 0 => main thread
	// processI != 0 => worker
	if (processI == 0)
	{
		processMainThread(processI, processN);
	}
	else
	{
		processWorker(processI, 0);
	}

	// finalize MPI
	MPI_Finalize();

	chrono::high_resolution_clock::time_point t2 = chrono::high_resolution_clock::now();
	chrono::duration<double> time = chrono::duration_cast<chrono::duration<double>>(t2 - t1);

	if (processI == 0)
	{
		cout << "Time: " << time.count() << "\n";
	}

	return 0;
}
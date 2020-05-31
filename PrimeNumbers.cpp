// PrimeNumbers.cpp : définit le point d'entrée de l'application.
//

#include "PrimeNumbers.h"

constexpr auto THREAD_POOL_SIZE = 8;

using namespace std;

#pragma region Globals
// The locks for updating global values.
static mutex g_PickupLock, g_IncrementLock;

static int g_FinishedNumbers = 0;
static int g_MaxNumber = 0;
static int g_PrimeNumbers = 0;
static queue<int> g_Queue = queue<int>();

static vector<int> g_PrimeNumbersList;

#pragma endregion

void ComputeTask(int* number) {
	int counter = *number;
	bool prime = true;
	while (counter > 2 && prime) {
		counter--;
		if (*number % counter == 0)
		{
			prime = false;
			break;
		}
	}
	g_IncrementLock.lock();
	g_FinishedNumbers++;
	if (prime) {
		g_PrimeNumbers++;
		g_PrimeNumbersList.push_back(*number);
	}
	else {
	}
	g_IncrementLock.unlock();
}

void ThreadTask() {
	bool finished = true;
	while (finished && g_Queue.size() != 0) {
		g_PickupLock.lock();
		int number = g_Queue.front();
		g_Queue.pop();
		finished = g_Queue.size() != 0;
		g_PickupLock.unlock();
		ComputeTask(&number);
	}
}


int main()
{
	thread threadPool[THREAD_POOL_SIZE];	
	cin >> g_MaxNumber;

	for (int i = 0; i < g_MaxNumber + 2;i++) {
		if (i == 0 || i == 1) continue;
		g_Queue.push(i);
	}

	cout << "Initialized queue with " << g_Queue.size() << " tasks" << endl;
	/* Create the compute threads */
	for (int i = 0; i < THREAD_POOL_SIZE; i++) {
		threadPool[i] = thread(ThreadTask);
	}

	int lastResolved = 0;
	while (g_MaxNumber > g_FinishedNumbers) {
		cout << "Computing status : " << g_FinishedNumbers << "/" << g_MaxNumber << " (" << g_FinishedNumbers - lastResolved << "/s)"  << endl;
		lastResolved = g_FinishedNumbers;
		this_thread::sleep_for(chrono::seconds(1));
	}

	/* Just make sure that all the threads are stopped */
	for (int i = 0; i < THREAD_POOL_SIZE; i++) {
		threadPool[i].join();
	}

	cout << "Computation finished. " << g_FinishedNumbers << " tasks finished." << endl << "We found " << g_PrimeNumbers << " prime numbers." << endl;

	ofstream myfile;
	myfile.open("0-" + to_string(g_MaxNumber) +".html");
	myfile << "<h1>Prime numbers between 2 and " << g_MaxNumber << "</h1>" << endl;
	myfile << "<ul>" << endl;

	for (int i = 0; i < g_PrimeNumbersList.size(); i++) {
		myfile << "	<li>" << (g_PrimeNumbersList.at(i)) << "</li>" << endl;
	}
	myfile << "</ul>" << endl;
	myfile.close();
	cout << "Saved to 0-" << g_MaxNumber << ".html" << endl;
	
	return 0;
}

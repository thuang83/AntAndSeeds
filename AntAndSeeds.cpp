/*
Author: Tiffany Huang
Last Date Modified: 10/14/2021
Description:
This code finds the average number of moves it takes for a ant to move seeds 
from the bottom to the top row of a 5x5 grid while walking randomly(Project Euler #280) This is done
using multiple threads to save computing time. The output is printed to answer.txt

Compile this code with flags --std=c++17 -pthread -O3
ex: g++ AndAndSeeds.cpp --std=c++17 -pthread -O3
*/
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <iostream>
#include <random>
#include <thread>
#include <set>
#include <cmath>
#include <mutex>
#include <atomic>
#include <iomanip>
#include <fstream>
using namespace std;

//initialize counter for total number of moves
unsigned long long totalMoves(0);
std::mutex mtx;

void moveAnt(int numTrials)
{
	//std::cout << "From Thread ID: "<< std::this_thread::get_id() << "\n";
	//create random generator for ant movement
	//std::default_random_engine generator((unsigned)time(NULL));;
	std::random_device dev;
	std::mt19937 rng(dev());
	std::uniform_int_distribution<std::mt19937::result_type> dist4(1, 4);

	//std::default_random_engine generator()
	//std::uniform_int_distribution<int> distribution(1, 4);

	//thread moves
	unsigned long long threadMoves(0);
	for (int i = 0; i < numTrials; i++)
	{
		//seed locations
		std::set<int> botSeeds= { 1,2,3,4,5 };

		//new seed locations
		std:set<int> topSeeds = {};

		//ant location
		std::pair <int, int> antLoc(3, 3);

		//ant has seed
		bool hasSeed = false;

		//total dropped seeds
		int droppedSeeds = 0;

		//num moves for this trial
		unsigned long long numMoves = 0;

		while (droppedSeeds < 5)
		{
			//std::cout << antLoc.first << ", " << antLoc.second << "\n";
			//check if ant is already carrying seed
			if (hasSeed)
			{
				//check if ant is on top row
				if (antLoc.first == 5)
				{
					//check if square is empty
					if (topSeeds.count(antLoc.second) == 0)
					{
						topSeeds.insert(antLoc.second);
						droppedSeeds += 1;
						hasSeed = false;
					}
				}
			}
			//if not carrying seed, check if ant is on bottom row
			else if (antLoc.first == 1)
			{
				//check if seed exists on current location, then pick up
				if (botSeeds.count(antLoc.second) != 0)
				{
					botSeeds.erase(antLoc.second);
					hasSeed = true;
				}
			}

			//edge case, if ant has already reached 5 dropped seeds
			if (droppedSeeds == 5)
			{
				break;
			}

			int nextMove = dist4(rng);
			switch (nextMove)
			{
			case 1:
				if (antLoc.first + 1 <= 5)
				{
					antLoc.first++;
					numMoves++;
				}
				break;
			case 2:
				if (antLoc.first - 1 >= 1)
				{
					antLoc.first--;
					numMoves++;
				}
				break;
			case 3:
				if (antLoc.second + 1 <= 5)
				{
					antLoc.second++;
					numMoves++;
				}

				break;
			case 4:
				if (antLoc.second - 1 >= 1)
				{
					antLoc.second--;
					numMoves++;
				}
				break;
			}
			
		}
		threadMoves += numMoves;
		//std::cout << "Trial" << i << "moves" << numMoves << "\n";
	}
	mtx.lock();
	totalMoves += threadMoves;
	//std::cout << "\nthread " << std::this_thread::get_id() << " : " << threadMoves;
	mtx.unlock();
}


int main()
{
	unsigned int nThreads = std::thread::hardware_concurrency();
	std::vector<std::thread> threads;
	float numRuns = 100000000;
	//spawn max threads
	for (int i = 0; i < nThreads; i++)
	{
		threads.push_back(std::thread(moveAnt, numRuns / nThreads));
	}
	for (auto& th : threads) th.join();

	//calculate averages
	
	float average = totalMoves / numRuns;
	//std::cout << "total moves: " << totalMoves;
	//std::cout << "average moves: " << average;

	//write to fie
	fstream outputFile;
	outputFile.open("answer.txt", ios::out);
	outputFile << "Number of threads created : " << nThreads << "\n\n";
	outputFile << "Expected number of steps: " << fixed << std::setprecision(6) << average << "\n\n";
	outputFile << "Total number of runs needed for solution convergence: 10000000";
	outputFile.close();

	return 0;
}






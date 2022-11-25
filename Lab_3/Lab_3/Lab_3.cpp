// Lab_3.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "Sorts.h"
#include <random>
#include <chrono>

std::random_device rd;
std::mt19937 gen(rd());

int RandRange(int min, int max)
{
	std::uniform_int_distribution<> dist(min, max);
	return dist(gen);
}

void timeTest(int testSize, int testStep, int testStart, int repeats)
{

	
}

int main()
{
	timeTest(512, 1, 1, 10000);
}



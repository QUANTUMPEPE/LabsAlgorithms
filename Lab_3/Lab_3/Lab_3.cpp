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

int main()
{
	
}



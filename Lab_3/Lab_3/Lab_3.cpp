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
	const size_t kSize = 10;
	std::string* quickSort = new std::string[kSize];
	std::string* insertSort = new std::string[kSize];
	std::string* standartSort = new std::string[kSize];

	std::cout<< "_________Unsorted_________" << std::endl;
	std::cout<< "Quick\tInset\tStandard" << std::endl;

	for (size_t i = 0; i < kSize; ++i)
	{
		quickSort[i] = std::to_string(RandRange(-1000, 1000));
		insertSort[i] = quickSort[i];
		standartSort[i] = quickSort[i];
		std::cout << quickSort[i] << "\t" << insertSort[i] << "\t" << standartSort[i] << std::endl;
	}

	Sorts::insertions(insertSort, (insertSort + kSize - 1), [](std::string a, std::string b) { return a < b; });
	Sorts::quick(quickSort, (quickSort + kSize - 1), [](std::string a, std::string b) { return a < b; }, true);
	std::sort(standartSort, standartSort + kSize, [](std::string a, std::string b) { return a < b; });

	std::cout << "_________Sorted_________" << std::endl;
	std::cout << "Quick\tInset\tStandard" << std::endl;

	for (size_t i = 0; i < kSize; ++i)
	{
		std::cout << quickSort[i] << "\t" << insertSort[i] << "\t" << standartSort[i] << std::endl;
	}

	delete[] quickSort;
	delete[] insertSort;
	delete[] standartSort;
}



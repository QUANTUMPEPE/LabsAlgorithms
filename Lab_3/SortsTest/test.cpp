#include "pch.h"
#include "../Lab_3/Sorts.h"
#include <random>
#include <chrono>
#include <fstream>
#include <Windows.h>

std::random_device rd;
std::mt19937 gen(rd());

int RandRange(int min, int max)
{
	std::uniform_int_distribution<> dist(min, max);
	return dist(gen);
}

TEST(QuickSortTest, SortTest) 
{
	int* array = new int [100000];
	for (int i = 0; i < 100000; ++i)
	{
		array[i] = RandRange(INT_MIN, INT_MAX);
	}
	Sorts::quick(array, array + 100000 - 1, [](int a, int b) { return a < b; }, false);

	for (int i = 1; i < 100000; ++i)
	{
		EXPECT_TRUE(array[i-1] <= array[i]);
	}	
}

TEST(InsertSortTest, SortTest)
{
	int* array = new int[100000];
	for (int i = 0; i < 100000; ++i)
	{
		array[i] = RandRange(INT_MIN, INT_MAX);
	}
	Sorts::insertions(array, array + 100000 - 1, [](int a, int b) { return a < b; });

	for (int i = 1; i < 100000; ++i)
	{
		EXPECT_TRUE(array[i - 1] <= array[i]);
	}
}

TEST(CompareTest, TimeAndEqualityTestOptimized)
{
	int testSize = 512;
	int repeats = 10000;
	int stepSize = 4;
	bool worstCase = false;

	typedef std::chrono::high_resolution_clock Time;
	typedef std::chrono::nanoseconds ns;
	typedef std::chrono::duration<float> fsec;
	typedef std::chrono::steady_clock::time_point TimeType;

	std::ofstream os("Stats.txt", std::ios::out | std::ios::trunc);
	if (!os.is_open())
	{
		std::cout << "Could not open file!";
		os.close();
	}

	for (int i = 2; i <= testSize; i += stepSize)
	{
		ns quickSortTime = std::chrono::nanoseconds(0);
		ns insertSortTime = std::chrono::nanoseconds(0);

		/*LARGE_INTEGER quickSortTime;
		quickSortTime.QuadPart = 0;
		LARGE_INTEGER insertSortTime;
		insertSortTime.QuadPart = 0;

		LARGE_INTEGER frequency;
		QueryPerformanceFrequency(&frequency);*/

		std::cout << "Step = " << i << " in " << testSize << std::endl;
		for (int k = 0; k < repeats; ++k)
		{
			int* arrayA{ new int[i] };
			int* arrayB{ new int[i] };
			for (int j = 0; j < i; ++j)
			{
				if(worstCase)
				{
					arrayA[j] = i - j;
					arrayB[j] = RandRange(INT_MIN, INT_MAX);
				}
				else
				{
					arrayA[j] = RandRange(INT_MIN, INT_MAX);
					arrayB[j] = arrayA[j];
				}
			}

			TimeType startInsretTime;
			TimeType startQuickTime;
			TimeType endInsretTime;
			TimeType endQuickTime;

			/*LARGE_INTEGER startInsretTime;
			LARGE_INTEGER startQuickTime;
			LARGE_INTEGER endInsretTime;
			LARGE_INTEGER endQuickTime;
			*/

			startInsretTime = Time::now();
			//QueryPerformanceCounter(&startInsretTime);
			Sorts::insertions(arrayA, arrayA + i - 1, [](int a, int b) { return a < b; });
			//QueryPerformanceCounter(&endInsretTime);
			endInsretTime = Time::now();

			startQuickTime = Time::now();
			//QueryPerformanceCounter(&startQuickTime);
			Sorts::quick(arrayB, arrayB + i - 1, [](int a, int b) { return a < b; }, true);
			//QueryPerformanceCounter(&endQuickTime);
			endQuickTime = Time::now();

			quickSortTime += (endQuickTime - startQuickTime);
			insertSortTime += (endInsretTime- startInsretTime);

			if(!worstCase)
			{
				for (int j = 0; j < i; ++j)
				{
					EXPECT_EQ(arrayA[j], arrayB[j]);
				}
			}
			
			delete[] arrayA;
			delete[] arrayB;
		}

		quickSortTime /= repeats;
		insertSortTime /= repeats;

		//std::cout << "Insertions sort time = " << ((float)insertSortTime.QuadPart / frequency.QuadPart)*1000 << std::endl;
		std::cout << "Insertions sort time = " << insertSortTime.count() << std::endl;
		//std::cout << "Quick sort time = " << ((float)quickSortTime.QuadPart / frequency.QuadPart)*1000 << std::endl;
		std::cout << "Quick sort time = " << quickSortTime.count() << std::endl;

		std::cout << "_____________________________________________" << std::endl;

		os << i << "\t";
		os << insertSortTime.count() << "\t";
		//os << insertSortTime.QuadPart << "\t";
		os << quickSortTime.count() << std::endl;
		//os << quickSortTime.QuadPart << std::endl;

		if (os.fail())
		{
			std::cout << "Fail during recording file";
			os.close();
		}
	}

	os.close();
}
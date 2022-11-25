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

class EqualityTests : public testing::Test
{
protected:
	template<typename T, typename Compare>
	void EqualityTest(T* arraySample, size_t size, Compare comp)
	{
		T* quickSort = new T[size];
		T* insertSort = new T[size];
		T* standartSort = new T[size];

		for (size_t i = 0; i < size; ++i)
		{
			quickSort[i] = arraySample[i];
			insertSort[i] = arraySample[i];
			standartSort[i] = arraySample[i];
		}

		Sorts::insertions(insertSort, (insertSort + size - 1), comp);
		Sorts::quick(quickSort, (quickSort + size - 1), comp, true);
		std::sort(standartSort, standartSort + size, comp);

		for (size_t i = 0; i < size; ++i)
		{		
			EXPECT_EQ(standartSort[i], quickSort[i]);
			EXPECT_EQ(insertSort[i], quickSort[i]);
		}

		delete [] quickSort;
		delete [] insertSort;
		delete [] standartSort;
	}
};

#pragma region QuickSort
TEST(QuickSortTest, OneItem)
{
	int* array = new int{1};
	Sorts::quick(array, array, [](int a, int b) { return a < b; }, true);
	EXPECT_EQ(*array, 1);
}

TEST(QuickSortTest, SortTest)
{
	int* array = new int[100000];
	for (int i = 0; i < 100000; ++i)
	{
		array[i] = RandRange(INT_MIN, INT_MAX);
	}
	Sorts::quick(array, array + 100000 - 1, [](int a, int b) { return a < b; }, false);

	for (int i = 1; i < 100000; ++i)
	{
		EXPECT_TRUE(array[i - 1] <= array[i]);
	}
}
#pragma endregion QuickSort

#pragma region InsertSort
TEST(InsertSortTest, OneItem)
{
	int* array = new int{ 1 };
	Sorts::insertions(array, array, [](int a, int b) { return a < b; });
	EXPECT_EQ(*array, 1);
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
#pragma endregion InsertSort

#pragma region GeneralTests
TEST_F(EqualityTests, SortsEqualityResultTest)
{
	const size_t size = 100;
	const int repeats = 1000;
	int defaultTypeSample[size];
	std::string stringTypeSample[size];

	for (size_t j = 0; j < repeats; ++j)
	{
		for (size_t i = 0; i < size; ++i)
		{
			defaultTypeSample[i] = RandRange(INT_MIN, INT_MAX);
			stringTypeSample[i] = std::to_string(defaultTypeSample[i]);
		}

		EqualityTest(defaultTypeSample, size, [](int a, int b) { return a < b; });
		EqualityTest(stringTypeSample, size, [](std::string a, std::string b) { return a < b; });
	}
}

TEST(CompareTest, PerformanceAndEqualityTest)
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

		std::cout << "Step = " << i << " in " << testSize << std::endl;
		for (int k = 0; k < repeats; ++k)
		{
			int* arrayA{ new int[i] };
			int* arrayB{ new int[i] };
			for (int j = 0; j < i; ++j)
			{
				if (worstCase)
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

			startInsretTime = Time::now();
			Sorts::insertions(arrayA, arrayA + i - 1, [](int a, int b) { return a < b; });
			endInsretTime = Time::now();

			startQuickTime = Time::now();
			Sorts::quick(arrayB, arrayB + i - 1, [](int a, int b) { return a < b; }, true);
			endQuickTime = Time::now();

			quickSortTime += (endQuickTime - startQuickTime);
			insertSortTime += (endInsretTime - startInsretTime);

			delete[] arrayA;
			delete[] arrayB;
		}

		quickSortTime /= repeats;
		insertSortTime /= repeats;

		std::cout << "Insertions sort time = " << insertSortTime.count() << std::endl;
		std::cout << "Quick sort time = " << quickSortTime.count() << std::endl;

		std::cout << "_____________________________________________" << std::endl;

		os << i << "\t";
		os << insertSortTime.count() << "\t";
		os << quickSortTime.count() << std::endl;

		if (os.fail())
		{
			std::cout << "Fail during recording file";
			os.close();
		}
	}

	os.close();
}
#pragma endregion GeneralTests






#include "pch.h"
#include "../Lab_2/DynamicArray.h"

#pragma region DefaultType
TEST(DAConstructorTestInt, TestDefaultConstructor)
{
	DynamicArray<int> array;
	ASSERT_EQ(array.size(), 0);
	ASSERT_EQ(array.getCapcaity(), 8);
}

TEST(DAConstructorTestInt, TestSizeConstructor)
{
	DynamicArray<int> array(10);
	ASSERT_EQ(array.size(), 0);
	ASSERT_EQ(array.getCapcaity(), 10);
}

TEST(DAConstructorTestInt, TestCopyConstructor)
{
	DynamicArray<int> arrayA(10);
	for (int i = 0; i < arrayA.size(); ++i)
		arrayA.insert(i);

	DynamicArray<int> arrayB{ arrayA };

	ASSERT_EQ(arrayA.size(), arrayB.size());
	ASSERT_EQ(arrayA.getCapcaity(), arrayB.getCapcaity());

	for (int i = 0; i < arrayA.size(); ++i)
		ASSERT_EQ(arrayA[i], arrayB[i]);
}

TEST(DAConstructorTestInt, TestMoveConstructor)
{
	DynamicArray<int> arrayA(10);
	for (int i = 0; i < arrayA.size(); ++i)
		arrayA.insert(i);

	int arrayACapacity = arrayA.getCapcaity();
	int arrayASize = arrayA.size();

	DynamicArray<int> arrayB = std::move(arrayA);

	ASSERT_EQ(arrayACapacity, arrayB.getCapcaity());
	ASSERT_EQ(arrayASize, arrayB.size());
	for (int i = 0; i < arrayB.size(); ++i)
		ASSERT_EQ(arrayB[i], i);
}

TEST(DAConstructorTestInt, ExpansionTest)
{
	DynamicArray<int> array;
	ASSERT_EQ(array.getCapcaity(), 8);

	for (int i = 0; i < 9; ++i)
		array.insert(i);
	ASSERT_EQ(array.getCapcaity(), 16);

	for (int i = 0; i < 9; ++i)
		array.insert(i);
	ASSERT_EQ(array.getCapcaity(), 32);
}

TEST(DAConstructorTestInt, InsertTest)
{
	DynamicArray<int> array;	
	for (int i = 0; i < 5; ++i)
		array.insert(i);

	ASSERT_EQ(array.insert(3, 100), 3);
	ASSERT_EQ(array[3], 100);

	ASSERT_EQ(array.insert(0, 101), 0);
	ASSERT_EQ(array[0], 101);
}

TEST(DAConstructorTestInt, RemoveTest)
{
	DynamicArray<int> array;
	for (int i = 0; i < 5; ++i)
		array.insert(i);

	array.remove(3);
	ASSERT_EQ(array[3], 4);

	array.remove(0);
	ASSERT_EQ(array[0], 1);
}

#pragma endregion DefaultType

#pragma region std::string
TEST(DAConstructorTestString, TestDefaultConstructor)
{
	DynamicArray<std::string> array;
	ASSERT_EQ(array.size(), 0);
	ASSERT_EQ(array.getCapcaity(), 8);
}

TEST(DAConstructorTestString, TestSizeConstructor)
{
	DynamicArray<std::string> array(10);
	ASSERT_EQ(array.size(), 0);
	ASSERT_EQ(array.getCapcaity(), 10);
}

TEST(DAConstructorTestString, TestCopyConstructor)
{
	DynamicArray<std::string> arrayA(10);
	for (int i = 0; i < arrayA.size(); ++i)
		arrayA.insert(std::to_string(i));

	DynamicArray<std::string> arrayB{ arrayA };

	ASSERT_EQ(arrayA.size(), arrayB.size());
	ASSERT_EQ(arrayA.getCapcaity(), arrayB.getCapcaity());

	for (int i = 0; i < arrayA.size(); ++i)
		ASSERT_EQ(arrayA[i], arrayB[i]);
}

TEST(DAConstructorTestString, TestMoveConstructor)
{
	DynamicArray<std::string> arrayA(10);
	for (int i = 0; i < arrayA.size(); ++i)
		arrayA.insert(std::to_string(i));

	int arrayACapacity = arrayA.getCapcaity();
	int arrayASize = arrayA.size();

	DynamicArray<std::string> arrayB = std::move(arrayA);

	ASSERT_EQ(arrayACapacity, arrayB.getCapcaity());
	ASSERT_EQ(arrayASize, arrayB.size());
	for (int i = 0; i < arrayB.size(); ++i)
		ASSERT_EQ(arrayB[i], std::to_string(i));
}

TEST(DAConstructorTestString, ExpansionTest)
{
	DynamicArray<std::string> array;
	ASSERT_EQ(array.getCapcaity(), 8);

	for (int i = 0; i < 9; ++i)
		array.insert(std::to_string(i));
	ASSERT_EQ(array.getCapcaity(), 16);

	for (int i = 0; i < 9; ++i)
		array.insert(std::to_string(i));
	ASSERT_EQ(array.getCapcaity(), 32);
}

TEST(DAConstructorTestString, InsertTest)
{
	DynamicArray<std::string> array;
	for (int i = 0; i < 5; ++i)
		array.insert(std::to_string(i));

	ASSERT_EQ(array.insert(3, "100"), 3);
	ASSERT_EQ(array[3], "100");

	ASSERT_EQ(array.insert(0, "101"), 0);
	ASSERT_EQ(array[0], "101");
}

TEST(DAConstructorTestString, RemoveTest)
{
	DynamicArray<std::string> array;
	for (int i = 0; i < 5; ++i)
		array.insert(std::to_string(i));

	array.remove(3);
	ASSERT_EQ(array[3], "4");

	array.remove(0);
	ASSERT_EQ(array[0], "1");
}

#pragma endregion std::string

#pragma region Iterator
TEST(DAIterator, ForwardIterator)
{
	int count = 0;
	DynamicArray<int> array(10);
	for (int i = 0; i < array.size(); ++i)
		array.insert(i);

	for(auto it = array.iterator(); it.hasNext(); it.next())
	{
		EXPECT_EQ(it.get(), count);
		++count;
	}
}

TEST(DAIterator, ReversedIterator)
{
	int count = 9;
	DynamicArray<int> array(10);
	for (int i = 0; i < array.size(); ++i)
		array.insert(i);

	for (auto it = array.iterator(); it.hasNext(); it.next())
	{
		EXPECT_EQ(it.get(), count);
		--count;
	}
}
#pragma endregion Iterator
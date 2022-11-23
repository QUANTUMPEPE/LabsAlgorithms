// Lab_2.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "DynamicArray.h"

int main()
{
	DynamicArray<int> a;

	for (int i = 0; i < 32; ++i)
		a.insert(i + 1);

	a.insert(22, 10);
	DynamicArray<int> b = a;
	

	for (int i = 0; i < a.size(); ++i)
		a[i] *= 2;

	DynamicArray<int> c;
	c = a;

	for (auto it = c.reverseIterator(); it.hasNext(); it.next())
		std::cout << it.get() << std::endl;
}


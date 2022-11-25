#pragma once
#include <utility>
#include <stdexcept>

namespace Sorts
{
	template<typename T, typename Compare>
	void quick(T* first, T* last, Compare comp, bool useInsertion);

	template<typename T, typename Compare>
	void insertions(T* first, T* last, Compare comp);
}

template<typename T, typename Compare>
inline void Sorts::quick(T* first, T* last, Compare comp, bool useInsertion)
{
	if(!first || !last) throw std::exception_ptr("Nullptr in args!");

	T* firstPointer = first;
	T* lastPointer = last;

	while (firstPointer < lastPointer) 
	{	
		if (useInsertion && lastPointer - firstPointer <= 64) 
		{
			insertions(firstPointer, lastPointer, comp);
			return;
		}
		
		T* midPointer = firstPointer + (lastPointer - firstPointer) / 2;

		if (comp(*midPointer, *firstPointer)) std::swap(*midPointer, *firstPointer);
		if (comp(*lastPointer, *firstPointer)) std::swap(*lastPointer, *firstPointer);
		if (comp(*lastPointer, *midPointer)) std::swap(*lastPointer, *midPointer);

		T pivot = *midPointer;
		T* i = firstPointer;
		T* j = lastPointer;

		while (true) 
		{
			while (comp(*i, pivot)) i++;
			while (comp(pivot, *j)) j--;
			if (i >= j) break;
			std::swap(*i, *j);
			i++;
			j--;
		}

		if (lastPointer - j > j - firstPointer) 
		{
			quick(firstPointer, j, comp, useInsertion);
			firstPointer = j + 1;
		}
		else 
		{
			quick(j + 1, lastPointer, comp, useInsertion);
			lastPointer = j;
		}
	}
}

template<typename T, typename Compare>
inline void Sorts::insertions(T* first, T* last, Compare comp)
{	
	if (!first || !last) throw std::exception_ptr("Nullptr in args!");

	if (first == last) return;
	T temp;
	T* j;

	for (auto i = first; i <= last; i++)
	{
		temp = (*i);
		j = i;

		while (j > first && comp(temp, *(j - 1)))
		{
			*j = std::move(*(j - 1));
			--j;
		}

		*j = std::move(temp);
	}
}
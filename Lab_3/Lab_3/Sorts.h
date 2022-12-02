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
	if(!first || !last) throw std::exception("Nullptr in args!");

	T* firstPtr = first;
	T* lastPtr = last;

	while (firstPtr < lastPtr) 
	{	
		if (useInsertion && lastPtr - firstPtr <= 64) 
		{
			insertions(firstPtr, lastPtr, comp);
			return;
		}
		
		//Finding median
		T* midPtr = firstPtr + (lastPtr - firstPtr) / 2;
		if (comp(*midPtr, *firstPtr)) std::swap(*midPtr, *firstPtr);
		if (comp(*lastPtr, *firstPtr)) std::swap(*lastPtr, *firstPtr);
		if (comp(*lastPtr, *midPtr)) std::swap(*lastPtr, *midPtr);

		//Partition
		const T pivot = *midPtr;
		T* i = firstPtr;
		T* j = lastPtr;

		while (true) 
		{
			while (comp(*i, pivot)) i++;
			while (comp(pivot, *j)) j--;
			if (i >= j) break;
			std::swap(*i, *j);
			i++;
			j--;
		}

		if (lastPtr - j > j - firstPtr) 
		{
			quick(firstPtr, j, comp, useInsertion);
			firstPtr = j + 1;
		}
		else 
		{
			quick(j + 1, lastPtr, comp, useInsertion);
			lastPtr = j;
		}
	}
}

template<typename T, typename Compare>
inline void Sorts::insertions(T* first, T* last, Compare comp)
{	
	if (!first || !last) throw std::exception("Nullptr in args!");

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
#pragma once
#include <iostream>

template <typename T>
class DynamicArray final
{
public:
	class Iterator
	{
	public:
		Iterator(DynamicArray<T>* array, T* head, bool isReverced);

		const T& get() const;
		void set(const T& value);
		void next();
		bool hasNext() const;

		T& operator++ (int) { return *current_++; }
		T& operator-- (int) { return *current_--; }
		T& operator++ () { return *++current_; }
		T& operator-- () { return *--current_; }

		T& operator* () { return *current_; }

	private:
		T* head_;
		T* current_;
		DynamicArray<T>* array_;
		bool isReverced_;
	};

public:
	DynamicArray();
	DynamicArray(size_t capacity);
	DynamicArray(DynamicArray& other);
	DynamicArray(DynamicArray&& other) noexcept;
	~DynamicArray();
	
	int insert(const T& value);
	int insert(int index, const T& value);
	void remove(int index);
	int size() const;
	int getCapcaity() const { return capacity_; }
	Iterator iterator() { return Iterator(this, data_, false); }
	Iterator reverseIterator() { return Iterator(this, data_ + size_ - 1, true); }

	const T& operator[](int index) const;
	T& operator[](int index);
	DynamicArray<T>& operator = (DynamicArray& other);
	DynamicArray<T>& operator = (DynamicArray&& other) noexcept;

private:
	const size_t kDefaultSize = 8;
	const int kExpandFactor = 2;

	T* data_;
	size_t size_;
	size_t capacity_;

	void expand();
};

#pragma region Constructors
template <typename T>
inline DynamicArray<T>::DynamicArray()
{
	capacity_ = kDefaultSize;
	size_ = 0;
	data_ = (T*)malloc(capacity_ * sizeof(T));
}

template <typename T>
inline DynamicArray<T>::DynamicArray(size_t capacity)
{
	capacity_ = capacity;
	size_ = 0;
	data_ = (T*)malloc(capacity_ * sizeof(T));
}

template<typename T>
inline DynamicArray<T>::DynamicArray(DynamicArray& other)
{
	size_ = other.size_;
	capacity_ = other.capacity_;
	data_ = (T*)malloc(size_ * sizeof(T));
	for(size_t i = 0; i < size_; ++i)
		new (data_ + i) T(other.data_[i]);
}

// Should other be usable after move?
template<typename T>
inline DynamicArray<T>::DynamicArray(DynamicArray&& other) noexcept
{
	if(this == &other) return;

	size_ = other.size_;
	capacity_ = other.capacity_;
	data_ = other.data_;
	other.data_ = nullptr;
	other.size_ = 0;
	other.capacity_ = 0;
}

template <typename T>
DynamicArray<T>::~DynamicArray()
{
	for(size_t i = 0; i < size_; i++) { data_[i].~T(); }
	free(data_);
}
#pragma endregion Constructors
#pragma region PublicFunctions
template<typename T>
inline int DynamicArray<T>::insert(const T& value)
{
	if (size_ == capacity_) expand();

	new (data_ + size_) T(value);
	++size_;

	return size_ - 1;
}

template<typename T>
inline int DynamicArray<T>::insert(int index, const T& value)
{
	if (index < 0 || index >= size_) return -1;
	if (size_ == capacity_) expand();

	++size_;
	for (size_t i = size_ - 1; i > index; --i)
	{
		new (data_ + i) T(std::move(data_[i-1]));
		data_[i-1].~T();
	}

	new(data_ + index) T(value);
	return index;
}

template<typename T>
inline void DynamicArray<T>::remove(int index)
{
	if (index < 0 || index >= size_) throw "Index out of bounds\n";

	data_[index].~T();

	for (size_t i = index; i < size_ - 1; ++i) 
	{
		data_[i].~T();
		new (data_ + i) T(std::move(data_[i + 1]));
	}

	--size_;
}

template<typename T>
inline int DynamicArray<T>::size() const
{
	return size_;
}
#pragma endregion PublicFunctions
#pragma region PrivateFunctions
template<typename T>
inline void DynamicArray<T>::expand()
{
	capacity_ *= kExpandFactor;
	T* newData = (T*)malloc(capacity_ * sizeof(T));

	for(size_t i = 0; i < size_; ++i) 
		new (newData + i) T(std::move(data_[i]));

	for(size_t i = 0; i < size_; ++i) 
		data_[i].~T();

	free(data_);
	data_ = newData;
	newData = nullptr;
}
#pragma endregion PrivateFunctions
#pragma region Operators
template <typename T>
inline T& DynamicArray<T>::operator[](int index)
{
	if(index < 0 || index >= size_) throw std::out_of_range("Index out of bounds");
	return *(data_ + index);
}

template <typename T>
inline const T& DynamicArray<T>::operator[](int index) const
{
	if (index < 0 || index >= size_) throw std::out_of_range("Index out of bounds");
	return *(data_ + index);
}

template<typename T>
inline DynamicArray<T>& DynamicArray<T>::operator=(DynamicArray& other)
{
	for (size_t i = 0; i < size_; ++i) data_[i].~T();
	free(data_);

	size_ = other.size_;
	capacity_ = other.capacity_;
	data_ = (T*)malloc(size_ * sizeof(T));
	for (size_t i = 0; i < size_; ++i)
		new (data_ + i) T(other.data_[i]);

	return *this;
}

template<typename T>
inline DynamicArray<T>& DynamicArray<T>::operator=(DynamicArray&& other) noexcept
{
	if(this == &other) return *this;

	for (size_t i = 0; i < size_; ++i) data_[i].~T();
	free(data_);

	size_ = other.size_;
	capacity_ = other.capacity_;
	data_ = other.data_;
	other.data_ = nullptr;
	other.size_ = 0;
	other.capacity_ = 0;

	return *this;
}
#pragma endregion Operators

#pragma region Iterator

#pragma region ItConstructors
template<typename T>
inline DynamicArray<T>::Iterator::Iterator(DynamicArray<T>* array, T* head, bool isReverced) :
	array_(array), head_(head), isReverced_(isReverced), current_(head)
{
}
#pragma endregion ItConstructors
#pragma region ItPublicFunctions
template<typename T>
inline const T& DynamicArray<T>::Iterator::get() const
{
	return *current_;
}

template<typename T>
inline void DynamicArray<T>::Iterator::set(const T& value)
{
	*current_ = value;
}

template<typename T>
inline void DynamicArray<T>::Iterator::next()
{
	if(isReverced_) current_ -= 1;
	else			current_ += 1;
}

template<typename T>
inline bool DynamicArray<T>::Iterator::hasNext() const
{
	if(isReverced_)
	{
		if(current_ == head_ - array_->size())	return false;
		else									return true;
	}
	else
	{
		if(current_ == array_->size() + head_)	return false;
		else									return true;
	}
}
#pragma endregion ItPublicFunctions
#pragma region ItOperators

#pragma endregion ItOperators

#pragma endregion Iterator



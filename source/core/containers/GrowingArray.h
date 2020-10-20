#pragma once
#include "core/Types.h"
#include "logger/debug.h"
#include <initializer_list>

namespace Core
{
	template <typename T>
	class GrowingArray
	{
	public:
		GrowingArray();
		~GrowingArray();
		GrowingArray(int32 size)
			: m_Capacity(size)
			, m_Data(new T[m_Capacity])
		{
		}

		GrowingArray(std::initializer_list<T> initList)
		{
			m_Capacity = initList.size();
			m_Size = m_Capacity;
			m_Data = new T[m_Capacity];
			memcpy(m_Data, initList.begin(), m_Capacity * sizeof(T));
		}

		GrowingArray(const GrowingArray<T>& other) { *this = other; }

		GrowingArray<T>& operator=(const GrowingArray<T>& other)
		{
			m_Size = other.m_Size;
			m_Capacity = other.m_Capacity;
			m_Data = new T[m_Capacity];
			memcpy(&m_Data[0], &other.m_Data[0], sizeof(T) * m_Size);
			return *this;
		}

		T& operator[](uint32 index) { return m_Data[index]; }
		const T& operator[](uint32 index) const { return m_Data[index]; }

		void ReInit(int32 size)
		{
			delete[] m_Data;
			m_Capacity = size;
			m_Data = new T[m_Capacity];
		}

		/*
			Does change the size of the array, deletes the old array completely.
		*/
		void ReSize(int32 size)
		{
			ReInit(size);
			m_Size = size;
		}

		uint32 Size() const { return m_Size; }
		uint32 Capacity() const { return m_Capacity; }

		void Insert(const T& object, int32 index) { m_Data[index] = object; }

		void Add(const T& object)
		{

			if(m_Size == m_Capacity)
			{
				ASSERT(false, "Array is full. Can't add more to it. Will grow.");
				Grow(m_Capacity * 2);
			}

			if(m_Size < m_Capacity)
				m_Data[m_Size++] = object;
		}

		T& GetLast() { return m_Data[m_Size - 1]; }
		T& GetFirst() { return m_Data[0]; }

		void RemoveCyclicAtIndex(uint32 index) { m_Data[index--] = GetLast(); }

		typedef T* iterator;
		typedef const T* const_iterator;
		iterator begin() { return &m_Data[0]; }
		const_iterator begin() const { return &m_Data[0]; }
		iterator end() { return &m_Data[m_Size]; }
		const_iterator end() const { return &m_Data[m_Size]; }

	private:
		void Grow(int32 size)
		{
			m_Capacity = size;
			T* memory = new T[m_Capacity];
			memcpy(memory, m_Data, sizeof(T) * m_Size);
			delete[] m_Data;
			m_Data = memory;
		}

		uint32 m_Size = 0;
		uint32 m_Capacity = 10;
		T* m_Data = nullptr;
	};

	template <typename T>
	GrowingArray<T>::GrowingArray()
	{
		m_Data = new T[m_Capacity];
	}

	template <typename T>
	GrowingArray<T>::~GrowingArray()
	{
		delete[] m_Data;
		m_Data = nullptr;
		m_Size = 0;
		m_Capacity = 0;
	}

}; // namespace Core

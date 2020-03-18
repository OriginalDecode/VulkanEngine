#pragma once
#include "core/Types.h"
#include "logger/debug.h"

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

		uint32 Size() const { return m_Size; }
		uint32 Capacity() const { return m_Capacity; }

		void Add(const T& object)
		{
			ASSERT(m_Size < m_Capacity, "Array is full. Can't add more to it");

			if(m_Size < m_Capacity)
				m_Data[m_Size++] = object;
		}

		T& GetLast() { return m_Data[m_Size - 1]; }
		T& GetFirst() { return m_Data[0]; }

		void RemoveCyclicAtIndex(uint32 index) { m_Data[index--] = GetLast(); }

	private:
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

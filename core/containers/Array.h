#pragma once

namespace Core
{
	template <typename T>
	class Array
	{
	public:
		Array();
		~Array();

		T& operator[]( int index );
		const T& operator[]( int index ) const;

		T& operator=( const TArray<T>& other );

	private:
		int m_Size{ 0 };
		int m_Capacity{ 20 };
		T* m_Data{ nullptr };
	};

	template <typename T>
	Array<T>::Array()
	{
		m_Data = new T[m_Capacity];
	}

	template <typename T>
	Array<T>::~Array()
	{
		delete m_Data;
		m_Data = nullptr;
	}

}; // namespace Core
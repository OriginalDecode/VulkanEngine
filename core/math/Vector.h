#pragma once

#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"

namespace Core
{

	template <typename T>
	Vector2<T> GetNormalized( const Vector2<T>& v )
	{
		return v / v.Length();
	}

	template <typename T>
	Vector3<T> GetNormalized( const Vector3<T>& v )
	{
		return v / v.Length();
	}

	template <typename T>
	Vector4<T> GetNormalized( const Vector4<T>& v )
	{
		return v / v.Length();
	}

	template <typename T>
	Vector3<T> operator-( const Vector3<T>& l, const Vector4<T>& r )
	{
		return { l.x - r.x, l.y - r.y, l.z - r.z };
	}

	template <typename T>
	Vector4<T> operator-( const Vector4<T>& l, const Vector3<T>& r )
	{
		return { l.x - r.x, l.y - r.y, l.z - r.z, l.w };
	}

	template <typename T>
	Vector4<T>& Vector4<T>::operator=( const Vector3<T>& v )
	{
		x = v.x;
		y = v.y;
		z = v.z;
		w = 0;
		return *this;
	}

	template <typename T>
	Vector3<T>& Vector3<T>::operator=( const Vector4<T>& v )
	{
		x = v.x;
		y = v.y;
		z = v.z;
		return *this;
	}



}; // namespace Core

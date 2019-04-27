#pragma once
#include <cassert>

namespace Core
{
	template<typename T>
	class Vector4
	{
	public:
		Vector4() = default;
		~Vector4() = default;

		Vector4(T x_, T y_, T z_, T w_)
			: x(x_)
			, y(y_)
			, z(z_)
			, w(w_)
		{
		}

		Vector4(T vec[4]) 
			: vector(vec) 
		{
		}
		
		union
		{
			struct
			{
				T x = 0;
				T y = 0;
				T z = 0;
				T w = 0;
			};
			T vector[4];
		};
		template <typename T>
		T Length()
		{
			return sqrtf(Length2());
		};

		template <typename T>
		T Length2()
		{
			return ((x * x) + (y * y) + (z * z) + (w * w));
		};

		Vector4<T>& operator+=(const Vector4<T>& vec)
		{
			x += vec.x;
			y += vec.y;
			z += vec.z;
			w += vec.w;
			return *this;
		}

		Vector4<T>& operator-=(const Vector4<T>& vec)
		{
			x -= vec.x;
			y -= vec.y;
			z -= vec.z;
			w -= vec.w;
			return *this;
		}
		
		Vector4<T>& operator*=(const Vector4<T>& vec)
		{
			x *= vec.x;
			y *= vec.y;
			z *= vec.z;
			w *= vec.w;
			return *this;
		}

		Vector4<T>& operator/=(const Vector4<T>& vec)
		{
			assert(vec.x != 0 && vec.y != 0 && vec.z != 0 && vec.w != 0 && "Can't divide by Zero");
			x /= vec.x;
			y /= vec.y;
			z /= vec.z;
			w /= vec.w;
			return *this;
		}

		Vector4<T>& operator+=(const T scale)
		{
			x += scale;
			y += scale;
			z += scale;
			w += scale;
			return *this;
		}

		Vector4<T>& operator-=(const T scale)
		{
			x -= scale;
			y -= scale;
			z -= scale;
			w -= scale;
			return *this;
		}

		Vector4<T>& operator*=(const T scale)
		{
			x *= scale;
			y *= scale;
			z *= scale;
			w *= scale;
			return *this;
		}

		Vector4<T>& operator/=(const T scale)
		{
			assert(scale != 0 && "Can't divide by Zero");
			x /= scale;
			y /= scale;
			z /= scale;
			w /= scale;
			return *this;
		}
	};

	template<typename T>
	Vector4<T> operator+(const Vector4<T>& first, const Vector4<T>& second)
	{
		return{ (first.x + second.x), (first.y + second.y), (first.z + second.z), (first.w + second.w) };
	}

	template<typename T>
	Vector4<T> operator-(const Vector4<T>& first, const Vector4<T>& second)
	{
		return{ (first.x - second.x), (first.y - second.y), (first.z - second.z), (first.w - second.w) };
	}

	template<typename T>
	Vector4<T> operator*(const Vector4<T>& first, const Vector4<T>& second)
	{
		return{ (first.x * second.x), (first.y * second.y), (first.z * second.z), (first.w * second.w) };
	}

	template<typename T>
	Vector4<T> operator/(const Vector4<T>& first, const Vector4<T>& second)
	{
		assert(second.x >= 0 && second.y >= 0 && second.z >= 0 && second.w >= 0 && "Can't divide by Zero");
		return{ (first.x / second.x), (first.y / second.y), (first.z / second.z), (first.w / second.w) };
	}

	template<typename T>
	Vector4<T> operator+(const Vector4<T>& vector, const T scale)
	{
		return{ vector.x + scale, vector.y + scale, vector.z + scale, vector.w + scale };
	}

	template<typename T>
	Vector4<T> operator-(const Vector4<T>& vector, const T scale)
	{
		return{ vector.x - scale, vector.y - scale, vector.z - scale, vector.w - scale };
	}

	template<typename T>
	Vector4<T> operator*(const Vector4<T>& vector, const T scale)
	{
		return{ vector.x * scale, vector.y * scale, vector.z * scale, vector.w * scale };
	}

	template<typename T>
	Vector4<T> operator/(const Vector4<T>& vector, const T scale)
	{
		assert(scale != 0 && "Can't divide by Zero");
		return{ vector.x / scale, vector.y / scale, vector.z / scale, vector.w / scale };
	}

	template <typename T>
	void Normalize(Vector4<T>& vec)
	{
		vec /= Length(vec);
	}

	template <typename T>
	const Vector4<T> GetNormalized(const Vector4<T>& vec)
	{
		return vec / Length(vec);
	}

	template <typename T>
	T Dot(const Vector4<T>& first, const Vector4<T>& second)
	{
		return ((first.x*second.x) + (first.y*second.y) + (first.z*second.z) + (first.w*second.w));
	}

	template<typename T>
	const Vector4<T> Cross(const Vector4<T>& first, const Vector4<T>& second)
	{
		return{ (first.y * second.z) - (first.z * second.y), (first.z * second.x) - (first.x * second.z), (first.x * second.y) - (first.y * second.x), 1 };
	}

	using Vector4f = Vector4<float>;
};
#pragma once
namespace Core
{
	template<typename T>
	class Vector2
	{
	public:
		Vector2() = default;
		~Vector2() = default;

		Vector2(T x_, T y_) 
			: x(x_)
			, y(y_)
		{
		}

		T x = 0;
		T y = 0;

		const T Length()
		{
			return sqrtf(Length2());
		};

		const T Length2()
		{
			return ((x * x) + (y * y));
		};

		Vector2<T>& operator+=(const Vector2<T>& vector)
		{
			x += vector.x;
			y += vector.y;
			return *this;
		}

		Vector2<T>& operator-=(const Vector2<T>& vector)
		{
			x -= vector.x;
			y -= vector.y;
			return *this;
		}

		Vector2<T>& operator*=(const Vector2<T>& vector)
		{
			x *= vector.x;
			y *= vector.y;
			return *this;
		}

		Vector2<T>& operator/=(const Vector2<T>& vector)
		{
			assert(vector.x >= 0 && vector.y >= 0 && "Can't divide by Zero");
			x /= vector.x;
			y /= vector.y;
			return *this;
		}

		Vector2<T>& operator+=(const T scale)
		{
			x += scale;
			y += scale;
			return *this;
		}

		Vector2<T>& operator-=(const T scale)
		{
			x -= scale;
			y -= scale;
			return *this;
		}

		Vector2<T>& operator*=(const T scale)
		{
			x *= scale;
			y *= scale;
			return *this;
		}

		Vector2<T>& operator/=(const T aScale)
		{
			assert(aScale >= 0 && "Can't divide by Zero");
			x /= aScale;
			y /= aScale;
			return *this;
		}
	};

	template<typename T>
	Vector2<T> operator+(const Vector2<T>& first, const Vector2<T>& second)
	{
		return{ first.x + second.x, first.y + second.y };
	}

	template<typename T>
	Vector2<T> operator-(const Vector2<T>& first, const Vector2<T>& second)
	{
		return{ first.x - second.x, first.y - second.y };
	}

	template<typename T>
	Vector2<T> operator*(const Vector2<T>& first, const Vector2<T>& second)
	{
		return{ first.x * second.x, first.y * second.y };
	}

	template<typename T>
	Vector2<T> operator/(const Vector2<T>& first, const Vector2<T>& second)
	{
		assert(second.x >= 0 && second.y >= 0 && "Can't divide by Zero");
		return{ first.x / second.x, first.y / second.y };
	}

	template<typename T>
	Vector2<T> operator+(const Vector2<T>& first, const T scale)
	{
		return{ first.x + scale, first.y + scale };
	}

	template<typename T>
	Vector2<T> operator-(const Vector2<T>& first, const T scale)
	{
		return{ first.x - scale, first.y - scale };
	}

	template<typename T>
	Vector2<T> operator*(const Vector2<T>& first, const T scale)
	{

		return{ first.x *scale, first.y *scale };
	}

	template<typename T>
	Vector2<T> operator/(const Vector2<T>& first, const T scale)
	{
		assert(scale >= 0 && "Can't divide by Zero");
		return{ first.x / scale, first.y / scale };
	}


	template <typename T>
	void Normalize(Vector2<T>& vector)
	{
		vector /= Length(vector);
	}

	template <typename T>
	Vector2<T> GetNormalized(const Vector2<T>& vector)
	{
		return vector / Length(vector);
	}

	template <typename T>
	T Dot(const Vector2<T>& first, const Vector2<T>& second)
	{
		return (first.x * second.x) + (first.y * second.y);
	}

	using Vector2f = Vector2<float>;
};
#pragma once
#include "Vector3.h"
#include "Vector4.h"

#include <cassert>
#include <xmmintrin.h>
#include <math.h>
#include <utility>

namespace Core
{
	template<typename T>
	class Matrix44
	{
	public:
		Matrix44() {}
		~Matrix44() = default;

		Matrix44(const Matrix44<T>& matrix);
		Matrix44(T mat[16])
			: m_Matrix(mat)
		{
		}

		Matrix44<T>& operator=(const Matrix44<T>& matrix);

		const T& operator[](const int index) const { return m_Matrix[index]; }
		T& operator[](const int index) { return m_Matrix[index]; }

		const T GetXRotation() const;
		const T GetYRotation() const;
		const T GetZRotation() const;

		static Matrix44<T> CreateRotateAroundX(const float rad);
		static Matrix44<T> CreateRotateAroundY(const float rad);
		static Matrix44<T> CreateRotateAroundZ(const float rad);

		static Matrix44<T> CreateProjectionMatrixLH(T nearPlane, T farPlane, T aspectRatio, T fovAngle);
		static Matrix44<T> CreateOrthogonalMatrixLH(T width, T height, T nearPlane, T farPlane);
		static Matrix44<T> CreateOrthographicMatrixLH(T width, T height, T nearPlane, T farPlane);

		static Matrix44<T> CreateScaleMatrix(const Vector4<T>& scale);
		static Matrix44<T> CreateScaleMatrix(const float x, const float y, const float z, const float w);
		static Matrix44<T> Identity();


		void SetOrthographicProjection(float width, float height, float near_plane, float far_plane);
		void SetPerspectiveFOV(float fov, float aspect_ratio);
		
		static Matrix44<T> Transpose(const Matrix44<T>& mat);

		Vector4<T> GetRadRotations();

		void RotateAroundPointX(const Vector4<T>& point, float rad);
		void RotateAroundPointY(const Vector4<T>& point, float rad);
		void RotateAroundPointZ(const Vector4<T>& point, float rad);

		void SetRotation3dX(const float aRadian);
		void SetRotation3dY(const float aRadian);
		void SetRotation3dZ(const float aRadian);

		void SetTranslation(const T x, const T y, const T z, const T w);
		void SetTranslation(const Vector4<T>& aVector);

		const Vector4<T>& GetTranslation() const;

		void SetForward(const Vector4<T>& aVector);
		void SetRight(const Vector4<T>& aVector);
		void SetUp(const Vector4<T>& aVector);
		void SetPosition(const Vector4<T>& aVector);

		const Vector4<T> GetForward() const;
		const Vector4<T> GetRight() const;
		const Vector4<T> GetUp() const;
		const Vector4<T> GetScale() const;

		void LookAt(const Vector3<T>& eye, const Vector3<T>& target, const Vector3<T>& up);

		Vector4<T> GetColumn(int index) const;
		static Matrix44<T> Inverse(const Matrix44<T>& aMatrix);

		Matrix44<T>& operator+=(const Matrix44<T>& matrix);
		Matrix44<T>& operator-=(const Matrix44<T>& matrix);
		Matrix44<T>& operator*=(const Matrix44<T>& matrix);

		union
		{
			__declspec(align(16))T m_Matrix[16];
			T mat[4][4];
			Vector4<T> rows[4];
			struct {
				T m00, m01, m02, m03;
				T m10, m11, m12, m13;
				T m20, m21, m22, m23;
				T m30, m31, m32, m33;
			};
		};
	};
	using Matrix44f = Matrix44<float>;


	template<typename T>
	Matrix44<T>& Matrix44<T>::operator+=(const Matrix44<T>& matrix)
	{
		for (int i = 0; i < 16; ++i)
			m_Matrix[i] += matrix.m_Matrix[i];
		return *this;
	}

	template<typename T>
	Matrix44<T>& Matrix44<T>::operator-=(const Matrix44<T>& matrix)
	{
		for (int i = 0; i < 16; ++i)
			m_Matrix[i] -= matrix.m_Matrix[i];

		return *this;
	}

	template<typename T>
	Matrix44<T>& Matrix44<T>::operator*=(const Matrix44<T>& other)
	{
		const __m128 r0 = _mm_load_ps(&m_Matrix[0]);
		const __m128 r1 = _mm_load_ps(&m_Matrix[4]);
		const __m128 r2 = _mm_load_ps(&m_Matrix[8]);
		const __m128 r3 = _mm_load_ps(&m_Matrix[12]);

		for (size_t i = 0; i < 4; i++)
		{
			__m128 c0 = _mm_set1_ps(other.m_Matrix[0 + (i * 4)]);
			__m128 c1 = _mm_set1_ps(other.m_Matrix[1 + (i * 4)]);
			__m128 c2 = _mm_set1_ps(other.m_Matrix[2 + (i * 4)]);
			__m128 c3 = _mm_set1_ps(other.m_Matrix[3 + (i * 4)]);

			__m128 row = _mm_add_ps(
				_mm_add_ps(
					_mm_mul_ps(r0, c0),
					_mm_mul_ps(r1, c1)),
				_mm_add_ps(
					_mm_mul_ps(r2, c2),
					_mm_mul_ps(r3, c3)));

			_mm_store_ps(&m_Matrix[4 * i], row);
		}

		return *this;
	}

	template<typename T>
	Matrix44<T>& Matrix44<T>::operator=(const Matrix44<T>& aMatrix)
	{
		for (unsigned short i = 0; i < 16; ++i)
			m_Matrix[i] = aMatrix.m_Matrix[i];

		return *this;
	}


	template<typename T>
	void Matrix44<T>::SetOrthographicProjection(float width, float height, float near_plane, float far_plane)
	{
		m_Matrix[0] = 2.f / width;
		m_Matrix[5] = 2.f / height;
		m_Matrix[10] = 1.f / (far_plane - near_plane);
		m_Matrix[14] = near_plane / (near_plane - far_plane);
		m_Matrix[15] = 1.f;
	}

	template<typename T>
	void Matrix44<T>::LookAt(const Vector3<T>& eye, const Vector3<T>& target, const Vector3<T>& up)
	{
		Vector3<T> z = GetNormalized(eye - target);
		Vector3<T> x = GetNormalized(Cross(up, z));
		Vector3<T> y = Cross(z, x);

		m00 = x.x;			m01 = y.x;			m02 = z.x;			m03 = 0.f;//-Dot(x, eye);
		m10 = x.y;			m11 = y.y;			m12 = z.y;			m13 = 0.f;//-Dot(y, eye);
		m20 = x.z;			m21 = y.z;			m22 = z.z;			m23 = 0.f;//-Dot(z, eye);
		m30 = 0;			m31 = 0;			m32 = 0;			m33 = 1.f;

		*this = Transpose(*this);

	}

	template<typename T>
	Vector4<T> Matrix44<T>::GetColumn(int index) const
	{
		assert(index < 4 && index >= 0 && "invalid case!");
		switch (index)
		{
		case 0:
			return{ m_Matrix[0], m_Matrix[4], m_Matrix[8], m_Matrix[12] };
		case 1:
			return{ m_Matrix[1], m_Matrix[5], m_Matrix[9], m_Matrix[13] };
		case 2:
			return{ m_Matrix[2], m_Matrix[6], m_Matrix[10], m_Matrix[14] };
		case 3:
			return{ m_Matrix[3], m_Matrix[7], m_Matrix[11], m_Matrix[15] };
		}
		return Vector4<T>();
	}


	template<typename T>
	Matrix44<T> Matrix44<T>::Identity()
	{
		Matrix44<T> matrix;

		for (int i = 0; i < 16; i++)
			matrix.m_Matrix[i] = 0;

		matrix.m_Matrix[0] = 1;
		matrix.m_Matrix[5] = 1;
		matrix.m_Matrix[10] = 1;
		matrix.m_Matrix[15] = 1;

		return matrix;
	}

	template<typename T>
	Matrix44<T> Matrix44<T>::Transpose(const Matrix44<T>& mat)
	{
		Matrix44<T> result(mat);
		std::swap(result.m_Matrix[1], result.m_Matrix[4]);
		std::swap(result.m_Matrix[2], result.m_Matrix[8]);
		std::swap(result.m_Matrix[3], result.m_Matrix[12]);
		std::swap(result.m_Matrix[6], result.m_Matrix[9]);
		std::swap(result.m_Matrix[7], result.m_Matrix[13]);
		std::swap(result.m_Matrix[11], result.m_Matrix[14]);
		return result;
	}
	
	template<typename T>
	Vector4<T> Matrix44<T>::GetRadRotations()
	{
		return{ -atan2(m_Matrix[9], m_Matrix[10]),
				atan2(m_Matrix[8], m_Matrix[0]),
				-atan2(m_Matrix[4], m_Matrix[0]), 1.f };
	}

	template<typename T>
	Matrix44<T> Matrix44<T>::CreateOrthographicMatrixLH(T width, T height, T near_plane, T far_plane)
	{
		Matrix44<T> new_matrix;
		new_matrix.SetOrthographicProjection(width, height, near_plane, far_plane);
		return new_matrix;
	}
		
	template<typename T>
	Matrix44<T>::Matrix44(const Matrix44<T>& aMatrix)
	{
		*this = aMatrix;
	}

	template<typename T>
	const T Matrix44<T>::GetXRotation() const
	{
		return atan2f(m_Matrix[6], m_Matrix[10]);
	}

	template<typename T>
	const T Matrix44<T>::GetYRotation() const
	{
		return atan2f(-m_Matrix[8], sqrtf((2.f / m_Matrix[5]) + (2.f / m_Matrix[6] + 0.00001f))) + 0.00001f;
	}

	template<typename T>
	const T Matrix44<T>::GetZRotation() const
	{
		return atan2f(m_Matrix[1], m_Matrix[0]);
	}

	template<typename T>
	Matrix44<T> Matrix44<T>::CreateRotateAroundX(const float rad)
	{
		const T cosValue = cosf(rad);
		const T sinValue = sinf(rad);

		Matrix44<T> matrix = Matrix44<T>::Identity();

		matrix.m_Matrix[5] = cosValue;
		matrix.m_Matrix[6] = sinValue;

		matrix.m_Matrix[9] = -sinValue;
		matrix.m_Matrix[10] = cosValue;

		return matrix;
	}

	template<typename T>
	Matrix44<T> Matrix44<T>::CreateRotateAroundY(const float rad)
	{
		const T cosValue = cosf(rad);
		const T sinValue = sinf(rad);

		Matrix44<T> matrix = Matrix44<T>::Identity();

		matrix.m_Matrix[0] = cosValue;
		matrix.m_Matrix[2] = -sinValue;

		matrix.m_Matrix[8] = sinValue;
		matrix.m_Matrix[10] = cosValue;

		return matrix;
	}

	template<typename T>
	Matrix44<T> Matrix44<T>::CreateRotateAroundZ(const float rad)
	{
		const T cosValue = cosf(rad);
		const T sinValue = sinf(rad);

		Matrix44<T> matrix = Matrix44<T>::Identity();

		matrix.m_Matrix[0] = cosValue;
		matrix.m_Matrix[1] = sinValue;

		matrix.m_Matrix[4] = -sinValue;
		matrix.m_Matrix[5] = cosValue;

		return matrix;
	}

	template<typename T>
	Matrix44<T> Matrix44<T>::CreateScaleMatrix(const Vector4<T>& scale = Vector4<T>(1, 1, 1, 1))
	{
		Matrix44 to_return;
		to_return.rows[0] = { scale.x, 0, 0, 0 };
		to_return.rows[1] = { 0, scale.y, 0, 0 };
		to_return.rows[2] = { 0, 0, scale.z, 0 };
		to_return.rows[3] = { 0, 0, 0, scale.w };
		return to_return;
	}

	template<typename T>
	void Matrix44<T>::RotateAroundPointX(const Vector4<T>& point, float rad)
	{
		SetPosition(rows[3] - point);
		*this = *this * Matrix44<T>::CreateRotateAroundX(rad);	
		SetPosition(rows[3] + point);
	}

	template<typename T>
	void Matrix44<T>::RotateAroundPointY(const Vector4<T>& point, float rad)
	{
		SetPosition(rows[3] - point);
		*this = *this * Matrix44<T>::CreateRotateAroundY(rad);
		SetPosition(rows[3] + point);
	}

	template<typename T>
	void Matrix44<T>::RotateAroundPointZ(const Vector4<T>& point, float rad)
	{
		SetPosition(rows[3] - point);
		*this = *this * Matrix44<T>::CreateRotateAroundZ(rad);
		SetPosition(rows[3] + point);
	}

	template<typename T>
	Matrix44<T> Matrix44<T>::CreateScaleMatrix(const float x = 1.f, const float y = 1.f, const float z = 1.f, const float w = 1.f)
	{
		Matrix44<T> to_return;
		to_return.rows[0] = { x, 0, 0, 0 };
		to_return.rows[1] = { 0, y, 0, 0 };
		to_return.rows[2] = { 0, 0, z, 0 };
		to_return.rows[3] = { 0, 0, 0, w };
		return to_return;
	}

	template<typename T>
	void Matrix44<T>::SetRotation3dX(const float rad)
	{
		Matrix44<T> rotationMatrix = Matrix44<T>::CreateRotateAroundX(rad);
		Matrix44<T> tempMatrix = rotationMatrix * *this;

		m_Matrix[0] = tempMatrix.m_Matrix[0];
		m_Matrix[1] = tempMatrix.m_Matrix[1];
		m_Matrix[2] = tempMatrix.m_Matrix[2];

		m_Matrix[4] = tempMatrix.m_Matrix[4];
		m_Matrix[5] = tempMatrix.m_Matrix[5];
		m_Matrix[6] = tempMatrix.m_Matrix[6];

		m_Matrix[8] = tempMatrix.m_Matrix[8];
		m_Matrix[9] = tempMatrix.m_Matrix[9];
		m_Matrix[10] = tempMatrix.m_Matrix[10];
	}

	template<typename T>
	void Matrix44<T>::SetRotation3dY(const float rad)
	{
		Matrix44<T> rotationMatrix = Matrix44<T>::CreateRotateAroundY(rad);
		Matrix44<T> tempMatrix = rotationMatrix * *this;

		m_Matrix[0] = tempMatrix.m_Matrix[0];
		m_Matrix[1] = tempMatrix.m_Matrix[1];
		m_Matrix[2] = tempMatrix.m_Matrix[2];

		m_Matrix[4] = tempMatrix.m_Matrix[4];
		m_Matrix[5] = tempMatrix.m_Matrix[5];
		m_Matrix[6] = tempMatrix.m_Matrix[6];

		m_Matrix[8] = tempMatrix.m_Matrix[8];
		m_Matrix[9] = tempMatrix.m_Matrix[9];
		m_Matrix[10] = tempMatrix.m_Matrix[10];
	}

	template<typename T>
	void Matrix44<T>::SetRotation3dZ(const float rad)
	{
		Matrix44<T> rotationMatrix = Matrix44<T>::CreateRotateAroundZ(rad);
		Matrix44<T> tempMatrix = rotationMatrix * *this;

		m_Matrix[0] = tempMatrix.m_Matrix[0];
		m_Matrix[1] = tempMatrix.m_Matrix[1];
		m_Matrix[2] = tempMatrix.m_Matrix[2];

		m_Matrix[4] = tempMatrix.m_Matrix[4];
		m_Matrix[5] = tempMatrix.m_Matrix[5];
		m_Matrix[6] = tempMatrix.m_Matrix[6];

		m_Matrix[8] = tempMatrix.m_Matrix[8];
		m_Matrix[9] = tempMatrix.m_Matrix[9];
		m_Matrix[10] = tempMatrix.m_Matrix[10];
	}

	template<typename T>
	void Matrix44<T>::SetTranslation(const T x, const T y, const T z, const T w)
	{
		SetTranslation({ x, y, z, w });
	}

	template<typename T>
	void Matrix44<T>::SetTranslation(const Vector4<T>& vector)
	{
		rows[3] = vector;
	}

	template<typename T>
	const Vector4<T>& Matrix44<T>::GetTranslation() const
	{
		return rows[3];
	}

	template<typename T>
	const Vector4<T> Matrix44<T>::GetRight() const
	{
		return{ m_Matrix[0], m_Matrix[1], m_Matrix[2], m_Matrix[3] };
	}

	template<typename T>
	const Vector4<T> Matrix44<T>::GetUp() const
	{
		return{ m_Matrix[4], m_Matrix[5], m_Matrix[6], m_Matrix[7] };
	}

	template<typename T>
	const Vector4<T> Matrix44<T>::GetForward() const
	{
		return{ m_Matrix[8], m_Matrix[9], m_Matrix[10], m_Matrix[11] };
	}

	template<typename T>
	const Vector4<T> Matrix44<T>::GetScale() const
	{
		return{ rows[0].Length(), rows[1].Length() , rows[2].Length(), 1 };
	}

	template<typename T>
	void Matrix44<T>::SetRight(const Vector4<T>& vector)
	{
		rows[0] = vector;
	}

	template<typename T>
	void Matrix44<T>::SetUp(const Vector4<T>& vector)
	{ 
		rows[1] = vector;
	}

	template<typename T>
	void Matrix44<T>::SetForward(const Vector4<T>& vector)
	{
		rows[2] = vector;
	}

	template<typename T>
	void Matrix44<T>::SetPosition(const Vector4<T>& vector)
	{
		rows[3].x = vector.x;
		rows[3].y = vector.y;
		rows[3].z = vector.z;
	}

	template<typename T>
	Matrix44<T> Inverse(const Matrix44<T>& aMatrix)
	{
		Matrix44<T> inverse(aMatrix);

		Vector4<T> translation = inverse.GetTranslation();
		inverse.SetPosition(Vector4<T>(0, 0, 0, 1.f));
		translation *= -1.f;
		translation.w = 1.f;
		inverse = Transpose(inverse);
		translation = translation * inverse;

		inverse.SetPosition(translation);
		return inverse;
	}

	template<typename T>
	const Matrix44<T> operator+(const Matrix44<T>& aFirstMatrix, const Matrix44<T>& aSecondMatrix)
	{
		Matrix44<T> tempMatrix(aFirstMatrix);
		return tempMatrix += aSecondMatrix;
	}

	template<typename T>
	const Matrix44<T> operator-(const Matrix44<T>& aFirstMatrix, const Matrix44<T>& aSecondMatrix)
	{
		Matrix44<T> tempMatrix(aFirstMatrix);
		return tempMatrix -= aSecondMatrix;
	}

	template<typename T>
	Matrix44<T> operator*(const Matrix44<T>& first, const Matrix44<T>& second)
	{
		Matrix44<T> temp(first);
		return temp *= second;
	}

	template<typename T>
	const Vector4<T> operator*(const Vector4<T>& aVector, const Matrix44<T>& aMatrix)
	{
		return aVector *= aMatrix;
	}

	template<typename T>
	Vector4<T>& operator*=(Vector4<T>& v, const Matrix44<T>& m)
	{
		v.x = Dot(v, m.GetColumn(0));
		v.y = Dot(v, m.GetColumn(1));
		v.z = Dot(v, m.GetColumn(2));
		v.w = Dot(v, m.GetColumn(3));
		return v;
	}

	template<typename T>
	void Matrix44<T>::SetPerspectiveFOV(float fov, float aspect_ratio)
	{
		const float sin_fov = sinf(0.5f * fov);
		const float cos_fov = cosf(0.5f * fov);
		const float width = cos_fov / sin_fov;
		const float height = width / aspect_ratio;

		m_Matrix[0] = width;
		m_Matrix[5] = height;

	}

	template<typename T>
	Matrix44<T> Matrix44<T>::CreateProjectionMatrixLH(T nearPlane, T farPlane, T aspectRatio, T fovAngle)
	{
		Matrix44<T> temp = Matrix44<T>::Identity();

		T sinFov = sinf(0.5f * fovAngle);
		T cosFov = cosf(0.5f * fovAngle);

		T width = cosFov / sinFov;
		T height = width / aspectRatio;

		T scaling = farPlane / (farPlane - nearPlane);

		temp.m_Matrix[0] = width;
		temp.m_Matrix[5] = height;
		temp.m_Matrix[10] = scaling;
		temp.m_Matrix[11] = 1.0f;

		temp.m_Matrix[14] = -scaling * nearPlane;
		temp.m_Matrix[15] = 1.0f;
		return temp;
	}

	template<typename T>
	Matrix44<T> Matrix44<T>::CreateOrthogonalMatrixLH(T width, T height, T nearPlane, T farPlane)
	{
		Matrix44<T> toReturn;
		toReturn.m_Matrix[0] = 2.f / width;
		toReturn.m_Matrix[5] = 2.f / height;
		toReturn.m_Matrix[10] = 1.f / (farPlane - nearPlane);
		toReturn.m_Matrix[14] = nearPlane / (nearPlane - farPlane);
		toReturn.m_Matrix[15] = 1.f;
		return toReturn;
	}

	template<typename T>
	Matrix44<T> Matrix44<T>::Inverse(const Matrix44<T>& matrix)
	{
		Vector4<T> theTranslation;
		theTranslation = matrix.GetTranslation();
		theTranslation.x *= -1;
		theTranslation.y *= -1;
		theTranslation.z *= -1;
		theTranslation.w = 1;

		Matrix44<T> inverse = Transpose(matrix);
		theTranslation *= inverse;
		inverse.SetTranslation(theTranslation.x, theTranslation.y, theTranslation.z, 1);

		return inverse;
	}

	template<class T>
	Matrix44<T> CreateReflectionMatrixAboutAxis44(Vector3<T> reflectionVector)
	{
		Matrix44<T> reflectionMatrix;
		reflectionMatrix.m_Matrix[0] = 1 - 2 * (reflectionVector.x*reflectionVector.x);
		reflectionMatrix.m_Matrix[1] = -2 * (reflectionVector.x*reflectionVector.y);
		reflectionMatrix.m_Matrix[2] = -2 * (reflectionVector.x*reflectionVector.z);

		reflectionMatrix.m_Matrix[4] = -2 * (reflectionVector.y*reflectionVector.x);
		reflectionMatrix.m_Matrix[5] = 1 - 2 * (reflectionVector.y*reflectionVector.y);
		reflectionMatrix.m_Matrix[6] = -2 * (reflectionVector.y*reflectionVector.z);

		reflectionMatrix.m_Matrix[8] = -2 * (reflectionVector.z*reflectionVector.x);
		reflectionMatrix.m_Matrix[9] = -2 * (reflectionVector.z*reflectionVector.y);
		reflectionMatrix.m_Matrix[10] = 1 - 2 * (reflectionVector.z*reflectionVector.z);
		return reflectionMatrix;
	};

	template <typename T>
	Matrix44<T> InverseReal(const Matrix44<T>& aMatrix)
	{
		T inv[16];
		inv[0] = aMatrix.m_Matrix[5] * aMatrix.m_Matrix[10] * aMatrix.m_Matrix[15] -
			aMatrix.m_Matrix[5] * aMatrix.m_Matrix[11] * aMatrix.m_Matrix[14] -
			aMatrix.m_Matrix[9] * aMatrix.m_Matrix[6] * aMatrix.m_Matrix[15] +
			aMatrix.m_Matrix[9] * aMatrix.m_Matrix[7] * aMatrix.m_Matrix[14] +
			aMatrix.m_Matrix[13] * aMatrix.m_Matrix[6] * aMatrix.m_Matrix[11] -
			aMatrix.m_Matrix[13] * aMatrix.m_Matrix[7] * aMatrix.m_Matrix[10];

		inv[4] = -aMatrix.m_Matrix[4] * aMatrix.m_Matrix[10] * aMatrix.m_Matrix[15] +
			aMatrix.m_Matrix[4] * aMatrix.m_Matrix[11] * aMatrix.m_Matrix[14] +
			aMatrix.m_Matrix[8] * aMatrix.m_Matrix[6] * aMatrix.m_Matrix[15] -
			aMatrix.m_Matrix[8] * aMatrix.m_Matrix[7] * aMatrix.m_Matrix[14] -
			aMatrix.m_Matrix[12] * aMatrix.m_Matrix[6] * aMatrix.m_Matrix[11] +
			aMatrix.m_Matrix[12] * aMatrix.m_Matrix[7] * aMatrix.m_Matrix[10];

		inv[8] = aMatrix.m_Matrix[4] * aMatrix.m_Matrix[9] * aMatrix.m_Matrix[15] -
			aMatrix.m_Matrix[4] * aMatrix.m_Matrix[11] * aMatrix.m_Matrix[13] -
			aMatrix.m_Matrix[8] * aMatrix.m_Matrix[5] * aMatrix.m_Matrix[15] +
			aMatrix.m_Matrix[8] * aMatrix.m_Matrix[7] * aMatrix.m_Matrix[13] +
			aMatrix.m_Matrix[12] * aMatrix.m_Matrix[5] * aMatrix.m_Matrix[11] -
			aMatrix.m_Matrix[12] * aMatrix.m_Matrix[7] * aMatrix.m_Matrix[9];

		inv[12] = -aMatrix.m_Matrix[4] * aMatrix.m_Matrix[9] * aMatrix.m_Matrix[14] +
			aMatrix.m_Matrix[4] * aMatrix.m_Matrix[10] * aMatrix.m_Matrix[13] +
			aMatrix.m_Matrix[8] * aMatrix.m_Matrix[5] * aMatrix.m_Matrix[14] -
			aMatrix.m_Matrix[8] * aMatrix.m_Matrix[6] * aMatrix.m_Matrix[13] -
			aMatrix.m_Matrix[12] * aMatrix.m_Matrix[5] * aMatrix.m_Matrix[10] +
			aMatrix.m_Matrix[12] * aMatrix.m_Matrix[6] * aMatrix.m_Matrix[9];

		inv[1] = -aMatrix.m_Matrix[1] * aMatrix.m_Matrix[10] * aMatrix.m_Matrix[15] +
			aMatrix.m_Matrix[1] * aMatrix.m_Matrix[11] * aMatrix.m_Matrix[14] +
			aMatrix.m_Matrix[9] * aMatrix.m_Matrix[2] * aMatrix.m_Matrix[15] -
			aMatrix.m_Matrix[9] * aMatrix.m_Matrix[3] * aMatrix.m_Matrix[14] -
			aMatrix.m_Matrix[13] * aMatrix.m_Matrix[2] * aMatrix.m_Matrix[11] +
			aMatrix.m_Matrix[13] * aMatrix.m_Matrix[3] * aMatrix.m_Matrix[10];

		inv[5] = aMatrix.m_Matrix[0] * aMatrix.m_Matrix[10] * aMatrix.m_Matrix[15] -
			aMatrix.m_Matrix[0] * aMatrix.m_Matrix[11] * aMatrix.m_Matrix[14] -
			aMatrix.m_Matrix[8] * aMatrix.m_Matrix[2] * aMatrix.m_Matrix[15] +
			aMatrix.m_Matrix[8] * aMatrix.m_Matrix[3] * aMatrix.m_Matrix[14] +
			aMatrix.m_Matrix[12] * aMatrix.m_Matrix[2] * aMatrix.m_Matrix[11] -
			aMatrix.m_Matrix[12] * aMatrix.m_Matrix[3] * aMatrix.m_Matrix[10];

		inv[9] = -aMatrix.m_Matrix[0] * aMatrix.m_Matrix[9] * aMatrix.m_Matrix[15] +
			aMatrix.m_Matrix[0] * aMatrix.m_Matrix[11] * aMatrix.m_Matrix[13] +
			aMatrix.m_Matrix[8] * aMatrix.m_Matrix[1] * aMatrix.m_Matrix[15] -
			aMatrix.m_Matrix[8] * aMatrix.m_Matrix[3] * aMatrix.m_Matrix[13] -
			aMatrix.m_Matrix[12] * aMatrix.m_Matrix[1] * aMatrix.m_Matrix[11] +
			aMatrix.m_Matrix[12] * aMatrix.m_Matrix[3] * aMatrix.m_Matrix[9];

		inv[13] = aMatrix.m_Matrix[0] * aMatrix.m_Matrix[9] * aMatrix.m_Matrix[14] -
			aMatrix.m_Matrix[0] * aMatrix.m_Matrix[10] * aMatrix.m_Matrix[13] -
			aMatrix.m_Matrix[8] * aMatrix.m_Matrix[1] * aMatrix.m_Matrix[14] +
			aMatrix.m_Matrix[8] * aMatrix.m_Matrix[2] * aMatrix.m_Matrix[13] +
			aMatrix.m_Matrix[12] * aMatrix.m_Matrix[1] * aMatrix.m_Matrix[10] -
			aMatrix.m_Matrix[12] * aMatrix.m_Matrix[2] * aMatrix.m_Matrix[9];

		inv[2] = aMatrix.m_Matrix[1] * aMatrix.m_Matrix[6] * aMatrix.m_Matrix[15] -
			aMatrix.m_Matrix[1] * aMatrix.m_Matrix[7] * aMatrix.m_Matrix[14] -
			aMatrix.m_Matrix[5] * aMatrix.m_Matrix[2] * aMatrix.m_Matrix[15] +
			aMatrix.m_Matrix[5] * aMatrix.m_Matrix[3] * aMatrix.m_Matrix[14] +
			aMatrix.m_Matrix[13] * aMatrix.m_Matrix[2] * aMatrix.m_Matrix[7] -
			aMatrix.m_Matrix[13] * aMatrix.m_Matrix[3] * aMatrix.m_Matrix[6];

		inv[6] = -aMatrix.m_Matrix[0] * aMatrix.m_Matrix[6] * aMatrix.m_Matrix[15] +
			aMatrix.m_Matrix[0] * aMatrix.m_Matrix[7] * aMatrix.m_Matrix[14] +
			aMatrix.m_Matrix[4] * aMatrix.m_Matrix[2] * aMatrix.m_Matrix[15] -
			aMatrix.m_Matrix[4] * aMatrix.m_Matrix[3] * aMatrix.m_Matrix[14] -
			aMatrix.m_Matrix[12] * aMatrix.m_Matrix[2] * aMatrix.m_Matrix[7] +
			aMatrix.m_Matrix[12] * aMatrix.m_Matrix[3] * aMatrix.m_Matrix[6];

		inv[10] = aMatrix.m_Matrix[0] * aMatrix.m_Matrix[5] * aMatrix.m_Matrix[15] -
			aMatrix.m_Matrix[0] * aMatrix.m_Matrix[7] * aMatrix.m_Matrix[13] -
			aMatrix.m_Matrix[4] * aMatrix.m_Matrix[1] * aMatrix.m_Matrix[15] +
			aMatrix.m_Matrix[4] * aMatrix.m_Matrix[3] * aMatrix.m_Matrix[13] +
			aMatrix.m_Matrix[12] * aMatrix.m_Matrix[1] * aMatrix.m_Matrix[7] -
			aMatrix.m_Matrix[12] * aMatrix.m_Matrix[3] * aMatrix.m_Matrix[5];

		inv[14] = -aMatrix.m_Matrix[0] * aMatrix.m_Matrix[5] * aMatrix.m_Matrix[14] +
			aMatrix.m_Matrix[0] * aMatrix.m_Matrix[6] * aMatrix.m_Matrix[13] +
			aMatrix.m_Matrix[4] * aMatrix.m_Matrix[1] * aMatrix.m_Matrix[14] -
			aMatrix.m_Matrix[4] * aMatrix.m_Matrix[2] * aMatrix.m_Matrix[13] -
			aMatrix.m_Matrix[12] * aMatrix.m_Matrix[1] * aMatrix.m_Matrix[6] +
			aMatrix.m_Matrix[12] * aMatrix.m_Matrix[2] * aMatrix.m_Matrix[5];

		inv[3] = -aMatrix.m_Matrix[1] * aMatrix.m_Matrix[6] * aMatrix.m_Matrix[11] +
			aMatrix.m_Matrix[1] * aMatrix.m_Matrix[7] * aMatrix.m_Matrix[10] +
			aMatrix.m_Matrix[5] * aMatrix.m_Matrix[2] * aMatrix.m_Matrix[11] -
			aMatrix.m_Matrix[5] * aMatrix.m_Matrix[3] * aMatrix.m_Matrix[10] -
			aMatrix.m_Matrix[9] * aMatrix.m_Matrix[2] * aMatrix.m_Matrix[7] +
			aMatrix.m_Matrix[9] * aMatrix.m_Matrix[3] * aMatrix.m_Matrix[6];

		inv[7] = aMatrix.m_Matrix[0] * aMatrix.m_Matrix[6] * aMatrix.m_Matrix[11] -
			aMatrix.m_Matrix[0] * aMatrix.m_Matrix[7] * aMatrix.m_Matrix[10] -
			aMatrix.m_Matrix[4] * aMatrix.m_Matrix[2] * aMatrix.m_Matrix[11] +
			aMatrix.m_Matrix[4] * aMatrix.m_Matrix[3] * aMatrix.m_Matrix[10] +
			aMatrix.m_Matrix[8] * aMatrix.m_Matrix[2] * aMatrix.m_Matrix[7] -
			aMatrix.m_Matrix[8] * aMatrix.m_Matrix[3] * aMatrix.m_Matrix[6];

		inv[11] = -aMatrix.m_Matrix[0] * aMatrix.m_Matrix[5] * aMatrix.m_Matrix[11] +
			aMatrix.m_Matrix[0] * aMatrix.m_Matrix[7] * aMatrix.m_Matrix[9] +
			aMatrix.m_Matrix[4] * aMatrix.m_Matrix[1] * aMatrix.m_Matrix[11] -
			aMatrix.m_Matrix[4] * aMatrix.m_Matrix[3] * aMatrix.m_Matrix[9] -
			aMatrix.m_Matrix[8] * aMatrix.m_Matrix[1] * aMatrix.m_Matrix[7] +
			aMatrix.m_Matrix[8] * aMatrix.m_Matrix[3] * aMatrix.m_Matrix[5];

		inv[15] = aMatrix.m_Matrix[0] * aMatrix.m_Matrix[5] * aMatrix.m_Matrix[10] -
			aMatrix.m_Matrix[0] * aMatrix.m_Matrix[6] * aMatrix.m_Matrix[9] -
			aMatrix.m_Matrix[4] * aMatrix.m_Matrix[1] * aMatrix.m_Matrix[10] +
			aMatrix.m_Matrix[4] * aMatrix.m_Matrix[2] * aMatrix.m_Matrix[9] +
			aMatrix.m_Matrix[8] * aMatrix.m_Matrix[1] * aMatrix.m_Matrix[6] -
			aMatrix.m_Matrix[8] * aMatrix.m_Matrix[2] * aMatrix.m_Matrix[5];

		T det = aMatrix.m_Matrix[0] * inv[0] + aMatrix.m_Matrix[1] * inv[4] + aMatrix.m_Matrix[2] * inv[8] + aMatrix.m_Matrix[3] * inv[12];

		det = static_cast<T>(1.0) / det;

		Matrix44<T> returnMatrix;

		for (i = 0; i < 16; i++)
			returnMatrix.m_Matrix[i] = inv[i] * det;

		return returnMatrix;
	}

};

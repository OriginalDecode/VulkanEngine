#pragma once
#include "Matrix44Decl.h"
#include "../Vector/Vector3.h"
#include <xmmintrin.h>
namespace Core
{
	typedef Math::Matrix44<float> Matrix44f;
	namespace Math
	{
#pragma region Constructors
		/****************************\
		|		Constructors	     |
		\****************************/
		template<typename TYPE>
		Matrix44<TYPE>::Matrix44()
		{
			for (unsigned char i = 0; i < 16; i++)
			{
				if (i == 0 || i == 5 || i == 10 || i == 15)
					myMatrix[i] = 1;
				else
					myMatrix[i] = 0;
			}
		}

		template<typename TYPE>
		Matrix44<TYPE>::Matrix44(const Matrix44<TYPE>& aMatrix)
		{
			*this = aMatrix;
		}

#pragma endregion
		template<typename TYPE>
		Matrix44<TYPE>::~Matrix44()
		{
		}

#pragma region Functions
		/****************************\
		|		   Functions	     |
		\****************************/

		template<typename T>
		float Matrix44<T>::GetXRotation() const
		{
			return atan2f(myMatrix[6], myMatrix[10]);
		}

		template<typename T>
		float Matrix44<T>::GetYRotation() const
		{
			return atan2f(-myMatrix[8], sqrtf((2.f / myMatrix[5]) + (2.f / myMatrix[6] + 0.00001f))) + 0.00001f;
		}

		template<typename T>
		float Matrix44<T>::GetZRotation() const
		{
			return atan2f(myMatrix[1], myMatrix[0]);
		}

		template<typename TYPE>
		const Matrix44<TYPE> Matrix44<TYPE>::Calculate(const RotationType& rotation, const TYPE& someCos, const TYPE& someSin)
		{
			Matrix44<TYPE> matrix;

			switch (rotation)
			{
			case RotationType::Axis_X:
			{
				matrix.myMatrix[0] = 1;
				matrix.myMatrix[1] = 0;
				matrix.myMatrix[2] = 0;
				matrix.myMatrix[3] = 0;

				matrix.myMatrix[4] = 0;
				matrix.myMatrix[5] = someCos;
				matrix.myMatrix[6] = someSin;
				matrix.myMatrix[7] = 0;

				matrix.myMatrix[8] = 0;
				matrix.myMatrix[9] = -someSin;
				matrix.myMatrix[10] = someCos;
				matrix.myMatrix[11] = 0;

				matrix.myMatrix[12] = 0;
				matrix.myMatrix[13] = 0;
				matrix.myMatrix[14] = 0;
				matrix.myMatrix[15] = 1;
			} break;

			case RotationType::Axis_Y:
			{
				matrix.myMatrix[0] = someCos;
				matrix.myMatrix[1] = 0;
				matrix.myMatrix[2] = -someSin;
				matrix.myMatrix[3] = 0;

				matrix.myMatrix[4] = 0;
				matrix.myMatrix[5] = 1;
				matrix.myMatrix[6] = 0;
				matrix.myMatrix[7] = 0;

				matrix.myMatrix[8] = someSin;
				matrix.myMatrix[9] = 0;
				matrix.myMatrix[10] = someCos;
				matrix.myMatrix[11] = 0;

				matrix.myMatrix[12] = 0;
				matrix.myMatrix[13] = 0;
				matrix.myMatrix[14] = 0;
				matrix.myMatrix[15] = 1;

			} break;

			case RotationType::Axis_Z:
			{
				matrix.myMatrix[0] = someCos;
				matrix.myMatrix[1] = someSin;
				matrix.myMatrix[2] = 0;
				matrix.myMatrix[3] = 0;

				matrix.myMatrix[4] = -someSin;
				matrix.myMatrix[5] = someCos;
				matrix.myMatrix[6] = 0;
				matrix.myMatrix[7] = 0;

				matrix.myMatrix[8] = 0;
				matrix.myMatrix[9] = 0;
				matrix.myMatrix[10] = 1;
				matrix.myMatrix[11] = 0;

				matrix.myMatrix[12] = 0;
				matrix.myMatrix[13] = 0;
				matrix.myMatrix[14] = 0;
				matrix.myMatrix[15] = 1;
			} break;

			}

			return matrix;

		}

		template<typename TYPE>
		Matrix44<TYPE> Matrix44<TYPE>::CreateRotateAroundX(const TYPE& aRadian)
		{
			Matrix44<TYPE> matrix;

			TYPE _cos = cos(aRadian);
			TYPE _sin = sin(aRadian);

			return matrix.Calculate(RotationType::Axis_X, _cos, _sin);
		}

		template<typename TYPE>
		Matrix44<TYPE> Matrix44<TYPE>::CreateRotateAroundY(const TYPE& aRadian)
		{
			Matrix44<TYPE> matrix;

			TYPE _cos = cos(aRadian);
			TYPE _sin = sin(aRadian);

			return matrix.Calculate(RotationType::Axis_Y, _cos, _sin);
		}

		template<typename TYPE>
		Matrix44<TYPE> Matrix44<TYPE>::CreateRotateAroundZ(const TYPE& aRadian)
		{
			Matrix44<TYPE> matrix;

			TYPE _cos = cos(aRadian);
			TYPE _sin = sin(aRadian);

			return matrix.Calculate(RotationType::Axis_Z, _cos, _sin);
		}

		template<typename TYPE>
		Matrix44<TYPE> Matrix44<TYPE>::CreateScaleMatrix(const Vector4<TYPE>& scale = Vector4<TYPE>(1, 1, 1, 1))
		{
			Matrix44 to_return;
			to_return.rows[0] = Vector4<TYPE>(scale.x, 0, 0, 0);
			to_return.rows[1] = Vector4<TYPE>(0, scale.y, 0, 0);
			to_return.rows[2] = Vector4<TYPE>(0, 0, scale.z, 0);
			to_return.rows[3] = Vector4<TYPE>(0, 0, 0, scale.w);
			return to_return;
		}


		template<typename TYPE>
		Core::Math::Matrix44<TYPE> Core::Math::Matrix44<TYPE>::CreateScaleMatrix(const float x = 1.f, const float y = 1.f, const float z = 1.f, const float w = 1.f)
		{
			Matrix44<TYPE> to_return;
			to_return.rows[0] = Vector4<TYPE>(x, 0, 0, 0);
			to_return.rows[1] = Vector4<TYPE>(0, y, 0, 0);
			to_return.rows[2] = Vector4<TYPE>(0, 0, z, 0);
			to_return.rows[3] = Vector4<TYPE>(0, 0, 0, w);
			return to_return;
		}

		template<typename TYPE>
		void Matrix44<TYPE>::SetRotation3dX(const TYPE& aRadian)
		{
			Matrix44<TYPE> rotationMatrix = Matrix44::CreateRotateAroundX(aRadian);
			Matrix44<TYPE> tempMatrix = *this;
			tempMatrix = rotationMatrix * tempMatrix;

			myMatrix[0] = tempMatrix.myMatrix[0];
			myMatrix[1] = tempMatrix.myMatrix[1];
			myMatrix[2] = tempMatrix.myMatrix[2];

			myMatrix[4] = tempMatrix.myMatrix[4];
			myMatrix[5] = tempMatrix.myMatrix[5];
			myMatrix[6] = tempMatrix.myMatrix[6];

			myMatrix[8] = tempMatrix.myMatrix[8];
			myMatrix[9] = tempMatrix.myMatrix[9];
			myMatrix[10] = tempMatrix.myMatrix[10];
		}

		template<typename TYPE>
		void Matrix44<TYPE>::SetRotation3dY(const TYPE& aRadian)
		{
			Matrix44<TYPE> rotationMatrix = Matrix44::CreateRotateAroundY(aRadian);
			Matrix44<TYPE> tempMatrix = *this;
			tempMatrix = rotationMatrix * tempMatrix;

			myMatrix[0] = tempMatrix.myMatrix[0];
			myMatrix[1] = tempMatrix.myMatrix[1];
			myMatrix[2] = tempMatrix.myMatrix[2];

			myMatrix[4] = tempMatrix.myMatrix[4];
			myMatrix[5] = tempMatrix.myMatrix[5];
			myMatrix[6] = tempMatrix.myMatrix[6];

			myMatrix[8] = tempMatrix.myMatrix[8];
			myMatrix[9] = tempMatrix.myMatrix[9];
			myMatrix[10] = tempMatrix.myMatrix[10];
		}

		template<typename TYPE>
		void Matrix44<TYPE>::SetRotation3dZ(const TYPE& aRadian)
		{
			Matrix44<TYPE> rotationMatrix = Matrix44::CreateRotateAroundZ(aRadian);
			Matrix44<TYPE> tempMatrix = *this;
			tempMatrix = rotationMatrix * tempMatrix;


			myMatrix[0] = tempMatrix.myMatrix[0];
			myMatrix[1] = tempMatrix.myMatrix[1];
			myMatrix[2] = tempMatrix.myMatrix[2];

			myMatrix[4] = tempMatrix.myMatrix[4];
			myMatrix[5] = tempMatrix.myMatrix[5];
			myMatrix[6] = tempMatrix.myMatrix[6];

			myMatrix[8] = tempMatrix.myMatrix[8];
			myMatrix[9] = tempMatrix.myMatrix[9];
			myMatrix[10] = tempMatrix.myMatrix[10];
		}

		template<typename TYPE>
		void Matrix44<TYPE>::SetTranslation(const TYPE& anX, const TYPE& anY, const TYPE& anZ, const TYPE& anW)
		{
			myMatrix[12] = anX;
			myMatrix[13] = anY;
			myMatrix[14] = anZ;
			myMatrix[15] = anW;
		}

		template<typename TYPE>
		void Matrix44<TYPE>::SetTranslation(const Vector4<TYPE>& aVector)
		{
			myMatrix[12] = aVector.x;
			myMatrix[13] = aVector.y;
			myMatrix[14] = aVector.z;
			myMatrix[15] = aVector.w;
		}

		template<typename TYPE>
		const Vector4<TYPE> Matrix44<TYPE>::GetTranslation() const
		{
			return Vector4<TYPE>(myMatrix[12], myMatrix[13], myMatrix[14], myMatrix[15]);
		}

		template<typename TYPE>
		const Vector3<TYPE> Matrix44<TYPE>::GetPosition() const
		{
			return Vector3<TYPE>(myMatrix[12], myMatrix[13], myMatrix[14]);
		}

		template<typename TYPE>
		const Vector4<TYPE> Matrix44<TYPE>::GetRight() const
		{
			return Vector4<TYPE>(myMatrix[0], myMatrix[1], myMatrix[2], myMatrix[3]);
		}

		template<typename TYPE>
		const Vector4<TYPE> Matrix44<TYPE>::GetUp() const
		{
			return Vector4<TYPE>(myMatrix[4], myMatrix[5], myMatrix[6], myMatrix[7]);
		}

		template<typename TYPE>
		const Vector4<TYPE> Core::Math::Matrix44<TYPE>::GetScale() const
		{

			TYPE sx = Core::Math::Length(rows[0]);
			TYPE sy = Core::Math::Length(rows[1]);
			TYPE sz = Core::Math::Length(rows[2]);
			return Vector4<TYPE>(sx, sy, sz, 1);
		}

		template<typename TYPE>
		const Vector4<TYPE> Matrix44<TYPE>::GetForward() const
		{
			return Vector4<TYPE>(myMatrix[8], myMatrix[9], myMatrix[10], myMatrix[11]);
		}

		template<typename TYPE>
		void Matrix44<TYPE>::SetPosition(const Vector3<TYPE>& aVector)
		{
			myMatrix[12] = aVector.x;
			myMatrix[13] = aVector.y;
			myMatrix[14] = aVector.z;
		}

		template<typename TYPE>
		void Matrix44<TYPE>::SetPosition(const Vector4<TYPE>& aVector)
		{
			myMatrix[12] = aVector.x;
			myMatrix[13] = aVector.y;
			myMatrix[14] = aVector.z;
			myMatrix[15] = aVector.w;

		}


		template<typename TYPE>
		void Matrix44<TYPE>::SetRight(const Vector4<TYPE>& aVector)
		{
			myMatrix[0] = aVector.x;
			myMatrix[1] = aVector.y;
			myMatrix[2] = aVector.z;
			myMatrix[3] = aVector.w;
		}

		template<typename TYPE>
		void Matrix44<TYPE>::SetUp(const Vector4<TYPE>& aVector)
		{
			myMatrix[4] = aVector.x;
			myMatrix[5] = aVector.y;
			myMatrix[6] = aVector.z;
			myMatrix[7] = aVector.w;
		}

		template<typename TYPE>
		void Matrix44<TYPE>::SetForward(const Vector4<TYPE>& aVector)
		{
			myMatrix[8] = aVector.x;
			myMatrix[9] = aVector.y;
			myMatrix[10] = aVector.z;
			myMatrix[11] = aVector.w;
		}

		template<typename TYPE>
		Matrix44<TYPE> Inverse(const Matrix44<TYPE>& aMatrix)
		{
			Matrix44<TYPE> inverse(aMatrix);

			Vector4<TYPE> translation = inverse.GetTranslation();
			inverse.SetPosition(Vector4<TYPE>(0, 0, 0, 1.f));
			translation *= -1.f;
			translation.w = 1.f;
			inverse = Transpose(inverse);
			translation = translation * inverse;

			inverse.SetPosition(translation);
			return inverse;
		}


#pragma endregion

#pragma region Operators
		/****************************\
		|		Normal Operators	 |
		\****************************/

		template<typename TYPE>
		const Matrix44<TYPE> operator+(const Matrix44<TYPE>& aFirstMatrix, const Matrix44<TYPE>& aSecondMatrix)
		{
			Matrix44<TYPE> tempMatrix(aFirstMatrix);
			return tempMatrix += aSecondMatrix;
		}

		template<typename TYPE>
		const Matrix44<TYPE> operator-(const Matrix44<TYPE>& aFirstMatrix, const Matrix44<TYPE>& aSecondMatrix)
		{
			Matrix44<TYPE> tempMatrix(aFirstMatrix);
			return tempMatrix -= aSecondMatrix;
		}

		template<typename TYPE>
		const Matrix44<TYPE> operator*(const Matrix44<TYPE>& aFirstMatrix, const Matrix44<TYPE>& aSecondMatrix)
		{
			Matrix44<TYPE> out(aFirstMatrix);
			out *= aSecondMatrix;
			return out;
		}

		template<typename TYPE>
		const Vector4<TYPE> operator*(const Vector4<TYPE>& aVector, const Matrix44<TYPE>& aMatrix)
		{
			Vector4<TYPE> tempVector(aVector);
			return tempVector *= aMatrix;
		}

// 		template<typename TYPE>
// 		const Vector4<TYPE> operator*(const Matrix44<TYPE>& matrix, const Vector4<TYPE>& vector)
// 		{
// 			Vector4<TYPE> toReturn;
// 			toReturn.x = Dot(matrix.rows[0], vector);
// 			toReturn.y = Dot(matrix.rows[1], vector);
// 			toReturn.z = Dot(matrix.rows[2], vector);
// 			toReturn.w = Dot(matrix.rows[3], vector);
// 			return toReturn;
// 		}

// 		template<typename TYPE>
// 		const Vector3<TYPE> operator*(const Matrix44<TYPE>& matrix, const Vector3<TYPE>& vector)
// 		{
// 			return Vector3<TYPE>(
// 				matrix.myMatrix[0] * vector.x + matrix.myMatrix[1] * vector.y + matrix.myMatrix[2] * vector.z,
// 				matrix.myMatrix[4] * vector.x + matrix.myMatrix[5] * vector.y + matrix.myMatrix[6] * vector.z,
// 				matrix.myMatrix[8] * vector.x + matrix.myMatrix[9] * vector.y + matrix.myMatrix[10] * vector.z);
// 		}




		template<typename TYPE>
		Vector3<TYPE> operator*(const Vector3<TYPE>& v, const Matrix44<TYPE>& m)
		{
			return Vector3<TYPE>(
				m[0] * v.x + m[4] * v.y + m[8] * v.z,
				m[1] * v.x + m[5] * v.y + m[9] * v.z,
				m[2] * v.x + m[6] * v.y + m[10] * v.z);
		}


		/****************************\
		|		Compound Operators	 |
		\****************************/

		template<typename TYPE>
		const Matrix44<TYPE>& operator+=(Matrix44<TYPE>& aFirstMatrix, const Matrix44<TYPE>& aSecondMatrix)
		{
			for (int i = 0; i < 16; ++i)
			{
				aFirstMatrix.myMatrix[i] += aSecondMatrix.myMatrix[i];
			}
			return aFirstMatrix;
		}

		template<typename TYPE>
		const Matrix44<TYPE> operator-=(const Matrix44<TYPE>& aFirstMatrix, const Matrix44<TYPE>& aSecondMatrix)
		{
			for (int i = 0; i < 16; ++i)
			{
				aFirstMatrix.myMatrix[i] -= aSecondMatrix.myMatrix[i];
			}
			return aFirstMatrix;
		}


		template<typename TYPE>
		Matrix44<TYPE>& operator*=(Matrix44<TYPE>& left, const Matrix44<TYPE>& right)
		{
			const __m128 c0 = _mm_load_ps(&right.myMatrix[0]);
			const __m128 c1 = _mm_load_ps(&right.myMatrix[4]);
			const __m128 c2 = _mm_load_ps(&right.myMatrix[8]);
			const __m128 c3 = _mm_load_ps(&right.myMatrix[12]);

			float* leftPtr = &left.myMatrix[0];
			for (size_t i = 0; i < 4; i++)
			{
				__m128 r0 = _mm_set1_ps(leftPtr[0 + (i * 4)]);
				__m128 r1 = _mm_set1_ps(leftPtr[1 + (i * 4)]);
				__m128 r2 = _mm_set1_ps(leftPtr[2 + (i * 4)]);
				__m128 r3 = _mm_set1_ps(leftPtr[3 + (i * 4)]);

				__m128 row = _mm_add_ps(
					_mm_add_ps(
						_mm_mul_ps(r0, c0), 
						_mm_mul_ps(r1, c1)),	
					_mm_add_ps(	
						_mm_mul_ps(r2, c2), 
						_mm_mul_ps(r3, c3)));

				_mm_store_ps(&left.myMatrix[4 * i], row);
			}

			return left;
		}
		
		template<typename TYPE>
		const Vector4<TYPE> operator*=(const Vector4<TYPE>& aVector, const Matrix44<TYPE>& aMatrix)
		{
			Vector4<TYPE> vector = aVector;
			vector.x = Dot(aVector, aMatrix.GetColumn(0));
			vector.y = Dot(aVector, aMatrix.GetColumn(1));
			vector.z = Dot(aVector, aMatrix.GetColumn(2));
			vector.w = Dot(aVector, aMatrix.GetColumn(3));

			return vector;
		}

		/****************************\
		|		Other Operations     |
		\****************************/

		template<typename TYPE>
		bool operator==(Matrix44<TYPE>& aFirstMatrix, Matrix44<TYPE>& aSecondMatrix)
		{
			for (int i = 0; i < 16; ++i)
			{
				if (aFirstMatrix.myMatrix[i] != aSecondMatrix.myMatrix[i])
				{
					return false;
				}
			}
			return true;
		}


		template<typename TYPE>
		Matrix44<TYPE>& Matrix44<TYPE>::operator=(const Matrix44<TYPE>& aMatrix)
		{
			for (unsigned short i = 0; i < 16; ++i)
			{
				myMatrix[i] = aMatrix.myMatrix[i];
			}
			return *this;
		}

		template<typename TYPE>
		Matrix44<TYPE> Transpose(const Matrix44<TYPE>& aMatrix)
		{
			Matrix44<TYPE> tempMatrix;
			for (unsigned short i = 0; i < 4; ++i)
			{
				for (unsigned short j = 0; j < 4; ++j)
				{
					tempMatrix.myMatrix[i + (j * 4)] = aMatrix.myMatrix[j + (i * 4)];
				}
			}
			return tempMatrix;
		}

		template<typename TYPE>
		void Core::Math::Matrix44<TYPE>::SetPerspectiveFOV(float fov, float aspect_ratio)
		{
			float sin_fov = sin(0.5f * fov);
			float cos_fov = cos(0.5f * fov);
			float width = cos_fov / sin_fov;
			float height = width / aspect_ratio;

			myMatrix[0] = width;
			myMatrix[5] = height;

		}

		template<typename TYPE>
		Matrix44<TYPE> Matrix44<TYPE>::CreateProjectionMatrixLH(TYPE aNearZ, TYPE aFarZ, TYPE anAspectRatio, TYPE aFoVAngle)
		{
			Matrix44 temp;
			TYPE SinFov;
			TYPE CosFov;
			TYPE Height;
			TYPE Width;

			SinFov = sin(0.5f * aFoVAngle);
			CosFov = cos(0.5f * aFoVAngle);

			Width = CosFov / SinFov;
			Height = Width / anAspectRatio;

			TYPE scaling = aFarZ / (aFarZ - aNearZ);

			temp.myMatrix[0] = Width;
			temp.myMatrix[5] = Height;
			temp.myMatrix[10] = scaling;
			temp.myMatrix[11] = 1.0f;

			temp.myMatrix[14] = -scaling * aNearZ;
			temp.myMatrix[15] = 1.0f;
			return temp;
		}

		template<typename TYPE>
		Matrix44<TYPE> Matrix44<TYPE>::CreateOrthogonalMatrixLH(TYPE aWidth, TYPE aHeight, TYPE aNearZ, TYPE aFarZ)
		{
			Matrix44<TYPE> toReturn;
			toReturn.myMatrix[0] = 2.f / aWidth;
			toReturn.myMatrix[5] = 2.f / aHeight;
			toReturn.myMatrix[10] = 1.f / (aFarZ - aNearZ);
			toReturn.myMatrix[14] = aNearZ / (aNearZ - aFarZ);
			toReturn.myMatrix[15] = 1.f;
			return toReturn;
		}

		template<typename TYPE>
		const Matrix44<TYPE> Matrix44<TYPE>::Inverse(Matrix44<TYPE> &aMatrix)
		{
			Vector4<TYPE> theTranslation;
			theTranslation = aMatrix.GetTranslation();
			theTranslation.x *= -1;
			theTranslation.y *= -1;
			theTranslation.z *= -1;
			theTranslation.w = 1;

			//	Matrix44<TYPE> theMatrix(aMatrix);

			aMatrix.SetTranslation(0.0f, 0.0f, 0.0f, 1.0f);
			aMatrix = Transpose(aMatrix);

			theTranslation *= aMatrix;

			aMatrix.SetTranslation(theTranslation.x, theTranslation.y, theTranslation.z, 1);

			return aMatrix;
		}

		template<class TYPE>
		Matrix44<TYPE> CreateReflectionMatrixAboutAxis44(Vector3<TYPE> reflectionVector)
		{
			Matrix44<TYPE> reflectionMatrix;
			reflectionMatrix.myMatrix[0] = 1 - 2 * (reflectionVector.x*reflectionVector.x);
			reflectionMatrix.myMatrix[1] = -2 * (reflectionVector.x*reflectionVector.y);
			reflectionMatrix.myMatrix[2] = -2 * (reflectionVector.x*reflectionVector.z);

			reflectionMatrix.myMatrix[4] = -2 * (reflectionVector.y*reflectionVector.x);
			reflectionMatrix.myMatrix[5] = 1 - 2 * (reflectionVector.y*reflectionVector.y);
			reflectionMatrix.myMatrix[6] = -2 * (reflectionVector.y*reflectionVector.z);

			reflectionMatrix.myMatrix[8] = -2 * (reflectionVector.z*reflectionVector.x);
			reflectionMatrix.myMatrix[9] = -2 * (reflectionVector.z*reflectionVector.y);
			reflectionMatrix.myMatrix[10] = 1 - 2 * (reflectionVector.z*reflectionVector.z);
			return reflectionMatrix;
		};



#pragma endregion

		template <typename T>
		Matrix44<T> InverseReal(const Matrix44<T>& aMatrix)
		{
			T inv[16], det;
			int i;

			inv[0] = aMatrix.myMatrix[5] * aMatrix.myMatrix[10] * aMatrix.myMatrix[15] -
				aMatrix.myMatrix[5] * aMatrix.myMatrix[11] * aMatrix.myMatrix[14] -
				aMatrix.myMatrix[9] * aMatrix.myMatrix[6] * aMatrix.myMatrix[15] +
				aMatrix.myMatrix[9] * aMatrix.myMatrix[7] * aMatrix.myMatrix[14] +
				aMatrix.myMatrix[13] * aMatrix.myMatrix[6] * aMatrix.myMatrix[11] -
				aMatrix.myMatrix[13] * aMatrix.myMatrix[7] * aMatrix.myMatrix[10];

			inv[4] = -aMatrix.myMatrix[4] * aMatrix.myMatrix[10] * aMatrix.myMatrix[15] +
				aMatrix.myMatrix[4] * aMatrix.myMatrix[11] * aMatrix.myMatrix[14] +
				aMatrix.myMatrix[8] * aMatrix.myMatrix[6] * aMatrix.myMatrix[15] -
				aMatrix.myMatrix[8] * aMatrix.myMatrix[7] * aMatrix.myMatrix[14] -
				aMatrix.myMatrix[12] * aMatrix.myMatrix[6] * aMatrix.myMatrix[11] +
				aMatrix.myMatrix[12] * aMatrix.myMatrix[7] * aMatrix.myMatrix[10];

			inv[8] = aMatrix.myMatrix[4] * aMatrix.myMatrix[9] * aMatrix.myMatrix[15] -
				aMatrix.myMatrix[4] * aMatrix.myMatrix[11] * aMatrix.myMatrix[13] -
				aMatrix.myMatrix[8] * aMatrix.myMatrix[5] * aMatrix.myMatrix[15] +
				aMatrix.myMatrix[8] * aMatrix.myMatrix[7] * aMatrix.myMatrix[13] +
				aMatrix.myMatrix[12] * aMatrix.myMatrix[5] * aMatrix.myMatrix[11] -
				aMatrix.myMatrix[12] * aMatrix.myMatrix[7] * aMatrix.myMatrix[9];

			inv[12] = -aMatrix.myMatrix[4] * aMatrix.myMatrix[9] * aMatrix.myMatrix[14] +
				aMatrix.myMatrix[4] * aMatrix.myMatrix[10] * aMatrix.myMatrix[13] +
				aMatrix.myMatrix[8] * aMatrix.myMatrix[5] * aMatrix.myMatrix[14] -
				aMatrix.myMatrix[8] * aMatrix.myMatrix[6] * aMatrix.myMatrix[13] -
				aMatrix.myMatrix[12] * aMatrix.myMatrix[5] * aMatrix.myMatrix[10] +
				aMatrix.myMatrix[12] * aMatrix.myMatrix[6] * aMatrix.myMatrix[9];

			inv[1] = -aMatrix.myMatrix[1] * aMatrix.myMatrix[10] * aMatrix.myMatrix[15] +
				aMatrix.myMatrix[1] * aMatrix.myMatrix[11] * aMatrix.myMatrix[14] +
				aMatrix.myMatrix[9] * aMatrix.myMatrix[2] * aMatrix.myMatrix[15] -
				aMatrix.myMatrix[9] * aMatrix.myMatrix[3] * aMatrix.myMatrix[14] -
				aMatrix.myMatrix[13] * aMatrix.myMatrix[2] * aMatrix.myMatrix[11] +
				aMatrix.myMatrix[13] * aMatrix.myMatrix[3] * aMatrix.myMatrix[10];

			inv[5] = aMatrix.myMatrix[0] * aMatrix.myMatrix[10] * aMatrix.myMatrix[15] -
				aMatrix.myMatrix[0] * aMatrix.myMatrix[11] * aMatrix.myMatrix[14] -
				aMatrix.myMatrix[8] * aMatrix.myMatrix[2] * aMatrix.myMatrix[15] +
				aMatrix.myMatrix[8] * aMatrix.myMatrix[3] * aMatrix.myMatrix[14] +
				aMatrix.myMatrix[12] * aMatrix.myMatrix[2] * aMatrix.myMatrix[11] -
				aMatrix.myMatrix[12] * aMatrix.myMatrix[3] * aMatrix.myMatrix[10];

			inv[9] = -aMatrix.myMatrix[0] * aMatrix.myMatrix[9] * aMatrix.myMatrix[15] +
				aMatrix.myMatrix[0] * aMatrix.myMatrix[11] * aMatrix.myMatrix[13] +
				aMatrix.myMatrix[8] * aMatrix.myMatrix[1] * aMatrix.myMatrix[15] -
				aMatrix.myMatrix[8] * aMatrix.myMatrix[3] * aMatrix.myMatrix[13] -
				aMatrix.myMatrix[12] * aMatrix.myMatrix[1] * aMatrix.myMatrix[11] +
				aMatrix.myMatrix[12] * aMatrix.myMatrix[3] * aMatrix.myMatrix[9];

			inv[13] = aMatrix.myMatrix[0] * aMatrix.myMatrix[9] * aMatrix.myMatrix[14] -
				aMatrix.myMatrix[0] * aMatrix.myMatrix[10] * aMatrix.myMatrix[13] -
				aMatrix.myMatrix[8] * aMatrix.myMatrix[1] * aMatrix.myMatrix[14] +
				aMatrix.myMatrix[8] * aMatrix.myMatrix[2] * aMatrix.myMatrix[13] +
				aMatrix.myMatrix[12] * aMatrix.myMatrix[1] * aMatrix.myMatrix[10] -
				aMatrix.myMatrix[12] * aMatrix.myMatrix[2] * aMatrix.myMatrix[9];

			inv[2] = aMatrix.myMatrix[1] * aMatrix.myMatrix[6] * aMatrix.myMatrix[15] -
				aMatrix.myMatrix[1] * aMatrix.myMatrix[7] * aMatrix.myMatrix[14] -
				aMatrix.myMatrix[5] * aMatrix.myMatrix[2] * aMatrix.myMatrix[15] +
				aMatrix.myMatrix[5] * aMatrix.myMatrix[3] * aMatrix.myMatrix[14] +
				aMatrix.myMatrix[13] * aMatrix.myMatrix[2] * aMatrix.myMatrix[7] -
				aMatrix.myMatrix[13] * aMatrix.myMatrix[3] * aMatrix.myMatrix[6];

			inv[6] = -aMatrix.myMatrix[0] * aMatrix.myMatrix[6] * aMatrix.myMatrix[15] +
				aMatrix.myMatrix[0] * aMatrix.myMatrix[7] * aMatrix.myMatrix[14] +
				aMatrix.myMatrix[4] * aMatrix.myMatrix[2] * aMatrix.myMatrix[15] -
				aMatrix.myMatrix[4] * aMatrix.myMatrix[3] * aMatrix.myMatrix[14] -
				aMatrix.myMatrix[12] * aMatrix.myMatrix[2] * aMatrix.myMatrix[7] +
				aMatrix.myMatrix[12] * aMatrix.myMatrix[3] * aMatrix.myMatrix[6];

			inv[10] = aMatrix.myMatrix[0] * aMatrix.myMatrix[5] * aMatrix.myMatrix[15] -
				aMatrix.myMatrix[0] * aMatrix.myMatrix[7] * aMatrix.myMatrix[13] -
				aMatrix.myMatrix[4] * aMatrix.myMatrix[1] * aMatrix.myMatrix[15] +
				aMatrix.myMatrix[4] * aMatrix.myMatrix[3] * aMatrix.myMatrix[13] +
				aMatrix.myMatrix[12] * aMatrix.myMatrix[1] * aMatrix.myMatrix[7] -
				aMatrix.myMatrix[12] * aMatrix.myMatrix[3] * aMatrix.myMatrix[5];

			inv[14] = -aMatrix.myMatrix[0] * aMatrix.myMatrix[5] * aMatrix.myMatrix[14] +
				aMatrix.myMatrix[0] * aMatrix.myMatrix[6] * aMatrix.myMatrix[13] +
				aMatrix.myMatrix[4] * aMatrix.myMatrix[1] * aMatrix.myMatrix[14] -
				aMatrix.myMatrix[4] * aMatrix.myMatrix[2] * aMatrix.myMatrix[13] -
				aMatrix.myMatrix[12] * aMatrix.myMatrix[1] * aMatrix.myMatrix[6] +
				aMatrix.myMatrix[12] * aMatrix.myMatrix[2] * aMatrix.myMatrix[5];

			inv[3] = -aMatrix.myMatrix[1] * aMatrix.myMatrix[6] * aMatrix.myMatrix[11] +
				aMatrix.myMatrix[1] * aMatrix.myMatrix[7] * aMatrix.myMatrix[10] +
				aMatrix.myMatrix[5] * aMatrix.myMatrix[2] * aMatrix.myMatrix[11] -
				aMatrix.myMatrix[5] * aMatrix.myMatrix[3] * aMatrix.myMatrix[10] -
				aMatrix.myMatrix[9] * aMatrix.myMatrix[2] * aMatrix.myMatrix[7] +
				aMatrix.myMatrix[9] * aMatrix.myMatrix[3] * aMatrix.myMatrix[6];

			inv[7] = aMatrix.myMatrix[0] * aMatrix.myMatrix[6] * aMatrix.myMatrix[11] -
				aMatrix.myMatrix[0] * aMatrix.myMatrix[7] * aMatrix.myMatrix[10] -
				aMatrix.myMatrix[4] * aMatrix.myMatrix[2] * aMatrix.myMatrix[11] +
				aMatrix.myMatrix[4] * aMatrix.myMatrix[3] * aMatrix.myMatrix[10] +
				aMatrix.myMatrix[8] * aMatrix.myMatrix[2] * aMatrix.myMatrix[7] -
				aMatrix.myMatrix[8] * aMatrix.myMatrix[3] * aMatrix.myMatrix[6];

			inv[11] = -aMatrix.myMatrix[0] * aMatrix.myMatrix[5] * aMatrix.myMatrix[11] +
				aMatrix.myMatrix[0] * aMatrix.myMatrix[7] * aMatrix.myMatrix[9] +
				aMatrix.myMatrix[4] * aMatrix.myMatrix[1] * aMatrix.myMatrix[11] -
				aMatrix.myMatrix[4] * aMatrix.myMatrix[3] * aMatrix.myMatrix[9] -
				aMatrix.myMatrix[8] * aMatrix.myMatrix[1] * aMatrix.myMatrix[7] +
				aMatrix.myMatrix[8] * aMatrix.myMatrix[3] * aMatrix.myMatrix[5];

			inv[15] = aMatrix.myMatrix[0] * aMatrix.myMatrix[5] * aMatrix.myMatrix[10] -
				aMatrix.myMatrix[0] * aMatrix.myMatrix[6] * aMatrix.myMatrix[9] -
				aMatrix.myMatrix[4] * aMatrix.myMatrix[1] * aMatrix.myMatrix[10] +
				aMatrix.myMatrix[4] * aMatrix.myMatrix[2] * aMatrix.myMatrix[9] +
				aMatrix.myMatrix[8] * aMatrix.myMatrix[1] * aMatrix.myMatrix[6] -
				aMatrix.myMatrix[8] * aMatrix.myMatrix[2] * aMatrix.myMatrix[5];

			det = aMatrix.myMatrix[0] * inv[0] + aMatrix.myMatrix[1] * inv[4] + aMatrix.myMatrix[2] * inv[8] + aMatrix.myMatrix[3] * inv[12];

			det = static_cast<T>(1.0) / det;

			Matrix44<T> returnMatrix;

			for (i = 0; i < 16; i++)
				returnMatrix.myMatrix[i] = inv[i] * det;

			return returnMatrix;
		}



	};
};

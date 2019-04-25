#pragma once
#include <cassert>
namespace Core
{
	template <typename TYPE>
	class Matrix33
	{
	public:
		Matrix33();
		Matrix33(const Matrix33<TYPE>& aMatrix);
		~Matrix33() = default;

		Matrix33& operator=(const Matrix33& aMatrix);

		static Matrix33<TYPE> CreateRotateAroundX(TYPE aAngleInRadians);
		static Matrix33<TYPE> CreateRotateAroundY(TYPE aAngleInRadians);
		static Matrix33<TYPE> CreateRotateAroundZ(TYPE aAngleInRadians);


		TYPE m_Matrix[9];

		void Rotate2D(TYPE aRadian);
		void SetRotation2d(TYPE aRadian);
		void SetTranslation(TYPE anX, TYPE anY, TYPE anZ);
		void SetTranslation(const Vector3<TYPE>& aVector3);
		Vector3<TYPE> GetTranslation() const;
	};


	template<typename TYPE>
	Matrix33<TYPE>::Matrix33()
	{
		m_Matrix[1] = 0;
		m_Matrix[2] = 0;
		m_Matrix[3] = 0;
		m_Matrix[5] = 0;
		m_Matrix[6] = 0;
		m_Matrix[7] = 0;

		m_Matrix[0] = 1;
		m_Matrix[4] = 1;
		m_Matrix[8] = 1;
	}


	template<typename TYPE>
	Matrix33<TYPE>::Matrix33(const Matrix33<TYPE>& aMatrix)
	{
		*this = aMatrix;
	}

	template<typename TYPE>
	Matrix33<TYPE> Matrix33<TYPE>::CreateRotateAroundX(TYPE aAngleInRadians)
	{
		Matrix33<TYPE> matrix;

		matrix.m_Matrix[0] = 1;
		matrix.m_Matrix[1] = 0;
		matrix.m_Matrix[2] = 0;

		matrix.m_Matrix[3] = 0;
		matrix.m_Matrix[4] = cos(aAngleInRadians);
		matrix.m_Matrix[5] = sin(aAngleInRadians);

		matrix.m_Matrix[6] = 0;
		matrix.m_Matrix[7] = -sin(aAngleInRadians);
		matrix.m_Matrix[8] = cos(aAngleInRadians);

		return matrix;
	}

	template<typename TYPE>
	Matrix33<TYPE> Matrix33<TYPE>::CreateRotateAroundY(TYPE aAngleInRadians)
	{
		Matrix33<TYPE> matrix;

		matrix.m_Matrix[0] = cos(aAngleInRadians);
		matrix.m_Matrix[1] = 0;
		matrix.m_Matrix[2] = -sin(aAngleInRadians);

		matrix.m_Matrix[3] = 0;
		matrix.m_Matrix[4] = 1;
		matrix.m_Matrix[5] = 0;

		matrix.m_Matrix[6] = sin(aAngleInRadians);
		matrix.m_Matrix[7] = 0;
		matrix.m_Matrix[8] = cos(aAngleInRadians);

		return matrix;
	}

	template<typename TYPE>
	Matrix33<TYPE> Matrix33<TYPE>::CreateRotateAroundZ(TYPE aAngleInRadians)
	{
		Matrix33<TYPE> matrix;

		matrix.m_Matrix[0] = cos(aAngleInRadians);
		matrix.m_Matrix[1] = sin(aAngleInRadians);
		matrix.m_Matrix[2] = 0;

		matrix.m_Matrix[3] = -sin(aAngleInRadians);
		matrix.m_Matrix[4] = cos(aAngleInRadians);
		matrix.m_Matrix[5] = 0;

		matrix.m_Matrix[6] = 0;
		matrix.m_Matrix[7] = 0;
		matrix.m_Matrix[8] = 1;

		return matrix;
	}


	template<typename TYPE>
	void Matrix33<TYPE>::Rotate2D(TYPE aRadian)
	{
		Matrix33<TYPE> rotateMatrix = Matrix33::CreateRotateAroundZ(aRadian);
		Matrix33<TYPE> tempMatrix = *this;
		tempMatrix *= rotateMatrix;

		m_Matrix[0] = tempMatrix.m_Matrix[0];
		m_Matrix[1] = tempMatrix.m_Matrix[1];
		m_Matrix[3] = tempMatrix.m_Matrix[3];
		m_Matrix[4] = tempMatrix.m_Matrix[4];
	}
	template<typename TYPE>
	void Matrix33<TYPE>::SetRotation2d(TYPE aRadian)
	{
		m_Matrix[0] = cos(aRadian);
		m_Matrix[1] = sin(aRadian);
		m_Matrix[3] = -sin(aRadian);
		m_Matrix[4] = cos(aRadian);
	}

	template<typename TYPE>
	void Matrix33<TYPE>::SetTranslation(TYPE anX, TYPE anY, TYPE anZ)
	{
		m_Matrix[6] = anX;
		m_Matrix[7] = anY;
		m_Matrix[8] = anZ;
	}

	template<typename TYPE>
	void Matrix33<TYPE>::SetTranslation(const Vector3<TYPE>& aVector3)
	{
		m_Matrix[6] = aVector3.x;
		m_Matrix[7] = aVector3.y;
		m_Matrix[8] = aVector3.z;
	}

	template<typename TYPE>
	Vector3<TYPE> Matrix33<TYPE>::GetTranslation() const
	{
		return Vector3<TYPE>(m_Matrix[6], m_Matrix[7], m_Matrix[8]);
	}


#pragma region Operators

#pragma region NormalOperator

	template<typename TYPE>
	const Matrix33<TYPE> operator+(Matrix33<TYPE>& aFirstMatrix, const Matrix33<TYPE>& aSecondMatrix)
	{
		return aFirstMatrix += aSecondMatrix;
	}

	template<typename TYPE>
	const Matrix33<TYPE> operator-(Matrix33<TYPE>& aFirstMatrix, const Matrix33<TYPE>& aSecondMatrix)
	{
		return aFirstMatrix -= aSecondMatrix;
	}

	template<typename TYPE>
	const Matrix33<TYPE> operator*(Matrix33<TYPE>& aFirstMatrix, const Matrix33<TYPE>& aSecondMatrix)
	{
		return aFirstMatrix *= aSecondMatrix;
	}

	template<typename TYPE>
	const Vector3<TYPE> operator*(Vector3<TYPE>& aVector, const Matrix33<TYPE>& aMatrix)
	{
		return aVector *= aMatrix;
	}

	template<typename TYPE>
	Vector2<TYPE> operator*(Vector2<TYPE>& aVector, const Matrix33<TYPE>& aMatrix)
	{
		Vector3<TYPE> vector3(aVector.x, aVector.y, 1);
		vector3 = vector3*aMatrix;
		return Vector2<TYPE>(vector3.x, vector3.y);
	}

#pragma endregion

#pragma region CompoundOperators

	template<typename TYPE>
	Matrix33<TYPE> operator+=(Matrix33<TYPE>& aFirstMatrix, const Matrix33<TYPE>& aSecondMatrix)
	{
		for (int i = 0; i < 9; ++i)
		{
			aFirstMatrix.m_Matrix[i] += aSecondMatrix.m_Matrix[i];
		}
		return aFirstMatrix;
	}

	template<typename TYPE>
	Matrix33<TYPE> operator-=(Matrix33<TYPE>& aFirstMatrix, const Matrix33<TYPE>& aSecondMatrix)
	{
		for (int i = 0; i < 9; ++i)
		{
			aFirstMatrix.m_Matrix[i] -= aSecondMatrix.m_Matrix[i];
		}
		return aFirstMatrix;
	}

	template<typename TYPE>
	Matrix33<TYPE> operator*=(Matrix33<TYPE>& aFirstMatrix, const Matrix33<TYPE>& aSecondMatrix)
	{
		Vector3<TYPE> myFirstRow(aFirstMatrix.m_Matrix[0], aFirstMatrix.m_Matrix[1], aFirstMatrix.m_Matrix[2]);
		Vector3<TYPE> mySecondRow(aFirstMatrix.m_Matrix[3], aFirstMatrix.m_Matrix[4], aFirstMatrix.m_Matrix[5]);
		Vector3<TYPE> myThirdRow(aFirstMatrix.m_Matrix[6], aFirstMatrix.m_Matrix[7], aFirstMatrix.m_Matrix[8]);

		Vector3<TYPE> myFirstCollumn(aSecondMatrix.m_Matrix[0], aSecondMatrix.m_Matrix[3], aSecondMatrix.m_Matrix[6]);
		Vector3<TYPE> mySecondCollumn(aSecondMatrix.m_Matrix[1], aSecondMatrix.m_Matrix[4], aSecondMatrix.m_Matrix[7]);
		Vector3<TYPE> myThirdCollumn(aSecondMatrix.m_Matrix[2], aSecondMatrix.m_Matrix[5], aSecondMatrix.m_Matrix[8]);

		aFirstMatrix.m_Matrix[0] = Dot(myFirstRow, myFirstCollumn);
		aFirstMatrix.m_Matrix[1] = Dot(myFirstRow, mySecondCollumn);
		aFirstMatrix.m_Matrix[2] = Dot(myFirstRow, myThirdCollumn);

		aFirstMatrix.m_Matrix[3] = Dot(mySecondRow, myFirstCollumn);
		aFirstMatrix.m_Matrix[4] = Dot(mySecondRow, mySecondCollumn);
		aFirstMatrix.m_Matrix[5] = Dot(mySecondRow, myThirdCollumn);

		aFirstMatrix.m_Matrix[6] = Dot(myThirdRow, myFirstCollumn);
		aFirstMatrix.m_Matrix[7] = Dot(myThirdRow, mySecondCollumn);
		aFirstMatrix.m_Matrix[8] = Dot(myThirdRow, myThirdCollumn);

		return aFirstMatrix;
	}


	template<typename TYPE>
	Vector3<TYPE> operator*=(Vector3<TYPE>& aVector, const Matrix33<TYPE>& aMatrix)
	{

		Vector3<TYPE> myVector;

		Vector3<TYPE> myFirstCollumn(aMatrix.m_Matrix[0], aMatrix.m_Matrix[3], aMatrix.m_Matrix[6]);
		Vector3<TYPE> mySecondCollumn(aMatrix.m_Matrix[1], aMatrix.m_Matrix[4], aMatrix.m_Matrix[7]);
		Vector3<TYPE> myThirdCollumn(aMatrix.m_Matrix[2], aMatrix.m_Matrix[5], aMatrix.m_Matrix[8]);

		myVector.x = Dot(aVector, myFirstCollumn);
		myVector.y = Dot(aVector, mySecondCollumn);
		myVector.z = Dot(aVector, myThirdCollumn);

		return myVector;
	}

	template<typename TYPE>
	Vector2<TYPE> operator*=(Vector2<TYPE>& aVector, const Matrix33<TYPE>& aMatrix)
	{
		Vector2<TYPE> myVector;
		Vector2<TYPE> firstCollumn(aMatrix.m_Matrix[0], aMatrix.m_Matrix[3]);
		Vector2<TYPE> secondCollumn(aMatrix.m_Matrix[1], aMatrix.m_Matrix[4]);
		Vector2<TYPE> thirdCollumn(aMatrix.m_Matrix[3], aMatrix.m_Matrix[5]);

		myVector.x = Dot(aVector, firstCollumn);
		myVector.y = Dot(aVector, secondCollumn);
		//myVector.z = Dot(aVector, thirdCollumn);

		return myVector;
	}

#pragma endregion

#pragma region OtherOperations

	template<typename TYPE>
	bool operator==(Matrix33<TYPE>& aFirstMatrix, Matrix33<TYPE>& aSecondMatrix)
	{
		for (int i = 0; i < 9; ++i)
		{
			aFirstMatrix.m_Matrix[i] = aSecondMatrix.m_Matrix[i];
			if (aFirstMatrix.m_Matrix[i] != aSecondMatrix.m_Matrix[i])
			{
				return false;
			}
		}
		return true;
	}


	template<typename TYPE>
	Matrix33<TYPE>& Matrix33<TYPE>::operator=(const Matrix33<TYPE>& aMatrix)
	{
		for (unsigned short i = 0; i < 9; ++i)
			m_Matrix[i] = aMatrix.m_Matrix[i];

		return *this;
	}

	template<typename TYPE>
	Matrix33<TYPE> Transpose(const Matrix33<TYPE>& aMatrix)
	{
		Matrix33<TYPE> tempMatrix;
		for (unsigned short i = 0; i < 3; ++i)
		{
			for (int j = 0; j < 3; ++j)
			{
				tempMatrix.m_Matrix[i + (j * 3)] = aMatrix.m_Matrix[j + (i * 3)];
			}
		}
		return tempMatrix;
	}

	template<typename TYPE>
	Matrix33<TYPE> Inverse(Matrix33<TYPE> &aMatrix)
	{
		Vector3<TYPE> theTranslation;

		theTranslation = aMatrix.GetTranslation();
		theTranslation.x *= -1;
		theTranslation.y *= -1;
		theTranslation.z = 1;

		Matrix33<TYPE> theMatrix(aMatrix);

		theMatrix.SetTranslation(0.0f, 0.0f, 1.0f);
		theMatrix = Transpose(theMatrix);

		theTranslation = theTranslation*theMatrix;
		theMatrix.SetTranslation(theTranslation.x, theTranslation.y, theTranslation.z);

		return theMatrix;
	}

	template<class TYPE>
	Matrix33<TYPE> CreateReflectionMatrixAboutAxis(Vector3<TYPE> reflectionVector)
	{
		Matrix33<TYPE> reflectionMatrix;
		reflectionMatrix.m_Matrix[0] = 1 - 2 * (reflectionVector.x*reflectionVector.x);
		reflectionMatrix.m_Matrix[1] = -2 * (reflectionVector.x*reflectionVector.y);
		reflectionMatrix.m_Matrix[2] = -2 * (reflectionVector.x*reflectionVector.z);

		reflectionMatrix.m_Matrix[3] = -2 * (reflectionVector.y*reflectionVector.x);
		reflectionMatrix.m_Matrix[4] = 1 - 2 * (reflectionVector.y*reflectionVector.y);
		reflectionMatrix.m_Matrix[5] = -2 * (reflectionVector.y*reflectionVector.z);

		reflectionMatrix.m_Matrix[6] = -2 * (reflectionVector.z*reflectionVector.x);
		reflectionMatrix.m_Matrix[7] = -2 * (reflectionVector.z*reflectionVector.y);
		reflectionMatrix.m_Matrix[8] = 1 - 2 * (reflectionVector.z*reflectionVector.z);
		return reflectionMatrix;
	};

#pragma endregion

#pragma endregion

};

#pragma once
#include <cassert>
#include <xmmintrin.h>
namespace Core
{
	template<typename TYPE>
	class Matrix44
	{
	public:
		Matrix44();
		Matrix44(const Matrix44<TYPE>& aMatrix);
		Matrix44(TYPE mat[16])
			: myMatrix(mat)
		{
		}
		~Matrix44();

		Matrix44& operator=(const Matrix44& aMatrix);

		const TYPE& operator[](const int& anIndex) const;
		TYPE& operator[](const int& anIndex);

		float GetXRotation() const;
		float GetYRotation() const;
		float GetZRotation() const;
		static Matrix44<TYPE> CreateRotateAroundX(const TYPE& aRadian);
		static Matrix44<TYPE> CreateRotateAroundY(const TYPE& aRadian);
		static Matrix44<TYPE> CreateRotateAroundZ(const TYPE& aRadian);
		static Matrix44<TYPE> CreateProjectionMatrixLH(TYPE aNearZ, TYPE aFarZ, TYPE anAspectRatio, TYPE aFoVAngle);

		static Matrix44<TYPE> CreateOrthogonalMatrixLH(TYPE aWidth, TYPE aHeight, TYPE aNearZ, TYPE aFarZ);
		static Matrix44<TYPE> CreateOrthographicMatrixLH(float width, float height, float near_plane, float far_plane);

		static Matrix44<TYPE> CreateScaleMatrix(const Vector4<TYPE>& scale);
		static Matrix44<TYPE> CreateScaleMatrix(const float x, const float y, const float z, const float w);
		void RotateAroundPointX(const Core::Vector3f& point, float radian, float dt);
		void RotateAroundPointY(const Core::Vector3f& point, float radian, float dt);
		void RotateAroundPointZ(const Core::Vector3f& point, float radian, float dt);
		void RotateAroundPointX(const Core::Vector3f& point, float radian);
		void RotateAroundPointY(const Core::Vector3f& point, float radian);
		void RotateAroundPointZ(const Core::Vector3f& point, float radian);

		void SetPerspectiveFOV(float fov, float aspect_ratio);

		void SetOrthographicProjection(float width, float height, float near_plane, float far_plane);

		//Matrix44<TYPE>& operator*=(const Matrix44<TYPE>& rhs);


#pragma warning( push )
#pragma warning( disable: 4201 )
		union
		{
			__declspec(align(16))TYPE myMatrix[16];
			TYPE mat[4][4];
			Vector4<TYPE> rows[4];
			struct {
				TYPE m00, m01, m02, m03;
				TYPE m10, m11, m12, m13;
				TYPE m20, m21, m22, m23;
				TYPE m30, m31, m32, m33;
			};
		};
#pragma warning( pop )


		void ConvertFromCol(const TYPE aColMatrix[16]);
		void InitWithArray(const TYPE aColMatrix[16]);
		static Matrix44<TYPE> Transpose(const Matrix44<TYPE>& mat);

		Vector3<TYPE> GetRadRotations();
		Vector3<TYPE> GetGradRotations();


		void SetRotation3dX(const TYPE& aRadian);
		void SetRotation3dY(const TYPE& aRadian);
		void SetRotation3dZ(const TYPE& aRadian);

		void SetTranslation(const TYPE& anX, const TYPE& anY, const TYPE& anZ, const TYPE& anW);
		void SetTranslation(const Vector4<TYPE>& aVector);
		const Vector4<TYPE> GetTranslation() const;
		const Vector3<TYPE> GetPosition() const;

		void SetForward(const Vector4<TYPE>& aVector);
		void SetRight(const Vector4<TYPE>& aVector);
		void SetUp(const Vector4<TYPE>& aVector);
		void SetPosition(const Vector3<TYPE>& aVector);
		void SetPosition(const Vector4<TYPE>& aVector);

		const Vector4<TYPE> GetForward() const;
		const Vector4<TYPE> GetRight() const;
		const Vector4<TYPE> GetUp() const;
		const Vector4<TYPE> GetScale() const;

		void LookAt(const Vector3<TYPE>& eye, const Vector3<TYPE>& target, const Vector3<TYPE>& up);

		const Matrix44<TYPE> Inverse(Matrix44<TYPE>& aMatrix);
		void Init(TYPE* aMatrix)
		{
			myMatrix[0] = aMatrix[0];
			myMatrix[1] = aMatrix[1];
			myMatrix[2] = aMatrix[2];
			myMatrix[3] = aMatrix[3];

			myMatrix[4] = aMatrix[4];
			myMatrix[5] = aMatrix[5];
			myMatrix[6] = aMatrix[6];
			myMatrix[7] = aMatrix[7];

			myMatrix[8] = aMatrix[8];
			myMatrix[9] = aMatrix[9];
			myMatrix[10] = aMatrix[10];
			myMatrix[11] = aMatrix[11];

			myMatrix[12] = aMatrix[12];
			myMatrix[13] = aMatrix[13];
			myMatrix[14] = aMatrix[14];
			myMatrix[15] = aMatrix[15];
		}

		bool operator==(const Matrix44<TYPE>& m0);

		Matrix44<TYPE> operator~()
		{
			return InverseReal(*this);
		}



		const Vector4<TYPE> GetColumn(int index) const;
	private:

		enum class RotationType
		{
			Axis_X,
			Axis_Y,
			Axis_Z
		};
		const Matrix44<TYPE> Calculate(const RotationType& rotation, const TYPE& cos, const TYPE& sin);
	};

	template<typename TYPE>
	void Matrix44<TYPE>::SetOrthographicProjection(float width, float height, float near_plane, float far_plane)
	{
		myMatrix[0] = 2.f / width;

		myMatrix[5] = 2.f / height;

		myMatrix[10] = 1.f / (far_plane - near_plane);

		myMatrix[14] = near_plane / (near_plane - far_plane);
		myMatrix[15] = 1.f;
	}

	template<typename TYPE>
	void Matrix44<TYPE>::LookAt(const Vector3<TYPE>& eye, const Vector3<TYPE>& target, const Vector3<TYPE>& up)
	{
		Vector3<TYPE> z = GetNormalized(eye - target);
		Vector3<TYPE> x = GetNormalized(Cross(up, z));
		Vector3<TYPE> y = Cross(z, x);


		m00 = x.x;			m01 = y.x;			m02 = z.x;			m03 = 0.f;//-Dot(x, eye);
		m10 = x.y;			m11 = y.y;			m12 = z.y;			m13 = 0.f;//-Dot(y, eye);
		m20 = x.z;			m21 = y.z;			m22 = z.z;			m23 = 0.f;//-Dot(z, eye);
		m30 = 0;			m31 = 0;			m32 = 0;			m33 = 1.f;

		*this = Transpose(*this);

	}

	template<typename TYPE>
	const Vector4<TYPE> Matrix44<TYPE>::GetColumn(int index) const
	{
		assert(index < 4 && index >= 0 && "invalid case!");
		Vector4<TYPE> out;
		switch (index)
		{
		case 0:
			return Vector4<TYPE>(myMatrix[0], myMatrix[4], myMatrix[8], myMatrix[12]);
		case 1:
			return Vector4<TYPE>(myMatrix[1], myMatrix[5], myMatrix[9], myMatrix[13]);
		case 2:
			return Vector4<TYPE>(myMatrix[2], myMatrix[6], myMatrix[10], myMatrix[14]);
		case 3:
			return Vector4<TYPE>(myMatrix[3], myMatrix[7], myMatrix[11], myMatrix[15]);
		}
		return Vector4<TYPE>();
	}

	template<typename TYPE>
	Matrix44<TYPE> Matrix44<TYPE>::Transpose(const Matrix44<TYPE>& mat)
	{
		Matrix44<TYPE> result = mat;
		std::swap(result.myMatrix[1], result.myMatrix[4]);
		std::swap(result.myMatrix[2], result.myMatrix[8]);
		std::swap(result.myMatrix[3], result.myMatrix[12]);
		std::swap(result.myMatrix[6], result.myMatrix[9]);
		std::swap(result.myMatrix[7], result.myMatrix[13]);
		std::swap(result.myMatrix[11], result.myMatrix[14]);
		return result;
	}

	template<typename TYPE>
	bool Matrix44<TYPE>::operator==(const Matrix44<TYPE>& m0)
	{
		for (unsigned int i = 0; i < 16; i++)
		{
			if (m0[i] != myMatrix[i])
				return false;
		}

		return true;
	}



	template<typename TYPE>
	Vector3<TYPE> Matrix44<TYPE>::GetGradRotations()
	{
		Vector3<TYPE> output;
		output.x = (-atan2(myMatrix[9], myMatrix[10]))  * (180.f / 3.1415926535f);
		output.y = (atan2(myMatrix[8], sqrt((myMatrix[9] * myMatrix[9]) + (myMatrix[10] * myMatrix[10]))))  * (180.f / 3.1415926535f);
		output.z = (-atan2(myMatrix[4], myMatrix[0])) * (180.f / 3.1415926535f);
		return output;
	}

	template<typename TYPE>
	Vector3<TYPE> Matrix44<TYPE>::GetRadRotations()
	{
		Vector3<TYPE> output;
		output.x = -atan2(myMatrix[9], myMatrix[10]);
		output.y = atan2(myMatrix[8], myMatrix[0]);
		output.z = -atan2(myMatrix[4], myMatrix[0]);
		return output;
	}

	template<typename TYPE>
	void Core::Math::Matrix44<TYPE>::RotateAroundPointZ(const Core::Vector3f& point, float radian, float dt)
	{
		Core::Vector3f original_pos = GetPosition();
		SetPosition(original_pos - point);

		*this = *this * Core::Matrix44f::CreateRotateAroundZ(radian * dt);
		SetPosition(GetPosition() + point);
	}

	template<typename TYPE>
	void Core::Math::Matrix44<TYPE>::RotateAroundPointY(const Core::Vector3f& point, float radian, float dt)
	{
		Core::Vector3f original_pos = GetPosition();
		SetPosition(original_pos - point);

		*this = *this * Core::Matrix44f::CreateRotateAroundY(radian * dt);
		SetPosition(GetPosition() + point);
	}

	template<typename TYPE>
	void Core::Math::Matrix44<TYPE>::RotateAroundPointX(const Core::Vector3f& point, float radian, float dt)
	{
		Core::Vector3f original_pos = GetPosition();
		SetPosition(original_pos - point);

		*this = *this * Core::Matrix44f::CreateRotateAroundX(radian * dt);
		SetPosition(GetPosition() + point);
	}


	template<typename TYPE>
	void Core::Math::Matrix44<TYPE>::RotateAroundPointZ(const Core::Vector3f& point, float radian)
	{
		Core::Vector3f original_pos = GetPosition();
		SetPosition(original_pos - point);

		*this = *this * Core::Matrix44f::CreateRotateAroundZ(radian);
		SetPosition(GetPosition() + point);
	}

	template<typename TYPE>
	void Core::Math::Matrix44<TYPE>::RotateAroundPointY(const Core::Vector3f& point, float radian)
	{
		Core::Vector3f original_pos = GetPosition();
		SetPosition(original_pos - point);

		*this = *this * Core::Matrix44f::CreateRotateAroundY(radian);
		SetPosition(GetPosition() + point);
	}

	template<typename TYPE>
	void Core::Math::Matrix44<TYPE>::RotateAroundPointX(const Core::Vector3f& point, float radian)
	{
		Core::Vector3f original_pos = GetPosition();
		SetPosition(original_pos - point);

		*this = *this * Core::Matrix44f::CreateRotateAroundX(radian);
		SetPosition(GetPosition() + point);
	}

	template<typename TYPE>
	Matrix44<TYPE> Core::Math::Matrix44<TYPE>::CreateOrthographicMatrixLH(float width, float height, float near_plane, float far_plane)
	{
		Matrix44<TYPE> new_matrix;
		new_matrix.SetOrthographicProjection(width, height, near_plane, far_plane);
		return new_matrix;
	}


	template<typename TYPE>
	void Core::Math::Matrix44<TYPE>::InitWithArray(const TYPE aColMatrix[16])
	{
		for (int i = 0; i < 16; i++)
		{
			myMatrix[i] = aColMatrix[i];
		}
	}

	template<typename TYPE>
	void Core::Math::Matrix44<TYPE>::ConvertFromCol(const TYPE aColMatrix[16])
	{
		myMatrix[1] = aColMatrix[4];
		myMatrix[4] = aColMatrix[1];

		myMatrix[2] = aColMatrix[8];
		myMatrix[8] = aColMatrix[2];

		myMatrix[3] = aColMatrix[12];
		myMatrix[12] = aColMatrix[3];

		myMatrix[6] = aColMatrix[9];
		myMatrix[9] = aColMatrix[6];

		myMatrix[7] = aColMatrix[13];
		myMatrix[13] = aColMatrix[7];

		myMatrix[11] = aColMatrix[14];
		myMatrix[14] = aColMatrix[11];

	}

	template<typename TYPE>
	inline const TYPE& Core::Math::Matrix44<TYPE>::operator[](const int& anIndex) const
	{
		return myMatrix[anIndex];
	}

	template<typename TYPE>
	inline TYPE& Core::Math::Matrix44<TYPE>::operator[](const int& anIndex)
	{
		return myMatrix[anIndex];
	}


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

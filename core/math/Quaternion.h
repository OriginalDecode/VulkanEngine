#pragma once
#include "../Vector/Vector.h"
#include "../Matrix/Matrix.h"
namespace Core
{
	class Quaternion
	{
	public:
		Quaternion();
		Quaternion(const Core::Vector3f& aNormal, float anAngle);

		Quaternion Inverted() const;

		Quaternion operator*(const Quaternion& aQuaternion) const;
		
		Core::Vector3f operator*(const Core::Vector3f& aVector) const;
		Quaternion operator^(float aT) const;

		Quaternion Slerp(const Quaternion& other, float aT) const;

		Core::Matrix44f ConvertToRotationMatrix();
		void operator-=(const Core::Quaternion& quat)
		{
			v -= quat.v;
			w -= w;
		}
	private:
		void ToAxisAngle(Core::Vector3f& aVectorAxisOut, float& anAngleOut) const;
		Core::Vector3f v;
		float w;
	};

}
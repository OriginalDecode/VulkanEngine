#include "Quaternion.h"
#include <cmath>
namespace Core
{

	Quaternion::Quaternion()
		: v(0, 0, 0)
		, w(1.f)
	{
	}

	Quaternion::Quaternion(const Core::Vector3f& aNormal, float anAngle)
		: v(aNormal * sinf(anAngle * 0.5f))
		, w(cosf(anAngle * 0.5f))
	{
	}

	Quaternion Quaternion::Inverted() const
	{
		Quaternion q;

		q.w = w;
		q.v.x = -v.x;
		q.v.y = -v.y;
		q.v.z = -v.z;

		return q;
	}

	Quaternion Quaternion::operator*(const Quaternion& aQuaternion) const
	{
		Quaternion r;
		r.w = w * aQuaternion.w - Dot(v, aQuaternion.v);
		r.v = v * aQuaternion.w + aQuaternion.v * w + Cross(v, aQuaternion.v);

		return r;
	}

	Core::Vector3f Quaternion::operator*(const Core::Vector3f& aVector) const
	{
		Core::Vector3f vcV = Cross(v, aVector);
		return aVector + vcV * (2.f * w) + Cross(v, vcV) * 2.f;
	}

	Quaternion Quaternion::operator^(float aT) const
	{
		float a;
		Core::Vector3f n;

		ToAxisAngle(n, a);

		float aTimesT = a * aT;

		return Quaternion(n, aTimesT);
	}

	Quaternion Quaternion::Slerp(const Quaternion& other, float aT) const
	{
		return ((other * Inverted()) ^ aT) * (*this);
	}

	Core::Matrix44f Quaternion::ConvertToRotationMatrix()
	{
		float qx = v.x;
		float qy = v.y;
		float qz = v.z;
		float qw = w;

		const float n = 1.f / sqrt(
			(qx*qx) +
			(qy*qy) +
			(qz*qz) +
			(qw*qw));

		qx *= n;
		qy *= n;
		qz *= n;
		qw *= n;

		Core::Matrix44f matrix;
		matrix[0] = 1.0f - 2.0f*qy*qy - 2.0f*qz*qz;
		matrix[1] = 2.0f*qx*qy - 2.0f*qz*qw;
		matrix[2] = 2.0f*qx*qz + 2.0f*qy*qw, 0.0f;
		matrix[3] = 2.0f*qx*qy + 2.0f*qz*qw;

		matrix[4] = 1.0f - 2.0f*qx*qx - 2.0f*qz*qz;
		matrix[5] = 2.0f*qy*qz - 2.0f*qx*qw;
		matrix[6] = 0.0f;
		matrix[7] = 2.0f*qx*qz - 2.0f*qy*qw;

		matrix[8] = 2.0f*qy*qz + 2.0f*qx*qw;
		matrix[9] = 1.0f - 2.0f*qx*qx - 2.0f*qy*qy;
		matrix[10] = 0.0f;

		matrix.rows[3] = Core::Vector4f(0.f, 0.f, 0.f, 1.0f);
				
		return matrix;
	}

	void Quaternion::ToAxisAngle(Core::Vector3f& aVectorAxisOut, float& anAngleOut) const
	{
		if (aVectorAxisOut.Length2() < 0.0001f)
		{
			aVectorAxisOut = Core::Vector3f(1.f, 0, 0);
		}
		else
		{
			aVectorAxisOut = GetNormalized(v);
		}

		anAngleOut = acos(w) * 2.f;
	}
}

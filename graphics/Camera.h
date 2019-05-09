#pragma once
#include "Core/math/Matrix44.h"
namespace Graphics
{
	class Camera
	{
	public:
		Camera() = default;
		~Camera() = default;

		void InitOrthographicProjection(float width, float height, float near_plane, float far_plane);
		void InitOrthogonalProjection(float width, float height, float near_plane, float far_plane);
		void InitPerspectiveProjection(float width, float height, float near_plane, float far_plane, float fov);

		void Update();

		Core::Matrix44f* GetViewProjectionPointer();

	private:
		Core::Matrix44f m_ProjectionMatrix = Core::Matrix44f::Identity();
		Core::Matrix44f m_ViewMatrix = Core::Matrix44f::Identity();
		Core::Matrix44f m_ViewProjection = Core::Matrix44f::Identity();
	};
};
#include "Camera.h"


namespace Graphics
{
	void Graphics::Camera::InitOrthographicProjection(float width, float height, float near_plane, float far_plane)
	{
		m_ProjectionMatrix = Core::Matrix44f::CreateOrthogonalMatrixLH(width,height, near_plane, far_plane);
	}

	void Graphics::Camera::InitOrthogonalProjection(float width, float height, float near_plane, float far_plane)
	{
		m_ProjectionMatrix = Core::Matrix44f::CreateOrthogonalMatrixLH(width, height, near_plane, far_plane);
	}

	void Graphics::Camera::InitPerspectiveProjection(float width, float height, float near_plane, float far_plane, float fov)
	{
		m_ProjectionMatrix = Core::Matrix44f::CreateProjectionMatrixLH(near_plane, far_plane, height / width, fov * 3.1415f / 180.f);
	}

	void Graphics::Camera::Update() // called once per frame
	{
		m_ViewProjection = Core::Matrix44f::Inverse(m_ViewMatrix) * m_ProjectionMatrix;
	}

	Core::Matrix44f* Camera::GetViewProjectionPointer()
	{
		return &m_ViewProjection;
	}

};
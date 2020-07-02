#include "pch.h"

#include "PBR/Engine/Camera.h"


namespace EngineCore
{

	Camera::Camera()
	{

	}

	Camera::~Camera()
	{

	}

	void Camera::GetShaderParameters(CameraUniformStruct& camera) const
	{
		camera.Position = CameraTransform.GetPosition();
		camera.ViewMatrix = CameraTransform.Inverse();
		camera.ProjectionMatrix = ProjectionMatrix;
	}

}
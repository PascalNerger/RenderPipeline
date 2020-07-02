#pragma once

#include <glm/glm.hpp>

#include "PBR/Core/BaseTypes.h"
#include "PBR/Core/RefCounting.h"
#include "PBR/Core/Transform.h"


namespace EngineCore
{

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Camera //////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	struct CameraUniformStruct
	{

		alignas(16) glm::fvec3 Position;
		alignas(64) glm::fmat4 ViewMatrix;
		alignas(64) glm::fmat4 ProjectionMatrix;

	};

	class Camera : public RefCountedObject
	{

	public:

		Camera();
		virtual ~Camera();

		void GetShaderParameters(CameraUniformStruct& camera) const;


		inline const Transform& GetTransform() const { return CameraTransform; }
		inline Transform& GetTransform() { return CameraTransform; }

		inline void SetTransform(const Transform& transform) { CameraTransform = transform; }

		inline const glm::vec3& GetPosition() const { return CameraTransform.GetPosition(); }
		inline glm::mat4 GetViewMatrix() const { return CameraTransform.Inverse(); }
		inline const glm::mat4& GetProjectionMatrix() const { return ProjectionMatrix; }

		inline void SetProjectionMatrix(const glm::mat4& projectionMatrix) { ProjectionMatrix = projectionMatrix; }

	private:

		Transform CameraTransform;
		glm::mat4 ProjectionMatrix;

	};

	using CameraRef = Ref<Camera>;

}
#pragma once

#include "PBR/Core/RefCounting.h"
#include "PBR/Core/Transform.h"

#include "PBR/Engine/Light.h"

namespace EngineCore
{
	
	class Sun : public RefCountedObject
	{

	public:

		Sun();
		virtual ~Sun();

		void GetShaderParameters(LightUniformStruct& light) const;

		inline void SetTransform(const Transform& transform) { SunTransform = transform; }
		inline void SetProjection(const glm::mat4& projection) { SunProjection = projection; }
		inline void SetColor(const glm::vec3& color) { Color = color; }
		inline void SetShadowsEnabled(const bool value) { bShadowsEnabled = value; }

		inline const Transform& GetTransform() const { return SunTransform; }
		inline Transform& GetTransform() { return SunTransform; }

		inline glm::mat4 GetView() const { return SunTransform.Inverse(); }

		inline const glm::mat4& GetProjection() const { return SunProjection; }
		inline const glm::vec3& GetColor() const { return Color; }

	private:

		Transform SunTransform;
		glm::mat4 SunProjection;

		glm::vec3 Color;

		bool bShadowsEnabled;

	};

	using SunRef = Ref<Sun>;

}
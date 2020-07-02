#pragma once

#include <glm/glm.hpp>

#include "PBR/Core/BaseTypes.h"
#include "PBR/Core/RefCounting.h"
#include "PBR/Core/Transform.h"

namespace EngineCore
{

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Light ///////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	enum class LightType : uint32
	{

		None		= 0,
		Directional = 1,
		Point		= 2,
		Spot		= 3

	};

	struct LightUniformStruct
	{

		alignas(16) int32 Type;

		alignas(16) glm::fvec3 Color;
		alignas(16) glm::fvec3 Position;
		alignas(16) glm::fvec3 Direction;

	};

	class Light : public RefCountedObject
	{

	public:

		Light();
		virtual ~Light();

		void GetShaderParameters(LightUniformStruct& light) const;

		bool Affects(const glm::vec3& point) const;

		inline LightType GetType() const { return Type; }
		inline const glm::vec3& GetColor() const { return Color; }

		inline const Transform& GetTransform() const { return LightTransform; }
		inline Transform& GetTransform() { return LightTransform; }

		inline float GetRadius() const { return Radius; }
		inline bool CastsShadows() const { return bCastsShadows; }

		inline void SetType(const LightType type) { Type = type; }
		inline void SetColor(const glm::vec3& color) { Color = color; }
		inline void SetTransform(const Transform& transform) { LightTransform = transform; }
		inline void SetRadius(const float radius) { Radius = radius; }
		inline void SetCastsShadows(const bool value) { bCastsShadows = value; }

	private:

		bool DirectionalLightAffects(const glm::vec3& point) const;
		bool PointLightAffects(const glm::vec3& point) const;
		bool SpotLightAffects(const glm::vec3& point) const;

	private:

		LightType Type;

		glm::vec3 Color;

		Transform LightTransform;

		float Radius;
		bool bCastsShadows;

	};

	using LightRef = Ref<Light>;

}
#include "pch.h"

#include "PBR/Engine/Light.h"


namespace EngineCore
{

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Light ///////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	Light::Light()
		: Type(LightType::None)
		, Color()
		, LightTransform()
		, Radius(0.0f)
		, bCastsShadows(false)
	{

	}

	Light::~Light()
	{

	}

	void Light::GetShaderParameters(LightUniformStruct& light) const
	{
		light.Type = int32(Type);
		light.Color = Color;
		light.Position = LightTransform.GetPosition();
		light.Direction = LightTransform.GetForward();
	}

	bool Light::Affects(const glm::vec3& point) const
	{
		switch (Type)
		{
			case EngineCore::LightType::Directional:	return DirectionalLightAffects(point);
			case EngineCore::LightType::Point:			return PointLightAffects(point);
			case EngineCore::LightType::Spot:			return SpotLightAffects(point);

			default: break;
		}

		return false;
	}

	bool Light::DirectionalLightAffects(const glm::vec3& point) const
	{
		return true;
	}

	bool Light::PointLightAffects(const glm::vec3& point) const
	{
		float x = std::pow(point.x - LightTransform.GetPosition().x, 2.0f);
		float y = std::pow(point.y - LightTransform.GetPosition().y, 2.0f);
		float z = std::pow(point.z - LightTransform.GetPosition().z, 2.0f);

		float distance = x + y + z;
		return distance < Radius;
	}

	bool Light::SpotLightAffects(const glm::vec3& point) const
	{
		// TODO:
		return false;
	}

}
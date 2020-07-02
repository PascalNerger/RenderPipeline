#include "pch.h"

#include "PBR/Engine/Sun.h"


namespace EngineCore
{

	Sun::Sun()
		: SunTransform()
		, SunProjection(glm::mat4(1.0f))
		, bShadowsEnabled(true)
	{

	}

	Sun::~Sun()
	{

	}

	void Sun::GetShaderParameters(LightUniformStruct& light) const
	{
		light.Type = int32(LightType::Directional);
		light.Color = Color;
		light.Position = SunTransform.GetPosition();
		light.Direction = SunTransform.GetForward();
	}

}
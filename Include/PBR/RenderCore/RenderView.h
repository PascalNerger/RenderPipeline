#pragma once

#include <vector>

#include "PBR/Engine/Camera.h"
#include "PBR/Engine/Light.h"
#include "PBR/Engine/Sun.h"

#include "PBR/RenderCore/RenderPrimitive.h"


namespace EngineCore
{

	class RenderView
	{

	public:

		RenderView();
		~RenderView();

		void AddLight(const LightRef& light);
		void AddPrimitive(const RenderPrimitive& primitive);
		void AddInstancedPrimitive(const InstancedPrimitive& primitive);

		inline void SetCamera(const CameraRef& camera) { Camera = camera; }
		inline void SetSun(const SunRef& sun) { Sun = sun; }

		inline const CameraRef& GetCamera() const { return Camera; }
		inline const SunRef& GetSun() const { return Sun; }

		inline const std::vector<LightRef>& GetLights() const { return Lights; }
		inline const std::vector<RenderPrimitive>& GetRenderPrimitives() const { return RenderPrimitives; }
		inline const std::vector<InstancedPrimitive>& GetInstancedPrimitves() const { return InstancedPrimitives; }

	private:

		CameraRef Camera;
		SunRef Sun;

		std::vector<LightRef> Lights;
		std::vector<RenderPrimitive> RenderPrimitives;
		std::vector<InstancedPrimitive> InstancedPrimitives;

	};

}

#include "pch.h"

#include "PBR/RenderCore/RenderView.h"


namespace EngineCore
{

	RenderView::RenderView()
	{

	}

	RenderView::~RenderView()
	{

	}

	void RenderView::AddLight(const LightRef& light)
	{
		Lights.push_back(light);
	}

	void RenderView::AddPrimitive(const RenderPrimitive& primitive)
	{
		RenderPrimitives.push_back(primitive);
	}

	void RenderView::AddInstancedPrimitive(const InstancedPrimitive& primitive)
	{
		InstancedPrimitives.push_back(primitive);
	}

}
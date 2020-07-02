#pragma once

#include <vector>

#include "PBR/RenderCore/RenderPrimitive.h"


namespace EngineCore
{

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// RenderPrimitiveGroup ////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	class RenderPrimitiveGroup
	{

	public:

		RenderPrimitiveGroup(const MaterialRef& material);
		~RenderPrimitiveGroup();

		void Add(const RenderPrimitive& primitive);
		void OrderByDistance(const glm::vec3& origin);

		inline const MaterialRef& GetMaterial() const { return Material; }
		inline const std::vector<RenderPrimitive>& GetPrimitives() const { return RenderPrimitives; }


		inline const std::vector<RenderPrimitive>::iterator begin() { return RenderPrimitives.begin(); }
		inline const std::vector<RenderPrimitive>::iterator end() { return RenderPrimitives.end(); }

		inline const std::vector<RenderPrimitive>::const_iterator cbegin() { return RenderPrimitives.cbegin(); }
		inline const std::vector<RenderPrimitive>::const_iterator cend() { return RenderPrimitives.cend(); }

	private:

		MaterialRef Material;
		std::vector<RenderPrimitive> RenderPrimitives;

	};

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// RenderQueue /////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	class RenderQueue
	{

	public:

		RenderQueue();
		~RenderQueue();

		void Submit(const RenderPrimitive& primitive);
		void OrderByPrimitiveCount();
		void Clear();

		std::vector<RenderPrimitiveGroup>& GetPrimitiveGroups() { return RenderPrimitiveGroups; }

	private:

		RenderPrimitiveGroup& RegisterRenderGroup(const MaterialRef& material);

	private:

		std::vector<RenderPrimitiveGroup> RenderPrimitiveGroups;

	};

}
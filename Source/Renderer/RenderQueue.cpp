#include "pch.h"

#include "PBR/RenderCore/RenderQueue.h"


namespace EngineCore
{


	RenderPrimitiveGroup::RenderPrimitiveGroup(const MaterialRef& material)
		: Material(material)
	{

	}

	RenderPrimitiveGroup::~RenderPrimitiveGroup()
	{

	}

	void RenderPrimitiveGroup::Add(const RenderPrimitive& primitive)
	{
		if (primitive.GetMaterial() != Material)
		{
			assert(false);
			return;
		}

		RenderPrimitives.push_back(primitive);
	}

	void RenderPrimitiveGroup::OrderByDistance(const glm::vec3& origin)
	{
		struct DistanceFunctor
		{

		public:

			DistanceFunctor(const glm::vec3& origin)
				: Origin(origin)
			{
			}

			bool operator()(const RenderPrimitive& lhs, const RenderPrimitive& rhs) const
			{
				const glm::vec3& lhsPos = lhs.GetTransform().GetPosition();
				const glm::vec3& rhsPos = rhs.GetTransform().GetPosition();

				float lhsDistance = glm::distance(Origin, lhsPos);
				float rhsDistance = glm::distance(Origin, rhsPos);

				return lhsDistance < rhsDistance;
			}

		private:

			glm::vec3 Origin;

		};

		std::sort(RenderPrimitives.begin(), RenderPrimitives.end(), DistanceFunctor(origin));
	}





	RenderQueue::RenderQueue()
	{

	}

	RenderQueue::~RenderQueue()
	{

	}

	void RenderQueue::Submit(const RenderPrimitive& primitive)
	{
		MaterialRef material = primitive.GetMaterial();
		RenderPrimitiveGroup& group = RegisterRenderGroup(material);

		group.Add(primitive);
	}

	void RenderQueue::OrderByPrimitiveCount()
	{
		struct PrimitiveCountOrderFunctor
		{

			bool operator()(const RenderPrimitiveGroup& lhs, const RenderPrimitiveGroup& rhs) const
			{
				return lhs.GetPrimitives().size() > rhs.GetPrimitives().size();
			}

		};

		std::sort(RenderPrimitiveGroups.begin(), RenderPrimitiveGroups.end(), PrimitiveCountOrderFunctor());
	}

	void RenderQueue::Clear()
	{
		RenderPrimitiveGroups.clear();
	}

	RenderPrimitiveGroup& RenderQueue::RegisterRenderGroup(const MaterialRef& material)
	{
		struct GroupFindFunctor
		{

		public:

			GroupFindFunctor(const MaterialRef& material)
				: Material(material)
			{
			}

			bool operator()(const RenderPrimitiveGroup& group) const
			{
				return group.GetMaterial() == Material;
			}

		private:

			const MaterialRef Material;

		};

		auto it = std::find_if(RenderPrimitiveGroups.begin(), RenderPrimitiveGroups.end(), GroupFindFunctor(material));
		if (it != RenderPrimitiveGroups.end())
			return *it;

		RenderPrimitiveGroups.push_back(RenderPrimitiveGroup(material));
		return RenderPrimitiveGroups.back();
	}

}
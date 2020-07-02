#pragma once

#include "PBR/Core/Transform.h"

#include "PBR/Engine/Mesh.h"
#include "PBR/Engine/Material.h"

namespace EngineCore
{

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// RenderPrimitive /////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	class RenderPrimitive
	{

	public:

		RenderPrimitive();
		~RenderPrimitive();

		inline void SetMesh(const MeshRef& mesh) { Mesh = mesh; }
		inline void SetMaterial(const MaterialRef& material) { Material = material; }
		inline void SetTransform(const Transform& transform) { Transformation = transform; }

		inline const MeshRef& GetMesh() const { return Mesh; }
		inline const MaterialRef& GetMaterial() const { return Material; }

		inline const Transform& GetTransform() const { return Transformation; }
		inline Transform& GetTransform() { return Transformation; }

	private:

		MeshRef Mesh;
		MaterialRef Material;
		
		Transform Transformation;

	private:

		friend class Renderer;

	};

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// InstancedPrimitive //////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	class InstancedPrimitive
	{

	public:

		InstancedPrimitive();
		~InstancedPrimitive();

		inline void SetMesh(const InstancedMeshRef& mesh) { Mesh = mesh; }
		inline void SetMaterial(const MaterialRef& material) { Material = material; }

		inline const InstancedMeshRef& GetMesh() const { return Mesh; }
		inline const MaterialRef& GetMaterial() const { return Material; }

	private:

		InstancedMeshRef Mesh;
		MaterialRef Material;

	private:

		friend class Renderer;

	};

}
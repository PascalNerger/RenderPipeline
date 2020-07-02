#pragma once

#include <vector>

#include "PBR/Core/BaseTypes.h"
#include "PBR/Core/RefCounting.h"


namespace EngineCore
{

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// ShaderDataType //////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	enum class ShaderDataType
	{

		None = 0,

		Bool = 1,

		Int = 2,
		Int2 = 3,
		Int3 = 4,
		Int4 = 5,

		Float = 6,
		Float2 = 7,
		Float3 = 8,
		Float4 = 9,

		Mat3 = 10,
		Mat4 = 11

	};

	static uint32 GetShaderDataTypeSize(const ShaderDataType type)
	{
		switch (type)
		{
		case ShaderDataType::Int:		return 4;
		case ShaderDataType::Int2:		return 4 * 2;
		case ShaderDataType::Int3:		return 4 * 3;
		case ShaderDataType::Int4:		return 4 * 4;

		case ShaderDataType::Float:		return 4;
		case ShaderDataType::Float2:	return 4 * 2;
		case ShaderDataType::Float3:	return 4 * 3;
		case ShaderDataType::Float4:	return 4 * 4;

		case ShaderDataType::Mat3:		return 4 * 3 * 3;
		case ShaderDataType::Mat4:		return 4 * 4 * 4;
		case ShaderDataType::Bool:		return 1;

		default: break;
		}

		return 0;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// BufferElement ///////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	struct BufferElement
	{

		ShaderDataType Type;

		uint32 Size;
		uint32 Offset;

		bool Normalized;
		int32 Divisor;


		BufferElement()
			: Type()
			, Size()
			, Offset()
			, Normalized()
			, Divisor()
		{
		}

		BufferElement(const ShaderDataType type, const bool normalized = false)
			: Type(type)
			, Size(GetShaderDataTypeSize(type))
			, Offset(0)
			, Normalized(normalized)
			, Divisor(-1)
		{
		}

		BufferElement(const ShaderDataType type, const int32 divisor, const bool normalized = false)
			: Type(type)
			, Size(GetShaderDataTypeSize(type))
			, Offset(0)
			, Normalized(normalized)
			, Divisor(divisor)
		{
		}

		BufferElement(const ShaderDataType type, const uint32 size, const uint32 offset, const bool normalized, const int32 divisor)
			: Type(type)
			, Size(size)
			, Offset(offset)
			, Normalized(normalized)
			, Divisor(divisor)
		{
		}

		~BufferElement()
		{
		}

		uint32 GetComponentCount() const
		{
			switch (Type)
			{
			case ShaderDataType::Int:		return 1;
			case ShaderDataType::Int2:		return 2;
			case ShaderDataType::Int3:		return 3;
			case ShaderDataType::Int4:		return 4;

			case ShaderDataType::Float:		return 1;
			case ShaderDataType::Float2:	return 2;
			case ShaderDataType::Float3:	return 3;
			case ShaderDataType::Float4:	return 4;

			case ShaderDataType::Mat3:		return 3 * 3;
			case ShaderDataType::Mat4:		return 4 * 4;

			case ShaderDataType::Bool:		return 1;

			default: break;
			}

			return 0;
		}

	};

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// BufferLayout ////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	class BufferLayout
	{

	public:

		BufferLayout()
			: Elements()
			, Stride(0)
		{

		}

		BufferLayout(const std::initializer_list<BufferElement>& elements)
			: Elements(elements)
			, Stride(0)
		{
			CalculateStrideAndOffsets();
		}

		void Combine(const BufferLayout& other)
		{
			for (const auto& element : other)
				Elements.emplace_back(element.Type, element.Size, Stride + element.Offset, element.Normalized, element.Divisor);

			Stride += other.Stride;
		}

		inline const std::vector<BufferElement>& GetElements() const { return Elements; }
		inline uint32 GetStride() const { return Stride; }

		std::vector<BufferElement>::iterator begin() { return Elements.begin(); }
		std::vector<BufferElement>::iterator end() { return Elements.end(); }
		std::vector<BufferElement>::const_iterator begin() const { return Elements.cbegin(); }
		std::vector<BufferElement>::const_iterator end() const { return Elements.cend(); }

	private:

		void CalculateStrideAndOffsets()
		{
			Stride = 0;
			uint32 offset = 0;

			for (auto& element : Elements)
			{
				element.Offset = offset;

				offset += element.Size;
				Stride += element.Size;
			}
		}

	private:

		std::vector<BufferElement> Elements;
		uint32 Stride;

	};

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// BufferUsage /////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	enum class BufferUsage : uint32
	{

		None = 0,

		Static = 1,
		Dynamic = 2,
		Stream = 3

	};

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// VertexBuffer ////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	struct VertexBuffer : public RefCountedObject
	{

		virtual ~VertexBuffer() { }

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void SetData(const void* data, const uint32 offset, const uint32 size) = 0;

		virtual void SetLayout(const BufferLayout& layout) = 0;
		virtual const BufferLayout& GetLayout() const = 0;


		static Ref<VertexBuffer> Create(const void* data, const uint32 size, const BufferUsage usage = BufferUsage::Static);
		static Ref<VertexBuffer> CreateMapped(const uint32 size, const BufferUsage usage = BufferUsage::Dynamic);

	};

	using VertexBufferRef = Ref<VertexBuffer>;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// IndexBuffer /////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	struct IndexBuffer : public RefCountedObject
	{

		virtual ~IndexBuffer() { }

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual uint32 GetCount() const = 0;


		static Ref<IndexBuffer> Create(const uint32* data, const uint32 count, const BufferUsage usage = BufferUsage::Static);

	};

	using IndexBufferRef = Ref<IndexBuffer>;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// UniformBuffer ///////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	struct UniformBuffer : public RefCountedObject
	{

		virtual ~UniformBuffer() { }

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void BindBlock(const uint32 blockIndex) const = 0;
		virtual void BindBlockRange(const uint32 blockIndex, const uint32 offset, const uint32 size) const = 0;

		virtual void SetData(const void* data, const uint32 size, const uint32 offset = 0) = 0;

		virtual uint32 GetSize() const = 0;


		static Ref<UniformBuffer> Create(const uint32 size, const BufferUsage usage = BufferUsage::Static);
		static Ref<UniformBuffer> Create(const void* data, const uint32 size, const BufferUsage usage = BufferUsage::Static);

	};

	using UniformBufferRef = Ref<UniformBuffer>;

}
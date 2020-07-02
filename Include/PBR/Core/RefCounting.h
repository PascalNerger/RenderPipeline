#pragma once

#include <assert.h>
#include "BaseTypes.h"


namespace EngineCore
{

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// IRefCountedObject ///////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	struct IRefCountedObject
	{

		virtual ~IRefCountedObject() { }

		virtual uint32 AddRef() const = 0;
		virtual uint32 Release() const = 0;
		virtual uint32 GetRefCount() const = 0;

	};

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// RefCountedObject ////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	class RefCountedObject
	{

	public:

		RefCountedObject()
			: NumRefs(0)
		{
		}

		virtual ~RefCountedObject()
		{
			assert(!NumRefs);
		}

		uint32 AddRef() const
		{
			return uint32(++NumRefs);
		}

		uint32 Release() const
		{
			const uint32 refs = uint32(--NumRefs);
			if (refs == 0)
				delete this;

			return refs;
		}

		uint32 GetRefCount() const { return uint32(NumRefs); }

	private:

		mutable int32 NumRefs;

	};

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Ref /////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	template<typename T>
	class Ref
	{

	public:

		typedef T ElementType;

	public:

		Ref()
			: Reference(nullptr)
		{
		}

		Ref(T* reference)
			: Reference(reference)
		{
			if (Reference)
				Reference->AddRef();
		}

		Ref(const Ref& other)
		{
			Reference = other.Reference;
			if (Reference)
				Reference->AddRef();
		}

		Ref(Ref&& other)
		{
			Reference = other.Reference;
			other.Reference = nullptr;
		}

		~Ref()
		{
			if (Reference)
				Reference->Release();
		}

		Ref& operator=(T* reference)
		{
			T* oldRef = Reference;
			Reference = reference;

			if (Reference)
				Reference->AddRef();

			if (oldRef)
				oldRef->Release();

			return *this;
		}

		Ref& operator=(const Ref& other)
		{
			return *this = other.Reference;
		}

		Ref& operator=(Ref&& other)
		{
			if (&other != this)
			{
				T* oldRef = Reference;

				Reference = other.Reference;
				other.Reference = nullptr;

				if (oldRef)
					oldRef->Release();
			}

			return *this;
		}

		T* operator->() const
		{
			return Reference;
		}

		operator T* () const
		{
			return Reference;
		}

		T* GetReference() const
		{
			return Reference;
		}

		bool IsValid() const
		{
			return Reference != nullptr;
		}

		uint32 GetRefCount() const
		{
			uint32 result = 0;
			if (Reference)
			{
				result = Reference->GetRefCount();
				assert(result > 0);
			}

			return result;
		}

		void Swap(Ref& other)
		{
			T* oldRef = Reference;

			Reference = other.Reference;
			other.Reference = oldRef;
		}

		template<typename TFirst, typename TSecond>
		friend Ref<TFirst> RefCast(const Ref<TSecond>& other);

		template<typename TFirst, typename TSecond>
		friend Ref<TFirst> RefDynamicCast(const Ref<TSecond>& other);

	private:

		T* Reference;

	};

	template<typename T>
	bool operator==(const Ref<T>& ref, const Ref<T>& other)
	{
		return ref.GetReference() == other.GetReference();
	}

	template<typename T>
	bool operator!=(const Ref<T>& ref, const Ref<T>& other)
	{
		return ref.GetReference() != other.GetReference();
	}

	template<typename T>
	bool operator==(const Ref<T>& ref, T* ptr)
	{
		return ref.GetReference() == ptr;
	}

	template<typename T>
	bool operator!=(const Ref<T>& ref, T* ptr)
	{
		return ref.GetReference() != ptr;
	}

	template<typename T>
	bool operator==(T* ptr, const Ref<T>& ref)
	{
		return ptr == ref.GetReference();
	}

	template<typename T>
	bool operator!=(T* ptr, const Ref<T>& ref)
	{
		return ptr != ref.GetReference();
	}

	template<typename TFirst, typename TSecond>
	Ref<TFirst> RefCast(const Ref<TSecond>& other)
	{
		using CastType = typename Ref<TFirst>::ElementType;

		const auto raw = static_cast<CastType*>(other.Reference);
		return Ref(raw);
	}

	template<typename TFirst, typename TSecond>
	Ref<TFirst> RefDynamicCast(const Ref<TSecond>& other)
	{
		using CastType = typename Ref<TFirst>::ElementType;

		const auto raw = dynamic_cast<CastType*>(other.Reference);
		if (raw)
			return Ref(raw);

		return Ref<CastType>(nullptr);
	}

}

namespace std
{

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Ref Hash Struct /////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	template<typename T>
	struct hash<EngineCore::Ref<T>>
	{

		const std::size_t operator()(const EngineCore::Ref<T>& key) const
		{
			return hash<T*>()(key.GetReference());
		}

	};

}

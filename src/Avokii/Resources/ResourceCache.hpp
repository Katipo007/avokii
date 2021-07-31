#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "Concepts.hpp"
#include "ResourceId.hpp"
#include "ResourceTypes.hpp"

namespace Avokii
{
	class BaseResource;
	class ResourceLoader;
	class ResourceManager;

	class BaseResourceCache
	{
	protected:
		using UntypedResourcePtr = std::shared_ptr<const BaseResource>;
		using UntypedNonConstResourcePtr = std::shared_ptr<BaseResource>;

	public:
		explicit BaseResourceCache( ResourceManager& manager, const AssetType type );
		virtual ~BaseResourceCache();

		[[nodiscard]] AssetType GetResourceType() const noexcept { return mAssetType; }

		[[nodiscard]] bool Exists( ResourceId resource_id ) const noexcept;
		[[nodiscard]] UntypedResourcePtr GetUntyped( ResourceId _resourceId ) const noexcept;

		UntypedResourcePtr LoadUntyped( StringView _assetId );
		void Unload( ResourceId _resourceId );
		/// <summary>
		/// Unload resources that currently aren't being used.
		/// </summary>
		/// <param name="min_generations">Minimum number of generations this resource hasn't been used</param>
		void Purge( size_t _minGenerations = 3 );

		void NextGeneration();

	protected:
		UntypedResourcePtr AddResource( UntypedResourcePtr& new_resource );
		UntypedResourcePtr AddResource( UntypedNonConstResourcePtr& new_resource );
		[[nodiscard]] virtual UntypedNonConstResourcePtr LoadResource( ResourceLoader& loader ) const = 0;

		[[nodiscard]] UntypedResourcePtr FindIf( const std::function<bool( const BaseResource& )>& pred ) const;

	private:
		ResourceManager& mManager;
		const AssetType mAssetType;

		struct ResourceEntry
		{
			size_t generation;
			UntypedResourcePtr resource;

			ResourceEntry( UntypedResourcePtr ptr, size_t generation )
				: resource{ std::move( ptr ) }, generation{ generation }
			{}
		};
		using ResourceHashmap_T = std::unordered_map<ResourceId, ResourceEntry>;

		ResourceHashmap_T mResources;
		size_t mCurrentGeneration{ 0 };
	};

	template<Concepts::Resource R>
	class ResourceCache final
		: public BaseResourceCache
	{
	protected:
		using ResourcePtr = std::shared_ptr<const R>;

	public:
		ResourceCache( ResourceManager& manager )
			: BaseResourceCache( manager, R::GetResourceType() )
		{}
		virtual ~ResourceCache() {}

		[[nodiscard]] ResourcePtr Get( ResourceId _resourceId ) const
		{
			return std::dynamic_pointer_cast<const R>(GetUntyped( _resourceId ));
		}

		[[nodiscard]] ResourcePtr Load( StringView _assetId )
		{
			return std::dynamic_pointer_cast<const R>(LoadUntyped( _assetId ));
		}

		[[nodiscard]] ResourcePtr FindIf( const std::function<bool( const R& )>& pred ) const
		{
			const auto predicate_wrapper = [&pred]( const BaseResource& entry ) -> bool
			{
				return pred( dynamic_cast<const R&>(entry) );
			};

			if (const auto it = BaseResourceCache::FindIf( predicate_wrapper ))
				return std::dynamic_pointer_cast<const R>(it);

			return nullptr;
		}

		[[nodiscard]] virtual UntypedNonConstResourcePtr LoadResource( ResourceLoader& loader ) const override
		{
			if (auto loaded_resource = R::LoadResource( loader ))
			{
				static_assert(std::is_same<decltype(loaded_resource), std::shared_ptr<R>>::value, "R::LoadResource() returns the wrong type, expected a std::shared_ptr<const R>");
				return std::dynamic_pointer_cast<BaseResource>(loaded_resource);
			}

			return UntypedNonConstResourcePtr{};
		}
	};
}

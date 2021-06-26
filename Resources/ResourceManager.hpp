#pragma once

#include "Concepts.hpp"
#include "ResourceId.hpp"
#include "ResourceHandle.hpp"
#include "ResourceCache.hpp"

namespace Avokii
{
	class Core;
	enum class AssetType;
	class BaseResource;
	class BaseResourceCache;

	class ResourceManager final
	{
		using CacheCollection_T = std::unordered_map<AssetType, std::unique_ptr<BaseResourceCache>>;

	public:
		explicit ResourceManager( Core& core );
		virtual ~ResourceManager();

		template<Concepts::Resource R>
		void Init()
		{
			constexpr AssetType type{ R::GetResourceType() };

			if (const auto* existing_cache = GetCacheInternal( type ))
				throw std::runtime_error( "ResourceCache already initialised for this type of resource" );

			auto [it, success] = mCaches.try_emplace( type, std::make_unique<ResourceCache<R>>( *this ) );
			if (!success)
				throw std::runtime_error( "Failed to store ResourceCache of given type" );
		}

		template<Concepts::Resource R>
		[[nodiscard]] bool Exists( ResourceId_T _resource_id ) const noexcept { return GetCache<R>().Exists( _resource_id ); }

		template<Concepts::Resource R>
		[[nodiscard]] bool IsLoaded( ResourceId_T _resource_id ) const noexcept { return GetCache<R>().IsLoaded( _resource_id ); }

		template<Concepts::Resource R>
		[[nodiscard]] ResourceHandle<const R> Get( ResourceId_T _resource_id ) const { return ResourceHandle<const R>( GetCache<R>().Get( _resource_id ) ); }

		template<Concepts::Resource R>
		void Unload( ResourceId_T _resource_id ) { GetCache<R>().Unload( _resource_id ); }

		template<Concepts::Resource R>
		void Purge( size_t _min_generations = 3 ) { GetCache<R>().Purge( _min_generations ); }

		template<Concepts::Resource R>
		[[nodiscard]] const ResourceCache<R>& GetCache() const
		{
			constexpr AssetType type{ R::GetResourceType() };

			if (const auto* cache = GetCacheInternal( type ))
				return *dynamic_cast<const ResourceCache<R>*>(cache);

			throw std::runtime_error( "Given resource type is not initialised for this ResourceManager" );
		}

		Core& rGetCore() noexcept { return mCore; }
		const Core& GetCore() const noexcept { return mCore; }

	protected:
		BaseResourceCache* GetCacheInternal( const AssetType type );
		const BaseResourceCache* GetCacheInternal( const AssetType type ) const;

	private:
		CacheCollection_T mCaches;
		Core& mCore;
	};
}

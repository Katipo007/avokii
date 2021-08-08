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
		[[nodiscard]] bool Exists( ResourceId resource_id ) const noexcept { return GetCache<R>().Exists( resource_id ); }

		template<Concepts::Resource R>
		[[nodiscard]] bool IsLoaded( ResourceId resource_id ) const noexcept { return GetCache<R>().IsLoaded( resource_id ); }

		template<Concepts::Resource R>
		[[nodiscard]] ResourceHandle<const R> Get( ResourceId resource_id ) const { return ResourceHandle<const R>( GetCache<R>().Get( resource_id ) ); }

		template<Concepts::Resource R>
		[[nodiscard]] ResourceHandle<const R> GetOrLoad( StringView asset_id )
		{
			if (auto found = Get<R>( ToResourceId( asset_id ) ))
				return found;

			return Load<R>( asset_id );
		}

		template<Concepts::Resource R>
		ResourceHandle<const R> Load( StringView asset_id ) { return rGetCache<R>().Load( asset_id ); }

		template<Concepts::Resource R>
		void Unload( ResourceId resource_id ) { GetCache<R>().Unload( resource_id ); }

		template<Concepts::Resource R>
		void Purge( size_t minGenerations = 3 ) { GetCache<R>().Purge( minGenerations ); }

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

	private:
		template<Concepts::Resource R>
		[[nodiscard]] ResourceCache<R>& rGetCache()
		{
			constexpr AssetType type{ R::GetResourceType() };

			if (auto* cache = GetCacheInternal( type ))
				return *dynamic_cast<ResourceCache<R>*>(cache);

			throw std::runtime_error( "Given resource type is not initialised for this ResourceManager" );
		}

		BaseResourceCache* GetCacheInternal( const AssetType type );
		const BaseResourceCache* GetCacheInternal( const AssetType type ) const;

	private:
		CacheCollection_T mCaches;
		Core& mCore;
	};
}

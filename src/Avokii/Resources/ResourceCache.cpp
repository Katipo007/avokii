#include "ResourceCache.hpp"

#include <algorithm>
#include <execution>

#include "Avokii/Containers/ContainerOperations.hpp"
#include "BaseResource.hpp"
#include "ResourceLoader.hpp"

using namespace Avokii::ContainerOps;

namespace Avokii
{
	BaseResourceCache::BaseResourceCache( ResourceManager& manager, const AssetType type )
		: mManager{ manager }
		, mAssetType{ type }
	{
	}

	BaseResourceCache::~BaseResourceCache()
	{
		mResources.clear();
	}

	bool BaseResourceCache::Exists( ResourceId resource_id ) const noexcept
	{
		return mResources.find( resource_id ) != std::end( mResources );
	}

	BaseResourceCache::UntypedResourcePtr BaseResourceCache::GetUntyped( ResourceId resource_id ) const noexcept
	{
		if (const auto found = mResources.find( resource_id ); found != std::end( mResources ))
			return found->second.resource;
		
		return UntypedResourcePtr{};
	}

	BaseResourceCache::UntypedResourcePtr BaseResourceCache::LoadUntyped( StringView asset_id )
	{
		// not in the cache, try loading
		ResourceLoader loader{ mManager, asset_id };
		if (auto loaded_resource = LoadResource( loader ))
		{
			loaded_resource->mAssetId = loader.GetAssetId();
			loaded_resource->mResourceId = loader.GetResourceId();

			AV_ASSERT( loaded_resource->GetResourceId() != ResourceId{} );
			return AddResource( loaded_resource );
		}

		AV_LOG_ERROR( LoggingChannels::Resource, "Failed to load asset with id '{}'", asset_id );
		return UntypedResourcePtr{};
	}

	void BaseResourceCache::Unload( ResourceId resource_id )
	{
		if (const auto found = mResources.find( resource_id ); found != std::end( mResources ))
			mResources.erase( found );
	}

	void BaseResourceCache::Purge( size_t min_generations )
	{
		const auto generation_threshold = std::min( mCurrentGeneration, mCurrentGeneration - min_generations );

		const auto old_size = mResources.size();
		for (auto it = std::begin( mResources ), last = std::end( mResources ); it != last; )
		{
			if (it->second.generation < generation_threshold)
				it = mResources.erase( it );
			else
				++it;
		}
		
		AV_LOG_INFO( LoggingChannels::Resource, "Purged '{}' old resources from {} cache", old_size - mResources.size(), GetAssetTypeName( mAssetType ) );
	}

	void BaseResourceCache::NextGeneration()
	{
		std::for_each( std::execution::par_unseq, std::begin( mResources ), std::end( mResources ), [ g=this->mCurrentGeneration ]( ResourceHashmap_T::value_type& entry )
			{
				if (entry.second.resource.use_count() > 1)
					entry.second.generation = g;
			} );
	}

	BaseResourceCache::UntypedResourcePtr BaseResourceCache::AddResource( UntypedResourcePtr& new_resource )
	{
		AV_ASSERT( new_resource );
		if (!new_resource)
			return nullptr;

		const ResourceId resource_id{ new_resource->GetResourceId() };
		AV_ASSERT( resource_id != ResourceId{} );
		if (resource_id == ResourceId{})
			return nullptr;

		auto [it, success] = mResources.try_emplace( resource_id, new_resource, mCurrentGeneration );
		if (success)
			return it->second.resource;

		AV_ASSERT( false, "Failed to add resource, does a resource with the given id already exist?" );
		return nullptr;
	}

	BaseResourceCache::UntypedResourcePtr BaseResourceCache::AddResource( UntypedNonConstResourcePtr& new_resource )
	{
		AV_ASSERT( new_resource );
		if (!new_resource)
			return nullptr;

		const ResourceId resource_id{ new_resource->GetResourceId() };
		AV_ASSERT( resource_id != ResourceId{} );
		if (resource_id == ResourceId{})
			return nullptr;

		auto [it, success] = mResources.try_emplace( resource_id, new_resource, mCurrentGeneration );
		if (success)
			return it->second.resource;

		AV_ASSERT( false, "Failed to add resource, does a resource with the given id already exist?" );
		return nullptr;
	}

	BaseResourceCache::UntypedResourcePtr BaseResourceCache::FindIf( const std::function<bool( const BaseResource& )>& pred ) const
	{
		const auto pred_wrapper = [&]( const ResourceHashmap_T::value_type& entry ) -> bool
		{
			return pred( *entry.second.resource );
		};

		if (const auto found = ContainerOps::FindIf( mResources, pred_wrapper ); found != std::end( mResources ))
			return found->second.resource;

		return nullptr;
	}
}

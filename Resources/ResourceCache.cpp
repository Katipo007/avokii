#include "ResourceCache.hpp"

#include <algorithm>
#include <execution>

#include "Containers/ContainerOperations.hpp"
#include "BaseResource.hpp"
#include "ResourceLoader.hpp"

using namespace Avokii::ContainerOps;

namespace Avokii
{
	BaseResourceCache::BaseResourceCache( ResourceManager& _manager, const AssetType _type )
		: mManager{ _manager }
		, mAssetType{ _type }
	{
	}

	BaseResourceCache::~BaseResourceCache()
	{
		mResources.clear();
	}

	bool BaseResourceCache::Exists( ResourceId_T _resource_id ) const noexcept
	{
		return mResources.find( _resource_id ) != std::end( mResources );
	}

	BaseResourceCache::UntypedResourcePtr BaseResourceCache::GetUntyped( ResourceId_T _resource_id ) const noexcept
	{
		if (const auto found = mResources.find( _resource_id ); found != std::end( mResources ))
			return found->second.resource;
		
		return UntypedResourcePtr{};
	}

	BaseResourceCache::UntypedResourcePtr BaseResourceCache::Load( StringView asset_id )
	{
		// not in the cache, try loading
		ResourceLoader loader{ mManager, asset_id };
		if (auto loaded_resource = LoadResource( loader ))
		{
			loaded_resource->mAssetId = loader.GetAssetId();
			loaded_resource->mResourceId = loader.GetResourceId();

			AV_ASSERT( !loaded_resource->GetResourceId() != ResourceId_T{} );
			return AddResource( loaded_resource );
		}

		AV_LOG_ERROR( LoggingChannels::Assertion, "Failed to load asset with id '{}'", asset_id );
		return UntypedResourcePtr{};
	}

	void BaseResourceCache::Unload( ResourceId_T _resource_id )
	{
		if (const auto found = mResources.find( _resource_id ); found != std::end( mResources ))
			mResources.erase( found );
	}

	void BaseResourceCache::Purge( size_t _min_generations )
	{
		const auto generation_threshold = std::min( mCurrentGeneration, mCurrentGeneration - _min_generations );

		const auto old_size = mResources.size();
		for (auto it = std::begin( mResources ), last = std::end( mResources ); it != last; )
		{
			if (it->second.generation < generation_threshold)
				it = mResources.erase( it );
			else
				++it;
		}
		
		AV_LOG_INFO( LoggingChannels::Application, "Purged '{}' old resources from {} cache", old_size - mResources.size(), GetAssetTypeName( mAssetType ) );
	}

	void BaseResourceCache::NextGeneration()
	{
		std::for_each( std::execution::par_unseq, std::begin( mResources ), std::end( mResources ), [ g=this->mCurrentGeneration ]( ResourceHashmap_T::value_type& entry )
			{
				if (entry.second.resource.use_count() > 1)
					entry.second.generation = g;
			} );
	}

	BaseResourceCache::UntypedResourcePtr BaseResourceCache::AddResource( UntypedResourcePtr& _new_resource )
	{
		AV_ASSERT( _new_resource );
		if (!_new_resource)
			return nullptr;

		const ResourceId_T resource_id{ _new_resource->GetResourceId() };
		AV_ASSERT( resource_id != ResourceId_T{} );
		if (resource_id == ResourceId_T{})
			return nullptr;

		auto [it, success] = mResources.try_emplace( resource_id, _new_resource, mCurrentGeneration );
		if (success)
			return it->second.resource;

		AV_ASSERT( false, "Failed to add resource, does a resource with the given id already exist?" );
		return nullptr;
	}

	BaseResourceCache::UntypedResourcePtr BaseResourceCache::AddResource( UntypedNonConstResourcePtr& _new_resource )
	{
		AV_ASSERT( _new_resource );
		if (!_new_resource)
			return nullptr;

		const ResourceId_T resource_id{ _new_resource->GetResourceId() };
		AV_ASSERT( resource_id != ResourceId_T{} );
		if (resource_id == ResourceId_T{})
			return nullptr;

		auto [it, success] = mResources.try_emplace( resource_id, _new_resource, mCurrentGeneration );
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

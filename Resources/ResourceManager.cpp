#include "ResourceManager.hpp"

namespace Avokii
{
	ResourceManager::ResourceManager( Core& _core )
		: mCore{ _core }
	{
	}

	ResourceManager::~ResourceManager()
	{
		mCaches.clear();
	}

	BaseResourceCache* ResourceManager::GetCacheInternal( const AssetType type )
	{
		if (const auto found = mCaches.find( type ); found != std::end( mCaches ))
			return found->second.get();

		return nullptr;
	}

	const BaseResourceCache* ResourceManager::GetCacheInternal( const AssetType type ) const
	{
		if (const auto found = mCaches.find( type ); found != std::end( mCaches ))
			return found->second.get();

		return nullptr;
	}
}

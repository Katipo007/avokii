#include "ResourceLoader.hpp"
#include "ResourceLoader.hpp"

namespace Avokii
{
	ResourceLoader::ResourceLoader( ResourceManager& _manager, StringView _asset_id )
		: mManager{ _manager }
		, mAssetId{ _asset_id }
		, mResourceId{ ToResourceId( _asset_id ) }
	{
	}

	ResourceLoader::~ResourceLoader()
	{
	}
}

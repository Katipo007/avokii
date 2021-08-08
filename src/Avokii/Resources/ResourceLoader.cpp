#include "ResourceLoader.hpp"
#include "ResourceLoader.hpp"

namespace Avokii
{
	ResourceLoader::ResourceLoader( ResourceManager& r_manager, StringView asset_id )
		: mManager{ r_manager }
		, mAssetId{ asset_id }
		, mResourceId{ ToResourceId( asset_id ) }
	{
	}

	ResourceLoader::~ResourceLoader()
	{
	}
}

#pragma once

#include "Avokii/File/Filepath.hpp"
#include "Avokii/Resources/ResourceId.hpp"

namespace Avokii
{
	class ResourceManager;
	class BaseResourceCache;

	class ResourceLoader final
	{
	public:
		friend class BaseResourceCache;

		StringView GetAssetId() const noexcept { return mAssetId; }
		ResourceId_T GetResourceId() const noexcept { return mResourceId; }

		ResourceManager& GetManager() const noexcept { return mManager; }

	private:
		ResourceLoader( ResourceManager& manager, StringView asset_id );
		virtual ~ResourceLoader();

		String mAssetId;
		ResourceId_T mResourceId;
		ResourceManager& mManager;
	};
}

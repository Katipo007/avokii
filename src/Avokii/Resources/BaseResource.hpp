#pragma once

#include "ResourceId.hpp"

namespace Avokii
{
	enum class AssetType;

	class BaseResource
	{
	public:
		virtual ~BaseResource() = default;
		
		StringView GetAssetId() const noexcept { return mAssetId.has_value() ? *mAssetId : StringView{}; }
		ResourceId_T GetResourceId() const noexcept { return mResourceId; }
	
	protected:
		BaseResource() = default;

	private:
		friend class BaseResourceCache;
		std::optional<String> mAssetId;
		ResourceId_T mResourceId;
	};
}

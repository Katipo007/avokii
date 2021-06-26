#pragma once

#include <cinttypes>
#include <string>

#include "Geometry/Size.hpp"
#include "Resources/BaseResource.hpp"
#include "Resources/ResourceTypes.hpp"

namespace Avokii { class ResourceLoader; }

namespace Avokii::Graphics
{
	enum class TextureWrapSetting
	{
		ClampToEdge,
		Repeat,
		MirroredRepeat,
	};

	struct TextureDefinition
	{
		Size<uint32_t> size;
		TextureWrapSetting wrap_s = TextureWrapSetting::Repeat;
		TextureWrapSetting wrap_t = TextureWrapSetting::Repeat;
	};

	struct TextureLoadProperties
	{
		bool y_flip = true;
		TextureWrapSetting wrap_s = TextureWrapSetting::Repeat;
		TextureWrapSetting wrap_t = TextureWrapSetting::Repeat;
	};

	class Texture
		: public BaseResource
	{
	public:
		virtual ~Texture() = default;

		static constexpr AssetType GetResourceType() noexcept { return AssetType::Texture; }
		static std::shared_ptr<Texture> LoadResource( ResourceLoader& loader );

		virtual const Size<uint32_t>& GetSize() const noexcept = 0;

		virtual void SetData( void* data, uint32_t size ) = 0;

		virtual void Bind( uint32_t slot ) const = 0;

		virtual bool operator==( const Texture& other ) const = 0;

		virtual uint32_t GetNativeId() const noexcept = 0;
	};
}

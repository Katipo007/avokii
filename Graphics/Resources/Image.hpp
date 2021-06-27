#pragma once

#include <string>
#include <optional>

#include "Engine/Geometry/Rect.hpp"
#include "Engine/Geometry/Size.hpp"
#include "Visual/Graphics/Texture.hpp"

namespace Graphics
{
	class Texture;

	class Image
	{
	public:
		static const Image empty;

	public:
		Image();
		Image( std::shared_ptr<const ImageDefinition> image_def );
		virtual ~Image();

		Image( const Image& to_copy );
		Image operator=( const Image& to_copy );
		Image( Image&& to_move ) noexcept;
		Image& operator=( Image&& to_move ) noexcept;

		bool IsNull() const { return definition == nullptr; }
		operator bool() const { return !IsNull(); }

		const Size<unsigned>& GetSize() const;
		const Size<float>& GetSizeF() const;

		const std::shared_ptr<const Graphics::Texture>& GetSharedTexture() const;
		const Rect<float>& GetUVs() const;

	protected:
		std::shared_ptr<const ImageDefinition> definition;
	};
}

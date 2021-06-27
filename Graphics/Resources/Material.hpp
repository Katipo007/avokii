#pragma once

#include <memory>

#include "Resources/BaseResource.hpp"
#include "Resources/ResourceTypes.hpp"

#include "Graphics/Shader.hpp"
#include "Graphics/Texture.hpp"
#include "Graphics/Types.hpp"

namespace Avokii
{
	class ResourceLoader;

	namespace Graphics
	{
		class Material
			: public BaseResource
		{
		public:
			Material();
			explicit Material( ResourceLoader& rLoader );

			static constexpr AssetType GetResourceType() noexcept { return AssetType::Material; }
			static std::shared_ptr<Material> LoadResource( ResourceLoader& rLoader );

			const std::shared_ptr<const Shader> GetShader() const noexcept { return shader; }
			const std::vector<std::shared_ptr<const Texture>> GetTextures() const noexcept { return textures; }

			void SetShader( std::shared_ptr<const Shader> shader );
			void SetTextures( std::vector<std::shared_ptr<const Texture>> textures );

		protected:
			std::shared_ptr<const Shader> shader;
			std::vector<std::shared_ptr<const Texture>> textures;
		};
	}
}

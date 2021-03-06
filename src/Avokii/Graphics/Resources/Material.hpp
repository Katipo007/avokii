#pragma once

#include <memory>

#include "Avokii/Resources/BaseResource.hpp"
#include "Avokii/Resources/ResourceTypes.hpp"

#include "Avokii/Graphics/Shader.hpp"
#include "Avokii/Graphics/Texture.hpp"
#include "Avokii/Graphics/Types.hpp"

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

			const std::shared_ptr<const Shader> GetShader() const noexcept { return mShader; }
			const std::vector<std::shared_ptr<const Texture>> GetTextures() const noexcept { return mTextures; }

			void SetShader( std::shared_ptr<const Shader> shader );
			void SetTextures( std::vector<std::shared_ptr<const Texture>> textures );

		protected:
			std::shared_ptr<const Shader> mShader;
			std::vector<std::shared_ptr<const Texture>> mTextures;
		};
	}
}

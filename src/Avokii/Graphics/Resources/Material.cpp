#include "Material.hpp"

namespace Avokii::Graphics
{
	Material::Material()
	{
	}

	Material::Material( ResourceLoader& r_loader )
	{
		(void)r_loader;
		AV_NOT_IMPLEMENTED;
	}

	std::shared_ptr<Material> Material::LoadResource( ResourceLoader& r_loader )
	{
		(void)r_loader;
		AV_NOT_IMPLEMENTED;
		return {};
	}

	void Material::SetShader( std::shared_ptr<const Shader> new_shader )
	{
		mShader = new_shader;
	}

	void Material::SetTextures( std::vector<std::shared_ptr<const Texture>> new_textures )
	{
		mTextures = new_textures;
	}
}

#include "Material.hpp"

namespace Avokii::Graphics
{
	Material::Material()
	{
	}

	Material::Material( ResourceLoader& _rLoader )
	{
		(void)_rLoader;
		AV_NOT_IMPLEMENTED;
	}

	std::shared_ptr<Material> Material::LoadResource( ResourceLoader& _rLoader )
	{
		(void)_rLoader;
		AV_NOT_IMPLEMENTED;
		return {};
	}

	void Material::SetShader( std::shared_ptr<const Shader> _newShader )
	{
		shader = _newShader;
	}

	void Material::SetTextures( std::vector<std::shared_ptr<const Texture>> _newTextures )
	{
		textures = _newTextures;
	}
}

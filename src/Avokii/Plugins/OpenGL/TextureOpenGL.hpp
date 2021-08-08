#pragma once

#include "Avokii/File/Filepath.hpp"
#include "Avokii/Graphics/Texture.hpp"

namespace Avokii::Plugins
{
	class TextureOpenGL
		: public Graphics::Texture
	{
	public:
		TextureOpenGL( const Graphics::TextureDefinition& props );
		TextureOpenGL( const Filepath& path, const Graphics::TextureLoadProperties& props );
		virtual ~TextureOpenGL() override;

		virtual const Size<uint32_t>& GetSize() const noexcept override { return mSize; }

		virtual void SetData( void* data, uint32_t size ) override;

		virtual void Bind( uint32_t slot ) const override;

		virtual bool operator==( const Texture& other ) const override;

		virtual uint32_t GetNativeId() const noexcept override { return mOpenGlTextureId; }

	private:
		std::string mFilepath;
		Size<uint32_t> mSize;

		unsigned int mOpenGlInternalFormat;
		unsigned int mOpenGlDataFormat;
		unsigned int mOpenGlTextureId;
	};
}

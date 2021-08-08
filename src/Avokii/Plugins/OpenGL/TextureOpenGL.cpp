#include "TextureOpenGL.hpp"
#include "OpenGLHeader.hpp"

#include "Avokii/Utility/Unreachable.hpp"

#include <stb_image/stb_image.h>

namespace Avokii::Plugins
{
	namespace
	{
		GLuint ConvertTextureWrapSetting( Graphics::TextureWrapSetting value )
		{
			switch( value )
			{
			case Graphics::TextureWrapSetting::ClampToEdge:
				return GL_CLAMP_TO_EDGE;
			case Graphics::TextureWrapSetting::Repeat:
				return  GL_REPEAT;
			case Graphics::TextureWrapSetting::MirroredRepeat:
				return GL_MIRRORED_REPEAT;
			}

			unreachable();
		}
	}

	TextureOpenGL::TextureOpenGL( const Graphics::TextureDefinition& definition )
		: mSize( definition.size )
	{
		mOpenGlInternalFormat = GL_RGBA8;
		mOpenGlDataFormat = GL_RGBA;

		glCreateTextures( GL_TEXTURE_2D, 1, &mOpenGlTextureId );
		glTextureStorage2D( mOpenGlTextureId, 1, mOpenGlInternalFormat, static_cast<GLsizei>( mSize.width ), static_cast<GLsizei>( mSize.height ) );

		glTextureParameteri( mOpenGlTextureId, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTextureParameteri( mOpenGlTextureId, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

		glTextureParameteri( mOpenGlTextureId, GL_TEXTURE_WRAP_S, ConvertTextureWrapSetting( definition.wrap_s ) );
		glTextureParameteri( mOpenGlTextureId, GL_TEXTURE_WRAP_T, ConvertTextureWrapSetting( definition.wrap_t ) );
	}

	TextureOpenGL::TextureOpenGL( const Filepath& filepath, const Graphics::TextureLoadProperties& props )
	{
		AV_ASSERT( std::filesystem::is_regular_file( filepath ) );
		
		const auto filepath_str = filepath.string();
		int out_w, out_h, out_channels;
		stbi_set_flip_vertically_on_load( props.y_flip ? 1 : 0 );
		auto* p_data = stbi_load( filepath_str.c_str(), &out_w, &out_h, &out_channels, 0 );

		AV_ASSERT( p_data, "Failed to load image" );

		mSize = Size( (uint32_t)out_w, (uint32_t)out_h );

		GLenum internal_format = 0;
		GLenum data_format = 0;
		if( out_channels == 4 )
		{
			internal_format = GL_RGBA8;
			data_format = GL_RGBA;
		}
		else if( out_channels == 3 )
		{
			internal_format = GL_RGB8;
			data_format = GL_RGB;
		}
		else if( out_channels == 1 )
		{
			internal_format = GL_R8;
			data_format = GL_RED;
		}
		else
			AV_ASSERT( false, "Unsupported number of channels in image!" );

		mOpenGlInternalFormat = internal_format;
		mOpenGlDataFormat = data_format;

		AV_ASSERT( mOpenGlInternalFormat & mOpenGlDataFormat, "Unsupported image format!" );

		glCreateTextures( GL_TEXTURE_2D, 1, &mOpenGlTextureId );
		glTextureStorage2D( mOpenGlTextureId, 1, mOpenGlInternalFormat, static_cast<GLsizei>( mSize.width ), static_cast<GLsizei>( mSize.height ) );

		glTextureParameteri( mOpenGlTextureId, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTextureParameteri( mOpenGlTextureId, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

		glTextureParameteri( mOpenGlTextureId, GL_TEXTURE_WRAP_S, ConvertTextureWrapSetting( props.wrap_s ) );
		glTextureParameteri( mOpenGlTextureId, GL_TEXTURE_WRAP_T, ConvertTextureWrapSetting( props.wrap_t ) );

		glTextureSubImage2D( mOpenGlTextureId, 0, 0, 0, static_cast<GLsizei>( mSize.width ), static_cast<GLsizei>( mSize.height ), mOpenGlDataFormat, GL_UNSIGNED_BYTE, p_data );

		stbi_image_free( p_data );
		mFilepath = filepath.string();
	}

	TextureOpenGL::~TextureOpenGL()
	{
		glDeleteTextures( 1, &mOpenGlTextureId );
	}

	void TextureOpenGL::SetData( void* p_data, uint32_t data_size )
	{
		(void)data_size;
		uint32_t bpp = (mOpenGlDataFormat == GL_RGBA) ? 4 : 3; (void)bpp;
		AV_ASSERT( data_size == mSize.width * mSize.height * bpp, "Data size must exactly match texture!" );
		glTextureSubImage2D( mOpenGlTextureId, 0, 0, 0, static_cast<GLsizei>( mSize.width ), static_cast<GLsizei>( mSize.height ), mOpenGlDataFormat, GL_UNSIGNED_BYTE, p_data );
	}

	void TextureOpenGL::Bind( uint32_t slot ) const
	{
		glBindTextureUnit( static_cast<GLenum>( slot ), mOpenGlTextureId );
	}

	bool TextureOpenGL::operator==( const Texture& other ) const
	{
		const auto& opengl_other = dynamic_cast<const TextureOpenGL&>(other);
		return mOpenGlTextureId == opengl_other.mOpenGlTextureId;
	}
}

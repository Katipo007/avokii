#include "ShaderOpenGL.hpp"
#include "OpenGLHeader.hpp"

#include <fstream>

#pragma warning( push, 0 )
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr
#pragma warning( pop )

namespace
{
	static GLenum GetShaderTypeFromString( const std::string_view type )
	{
		if (type == "vertex")
			return GL_VERTEX_SHADER;
		else if (type == "fragment" || type == "pixel")
			return GL_FRAGMENT_SHADER;

		AV_ASSERT( false, "Unrecognised shader type!" );
		return 0;
	}
}

namespace Avokii::Plugins
{
	ShaderOpenGL::ShaderOpenGL( const Filepath& filepath )
		: mOpenGlProgramId( 0 )
	{
		AV_ASSERT( !std::filesystem::is_directory( filepath ) );
		AV_ASSERT( !filepath.empty(), "Empty filename!" );

		const auto file_src = ReadFile( filepath );
		const auto shader_sources = PreProcess( file_src );
		Compile( shader_sources );

		mName = filepath.filename().string();
	}

	ShaderOpenGL::ShaderOpenGL( std::string_view name_, std::string_view vertex_src_, std::string_view fragment_src_ )
		: mOpenGlProgramId( 0 )
		, mName( name_.data() )
	{
		std::unordered_map<GLenum, std::string> sources;
		sources[GL_VERTEX_SHADER] = static_cast<std::string>(vertex_src_);
		sources[GL_FRAGMENT_SHADER] = static_cast<std::string>(fragment_src_);
		Compile( sources );
	}

	ShaderOpenGL::~ShaderOpenGL()
	{
		if (mOpenGlProgramId)
			glDeleteProgram( mOpenGlProgramId );
	}

	void ShaderOpenGL::Bind() const
	{
		glUseProgram( mOpenGlProgramId );
	}

	void ShaderOpenGL::Unbind() const
	{
		glUseProgram( 0 );
	}

	void ShaderOpenGL::SetInt( const std::string& uniform_name, int value )
	{
		UploadUniformInt( uniform_name, value );
	}

	void ShaderOpenGL::SetUInt( const std::string& uniform_name, unsigned int value )
	{
		UploadUniformUInt( uniform_name, value );
	}

	void ShaderOpenGL::SetIntArray( const std::string& uniform_name, int* values, uint32_t count )
	{
		UploadUniformIntArray( uniform_name, values, count );
	}

	void ShaderOpenGL::SetUIntArray( const std::string& uniform_name, unsigned int* values, uint32_t count )
	{
		UploadUniformUIntArray( uniform_name, values, count );
	}

	void ShaderOpenGL::SetFloat( const std::string& uniform_name, float value )
	{
		UploadUniformFloat( uniform_name, value );
	}

	void ShaderOpenGL::SetFloat3( const std::string& uniform_name, glm::vec3 value )
	{
		UploadUniformFloat3( uniform_name, value );
	}

	void ShaderOpenGL::SetFloat4( const std::string& uniform_name, glm::vec4 value )
	{
		UploadUniformFloat4( uniform_name, value );
	}

	void ShaderOpenGL::SetMat4( const std::string& uniform_name, glm::mat4 value )
	{
		UploadUniformMat4( uniform_name, value );
	}

	void ShaderOpenGL::UploadUniformInt( const std::string& uniform_name, int value )
	{
		const GLint location = glGetUniformLocation( mOpenGlProgramId, uniform_name.c_str() ); // TODO: cache locations
		glUniform1i( location, value );
	}

	void ShaderOpenGL::UploadUniformUInt( const std::string& uniform_name, unsigned int value )
	{
		const GLint location = glGetUniformLocation( mOpenGlProgramId, uniform_name.c_str() ); // TODO: cache locations
		glUniform1ui( location, value );
	}

	void ShaderOpenGL::UploadUniformIntArray( const std::string& uniform_name, int* values, uint32_t count )
	{
		const GLint location = glGetUniformLocation( mOpenGlProgramId, uniform_name.c_str() ); // TODO: cache locations
		glUniform1iv( location, count, values );
	}

	void ShaderOpenGL::UploadUniformUIntArray( const std::string& uniform_name, unsigned int* values, uint32_t count )
	{
		const GLint location = glGetUniformLocation( mOpenGlProgramId, uniform_name.c_str() ); // TODO: cache locations
		glUniform1uiv( location, count, values );
	}

	void ShaderOpenGL::UploadUniformFloat( const std::string& uniform_name, float value )
	{
		const GLint location = glGetUniformLocation( mOpenGlProgramId, uniform_name.c_str() ); // TODO: cache locations
		glUniform1f( location, value );
	}

	void ShaderOpenGL::UploadUniformFloat2( const std::string& uniform_name, glm::vec2 value )
	{
		const GLint location = glGetUniformLocation( mOpenGlProgramId, uniform_name.c_str() ); // TODO: cache locations
		glUniform2f( location, value.x, value.y );
	}

	void ShaderOpenGL::UploadUniformFloat3( const std::string& uniform_name, glm::vec3 value )
	{
		const GLint location = glGetUniformLocation( mOpenGlProgramId, uniform_name.c_str() ); // TODO: cache locations
		glUniform3f( location, value.x, value.y, value.z );
	}

	void ShaderOpenGL::UploadUniformFloat4( const std::string& uniform_name, glm::vec4 value )
	{
		const GLint location = glGetUniformLocation( mOpenGlProgramId, uniform_name.c_str() ); // TODO: cache locations
		glUniform4f( location, value.x, value.y, value.z, value.w );
	}

	void ShaderOpenGL::UploadUniformMat3( const std::string& uniform_name, glm::mat3 value )
	{
		const GLint location = glGetUniformLocation( mOpenGlProgramId, uniform_name.c_str() ); // TODO: cache locations
		glUniformMatrix3fv( location, 1, GL_FALSE, glm::value_ptr( glm::mat3( value ) ) );
	}

	void ShaderOpenGL::UploadUniformMat4( const std::string& uniform_name, glm::mat4 value )
	{
		const GLint location = glGetUniformLocation( mOpenGlProgramId, uniform_name.c_str() ); // TODO: cache locations
		glUniformMatrix4fv( location, 1, GL_FALSE, glm::value_ptr( value ) );
	}

	std::string ShaderOpenGL::ReadFile( const std::filesystem::path& filepath )
	{
		std::string result;

		if (auto input_file = std::ifstream{ filepath, std::ios::in | std::ios::binary })
		{
			input_file.seekg( 0, std::ios::end );
			size_t filesize = input_file.tellg();
			if (filesize != static_cast<size_t>(-1))
			{
				result.resize( filesize );
				input_file.seekg( 0, std::ios::beg );
				input_file.read( &result[0], filesize );
			}
			else
			{
				AV_LOG_ERROR( LoggingChannels::OpenGL, "Could not read from file '{0}'", filepath.string().c_str() );
			}

			return result;

		}
		else
		{
			AV_LOG_ERROR( LoggingChannels::OpenGL, "Could not open file '{0}' for reading", filepath.string().c_str() );
		}

		return std::string();
	}

	std::unordered_map<GLenum, std::string> ShaderOpenGL::PreProcess( std::string_view source )
	{
		std::unordered_map<GLenum, std::string> sources;

		const auto type_token = "#type";
		const auto type_token_len = strlen( type_token );


		size_t type_token_pos = source.find( type_token, 0 );
		while (type_token_pos != std::string_view::npos)
		{
			// TODO: non-windows newlines
			const auto eol = source.find_first_of( "\r\n", type_token_pos ); // end of the shader type declaration line
			AV_ASSERT( eol != std::string_view::npos, "Syntax error" );

			const auto begin = type_token_pos + type_token_len + 1; // start of shader type name (right after the #type symbol)
			const auto type_name = source.substr( begin, eol - begin );

			const auto shader_type_id = GetShaderTypeFromString( type_name );
			AV_ASSERT( shader_type_id, "Invalid shader type specified!" );

			const auto next_line_pos = source.find_first_not_of( "\r\n", eol );
			AV_ASSERT( next_line_pos != std::string_view::npos, "Syntax error" );
			type_token_pos = source.find( type_token, next_line_pos );

			sources[shader_type_id] = (type_token_pos == std::string_view::npos) ? source.substr( next_line_pos ) : source.substr( next_line_pos, type_token_pos - next_line_pos );
		}

		return sources;
	}

	void ShaderOpenGL::Compile( const std::unordered_map<GLenum, std::string>& shader_sources )
	{
		GLuint program = glCreateProgram();

		AV_ASSERT( shader_sources.count( GL_VERTEX_SHADER ) > 0, "Missing vertex shader" );
		AV_ASSERT( shader_sources.count( GL_FRAGMENT_SHADER ) > 0, "Missing fragment shader" );

		std::unordered_map< GLenum /* type */, GLenum /* shader id */ > shader_ids;
		for (auto& entry : shader_sources)
		{
			const GLenum type = entry.first;
			const std::string& source = entry.second;

			GLuint shader = glCreateShader( type );

			// load source and compile
			const GLchar* source_cstr = source.c_str();
			glShaderSource( shader, 1, &source_cstr, 0 );
			glCompileShader( shader );

			// check if it compiled successfully
			{
				GLint is_compiled = GL_FALSE;
				glGetShaderiv( shader, GL_COMPILE_STATUS, &is_compiled );

				if (is_compiled == GL_FALSE)
				{
					// extract error
					GLint max_length = 0;
					glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &max_length ); // includes NULL char
					std::vector<GLchar> msg( (size_t)max_length );
					glGetShaderInfoLog( shader, max_length, &max_length, &msg[0] );

					// delete shader
					glDeleteShader( shader );

					// report error
					AV_LOG_ERROR( LoggingChannels::OpenGL, "Error compiling OpenGL shader: {0}", msg.data() );
					AV_ASSERT_CHANNEL( LoggingChannels::OpenGL, false, "Shader compilation failed" );
					break;
				}
			}

			glAttachShader( program, shader );
			shader_ids[type] = shader;
		}

		mOpenGlProgramId = program;

		// link the program
		glLinkProgram( program );

		// check link status
		{
			GLint is_linked = GL_FALSE;
			glGetProgramiv( program, GL_LINK_STATUS, &is_linked );
			if (is_linked == GL_FALSE)
			{
				// extract message
				GLint max_length = 0;
				glGetProgramiv( program, GL_INFO_LOG_LENGTH, &max_length ); // includes NULL char
				std::vector<GLchar> msg( (size_t)max_length );
				glGetProgramInfoLog( program, max_length, &max_length, &msg[0] );

				// delete program
				glDeleteProgram( program );
				program = 0;

				// delete shaders
				for (auto& shader : shader_ids)
					glDeleteShader( shader.second );

				// report error
				AV_LOG_ERROR( LoggingChannels::OpenGL, "Error linking OpenGL shader program: {0}", msg.data() );
				AV_ASSERT_CHANNEL( LoggingChannels::OpenGL, false, "Shader link failed" );
				return;
			}
		}

		// delete shaders (no longer needed since they are now linked)
		for (auto& shader : shader_ids)
		{
			glDetachShader( program, shader.second );
			glDeleteShader( shader.second );
		}
	}
}

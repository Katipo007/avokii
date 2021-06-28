#include "VertexArrayOpenGL.hpp"
#include "OpenGLHeader.hpp"

#include <stack>

namespace
{
	static GLenum GetShaderDataTypeToOpenGLBaseType( Avokii::Graphics::ShaderDataType type )
	{
		switch( type )
		{
			using enum Avokii::Graphics::ShaderDataType;

			case Float:
			case Float2:
			case Float3:
			case Float4:
			case Mat3:
			case Mat4:
				return GL_FLOAT;

			case Int:
			case Int2:
			case Int3:
			case Int4:
				return GL_INT;

			case uInt:
			case uInt2:
			case uInt3:
			case uInt4:
				return GL_UNSIGNED_INT;

			case Bool:
				return GL_BOOL;
		}

		AV_ASSERT( false, "Unrecognised ShaderDataType!" );
		return 0;
	}

	std::stack<GLuint> bound_vao_stack;

	void PushBoundVao()
	{
		GLint current = 0;
		glGetIntegerv( GL_VERTEX_ARRAY_BINDING, &current );
		bound_vao_stack.push( current );
	}

	void PopBoundVao()
	{
		AV_ASSERT( !bound_vao_stack.empty() );
		glBindVertexArray( bound_vao_stack.top() );
		bound_vao_stack.pop();
	}

	GLuint GetBoundVao()
	{
		GLint current = 0;
		glGetIntegerv( GL_VERTEX_ARRAY_BINDING, &current );
		return current;
	}
}

namespace Avokii::Plugins
{
	VertexArrayOpenGL::VertexArrayOpenGL( const Graphics::VertexArrayDefinition& props )
		: name( props.name ? *props.name : "Unnamed vertex array" )
		, vao( 0 )
		, vbi( 0 )
	{
		PushBoundVao();

		glCreateVertexArrays( 1, &vao );
		Bind();

		if (props.name)
			glObjectLabel( GL_VERTEX_ARRAY, vao, -1, props.name.value().c_str() );

		for (auto& vb : props.vertex_buffers)
			AddVertexBuffer( vb );

		if (props.index_buffer != nullptr)
			SetIndexBuffer( props.index_buffer );

		Unbind();
		glBindBuffer( GL_ARRAY_BUFFER, 0 );
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

		PopBoundVao();
	}

	VertexArrayOpenGL::~VertexArrayOpenGL()
	{
		glDeleteVertexArrays( 1, &vao );
	}

	void VertexArrayOpenGL::Bind() const
	{
		glBindVertexArray( vao );
	}

	void VertexArrayOpenGL::Unbind() const
	{
		glBindVertexArray( 0 );
	}

	void VertexArrayOpenGL::AddVertexBuffer( const std::shared_ptr<Graphics::VertexBuffer>& vertex_buffer )
	{
		AV_ASSERT( GetBoundVao() == vao );
		AV_ASSERT( vertex_buffer->GetLayout().GetElements().size(), "Vertex buffer has no layout!" );

		vertex_buffer->Bind();
		vertex_buffers.push_back( vertex_buffer );

		const auto& layout = vertex_buffer->GetLayout();
		for( const auto& element : layout )
		{
			switch( element.type )
			{
				using enum Graphics::ShaderDataType;

				case Float:
				case Float2:
				case Float3:
				case Float4:
				{
					glEnableVertexAttribArray( vbi );
					glVertexAttribPointer( vbi
										   , static_cast<GLint>( element.GetComponentCount() )
										   , GetShaderDataTypeToOpenGLBaseType( element.type )
										   , element.normalised ? GL_TRUE : GL_FALSE
										   , static_cast<GLsizei>( layout.GetStride() )
										   , (const void*)element.offset
					);
					++vbi;
					break;
				}

				case Int:
				case Int2:
				case Int3:
				case Int4:
				case uInt:
				case uInt2:
				case uInt3:
				case uInt4:
				case Bool:
				{
					glEnableVertexAttribArray( vbi );
					glVertexAttribIPointer( vbi
						, static_cast<GLint>(element.GetComponentCount())
						, GetShaderDataTypeToOpenGLBaseType( element.type )
						, static_cast<GLsizei>(layout.GetStride())
						, (const void*)element.offset
					);
					++vbi;
					break;
				}

				case Mat3:
				case Mat4:
				{
					const auto count = element.GetComponentCount();
					for( uint32_t i = 0; i < count; ++i )
					{
						glEnableVertexAttribArray( vao );
						glVertexAttribPointer( vbi
											   , static_cast<GLint>( count )
											   , GetShaderDataTypeToOpenGLBaseType( element.type )
											   , element.normalised ? GL_TRUE : GL_FALSE
											   , static_cast<GLsizei>( layout.GetStride() )
											   , (const void*)( element.offset + sizeof( float ) * count * i )
						);
						glVertexAttribDivisor( vbi, 1 );
						++vbi;
					}
					break;
				}

				default:
					AV_ASSERT( false, "Unrecognised ShaderDataType!" );
					break;
			}
		}
	}

	void VertexArrayOpenGL::SetIndexBuffer( const std::shared_ptr<Graphics::IndexBuffer>& new_index_buffer )
	{
		AV_ASSERT( GetBoundVao() == vao );
		AV_ASSERT( !this->index_buffer );

		new_index_buffer->Bind();
		this->index_buffer = new_index_buffer;
	}

	const std::shared_ptr<Graphics::VertexBuffer>& VertexArrayOpenGL::GetVertexBuffer( size_t i ) const
	{
		AV_ASSERT( i < vertex_buffers.size() );
		return vertex_buffers[ i ];
	}

	size_t VertexArrayOpenGL::GetVertexBufferCount() const
	{
		return vertex_buffers.size();
	}
}

#pragma once

#include "Graphics/GraphicsBuffer.hpp"

namespace Avokii::API
{
	class VertexBufferOpenGL
		: public Graphics::VertexBuffer
	{
	public:
		VertexBufferOpenGL( const Graphics::VertexBufferDefinition& props );
		virtual ~VertexBufferOpenGL();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void SetData( const void* data, uint32_t size ) override;

		virtual const Graphics::BufferLayout& GetLayout() const override { return layout; }
		virtual void SetLayout( const Graphics::BufferLayout& layout ) override;

	private:
		std::string name;
		uint32_t vbo;
		Graphics::BufferLayout layout;
	};

	class IndexBufferOpenGL
		: public Graphics::IndexBuffer
	{
	public:
		IndexBufferOpenGL( const Graphics::IndexBufferDefinition& props );
		virtual ~IndexBufferOpenGL();

		virtual void Bind() const;
		virtual void Unbind() const;

		virtual uint32_t GetCount() const { return count; }
	private:
		std::string name;
		uint32_t ibo;
		uint32_t count;
	};
}

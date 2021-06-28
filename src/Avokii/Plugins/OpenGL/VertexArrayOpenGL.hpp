#pragma once

#include "Avokii/Graphics/VertexArray.hpp"

namespace Avokii::API
{
	class VertexArrayOpenGL
		: public Graphics::VertexArray
	{
	public:
		VertexArrayOpenGL( const Graphics::VertexArrayDefinition& definition );
		virtual ~VertexArrayOpenGL() override;

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void AddVertexBuffer( const std::shared_ptr<Graphics::VertexBuffer>& vertex_buffer ) override;
		virtual void SetIndexBuffer( const std::shared_ptr<Graphics::IndexBuffer>& index_buffer ) override;

		virtual const std::vector< std::shared_ptr<Graphics::VertexBuffer> >& GetVertexBuffers() const override { return vertex_buffers; }
		virtual const std::shared_ptr<Graphics::VertexBuffer>& GetVertexBuffer( size_t i ) const override;
		virtual size_t GetVertexBufferCount() const override;

		virtual const std::shared_ptr<Graphics::IndexBuffer>& GetIndexBuffer() const override { return index_buffer; }

	private:
		std::string name;
		unsigned int vao;
		unsigned int vbi; // vertex buffer index

		std::vector< std::shared_ptr<Graphics::VertexBuffer> > vertex_buffers;
		std::shared_ptr<Graphics::IndexBuffer> index_buffer;
	};
}

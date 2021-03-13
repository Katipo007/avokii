#pragma once

#include <cinttypes>
#include <memory>

#include "Visual/Graphics/Colour.hpp"
#include "RendererAPIs.hpp"
#include "RendererCapabilities.hpp"
#include "FrameBuffer.hpp"
#include "Texture.hpp"
#include "VertexArray.hpp"

namespace Visual::Device
{
	class VertexBuffer;
	class IndexBuffer;
	class Shader;

	class RendererAPI
	{
	public:
		virtual ~RendererAPI() = default;

		virtual void Init() = 0;
		virtual void Shutdown() = 0;

		virtual void SetViewport( uint32_t x, uint32_t y, uint32_t width, uint32_t height ) = 0;
		virtual uint32_t GetViewportX() = 0;
		virtual uint32_t GetViewportY() = 0;
		virtual uint32_t GetViewportWidth() = 0;
		virtual uint32_t GetViewportHeight() = 0;

		virtual void SetClearColour( ColourRGBA colour ) = 0;
		virtual void Clear() = 0;

		virtual void DrawIndexed( const std::shared_ptr<VertexArray>& vertex_array, uint32_t index_count = 0 ) = 0;
		virtual const RendererCapabilities& GetCapabilities() const = 0;

		virtual RendererAPIs::API GetAPIType() const = 0;

		virtual std::shared_ptr<VertexBuffer> CreateVertexBuffer( uint32_t size ) const = 0;
		virtual std::shared_ptr<VertexBuffer> CreateVertexBuffer( float* vertices, uint32_t size ) const = 0;
		virtual std::shared_ptr<IndexBuffer> CreateIndexBuffer( uint32_t* indices, uint32_t size ) const = 0;
		virtual std::shared_ptr<FrameBuffer> CreateFrameBuffer( const FrameBuffer::Specification& spec ) const = 0;
		virtual std::shared_ptr<Shader> CreateShader( std::string_view filepath ) const = 0;
		virtual std::shared_ptr<Shader> CreateShader( std::string_view name, std::string_view vertex_src, std::string_view fragment_src ) const = 0;
		virtual std::shared_ptr<Texture2D> CreateTexture2D( uint32_t width, uint32_t height, const Texture::CreationProperties& props = Texture::CreationProperties() ) const = 0;
		virtual std::shared_ptr<Texture2D> CreateTexture2D( std::string_view filepath, const Texture::LoadProperties& props = Texture::LoadProperties() ) const = 0;
		virtual std::shared_ptr<VertexArray> CreateVertexArray() const = 0;
	};
}
#pragma once

#include "Avokii/Graphics/FrameBuffer.hpp"

namespace Avokii::API
{
	class FrameBufferOpenGL
		: public Graphics::FrameBuffer
	{
	public:
		FrameBufferOpenGL( const Graphics::FrameBufferSpecification& spec );
		virtual ~FrameBufferOpenGL() override;

		void Invalidate();

		virtual void Bind() override;
		virtual void Unbind() override;

		virtual void Resize( uint32_t width, uint32_t height ) override;

		virtual uint32_t GetNativeColourAttachment() const override { return static_cast<uint32_t>( opengl_colour_attachment ); }
		virtual const Graphics::FrameBufferSpecification& GetSpecification() const override { return specification; }

	private:
		Graphics::FrameBufferSpecification specification;
		unsigned int opengl_framebuffer_id;
		unsigned int opengl_colour_attachment;
		unsigned int opengl_depth_attachment;
	};
}

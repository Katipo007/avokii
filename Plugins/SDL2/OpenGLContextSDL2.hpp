#pragma once

#include "Graphics/OpenGLContext.hpp"

struct SDL_Window;
typedef void* SDL_GLContext;

namespace Avokii::Plugins
{
	class OpenGLContextSDL2
		: public Graphics::OpenGLContext
	{
	public:
		explicit OpenGLContextSDL2( SDL_Window* window );
		explicit OpenGLContextSDL2( SDL_Window* window, SDL_GLContext shared );
		virtual ~OpenGLContextSDL2();

		virtual void Bind() override;
		virtual std::unique_ptr<Graphics::OpenGLContext> CreateSharedContext() override;

	private:
		SDL_Window* const window;
		SDL_GLContext context;
		SDL_GLContext shared_context;
		const bool context_owner;
	};
}

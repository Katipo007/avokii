#include "WindowSDL2.hpp"

#include "SDL2Include.hpp"
#ifdef AVOKII_PLATFORM_WINDOWS
#	include <SDL2/SDL_syswm.h>
#endif

namespace Avokii::Plugins
{
	WindowSDL2::WindowSDL2( SDL_Window* sdl_window_ )
		: sdl_window( sdl_window_ )
	{
		AV_ASSERT( sdl_window != nullptr );
	}

	WindowSDL2::~WindowSDL2()
	{
	}

	void WindowSDL2::Destroy()
	{
		SDL_HideWindow( sdl_window );
		SDL_DestroyWindow( sdl_window );
		sdl_window = nullptr;
	}

	Size<uint32_t> WindowSDL2::GetSize() const
	{
		int w, h;
		SDL_GetWindowSize( sdl_window, &w, &h );
		return { (uint32_t)w, (uint32_t)h };
	}

	void WindowSDL2::SetSize( const Size<uint32_t> new_size )
	{
		SDL_SetWindowSize( sdl_window, new_size.width, new_size.height );
	}

	void WindowSDL2::SetMode( const Graphics::WindowMode new_mode )
	{
		switch (new_mode)
		{
		case Graphics::WindowMode::Window:
			SDL_SetWindowBordered( sdl_window, SDL_TRUE );
			SDL_SetWindowFullscreen( sdl_window, 0 );
			break;

		case Graphics::WindowMode::Fullscreen:
			SDL_SetWindowBordered( sdl_window, SDL_TRUE );
			SDL_SetWindowFullscreen( sdl_window, SDL_WINDOW_FULLSCREEN );
			break;

		case Graphics::WindowMode::BorderlessFullscreen:
			SDL_SetWindowBordered( sdl_window, SDL_FALSE );
			SDL_SetWindowFullscreen( sdl_window, SDL_WINDOW_FULLSCREEN_DESKTOP );
			break;
		}
	}

	Graphics::WindowMode WindowSDL2::GetMode() const
	{
		auto flags = SDL_GetWindowFlags( sdl_window );

		if ((flags & SDL_WINDOW_FULLSCREEN_DESKTOP) && (flags & SDL_WINDOW_BORDERLESS))
			return Graphics::WindowMode::BorderlessFullscreen;
		else if (flags & SDL_WINDOW_FULLSCREEN)
			return Graphics::WindowMode::Fullscreen;

		return Graphics::WindowMode::Window;
	}

	void WindowSDL2::SetState( const Graphics::WindowState new_state )
	{
		switch (new_state)
		{
		case Graphics::WindowState::Maximised:
			SDL_MaximizeWindow( sdl_window );
			break;

		case Graphics::WindowState::Minimised:
			SDL_MinimizeWindow( sdl_window );
			break;

		case Graphics::WindowState::Normal:
			SDL_RestoreWindow( sdl_window );
			break;
		}
	}

	Graphics::WindowState WindowSDL2::GetState() const
	{
		auto flags = SDL_GetWindowFlags( sdl_window );
		
		if (flags & SDL_WINDOW_MINIMIZED)
			return Graphics::WindowState::Minimised;
		else if (flags & SDL_WINDOW_MAXIMIZED)
			return Graphics::WindowState::Maximised;

		return Graphics::WindowState::Normal;
	}

	void WindowSDL2::Show()
	{
		SDL_ShowWindow( sdl_window );
	}

	void WindowSDL2::Hide()
	{
		SDL_HideWindow( sdl_window );
	}

	void WindowSDL2::SetVSync( const bool vsync )
	{
		SDL_GL_SetSwapInterval( vsync ? 1 : 0 );
	}

	void WindowSDL2::SwapBuffers()
	{
		SDL_GL_SwapWindow( sdl_window );
	}

	uint32_t WindowSDL2::GetSDLWindowId() const
	{
		return SDL_GetWindowID( sdl_window );
	}

	void WindowSDL2::HandleEvent( const SDL_WindowEvent& e )
	{
		switch (e.event)
		{
		case SDL_WINDOWEVENT_RESIZED:
			
			break;

		case SDL_WINDOWEVENT_MAXIMIZED:
			break;

		case SDL_WINDOWEVENT_MINIMIZED:
			break;

		case SDL_WINDOWEVENT_RESTORED:
			break;
		}
	}

	void* WindowSDL2::GetNativeHandle()
	{
#ifdef AVOKII_PLATFORM_WINDOWS
		SDL_SysWMinfo wminfo;
		SDL_VERSION( &wminfo.version );
		if (SDL_GetWindowWMInfo( sdl_window, &wminfo ) == 1)
			return wminfo.info.win.window;
#else
		return sdl_window;
#endif
		return nullptr;
	}

	std::string_view WindowSDL2::GetNativeHandleType() const
	{
#ifdef AVOKII_PLATFORM_WINDOWS
		return "HWND";
#elif defined(AVOKII_PLATFORM_APPLE)
		return "SDL";
#else
		return "";
#endif
	}
}

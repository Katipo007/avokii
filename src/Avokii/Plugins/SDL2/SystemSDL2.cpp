#include "SystemSDL2.hpp"

#include <chrono>
#include <thread>

#include "Avokii/API/DearImGuiAPI.hpp"
#include "Avokii/API/InputAPI.hpp"

#include "SDL2Include.hpp"
#include "OpenGLContextSDL2.hpp"
#include "InputSDL2.hpp"
#include "WindowSDL2.hpp"

namespace Avokii::Plugins
{
    SystemSDL2::SystemSDL2()
    {
    }

    SystemSDL2::~SystemSDL2()
    {
    }

    void SystemSDL2::Init()
    {
        Uint32 flags = 0;
        if (!SDL_WasInit( flags ))
        {
            if (SDL_Init( flags ) != 0)
            {
                AV_LOG_CRITICAL( LoggingChannels::Application, "SDL2 could not initialize! SDL_Error: '{0}'", SDL_GetError() );
                AV_FATAL( "SDL2 could not initialize!" );
            }

            if (SDL_InitSubSystem( SDL_INIT_EVENTS ) != 0)
                throw std::runtime_error{ std::string{} + "Failed to init SDL2 events subsystem: " + SDL_GetError() };

            if (SDL_InitSubSystem( SDL_INIT_TIMER ) != 0)
                throw std::runtime_error{ std::string{} + "Failed to init SDL2 timer subsystem: " + SDL_GetError() };
        }
    }

    void SystemSDL2::Shutdown()
    {
        SDL_QuitSubSystem( SDL_INIT_EVENTS );
        SDL_InitSubSystem( SDL_INIT_TIMER );
        SDL_Quit();
    }

    void SystemSDL2::InitVideo() const
    {
        if (video_init)
            return;

        if (SDL_InitSubSystem( SDL_INIT_VIDEO ) != 0)
        {
            using namespace std::string_literals;
            throw std::runtime_error( "Failed to init SDL2 video subsystem: "s + SDL_GetError() );
        }

        SDL_VideoInit( nullptr );
        video_init = true;
    }

    void SystemSDL2::ShutdownVideo()
    {
        if (!video_init)
            return;

        SDL_VideoQuit();
        SDL_QuitSubSystem( SDL_INIT_VIDEO );
        video_init = false;
    }

    const Filepath& SystemSDL2::GetAssetsFilepath() const
    {
        static auto asset_path = std::filesystem::current_path() / "Assets";
        return asset_path;
    }

    std::unique_ptr<Graphics::OpenGLContext> SystemSDL2::CreateOpenGLContext()
    {
        AV_ASSERT( !windows.empty() );
        if (!windows.empty())
            return std::make_unique<OpenGLContextSDL2>( windows.front()->GetSDLWindow() );

        return nullptr;
    }

    std::shared_ptr<Graphics::Window> SystemSDL2::CreateWindow( const Graphics::WindowDefinition& definition )
    {
        InitVideo();

        enum class RendererType
        {
            OpenGL
        } type = RendererType::OpenGL;
        
        Uint32 flags = SDL_WINDOW_ALLOW_HIGHDPI;

        switch (type)
        {
        case RendererType::OpenGL:
            flags |= SDL_WINDOW_OPENGL;

            SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 8 );
            SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 8 );
            SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 8 );
            SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 24 );
            SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );

            SDL_GL_SetAttribute( SDL_GL_ACCELERATED_VISUAL, 1 );
            SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 4 );
            SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 1 );
            SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );
            break;
        }

        switch (definition.mode)
        {
        case Graphics::WindowMode::Window:
            flags |= SDL_WINDOW_RESIZABLE;
            break;

        case Graphics::WindowMode::BorderlessFullscreen:
            flags |= SDL_WINDOW_BORDERLESS;
            break;

        case Graphics::WindowMode::Fullscreen:
            flags |= SDL_WINDOW_FULLSCREEN;
            break;
        }

        SDL_SetHint( SDL_HINT_VIDEO_HIGHDPI_DISABLED, "1" );

        const auto screen_size = GetScreenSize();
        Size<uint32_t> window_size = definition.size;
        Point2D<uint32_t> window_pos = (screen_size - window_size) / 2;
        auto sdl_window = SDL_CreateWindow( definition.title.value_or( "Diorama Roguelite" ).c_str(), window_pos.x, window_pos.y, window_size.width, window_size.height, flags );
        if (!sdl_window)
        {
            using namespace std::string_literals;
            throw std::runtime_error( "Error creating SDL window: "s + SDL_GetError() );
        }

        auto window = std::make_shared<WindowSDL2>( sdl_window );
        window->Show();
        windows.push_back( window );

        return window;
    }

    void SystemSDL2::DestroyWindow( std::shared_ptr<Graphics::Window> window )
    {
        auto it = std::find( std::begin( windows ), std::end( windows ), window );
        if (it != std::end( windows ))
        {
            (*it)->Destroy();
            windows.erase( it );
        }

        if (windows.empty())
            ShutdownVideo();
    }

    Size<uint32_t> SystemSDL2::GetScreenSize() const
    {
        InitVideo();
        SDL_DisplayMode info;
        if (SDL_GetDesktopDisplayMode( 0, &info ) == 0)
            return { (uint32_t)info.w, (uint32_t)info.h };

        return { 0, 0 };
    }

    std::thread SystemSDL2::CreateThread( std::string_view name_, std::function<void()> runnable_ )
    {
        std::string name = (std::string)name_;
        return std::thread( [ = ]()
            {
                SetThreadName( name );

                if( runnable_ )
                    runnable_();
            } );
    }

    void SystemSDL2::Sleep( unsigned long milliseconds )
    {
        auto start = std::chrono::high_resolution_clock::now();
        auto end = start + std::chrono::milliseconds( milliseconds );
        do
        {
            std::this_thread::yield();
        } while (std::chrono::high_resolution_clock::now() < end);
    }

    bool SystemSDL2::GenerateEvents( API::VideoAPI* video, API::InputAPI* input, API::DearImGuiAPI* dearimgui )
    {
        SDL_Event event;
        SDL_PumpEvents();
        while (SDL_PeepEvents( &event, 1, SDL_GETEVENT, SDL_FIRSTEVENT, SDL_LASTEVENT ))
        {
            if (dearimgui)
                dearimgui->ProcessSystemEvent( &event );

            switch (event.type)
            {
            case SDL_KEYDOWN:
            case SDL_KEYUP:
            {
                if (auto* input_sdl2 = dynamic_cast<Plugins::InputSDL2*>(input); (input_sdl2 != nullptr) && !(dearimgui && dearimgui->WantsToCaptureKeyboard()))
                    input_sdl2->ProcessEvent( event.key );
                break;
            }
            case SDL_TEXTINPUT:
            {
                if (auto* input_sdl2 = dynamic_cast<Plugins::InputSDL2*>(input); (input_sdl2 != nullptr) && !(dearimgui && dearimgui->WantsToCaptureKeyboard()))
                    input_sdl2->ProcessEvent( event.text );
                break;
            }
            case SDL_TEXTEDITING:
            {
                if (auto* input_sdl2 = dynamic_cast<Plugins::InputSDL2*>(input); (input_sdl2 != nullptr) && !(dearimgui && dearimgui->WantsToCaptureKeyboard()))
                    input_sdl2->ProcessEvent( event.edit );
                break;
            }
            case SDL_MOUSEMOTION:
            case SDL_MOUSEBUTTONUP:
            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEWHEEL:
            {
                (void)input; // TODO
                //if (auto sdl_input = dynamic_cast<InputSDL*>(input))
                //    sdl_input->HandleEvent( event );
                break;
            }
            case SDL_QUIT:
            {
                return false;
            }
            case SDL_WINDOWEVENT:
            {
                if (video)
                {
                    auto it = std::find_if( std::begin( windows ), std::end( windows ), [&event]( const std::shared_ptr<WindowSDL2>& window )
                    {
                        return window && (window->GetSDLWindowId() == event.window.windowID);
                    } );
                    if (it != std::end( windows ))
                        (*it)->HandleEvent( event.window );
                }
                break;
            }
            case SDL_CONTROLLERAXISMOTION:
            {
                if (auto* input_sdl2 = dynamic_cast<Plugins::InputSDL2*>(input); (input_sdl2 != nullptr))
                    input_sdl2->ProcessEvent( event.caxis );
                break;
            }
            case SDL_CONTROLLERBUTTONUP:
            case SDL_CONTROLLERBUTTONDOWN:
            {
                if (auto* input_sdl2 = dynamic_cast<Plugins::InputSDL2*>(input); (input_sdl2 != nullptr))
                    input_sdl2->ProcessEvent( event.cbutton );
                break;
            }
            case SDL_CONTROLLERDEVICEADDED:
            case SDL_CONTROLLERDEVICEREMAPPED:
            case SDL_CONTROLLERDEVICEREMOVED:
            {
                if (auto* input_sdl2 = dynamic_cast<Plugins::InputSDL2*>(input); (input_sdl2 != nullptr))
                    input_sdl2->ProcessEvent( event.cdevice );
                break;
            }
            case SDL_CONTROLLERSENSORUPDATE:
            {
                if (auto* input_sdl2 = dynamic_cast<Plugins::InputSDL2*>(input); (input_sdl2 != nullptr))
                    input_sdl2->ProcessEvent( event.csensor );
                break;
            }
            case SDL_CONTROLLERTOUCHPADDOWN:
            case SDL_CONTROLLERTOUCHPADUP:
            case SDL_CONTROLLERTOUCHPADMOTION:
            {
                if (auto* input_sdl2 = dynamic_cast<Plugins::InputSDL2*>(input); (input_sdl2 != nullptr))
                    input_sdl2->ProcessEvent( event.ctouchpad );
                break;
            }

            } // end switch
        }

        return true;
    }

    std::string_view SystemSDL2::GetName() const noexcept
    {
        return "SDL2";
    }

    void SystemSDL2::OnVariableUpdate( const PreciseTimestep&, const StepType )
    {
    }
}

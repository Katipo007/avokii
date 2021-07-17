#pragma once

#include "Avokii/API/InputAPI.hpp"
#include "Avokii/Utility/Signal.hpp"

namespace Avokii::API { class SystemAPI; }

struct SDL_KeyboardEvent;
struct SDL_TextEditingEvent;
struct SDL_TextInputEvent;
struct SDL_MouseMotionEvent;
struct SDL_MouseButtonEvent;
struct SDL_MouseWheelEvent;
struct SDL_JoyAxisEvent;
struct SDL_JoyBallEvent;
struct SDL_JoyDeviceEvent;
struct SDL_JoyHatEvent;
struct SDL_JoyButtonEvent;
struct SDL_ControllerAxisEvent;
struct SDL_ControllerButtonEvent;
struct SDL_ControllerDeviceEvent;
struct SDL_ControllerTouchpadEvent;
struct SDL_ControllerSensorEvent;

namespace Avokii::Plugins
{
	class KeyboardInputSDL2;
	class GamepadInputSDL2;

	class InputSDL2 final
		: public API::InputAPI
	{
	public:
		explicit InputSDL2( API::SystemAPI& system );
		~InputSDL2();

		std::string_view GetName() const noexcept override { return "SDL2 Input"; }

		size_t GetKeyboardCount() const override;
		std::shared_ptr<Input::KeyboardInput> GetKeyboard( size_t idx ) const override;
		size_t GetGamepadCount() const override;
		std::shared_ptr<Input::GamepadInput> GetGamepad( size_t idx ) const override;

		void ProcessEvent( SDL_KeyboardEvent& e );
		void ProcessEvent( SDL_TextEditingEvent& e );
		void ProcessEvent( SDL_TextInputEvent& e );
		void ProcessEvent( SDL_ControllerAxisEvent& e );
		void ProcessEvent( SDL_ControllerButtonEvent& e );
		void ProcessEvent( SDL_ControllerDeviceEvent& e );
		void ProcessEvent( SDL_ControllerTouchpadEvent& e );
		void ProcessEvent( SDL_ControllerSensorEvent& e );

		Signal::signal<int> GamepadConnected;
		Signal::signal<int> GamepadDisconnected;

	private:
		void Init() override;
		void Shutdown() override;
		void BeginEvents( const PreciseTimestep& ts ) override;

	private:
		API::SystemAPI& system;

		std::vector<std::shared_ptr<KeyboardInputSDL2>> keyboards;
		std::unordered_map<int, std::shared_ptr<GamepadInputSDL2>> gamepads;
	};
}

#include "InputSDL2.hpp"

#include "Avokii/API/SystemAPI.hpp"
#include "KeyboardInputSDL2.hpp"
#include "GamepadInputSDL2.hpp"
#include "SDL2Include.hpp"

#include <SDL2/SDL_gamecontroller.h>

namespace
{
	Avokii::Input::GamepadButton TranslateGamepadButtonCode( SDL_GameControllerButton sdl_code )
	{
		using namespace Avokii::Input;

		switch (sdl_code)
		{
		using enum SDL_GameControllerButton;

		case SDL_CONTROLLER_BUTTON_A: return GamepadButton::Face1;
		case SDL_CONTROLLER_BUTTON_B: return GamepadButton::Face2;
		case SDL_CONTROLLER_BUTTON_X: return GamepadButton::Face3;
		case SDL_CONTROLLER_BUTTON_Y: return GamepadButton::Face4;
		case SDL_CONTROLLER_BUTTON_BACK: return GamepadButton::Select;

		case SDL_CONTROLLER_BUTTON_TOUCHPAD:
		case SDL_CONTROLLER_BUTTON_START:
			return GamepadButton::Start;

		case SDL_CONTROLLER_BUTTON_DPAD_DOWN: return GamepadButton::DpadDown;
		case SDL_CONTROLLER_BUTTON_DPAD_UP: return GamepadButton::DpadUp;
		case SDL_CONTROLLER_BUTTON_DPAD_LEFT: return GamepadButton::DpadLeft;
		case SDL_CONTROLLER_BUTTON_DPAD_RIGHT: return GamepadButton::DpadRight;
		case SDL_CONTROLLER_BUTTON_LEFTSHOULDER: return GamepadButton::LeftShoulder;
		case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER: return GamepadButton::RightShoulder;
		case SDL_CONTROLLER_BUTTON_LEFTSTICK: return GamepadButton::LeftThumbButton;
		case SDL_CONTROLLER_BUTTON_RIGHTSTICK: return GamepadButton::RightThumbButton;

		default:
			AV_NOT_IMPLEMENTED;
			return GamepadButton::Invalid;
		}
	}
}

namespace Avokii::Plugins
{
	InputSDL2::InputSDL2( API::SystemAPI& system )
		: system( system )
	{

	}

	InputSDL2::~InputSDL2() = default;

	void InputSDL2::Init()
	{
		if (SDL_InitSubSystem( SDL_INIT_GAMECONTROLLER ) != 0)
		{
			using namespace std::string_literals;
			throw std::runtime_error( "Failed to init SDL2 input subsystem: "s + SDL_GetError() );
		}

		keyboards.emplace_back( new KeyboardInputSDL2() );
	}

	void InputSDL2::Shutdown()
	{
		keyboards.clear();

		SDL_QuitSubSystem( SDL_INIT_GAMECONTROLLER );
	}

	void InputSDL2::BeginEvents( const PreciseTimestep& )
	{
		for (const auto& keyboard : keyboards)
			keyboard->Update();

		for (const auto& [idx, gamepad] : gamepads)
			gamepad->Update();
	}


	size_t InputSDL2::GetKeyboardCount() const
	{
		return keyboards.size();
	}

	std::shared_ptr<Input::KeyboardInput> InputSDL2::GetKeyboard( size_t idx ) const
	{
		return keyboards.at( idx );
	}

	size_t InputSDL2::GetGamepadCount() const
	{
		return gamepads.size();
	}

	std::shared_ptr<Input::GamepadInput> InputSDL2::GetGamepad( size_t idx ) const
	{
		return gamepads.at( (int)idx );
	}

	void InputSDL2::ProcessEvent( SDL_KeyboardEvent& e )
	{
		switch (e.type)
		{
		case SDL_KEYUP:
		case SDL_KEYDOWN:
			for (const auto& keyboard : keyboards)
				keyboard->ProcessEvent( e );
			break;
		}
	}

	void InputSDL2::ProcessEvent( SDL_TextEditingEvent& e )
	{
		switch (e.type)
		{
		case SDL_TEXTEDITING:
			for (const auto& keyboard : keyboards)
				keyboard->ProcessEvent( e );
			break;
		}
	}

	void InputSDL2::ProcessEvent( SDL_TextInputEvent& e )
	{
		switch (e.type)
		{
		case SDL_TEXTINPUT:
			for (const auto& keyboard : keyboards)
				keyboard->ProcessEvent( e );
			break;
		}
	}

	void InputSDL2::ProcessEvent( SDL_ControllerAxisEvent& e )
	{
		AV_ASSERT( e.type == SDL_CONTROLLERAXISMOTION );

		if (const auto& gamepad = gamepads.at( e.which ))
		{
			switch (e.axis)
			{
			case SDL_CONTROLLER_AXIS_TRIGGERLEFT:
				break;
			case SDL_CONTROLLER_AXIS_TRIGGERRIGHT:
				break;
			}
		}
	}

	void InputSDL2::ProcessEvent( SDL_ControllerButtonEvent& e )
	{
		if (e.type == SDL_CONTROLLERBUTTONUP)
		{
			if (const auto& gamepad = gamepads.at( e.which ))
				gamepad->OnButtonReleased( static_cast<Input::ButtonCode_T>(TranslateGamepadButtonCode( static_cast<SDL_GameControllerButton>(e.button) )) );
			else
				AV_LOG_ERROR( LoggingChannels::Application, "Got controller button up event for unregonised gamepad '{}'", e.which );
		}
		else if (e.type == SDL_CONTROLLERBUTTONDOWN)
		{
			if (const auto& gamepad = gamepads.at( e.which ))
				gamepad->OnButtonPressed( static_cast<Input::ButtonCode_T>(TranslateGamepadButtonCode( static_cast<SDL_GameControllerButton>(e.button) )) );
			else
				AV_LOG_ERROR( LoggingChannels::Application, "Got controller button down event for unregonised gamepad '{}'", e.which );
		}
		else
			AV_NOT_IMPLEMENTED;
	}

	void InputSDL2::ProcessEvent( SDL_ControllerDeviceEvent& e )
	{
		switch (e.type)
		{
		case SDL_CONTROLLERDEVICEADDED:
		{
			const auto [it, success] = gamepads.try_emplace( e.which, std::shared_ptr<GamepadInputSDL2>{ new GamepadInputSDL2( int( e.which ) ) } );
			AV_ASSERT( success );
			if (success)
			{
				GamepadConnected( it->first );
				AV_LOG_INFO( LoggingChannels::Application, "Gamepad {} connected", e.which );
			}
			break;
		}

		case SDL_CONTROLLERDEVICEREMOVED:
		{
			gamepads.erase( e.which );
			GamepadDisconnected( e.which );
			AV_LOG_INFO( LoggingChannels::Application, "Gamepad {} disconnected", e.which );
			break;
		}

		case SDL_CONTROLLERDEVICEREMAPPED:
			AV_ASSERT( "Gamepad remapping event not handled" );
			break;
		}
	}

	void InputSDL2::ProcessEvent( SDL_ControllerTouchpadEvent& e )
	{
		(void)e;
		AV_NOT_IMPLEMENTED;
	}

	void InputSDL2::ProcessEvent( SDL_ControllerSensorEvent& e )
	{
		(void)e;
		// TODO
	}
}

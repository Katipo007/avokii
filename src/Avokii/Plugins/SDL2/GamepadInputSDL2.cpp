#include "GamepadInputSDL2.hpp"

#include <SDL2/SDL_gamecontroller.h>

namespace Avokii::Plugins
{
	GamepadInputSDL2::~GamepadInputSDL2()
	{
		SDL_GameControllerClose( mControllerPtr );
	}

	std::string_view GamepadInputSDL2::GetButtonName( Input::ButtonCode_T scancode ) const
	{
		AV_NOT_IMPLEMENTED;
		(void)scancode;
		return std::string_view();
	}

	float GamepadInputSDL2::GetAxisValue( Input::GamepadAxis axis )
	{
		(void)axis;
		AV_NOT_IMPLEMENTED;
		return 0.0f;
	}

	Vec2f GamepadInputSDL2::GetThumbstickDirection( Input::GamepadThumbstick stick )
	{
		(void)stick;
		AV_NOT_IMPLEMENTED;
		return Vec2f();
	}

	void GamepadInputSDL2::Update()
	{
		ClearButtonPresses();
	}

	GamepadInputSDL2::GamepadInputSDL2( const int _idx )
		: GamepadInput( static_cast<size_t>(SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_MAX) )
		, mControllerIdx{ _idx }
		, mControllerPtr{ SDL_GameControllerOpen( mControllerIdx ) }
	{
		AV_ASSERT( mControllerPtr != NULL );
	}
}

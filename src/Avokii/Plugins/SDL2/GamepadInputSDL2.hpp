#pragma once

#include "Avokii/Input/GamepadInput.hpp"

struct SDL_JoyAxisEvent;
struct SDL_JoyBallEvent;
struct SDL_JoyHatEvent;
struct SDL_JoyButtonEvent;
struct SDL_ControllerAxisEvent;
struct SDL_ControllerButtonEvent;
struct SDL_ControllerTouchpadEvent;
struct SDL_ControllerSensorEvent;

struct _SDL_GameController;
typedef struct _SDL_GameController SDL_GameController;

namespace Avokii::Plugins
{
	class InputSDL2;

	class GamepadInputSDL2 final
		: public Input::GamepadInput
	{
		friend class InputSDL2;

	public:
		~GamepadInputSDL2();

		std::string_view GetButtonName( Input::ButtonCode_T scancode ) const override;

		float GetAxisValue( Input::GamepadAxis axis ) override;
		Vec2f GetThumbstickDirection( Input::GamepadThumbstick stick ) override;

	private:
		GamepadInputSDL2( int idx );

		void Update();

	private:
		const int mControllerIdx;
		SDL_GameController* mControllerPtr;

		std::array<float, 12> mAxisValues;
	};
}

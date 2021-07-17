#pragma once

#include "Avokii/Input/InputButtonDevice.hpp"
#include "Avokii/Types/Vec2.hpp"

namespace Avokii::Input
{
	enum class GamepadButton : ButtonCode_T
	{
		Invalid = -1,
		Face1 = 0, // A/Cross
		Face2, // B/Circle
		Face3, // X/Square
		Face4, // Y/Triangle
		LeftShoulder,
		RightShoulder,
		LeftTrigger,
		RightTrigger,
		DpadLeft,
		DpadRight,
		DpadUp,
		DpadDown,
		LeftThumbButton,
		RightThumbButton,
		Start, // Start/Options
		Select, // Back/Select
	};

	enum class GamepadThumbstick
	{
		Invalid = -1,
		LeftThumbstick = 0,
		RightThumbstick,
	};

	enum class GamepadAxis
	{
		Invalid = -1,
		LeftThumbstick = 0,
		RightThumbstick,
		LeftTrigger,
		RightTrigger,
	};

	class GamepadInput
		: public InputButtonDevice
	{
	public:
		GamepadInput( size_t button_count );
		virtual ~GamepadInput();

	protected:
		void OnButtonPressed( ButtonCode_T scancode ) override;
		void OnButtonReleased( ButtonCode_T scancode ) override;
		void OnAxisMotion( GamepadAxis axis );

		virtual float GetAxisValue( GamepadAxis axis ) = 0;
		virtual Vec2f GetThumbstickDirection( GamepadThumbstick stick ) = 0;
	};
}

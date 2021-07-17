#include "GamepadInput.hpp"

namespace Avokii::Input
{
	GamepadInput::GamepadInput( size_t button_count )
		: InputButtonDevice{ button_count }
	{
	}

	GamepadInput::~GamepadInput() = default;

	void GamepadInput::OnButtonPressed( ButtonCode_T scancode )
	{
		InputButtonDevice::OnButtonPressed( scancode );
	}

	void GamepadInput::OnButtonReleased( ButtonCode_T scancode )
	{
		InputButtonDevice::OnButtonReleased( scancode );
	}

	void GamepadInput::OnAxisMotion( GamepadAxis axis )
	{
		AV_NOT_IMPLEMENTED;
		(void)axis;
	}
}

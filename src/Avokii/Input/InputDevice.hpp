#pragma once

namespace Avokii::Input
{
	class InputDevice
	{
	public:
		virtual ~InputDevice() = default;

		virtual bool IsEnabled() const noexcept { return true; }
		virtual StringView GetName() const { return ""; }
	};
}

#pragma once

#include "Types/String.hpp"

namespace Avokii::OS
{
	enum class MessageBoxTypes
	{
		Error,
		Warning,
		Info,
	};

	void ShowMessageBox( StringView message, StringView title, const MessageBoxTypes type = MessageBoxTypes::Info );
}

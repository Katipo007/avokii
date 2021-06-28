#pragma once

#include "Concepts.hpp"

namespace Avokii
{
	namespace fsm
	{
		/// <summary>
		/// Informs that the state machine does not change from the event
		/// </summary>
		struct NoAction
		{
			constexpr NoAction() = default;

			template<typename Machine, Concepts::State State, Concepts::Event Event>
			inline void Execute( Machine&, State&, const Event& ) {}
		};
	}
}

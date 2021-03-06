#pragma once

#include <concepts>
#include <type_traits>

namespace Avokii
{
	namespace fsm::Concepts
	{
		template<class T>
		concept State =
			std::is_default_constructible<T>::value
			|| (std::movable<T> && std::copyable<T>)
			;

		template<class T>
		concept Event = true;/*requires
		{
			std::movable<T>;
			std::is_trivially_move_constructible<T>::value;
		};*/

	}

	namespace fsm
	{
		template<Concepts::State... States_>
		class States;

		template<Concepts::Event... Events_>
		class Events;

		template<class States, class Events>
		class Machine;
	}

	namespace fsm::Concepts
	{
		template<class T>
		concept Action = requires(T t)
		{
			std::copyable<T>;
			std::movable<T>;
			std::is_trivially_move_constructible<T>::value;
			{ t.Execute( std::placeholders::_1, std::placeholders::_2, std::placeholders::_3 ) } -> std::same_as<void>;
		};
	}
}

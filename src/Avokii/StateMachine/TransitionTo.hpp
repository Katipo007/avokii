#pragma once

#include <type_traits>

#include "NoAction.hpp"
#include "Concepts.hpp"

namespace Avokii
{
	namespace fsm
	{
		namespace detail
		{
			template<typename T>
			concept HasDefaultOnEnterMethod = requires(T t)
			{
				{ t.OnEnter() };
			};

			template<typename T, typename... Args>
			concept HasOnEnterMethod = requires(T t)
			{
				{ t.OnEnter( std::declval<Args>()... ) };
			};

			template<typename T>
			concept HasDefaultOnLeaveMethod = requires(T t)
			{
				{ t.OnLeave() };
			};

			template<typename T, typename... Args>
			concept HasOnLeaveMethod = requires(T t)
			{
				{ t.OnLeave( std::declval<Args>()... ) };
			};
		}

		/// <summary>
		/// Have the state machine transition to the given state
		/// </summary>
		/// <typeparam name="TargetState">State type to transition to</typeparam>
		template<class TargetState>
		class TransitionTo
		{
		public:
			constexpr TransitionTo() = default;

			template<typename Machine, Concepts::State PreviousState, Concepts::Event Event>
			void Execute( Machine& machine, PreviousState& previous_state, const Event& e )
			{
				static_assert(std::same_as<PreviousState, TargetState> == false, "Not allowed to transition to current state");
				static_assert(Concepts::State<TargetState>); // we have this concept requirement inside the definition so that we can using in-complete types

				if constexpr (detail::HasOnLeaveMethod<PreviousState, const Event&>)
					previous_state.OnLeave( e );
				else if constexpr (detail::HasDefaultOnLeaveMethod<PreviousState>)
					previous_state.OnLeave();

				TargetState& new_state = machine.template TransitionTo<TargetState>();

				if constexpr (detail::HasOnEnterMethod<TargetState, const Event&>)
					new_state.OnEnter( e ).Execute( machine, new_state, e );
				else if constexpr (detail::HasDefaultOnEnterMethod<TargetState>)
					new_state.OnEnter().Execute( machine, new_state, e );
			}
		};
	}
}

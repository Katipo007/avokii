#pragma once

/// <summary>
/// Draws heavily from the following articles (some chunks are a direct translation)
/// https://sii.pl/blog/implementing-a-state-machine-in-c17/
/// https://sii.pl/blog/implementing-a-state-machine-in-c17-part-2/
/// https://sii.pl/blog/implementing-a-state-machine-in-c17-part-3-compile-time-strings/
/// 
/// All the code is also avaliable in a repo here:
/// https://github.com/AdamsPL/state-machine
/// </summary>

#include <tuple>
#include <type_traits>
#include <utility>
#include <variant>

#include "Avokii/Utility/TupleReflection.hpp"
#include "Concepts.hpp"

namespace Avokii
{
	namespace fsm
	{
		template<Concepts::State... States_>
		class States;

		template<Concepts::Event... Events_>
		class Events;

		template<class States, class Events>
		class Machine;

		template<class>
		class TransitionTo;

		/// <summary>
		/// Compiletime enforced state machine.
		/// States can also optionally provide OnEnter() and OnLeave() methods
		/// </summary>
		/// <typeparam name="..._States"></typeparam>
		/// <typeparam name="..._Events"></typeparam>
		template<Concepts::State... States_, Concepts::Event... Events_>
		class Machine<States<States_...>, Events<Events_...>>
		{
			// Allow TransitionTo action to access protected members so it can call TransitionTo method
			template<class>
			friend class ::Avokii::fsm::TransitionTo;

			using States_T = std::tuple<States_...>;
			using Events_T = std::tuple<Events_...>;
		public:
			using EventsVariant_T = std::variant<Events_...>;

			Machine()
				: mStates{}
				, mCurrentState{ &std::get<0>( mStates ) }
			{}
			virtual ~Machine() noexcept {}

			// contruct the machine with pre-created states
			explicit Machine( States_&&... states_ )
				: mStates{ std::forward<States_>( states_ )... }
				, mCurrentState{ &std::get<0>( mStates ) }
			{
			}

			/// <summary>
			/// Get the current state variant
			/// </summary>
			std::variant<States_*...> GetActiveState() { return mCurrentState; }

			/// <summary>
			/// Test whether the machine is currently in a given state
			/// </summary>
			template<Concepts::State State>
			bool IsInState() const
			{
				static_assert(TupleReflection::tuple_contains<States_T, State>(), "StateMachine doesn't contain state State");

				return std::get_if<State*>( &mCurrentState ) != nullptr;
			}

			/// <summary>
			/// Get a reference to a state, even if it isn't the current state
			/// </summary>
			template<Concepts::State State>
			State& GetState()
			{
				static_assert(TupleReflection::tuple_contains<States_T, State>(), "StateMachine doesn't contain state State");

				constexpr auto state_index = TupleReflection::tuple_index<States_T, State>::value;
				return std::get<state_index>( mStates );
			}

			/// <summary>
			/// Get a const reference to a state, even if it isn't the current state
			/// </summary>
			template<Concepts::State State>
			const State& GetState() const
			{
				static_assert(TupleReflection::tuple_contains<States_T, State>(), "StateMachine doesn't contain state State");

				constexpr auto state_index = TupleReflection::tuple_index<States_T, State>::value;
				return std::get<state_index>( mStates );
			}

			/// <summary>
			/// Have the current state handle an event
			/// </summary>
			template<Concepts::Event Event>
			void Handle( const Event& event )
			{
				static_assert(TupleReflection::tuple_contains<Events_T, Event>(), "Unhandled event type");
				HandleBy( event, *this );
			}

		protected:
			/// <summary>
			/// Have the current state of this machine handle an event, performing the action on a given machine
			/// </summary>
			template<Concepts::Event Event>
			void HandleBy( const Event& e, Machine& machine )
			{
				auto PassEventToState = [&machine, &e]( auto* state )
				{
					Concepts::Action auto action = state->HandleEvent( e );
					action.Execute( machine, *state, e );
				};
				std::visit( PassEventToState, mCurrentState );
			}

			/// <summary>
			/// IMPORTANT: DOES NOT CALL OnEnter or OnLeave on the new/old state!
			/// Have the machine transition to the given state.
			/// </summary>
			/// <returns></returns>
			template<Concepts::State State>
			State& TransitionTo()
			{
				auto& new_state = std::get<State>( mStates );
				mCurrentState = &new_state;
				return new_state;
			}

		private:
			std::tuple<States_...> mStates;
			std::variant<States_*...> mCurrentState; // start in the first listed event

		private:
			//
			// We don't support copy or move constructors yet :(
			//
			Machine( const Machine& ) = delete;
			Machine( Machine&& ) = delete;
			Machine& operator=( const Machine& ) = delete;
			Machine& operator=( Machine&& ) = delete;
		};
	}
}

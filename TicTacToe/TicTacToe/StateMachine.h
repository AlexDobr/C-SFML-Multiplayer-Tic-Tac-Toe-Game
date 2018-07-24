#pragma once


#include <memory>
#include <stack>
#include "State.h"



namespace TTT

{

	typedef std::unique_ptr<State> StateRef;


	// manage states that are used in Game.  E.g: Main menu state

	class StateMachine

	{

	public:

		StateMachine() { }

		~StateMachine() { }



		void AddState(StateRef newState, bool isReplacing = true);

		void RemoveState();

		void ProcessStateChanges();



		StateRef &GetActiveState();



	private:

		std::stack<StateRef> _states;

		StateRef _newState;



		bool _isRemoving;

		bool _isAdding, _isReplacing;

	};

}
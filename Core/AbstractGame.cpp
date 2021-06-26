#include "AbstractGame.hpp"

namespace Avokii
{
	AbstractGame::~AbstractGame()
	{
	}

	void AbstractGame::Exit( int exit_code_ )
	{
		mApplicationExitCode = exit_code_;
	}
}

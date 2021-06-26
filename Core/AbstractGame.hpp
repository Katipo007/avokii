#pragma once

#include <optional>
#include "Timestep.hpp"

namespace Avokii
{
	class Core;
	class ResourceManager;

	class AbstractGame
	{
		friend class Core;

	public:
		virtual ~AbstractGame();

		void Exit( int exit_code );

	protected:
		virtual void Init() = 0;
		virtual void OnGameEnd() = 0;
		std::optional<int> GetExitCode() const noexcept { return mApplicationExitCode; }

		virtual void OnFixedUpdate( const PreciseTimestep& ts ) = 0;
		virtual void OnVariableUpdate( const PreciseTimestep& ts ) = 0;
		virtual void OnRender( const PreciseTimestep& ts ) = 0;

		Core& GetCore() noexcept { AV_ASSERT( mpCore != nullptr ); return *mpCore; }
		const Core& rGetCore() const noexcept { AV_ASSERT( mpCore != nullptr ); return *mpCore; }

	private:
		Core* mpCore = nullptr;
		ResourceManager* mpResourceManager = nullptr;
		std::optional<int> mApplicationExitCode = std::nullopt;
	};
}

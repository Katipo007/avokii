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

		/// <summary>
		/// Doesn't cause an immediate exit. Sets the exit code which Core::Dispatch will return
		/// </summary>
		void Exit( int exit_code );

	protected:
		virtual void Init() = 0;
		virtual void OnGameEnd() = 0;
		std::optional<int> GetExitCode() const noexcept { return mApplicationExitCode; }

		virtual void OnFixedUpdate( const PreciseTimestep& ts ) = 0;
		virtual void OnVariableUpdate( const PreciseTimestep& ts ) = 0;
		virtual void OnRender( const PreciseTimestep& ts ) = 0;

		Core& rGetCore() noexcept { AV_ASSERT( mpCore != nullptr ); return *mpCore; }
		const Core& GetCore() const noexcept { AV_ASSERT( mpCore != nullptr ); return *mpCore; }

		ResourceManager& rGetResourceManager() noexcept { AV_ASSERT( mpResourceManager != nullptr ); return *mpResourceManager; }
		const ResourceManager& GetResourceManager() const noexcept { AV_ASSERT( mpResourceManager != nullptr ); return *mpResourceManager; }

	private:
		Core* mpCore = nullptr;
		ResourceManager* mpResourceManager = nullptr;
		std::optional<int> mApplicationExitCode = std::nullopt;
	};
}

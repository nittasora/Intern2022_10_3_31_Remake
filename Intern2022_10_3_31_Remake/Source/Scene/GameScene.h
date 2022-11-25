#pragma once

#include "SceneCommon.h"

namespace MySystem
{
	class GameScene : public App::Scene
	{
	private:
		class Impl;
		std::unique_ptr<Impl> _pImpl;

	public:
		GameScene(const InitData& init);
		~GameScene();

		void update() override;
		void draw() const override;
	};
}


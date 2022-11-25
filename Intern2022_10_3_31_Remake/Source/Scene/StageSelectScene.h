#pragma once

#include "SceneCommon.h"

namespace MySystem
{
	class SelectScene : public App::Scene
	{
	private:
		class Impl;
		std::unique_ptr<Impl> _pImpl;

	public:
		SelectScene(const InitData& init);
		~SelectScene();

		void update() override;
		void draw() const override;
	};
}


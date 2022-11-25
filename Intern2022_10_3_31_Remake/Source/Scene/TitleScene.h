#pragma once

#include "SceneCommon.h"

namespace MySystem
{
	class TitleScene : public App::Scene
	{
	private:
		class Impl;
		std::unique_ptr<Impl> _pImpl;

	public:
		TitleScene(const InitData& init);
		~TitleScene();

		void update() override;
		void draw() const override;
	};
}

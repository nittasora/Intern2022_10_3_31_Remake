# include <Siv3D.hpp>
#include "Scene/SceneCommon.h"
#include "Scene/TitleScene.h"
#include "Scene/StageSelectScene.h"
#include "Scene/GameScene.h"

void Main()
{
	Window::Resize(1200, 720);

	MySystem::App manager;
	manager.add<MySystem::TitleScene>(U"Title");
	manager.add<MySystem::SelectScene>(U"Select");
	manager.add<MySystem::GameScene>(U"Game");

	while (System::Update())
	{
		if (manager.update())
		{

		}
	}
}

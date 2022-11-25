#include "GameScene.h"
#include "../GameObject/Object/Stage.h"

namespace MySystem
{
	class GameScene::Impl
	{
	public:
		Stage stage;

		Impl() :
			stage{ Scene::Center(), Vec2{1.0, 1.0}, 0.0, nullptr }
		{

		}
	};

	GameScene::GameScene(const InitData& init) :
		IScene{ init },
		_pImpl{ new Impl{} }
	{
		auto& data = getData();
		_pImpl->stage.CreateStage(data.worldIndex, data.stageIndex);
	}

	GameScene::~GameScene()
	{

	}

	void GameScene::update()
	{
		_pImpl->stage.Update();

		if (_pImpl->stage.LoadStageWait())
		{
			changeScene(U"Game");
		}
		else if (_pImpl->stage.LoadSelectWait())
		{
			changeScene(U"Select");
		}
	}

	void GameScene::draw() const
	{
		_pImpl->stage.Draw();
	}

}

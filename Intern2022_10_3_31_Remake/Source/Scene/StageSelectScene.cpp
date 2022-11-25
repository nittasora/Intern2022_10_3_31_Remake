#include "StageSelectScene.h"
#include "../GameObject/Object/Stage.h"
#include "../Utility/CommonKey.h"
#include "../GUI/ButtonGUI.h"

namespace MySystem
{
	class Header
	{
	private:
		static constexpr double _OFFSET_Y = 10.0;

		double	_rate;

		String	_worldText;
		String	_stageText;
		Font	_worldFont;
		Font	_stageFont;

		Circle			_worldBack;
		Polygon			_stageBack;
		Polygon			_shutter;
		Array<Polygon>	_frames;

	public:
		Header() :
			_rate{ 0.0 },
			_worldText{ U"" },
			_stageText{ U"" }
		{
			auto seneSize = Scene::Size();
			auto halfSceneSize = seneSize * 0.5;

			auto size = seneSize - (seneSize * 0.75);

			auto halfSize = size * 0.5;
			auto quarterSize = size * 0.25;

			_shutter = Polygon{ Vec2{ 0.0, 0.0 }, Vec2{ seneSize.x, 0.0 }, Vec2{ seneSize.x, quarterSize.y }, Vec2{ halfSceneSize.x, quarterSize.y }, Vec2{ halfSceneSize.x, halfSize.y }, Vec2{ 0.0, halfSize.y } };

			_stageBack = RoundRect{ 0.0, 0.0, halfSceneSize.x, halfSize.y, halfSize.y }.stretched(-halfSize.y * 0.1).asPolygon();

			_worldBack = Circle{ Arg::topLeft = Vec2{ 0.0, 0.0 }, quarterSize.y };

			auto temps = Geometry2D::Subtract(_shutter, _worldBack.asPolygon());

			for (auto temp : temps)
			{
				_frames.append(Geometry2D::Subtract(temp, _stageBack));
			}

			_worldFont = Font{ static_cast<int32>(quarterSize.y * 0.8), Typeface::Heavy };
			_stageFont = Font{ static_cast<int32>(quarterSize.y * 0.5), Typeface::Heavy };
		}

		void Draw() const
		{
			const double THICKNESS = 5.0;
			const double OFFSET_Y = sin(_rate * 90.0_deg) * -_OFFSET_Y;

			_stageBack.
				movedBy(Vec2{ 0.0, OFFSET_Y }).
				draw();

			_worldBack.
				movedBy(Vec2{ 0.0, OFFSET_Y }).
				draw(Palette::Lightgray).
				drawFrame(THICKNESS, Palette::Gray);

			auto worldFontPosition = _worldBack.center;
			auto stageFontPosition = _stageBack.centroid();

			worldFontPosition.y += OFFSET_Y;
			stageFontPosition.y += OFFSET_Y;

			_worldFont(_worldText).drawAt(worldFontPosition, Palette::Black);
			_stageFont(_stageText).drawAt(stageFontPosition, Palette::Black);

			for (auto frame : _frames)
			{
				frame.
					movedBy(Vec2{ 0.0, OFFSET_Y }).
					draw(Palette::Orange).
					drawFrame(THICKNESS, Palette::Orangered);
			}
		}

		void Create(const StageData::WorldIndex& worldIndex, const StageData::StageIndex& stageIndex)
		{
			_worldText = U"W{:2d}"_fmt(worldIndex + 1);
			_stageText = U"S{:2d} : "_fmt(stageIndex + 1);

			_stageText += StageData::GetCurrentData()->stageName;
		}

		void SetRate(const double rate)
		{
			_rate = rate;
		}
	};

	class Footer
	{
	private:
		static constexpr double _OFFSET_Y = 10.0;

		double	_rate;

		ImageButtonGUI	_playButton;
		Polygon			_frame;

	public:
		Footer() :
			_rate{ 0.0 },
			_playButton{ U"Assets/Image/StartButton.png", Vec2{ 0.0, 0.0 }, Vec2{ 0.0, 0.0 } }
		{
			auto seneSize = Scene::Size();
			auto halfSceneSize = seneSize * 0.5;

			auto size = seneSize - (seneSize * 0.75);

			auto halfSize = size * 0.5;
			auto quarterSize = size * 0.25;

			_playButton.SetIsCircle(true);

			auto radius = size.y / 1.5;
			_playButton.SetSize(Vec2{ radius, radius });
			_playButton.SetPosition(seneSize - _playButton.GetSize() * 2);

			_frame = Polygon{
				Vec2{ 0.0, seneSize.y - quarterSize.y },
				Vec2{ halfSceneSize.x, seneSize.y - quarterSize.y },
				Vec2{ halfSceneSize.x, seneSize.y - halfSize.y },
				Vec2{ seneSize.x, seneSize.y - halfSize.y },
				Vec2{ seneSize.x, seneSize.y },
				Vec2{ 0.0, seneSize.y }
			};
		}

		void Update()
		{
			const double OFFSET_Y = sin(_rate * 90.0_deg) * _OFFSET_Y;
			auto position = Scene::Size() - _playButton.GetSize() * 2.0;
			position.y += OFFSET_Y;

			_playButton.SetPosition(position);

			_playButton.Update();
		}

		void Draw() const
		{
			const double THICKNESS = 5.0;
			const double OFFSET_Y = sin(_rate * 90.0_deg) * _OFFSET_Y;

			_frame.
				movedBy(Vec2{ 0.0, OFFSET_Y }).
				draw(Palette::Orange).
				drawFrame(THICKNESS, Palette::Orangered);

			_playButton.Draw();
		}

		void SetRate(const double rate)
		{
			_rate = rate;
		}

		bool GetPlayButtonIsOn()
		{
			return _playButton.GetIsOn();
		}
	};

	class StageView
	{
	private:
		enum class _Type
		{
			Main,
			UnPop,
			Pop,
			WaitLoadStage
		};

		static constexpr double _STAGE_SCALE = 0.6;

		StageData::WorldIndex _worldIndex;
		StageData::StageIndex _stageIndex;

		_Type		_type;
		Stopwatch	_stopWatch;

		Stage	_stage;
		Header	_header;
		Footer	_footer;

	public:
		StageView(const StageData::WorldIndex& worldIndex, const StageData::StageIndex& stageIndex) :
			_worldIndex{worldIndex},
			_stageIndex{stageIndex},
			_type{ _Type::Main },
			_stopWatch{},
			_stage{ Scene::CenterF(), Vec2{ _STAGE_SCALE, _STAGE_SCALE }, 0.0, nullptr },
			_header{},
			_footer{}
		{
			auto size = Scene::Size() - (Scene::Size() * _STAGE_SCALE);

			Create(_worldIndex, _stageIndex);
		}

		void Update(CommonSceneData& data)
		{
			switch (_type)
			{
			case MySystem::StageView::_Type::Main:
				MainUpdate(data);
				break;
			case MySystem::StageView::_Type::UnPop:
				UnPopUpdate();
				break;
			case MySystem::StageView::_Type::Pop:
				PopUpdate();
				break;
			case MySystem::StageView::_Type::WaitLoadStage:
				break;
			}

			_footer.Update();

			if (_footer.GetPlayButtonIsOn())
			{
				_type = _Type::WaitLoadStage;
			}
		}

		void Draw() const
		{
			_stage.Draw();
			_header.Draw();
			_footer.Draw();
		}

		bool GetIsWaitLoadStage()
		{
			return _type == _Type::WaitLoadStage;
		}

	private:
		void MainUpdate(CommonSceneData& data)
		{
			if (CommonKeyState::gINPUT_LEFT.down())
			{
				if (data.stageIndex > 0)
				{
					data.stageIndex--;
				}
			}
			if (CommonKeyState::gINPUT_RIGHT.down())
			{
				if (data.stageIndex < CommonSceneData::gMAX_STAGE_NUM - 1)
				{
					data.stageIndex++;
				}
			}
			if (CommonKeyState::gINPUT_DOWN.down())
			{
				if (data.worldIndex > 0)
				{
					data.worldIndex--;
				}
			}
			if (CommonKeyState::gINPUT_UP.down())
			{
				if (data.worldIndex < CommonSceneData::gMAX_WORLD_NUM - 1)
				{
					data.worldIndex++;
				}
			}

			if (data.stageIndex != _stageIndex || data.worldIndex != _worldIndex)
			{
				_worldIndex = data.worldIndex;
				_stageIndex = data.stageIndex;

				_type = _Type::UnPop;
				_stopWatch.restart();
			}
		}

		void UnPopUpdate()
		{
			const double FADE_IN_TIME = 0.2;

			double rate = _stopWatch.sF() / FADE_IN_TIME;

			if (rate >= 1.0)
			{
				rate = 1.0;
				Create(_worldIndex, _stageIndex);
				_stopWatch.restart();
				_type = _Type::Pop;
			}

			auto newScale = _STAGE_SCALE * (1.0 - rate);
			_stage.SetLocalScale(Vec2{ newScale, newScale });
			_header.SetRate(rate);
			_footer.SetRate(rate);
		}

		void PopUpdate()
		{
			const double FADE_IN_TIME = 0.25;

			double rate = _stopWatch.sF() / FADE_IN_TIME;

			if (rate >= 1.0)
			{
				rate = 1.0;
				_stopWatch.restart();
				_type = _Type::Main;
			}

			auto newScale = _STAGE_SCALE * rate;
			_stage.SetLocalScale(Vec2{ newScale, newScale });
			_header.SetRate(1.0 - rate);
			_footer.SetRate(1.0 - rate);
		}

		void Create(const StageData::WorldIndex& worldIndex, const StageData::StageIndex& stageIndex)
		{
			_stage.CreateStage(worldIndex, stageIndex);
			_header.Create(worldIndex, stageIndex);
		}
	};

	class SelectScene::Impl
	{
	public:
		StageView stage;

		Impl(const StageData::WorldIndex& worldIndex, const StageData::StageIndex& stageIndex) :
			stage{ worldIndex, stageIndex }
		{

		}
	};

	SelectScene::SelectScene(const InitData& init) :
		IScene{ init },
		_pImpl{ new Impl{ getData().worldIndex, getData().stageIndex }}
	{
	}

	SelectScene::~SelectScene()
	{
	}

	void SelectScene::update()
	{
		_pImpl->stage.Update(getData());

		if (_pImpl->stage.GetIsWaitLoadStage())
		{
			changeScene(U"Game");
		}
	}

	void SelectScene::draw() const
	{
		_pImpl->stage.Draw();

		//auto size = Scene::Size() - (Scene::Size() * 0.75);

		//auto halfWidth = Scene::Width() * 0.5;
		//auto halfHeight = Scene::Height() * 0.5;
		//auto QuarterWidth = Scene::Width() * 0.25;
		//auto QuarterHeight = Scene::Height() * 0.25;

		//RectF{ 0.0, 0.0, halfWidth, size.y / 2.0 }.draw();
		//RectF{ halfWidth, 0.0, halfWidth, size.y / 4.0 }.draw();

		//RoundRect{ 0.0, 0.0, halfWidth, size.y * 0.5, size.y * 0.5 }.stretched(-size.y * 0.5 * 0.1).draw(Palette::Black);

		//Circle{ Arg::topLeft = Vec2{ 0.0, 0.0 }, size.y / 4.0 }.draw(Palette::Gray);

		//RectF{ halfWidth, Scene::Height() - size.y / 2.0, halfWidth, size.y / 2.0}.draw();
		//RectF{ 0.0, Scene::Height() - size.y / 4.0, halfWidth, size.y / 4.0 }.draw();

		//Circle{ Arg::bottomRight = Vec2{ Scene::Width() - (size.y / 3.0 * 2.0 + size.y / 1.5), Scene::Height() }, size.y / 3.0}.draw(Palette::Lightgray);
		//Circle{ Arg::bottomRight = Scene::Size(), size.y / 1.5 }.draw(Palette::Gray);
	}
}

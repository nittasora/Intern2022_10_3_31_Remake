#include "TitleScene.h"

namespace MySystem
{
	class LogoRect
	{
	private:
		Vec2		_position;
		Vec2		_from;
		Vec2		_to;
		Vec2		_size;
		ColorF		_color;

	public:
		LogoRect(const Vec2& from, const Vec2& to, const Vec2& size, const ColorF& color) :
			_position{ to.normalized() * (Scene::Size().length() + Random() * 100.0) },
			_from{ _position },
			_to{ to },
			_size{ size },
			_color{ color }
		{
		}

		void FadeInUpdate(const double rate)
		{
			_position = _from.lerp(_to, rate);
		}

		void Draw(const Vec2& offset) const
		{
			RectF{ Arg::center = _position + offset, _size }.draw(_color);
		}
	};

	class LogoFont
	{
	private:
		static constexpr Vec2 _INFORMATION_OFFSET{ 0.0, 200.0 };

		static const String	_TITLE;
		static const String	_INFORMATION;

		Font	_titleFont;
		Font	_informationFont;
		double	_rate;

	public:
		LogoFont() :
			_titleFont{ FontMethod::SDF, 120, Typeface::Heavy },
			_informationFont{ FontMethod::SDF, 40 },
			_rate{ 0.0 }
		{

		}

		void FadeInUpdate(const double rate)
		{
			_rate = rate;
		}

		void Draw(const Vec2& offset) const
		{
			const double informationRate = Periodic::Sine0_1(2s) / 2.0 + 0.5;
			ColorF informationColor = Palette::Yellow;
			ColorF informationOutLineColor = Palette::Orange;

			informationColor.a *= informationRate;
			informationOutLineColor.a *= informationRate;

			_titleFont(_TITLE.substr(0, _TITLE.length() * _rate)).
				draw(TextStyle::Outline(1.1, Palette::Black), Arg::center = offset, Palette::Red);

			_informationFont(_INFORMATION.substr(0, _INFORMATION.length() * _rate)).				
				draw(TextStyle::Outline(0.1, informationOutLineColor), Arg::center = offset + _INFORMATION_OFFSET, informationColor);
		}
	};

	const String LogoFont::_TITLE = U"カサネルート";
	const String LogoFont::_INFORMATION = U"Press Any Key!!";

	class TitleLogo
	{
	private:
		enum class _Type
		{
			FadeIn,
			Main,
			FadeOut,
			Finish
		};

		bool			_isFirst;
		_Type			_type;
		Stopwatch		_stopWatch;
		LogoFont		_titleText;
		Array<LogoRect> _aBackRects;

	public:
		TitleLogo() noexcept :
			_isFirst{ true },
			_type{ _Type::FadeIn },
			_stopWatch{},
			_titleText{}
		{
			_aBackRects.emplace_back(LogoRect{ Vec2{ 0.0, 0.0 }, Vec2{   70.0, -165.0 }, Vec2{ 110.0,  90.0 }, ColorF{ 0.2, 0.2, 0.2, 1.0 } });
			_aBackRects.emplace_back(LogoRect{ Vec2{ 0.0, 0.0 }, Vec2{  100.0,  -75.0 }, Vec2{ 130.0,  95.0 }, ColorF{ 1.0, 1.0, 1.0, 1.0 } });
			_aBackRects.emplace_back(LogoRect{ Vec2{ 0.0, 0.0 }, Vec2{  -20.0,  120.0 }, Vec2{ 190.0, 170.0 }, ColorF{ 0.2, 0.2, 0.2, 1.0 } });
			_aBackRects.emplace_back(LogoRect{ Vec2{ 0.0, 0.0 }, Vec2{  -70.0,   20.0 }, Vec2{ 195.0, 165.0 }, ColorF{ 1.0, 1.0, 1.0, 1.0 } });
			_aBackRects.emplace_back(LogoRect{ Vec2{ 0.0, 0.0 }, Vec2{ -180.0,   70.0 }, Vec2{ 240.0, 190.0 }, ColorF{ 0.5, 0.5, 0.5, 1.0 } });
			_aBackRects.emplace_back(LogoRect{ Vec2{ 0.0, 0.0 }, Vec2{ -300.0,  -25.0 }, Vec2{ 160.0, 155.0 }, ColorF{ 1.0, 1.0, 1.0, 1.0 } });
			_aBackRects.emplace_back(LogoRect{ Vec2{ 0.0, 0.0 }, Vec2{ -200.0,   70.0 }, Vec2{  50.0,  50.0 }, ColorF{ 1.0, 1.0, 1.0, 1.0 } });
			_aBackRects.emplace_back(LogoRect{ Vec2{ 0.0, 0.0 }, Vec2{  100.0,   45.0 }, Vec2{ 275.0, 175.0 }, ColorF{ 0.3, 0.3, 0.3, 1.0 } });
			_aBackRects.emplace_back(LogoRect{ Vec2{ 0.0, 0.0 }, Vec2{  280.0,  110.0 }, Vec2{ 195.0, 110.0 }, ColorF{ 0.9, 0.9, 0.9, 1.0 } });
			_aBackRects.emplace_back(LogoRect{ Vec2{ 0.0, 0.0 }, Vec2{   -5.0,  -25.0 }, Vec2{  50.0, 165.0 }, ColorF{ 0.7, 0.7, 0.7, 1.0 } });
			_aBackRects.emplace_back(LogoRect{ Vec2{ 0.0, 0.0 }, Vec2{    0.0, -125.0 }, Vec2{ 185.0, 100.0 }, ColorF{ 0.3, 0.3, 0.3, 1.0 } });
			_aBackRects.emplace_back(LogoRect{ Vec2{ 0.0, 0.0 }, Vec2{ -185.0,  -70.0 }, Vec2{  90.0,  80.0 }, ColorF{ 0.3, 0.3, 0.3, 1.0 } });
			_aBackRects.emplace_back(LogoRect{ Vec2{ 0.0, 0.0 }, Vec2{ -160.0, -120.0 }, Vec2{  35.0,  60.0 }, ColorF{ 1.0, 1.0, 1.0, 1.0 } });
			_aBackRects.emplace_back(LogoRect{ Vec2{ 0.0, 0.0 }, Vec2{ -100.0, -160.0 }, Vec2{ 105.0,  60.0 }, ColorF{ 0.9, 0.9, 0.9, 1.0 } });
			_aBackRects.emplace_back(LogoRect{ Vec2{ 0.0, 0.0 }, Vec2{  145.0, -135.0 }, Vec2{ 150.0,  95.0 }, ColorF{ 0.5, 0.5, 0.5, 1.0 } });
			_aBackRects.emplace_back(LogoRect{ Vec2{ 0.0, 0.0 }, Vec2{   95.0,  -75.0 }, Vec2{  25.0, 120.0 }, ColorF{ 0.7, 0.7, 0.7, 1.0 } });
			_aBackRects.emplace_back(LogoRect{ Vec2{ 0.0, 0.0 }, Vec2{  220.0,  -85.0 }, Vec2{  90.0,  80.0 }, ColorF{ 1.0, 1.0, 1.0, 1.0 } });
		}

		void Update()
		{
			if (_isFirst)
			{
				_isFirst = false;
				_stopWatch.restart();
			}

			switch (_type)
			{
			case MySystem::TitleLogo::_Type::FadeIn:
				FadeInUpdate();
				break;
			case MySystem::TitleLogo::_Type::Main:
				MainUpdate();
				break;
			case MySystem::TitleLogo::_Type::FadeOut:
				FadeOutUpdate();
				break;
			case MySystem::TitleLogo::_Type::Finish:
				break;
			}
		}

		void Draw() const
		{
			Vec2 offset = Scene::CenterF();

			for (auto rect : _aBackRects)
			{
				rect.Draw(offset);
			}

			_titleText.Draw(offset);
		}

		bool GetIsFinish()
		{
			return _type == _Type::Finish;
		}

	private:
		void FadeInUpdate()
		{
			const double FADE_IN_TIME = 0.25;

			double rate = _stopWatch.sF() / FADE_IN_TIME;

			if (rate >= 1.0)
			{
				rate = 1.0;
				_stopWatch.pause();
				_type = _Type::Main;
			}

			for (auto& rect : _aBackRects)
			{
				rect.FadeInUpdate(rate);
			}

			_titleText.FadeInUpdate(rate);
		}

		void MainUpdate()
		{
			bool isDown = false;

			for (auto input : Keyboard::GetAllInputs())
			{
				if (not input.down())
				{
					continue;
				}

				isDown = true;
				break;
			}

			if (isDown)
			{
				_type = _Type::FadeOut;
				_isFirst = true;
			}
		}

		void FadeOutUpdate()
		{
			const double FADE_IN_TIME = 0.5;

			double rate = _stopWatch.sF() / FADE_IN_TIME;

			if (rate >= 1.0)
			{
				rate = 1.0;
				_stopWatch.pause();
				_type = _Type::Finish;
			}

			for (auto& rect : _aBackRects)
			{
				rect.FadeInUpdate(1.0 - rate);
			}

			_titleText.FadeInUpdate(1.0 - rate);
		}
	};

	class TitleScene::Impl
	{
	private:
		TitleLogo	_logo;

	public:
		Impl()
		{

		}

		bool Update()
		{
			_logo.Update();

			return _logo.GetIsFinish();
		}

		void Draw() const
		{
			_logo.Draw();
		}
	};

	TitleScene::TitleScene(const InitData& init) :
		IScene{ init },
		_pImpl{ new Impl{} }
	{
		TextureAsset::Register(U"TitleImage", U"Assets/Image/Title.png");
	}

	TitleScene::~TitleScene()
	{
	}

	void TitleScene::update()
	{
		if (_pImpl->Update())
		{
			changeScene(U"Select");
		}
	}

	void TitleScene::draw() const
	{
		_pImpl->Draw();
	}
}

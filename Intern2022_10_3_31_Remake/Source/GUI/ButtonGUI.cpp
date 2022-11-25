/** ****************************************************************************************************
 * @file
 * @brief GUIボタン機能
 **************************************************************************************************** */
#include "ButtonGUI.h"
#include "CommonGUI.h"
#include "../Utility/BitFlag.h"

namespace MySystem
{
	namespace
	{
		using Flag = uint8;	//!< フラグ用

		constexpr Flag gENABLE = BitFlag::Get<Flag>(0);		//!< 有効かどうか
		constexpr Flag gIS_ON = BitFlag::Get<Flag>(1);		//!< 押されているかどうか
		constexpr Flag gIS_TRIGGER = BitFlag::Get<Flag>(2);	//!< トリガーボタンかどうか
		constexpr Flag gIS_CIRCLE = BitFlag::Get<Flag>(3);	//!< 円で表示するかどうか

		constexpr ColorF gCLICKED_COLOR = ColorF{ 0.8 };	//!< クリックされたときの表示色
	}

	class TextButtonGUI::Impl
	{
	public:
		Vec2	position;	//!< 表示位置
		double	width;		//!< 表示サイズ
		Flag	flag;		//!< 各種フラグをまとめたもの
		String	text;		//!< 表示文字

	public:
		Impl(const Vec2& position, const double width, const bool enable, const bool isTrigger, const String& text) :
			position{ position },
			width{ width },
			flag{ static_cast<Flag>(0) },
			text{ text }
		{
			BitFlag::PopOrUnPop(flag, gENABLE, enable);
			BitFlag::PopOrUnPop(flag, gIS_TRIGGER, isTrigger);
		}
	};

	TextButtonGUI::TextButtonGUI(const String& label, const Vec2& position, const double width) :
		TextButtonGUI(label, position, width, true, true)
	{

	}

	TextButtonGUI::TextButtonGUI(const String& label, const Vec2& position, const double width, const bool isTrigger) :
		TextButtonGUI(label, position, width, isTrigger, true)
	{

	}

	TextButtonGUI::TextButtonGUI(const String& label, const Vec2& position, const double width, const bool isTrigger, const bool enable) :
		_pImpl{ new Impl{ position, width, enable, isTrigger, label } }
	{
		SetSize(_pImpl->width);
	}

	TextButtonGUI::~TextButtonGUI() = default;

	bool TextButtonGUI::Update(const Vec2& parent) noexcept
	{
		auto parameter = _pImpl.get();
		const RectF hitBox{ parameter->position + parent, GetSize() };
		auto enable = GetEnable();
		auto isTrigger = GetIsTrigger();
		auto mouseOver = hitBox.mouseOver() && enable;

		if (isTrigger)
		{
			BitFlag::UnPop(parameter->flag, gIS_ON);
		}

		// マウスが当たっていないか無効状態 == ボタンが操作されない -> 処理を省略
		if (not mouseOver || not enable)
		{
			return mouseOver;
		}

		Cursor::RequestStyle(CursorStyle::Hand);

		if (MouseL.down())
		{
			// フラグの立て折りを逆転
			BitFlag::PopOrUnPop(parameter->flag, gIS_ON, not BitFlag::CheckAnyPop(parameter->flag, gIS_ON));
		}

		return mouseOver;
	}

	void TextButtonGUI::Draw(const Vec2& parent) const noexcept
	{
		auto parameter = _pImpl.get();
		const RectF backBox{ parameter->position + parent, GetSize() };
		auto isOn = BitFlag::CheckAnyPop(parameter->flag, gIS_ON);
		ColorF drawColor = isOn ? gCLICKED_COLOR : CommonGUI::GetBackColor(GetEnable(), backBox.mouseOver());

		backBox.draw(drawColor);
		backBox.drawFrame(1, 0, Palette::Gray);

		const Font& font = CommonGUI::GetFont();
		const auto dtext = font(parameter->text);
		const double labelWidth = Math::Ceil(dtext.region().w);
		const Vec2 labelPos{ (backBox.x + (backBox.w - labelWidth) / 2.0), (backBox.center().y - font.height() / 2.0 + CommonGUI::GetFontOffsetY()) };
		dtext.draw(labelPos, CommonGUI::GetTextColor(BitFlag::CheckAnyPop(parameter->flag, gENABLE)));
	}

	TextButtonGUI& TextButtonGUI::SetPosition(const Vec2& position) noexcept
	{
		_pImpl->position = position;

		return *this;
	}

	Vec2 TextButtonGUI::GetPosition() const noexcept
	{
		return _pImpl->position;
	}

	TextButtonGUI& TextButtonGUI::SetSize(const double width) noexcept
	{
		const Font& font = CommonGUI::GetFont();
		_pImpl->width = Max(width, Math::Ceil(font(_pImpl->text).region().w + 20));

		return *this;
	}

	Vec2 TextButtonGUI::GetSize() const noexcept
	{
		return Vec2{ _pImpl->width, CommonGUI::GetCellSize() };
	}

	TextButtonGUI& TextButtonGUI::SetEnable(const bool enable) noexcept
	{
		BitFlag::PopOrUnPop(_pImpl->flag, gENABLE, enable);

		return *this;
	}

	bool TextButtonGUI::GetEnable() const noexcept
	{
		return BitFlag::CheckAnyPop(_pImpl->flag, gENABLE);
	}

	TextButtonGUI& TextButtonGUI::SetIsTrigger(const bool isTrigger) noexcept
	{
		BitFlag::PopOrUnPop(_pImpl->flag, gIS_TRIGGER, isTrigger);

		return *this;
	}

	bool TextButtonGUI::GetIsTrigger() const noexcept
	{
		return BitFlag::CheckAnyPop(_pImpl->flag, gIS_TRIGGER);
	}

	TextButtonGUI& TextButtonGUI::SetText(const String& text) noexcept
	{
		_pImpl->text = text;

		// 文字が変わるとサイズも変えないといけない恐れあり
		return SetSize(_pImpl->width);
	}

	String TextButtonGUI::GetText() const noexcept
	{
		return _pImpl->text;
	}

	class ImageButtonGUI::Impl
	{
	public:
		Vec2	position;	//!< 表示位置
		Vec2	size;		//!< 表示サイズ
		Flag	flag;		//!< 各種フラグをまとめたもの
		Texture	texture;	//!< 表示画像

	public:
		Impl(const Vec2& position, const Vec2& size, const bool enable, const bool isTrigger, const String& path) :
			position{ position },
			size{ size },
			flag{ static_cast<Flag>(0) },
			texture{ path }
		{
			BitFlag::PopOrUnPop(flag, gENABLE, enable);
			BitFlag::PopOrUnPop(flag, gIS_TRIGGER, isTrigger);
		}
	};

	ImageButtonGUI::ImageButtonGUI(const String& path, const Vec2& position, const Vec2 size) :
		ImageButtonGUI{ path, position, size, true, true }
	{

	}

	ImageButtonGUI::ImageButtonGUI(const String& path, const Vec2& position, const Vec2 size, const bool isTrigger) :
		ImageButtonGUI{ path, position, size, isTrigger, true }
	{

	}

	ImageButtonGUI::ImageButtonGUI(const String& path, const Vec2& position, const Vec2 size, const bool isTrigger, const bool enable) :
		_pImpl{ new Impl{ position, size, enable, isTrigger, path } }
	{

	}

	ImageButtonGUI::~ImageButtonGUI() = default;

	bool ImageButtonGUI::Update(const Vec2& parent) noexcept
	{
		auto parameter = _pImpl.get();
		const RectF hitBox{ parameter->position + parent, GetSize() };
		auto enable = GetEnable();
		auto isTrigger = GetIsTrigger();
		bool mouseOver;// = hitBox.mouseOver() && enable;

		if (not GetIsCircle())
		{
			mouseOver = RectF{ parameter->position + parent, GetSize() }.mouseOver() && enable;
		}
		else {
			auto radius = GetRadius();
			auto center = parameter->position + parent + Vec2{ radius, radius };

			mouseOver = Circle{ center, radius }.mouseOver() && enable;
		}

		if (isTrigger)
		{
			BitFlag::UnPop(parameter->flag, gIS_ON);
		}

		// マウスが当たっていないか無効状態 == ボタンが操作されない -> 処理を省略
		if (not mouseOver || not enable)
		{
			return mouseOver;
		}

		Cursor::RequestStyle(CursorStyle::Hand);

		if (MouseL.down())
		{
			// フラグの立て折りを逆転
			BitFlag::PopOrUnPop(parameter->flag, gIS_ON, not BitFlag::CheckAnyPop(parameter->flag, gIS_ON));
		}

		return mouseOver;
	}

	void ImageButtonGUI::Draw(const Vec2& parent) const noexcept
	{
		auto parameter = _pImpl.get();
		//const RectF backBox{ parameter->position + parent, GetSize() };
		auto isOn = BitFlag::CheckAnyPop(parameter->flag, gIS_ON);

		if (not GetIsCircle())
		{
			RectF shape{ parameter->position + parent, GetSize() };

			ColorF drawColor = isOn ? gCLICKED_COLOR : CommonGUI::GetBackColor(GetEnable(), shape.mouseOver());

			shape(parameter->texture)
				.draw(drawColor)
				.drawFrame(1, 0, Palette::Gray);
		}
		else {
			auto radius = GetRadius();
			auto center = parameter->position + parent + Vec2{ radius, radius };

			Circle shape{ center, radius };

			ColorF drawColor = isOn ? gCLICKED_COLOR : CommonGUI::GetBackColor(GetEnable(), shape.mouseOver());

			shape(parameter->texture)
				.draw(drawColor)
				.drawFrame(1, 0, Palette::Gray);
		}
	}

	ImageButtonGUI& ImageButtonGUI::SetPosition(const Vec2& position) noexcept
	{
		_pImpl->position = position;

		return *this;
	}

	Vec2 ImageButtonGUI::GetPosition() const noexcept
	{
		return _pImpl->position;
	}

	ImageButtonGUI& ImageButtonGUI::SetSize(const Vec2& size) noexcept
	{
		_pImpl->size = size;

		return *this;
	}

	Vec2 ImageButtonGUI::GetSize() const noexcept
	{
		return _pImpl->size;
	}

	double ImageButtonGUI::GetRadius() const noexcept
	{
		auto size = _pImpl->size;

		return Max(size.x, size.y);
	}

	ImageButtonGUI& ImageButtonGUI::SetEnable(const bool enable) noexcept
	{
		BitFlag::PopOrUnPop(_pImpl->flag, gENABLE, enable);

		return *this;
	}

	bool ImageButtonGUI::GetEnable() const noexcept
	{
		return BitFlag::CheckAnyPop(_pImpl->flag, gENABLE);
	}

	ImageButtonGUI& ImageButtonGUI::SetIsTrigger(const bool isTrigger) noexcept
	{
		BitFlag::PopOrUnPop(_pImpl->flag, gIS_TRIGGER, isTrigger);

		return *this;
	}

	bool ImageButtonGUI::GetIsTrigger() const noexcept
	{
		return BitFlag::CheckAnyPop(_pImpl->flag, gIS_TRIGGER);
	}

	ImageButtonGUI& ImageButtonGUI::SetIsCircle(const bool isCircle) noexcept
	{
		BitFlag::PopOrUnPop(_pImpl->flag, gIS_CIRCLE, isCircle);

		return *this;
	}

	bool ImageButtonGUI::GetIsCircle() const noexcept
	{
		return BitFlag::CheckAnyPop(_pImpl->flag, gIS_CIRCLE);
	}

	bool ImageButtonGUI::GetIsOn() const noexcept
	{
		return BitFlag::CheckAnyPop(_pImpl->flag, gIS_ON);
	}

	ImageButtonGUI& ImageButtonGUI::SetTexture(const String& path) noexcept
	{
		_pImpl->texture = Texture{ path };

		return *this;
	}

	Texture ImageButtonGUI::GetTexture() const noexcept
	{
		return _pImpl->texture;
	}
}


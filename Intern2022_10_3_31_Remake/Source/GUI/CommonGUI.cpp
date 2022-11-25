/** ****************************************************************************************************
 * @file
 * @brief GUI共通機能
 **************************************************************************************************** */
#include "CommonGUI.h"

namespace MySystem
{
	namespace
	{
		constexpr int32 gCELL_SIZE = 40;		//!< GUIのひとマス
		constexpr int32 gFONT_Y_OFFSET = -1;	//!< フォント表示位置のオフセット

		constexpr ColorF gACTIVE_TEXT_COLOR{ 0.11 };	//!< 有効状態の文字色
		constexpr ColorF gDISABLED_TEXT_COLOR{ 0.67 };	//!< 無効状態の文字色

		constexpr ColorF gMOUSE_OVER_COLOR{ 0.9, 0.95, 1.0 };	//!< マウス重複中の表示色
		constexpr ColorF gACTIVE_BACK_COLOR{ 1.0 };				//!< 有効状態の表示色
		constexpr ColorF gDISABLED_BACKGROUND_COLOR{ 0.92 };	//!< 無効状態の表示色

		std::unique_ptr<Font> gpDefaultFont;	//!< GUI用フォント
		Array<Font> gaIconFonts;				//!< フォールバックフォント
	}

	namespace CommonGUI
	{
		void Init()
		{
			gpDefaultFont = std::make_unique<Font>(FontMethod::MSDF, 20, Typeface::CJK_Regular_JP);

			if (Font::IsAvailable(Typeface::Icon_Awesome_Solid))
			{
				gaIconFonts.emplace_back(FontMethod::MSDF, 20, Typeface::Icon_Awesome_Solid);
			}

			if (Font::IsAvailable(Typeface::Icon_MaterialDesign))
			{
				gaIconFonts.emplace_back(FontMethod::MSDF, 20, Typeface::Icon_MaterialDesign);
			}

			for (const auto& iconFont : gaIconFonts)
			{
				gpDefaultFont->addFallback(iconFont);
			}
		}

		Font GetFont()
		{
			return *gpDefaultFont;
		}

		int32 GetCellSize()
		{
			return gCELL_SIZE;
		}

		int32 GetFontOffsetY()
		{
			return gFONT_Y_OFFSET;
		}

		ColorF GetActiveTextColor()
		{
			return gACTIVE_TEXT_COLOR;
		}

		ColorF GetDisAbleTextColor()
		{
			return gDISABLED_TEXT_COLOR;
		}

		ColorF GetMouseOverColor()
		{
			return gMOUSE_OVER_COLOR;
		}

		ColorF GetActiveBackColor()
		{
			return gACTIVE_BACK_COLOR;
		}

		ColorF GetDisAbleBackColor()
		{
			return gDISABLED_TEXT_COLOR;
		}

		ColorF GetTextColor(const bool enable)
		{
			return enable ? gACTIVE_TEXT_COLOR : gDISABLED_TEXT_COLOR;
		}

		ColorF GetBackColor(const bool enable)
		{
			return enable ? gACTIVE_BACK_COLOR : gDISABLED_BACKGROUND_COLOR;
		}

		ColorF GetBackColor(const bool enable, const bool mouseOver)
		{
			return enable ? (mouseOver ? gMOUSE_OVER_COLOR : gACTIVE_BACK_COLOR) : gDISABLED_BACKGROUND_COLOR;
		}
	}
}

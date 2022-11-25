﻿/** ****************************************************************************************************
 * @file
 * @brief GUI共通機能
 **************************************************************************************************** */
#pragma once

#include <Siv3D.hpp>

/** ****************************************************************************************************
 * @brief GUI共通機能
 **************************************************************************************************** */
namespace MySystem::CommonGUI
{
	/** ****************************************************************************************************
	 * @brief 初期化
	 **************************************************************************************************** */
	void Init();

	/** ****************************************************************************************************
	* @brief	GUI用フォントの取得
	* @return	フォント
	**************************************************************************************************** */
	[[nodiscard]]
	Font GetFont();

	/** ****************************************************************************************************
	 * @brief	GUIの1マスの大きさ取得
	 * @return	大きさ
	 **************************************************************************************************** */
	[[nodiscard]]
	int32 GetCellSize();

	/** ****************************************************************************************************
	 * @brief	フォント表示位置のY軸方向オフセット値取得
	 * @return	オフセット値
	 **************************************************************************************************** */
	[[nodiscard]]
	int32 GetFontOffsetY();

	/** ****************************************************************************************************
	 * @brief	有効時の文字色取得
	 * @return	色
	 **************************************************************************************************** */
	[[nodiscard]]
	ColorF GetActiveTextColor();

	/** ****************************************************************************************************
	 * @brief	無効時の文字色取得
	 * @return	色
	 **************************************************************************************************** */
	[[nodiscard]]
	ColorF GetDisAbleTextColor();

	/** ****************************************************************************************************
	 * @brief	マウス重複中の表示色取得
	 * @return	色
	 **************************************************************************************************** */
	[[nodiscard]]
	ColorF GetMouseOverColor();

	/** ****************************************************************************************************
	 * @brief	有効時の表示色取得
	 * @return	色
	 **************************************************************************************************** */
	[[nodiscard]]
	ColorF GetActiveBackColor();

	/** ****************************************************************************************************
	 * @brief	無効時の表示色取得
	 * @return	色
	 **************************************************************************************************** */
	[[nodiscard]]
	ColorF GetDisAbleBackColor();

	/** ****************************************************************************************************
	 * @brief					文字の表示色取得
	 * @param[in] enable		有効か
	 * @retval enable == true	有効色
	 * @retval enable == false	無効色
	 **************************************************************************************************** */
	[[nodiscard]]
	ColorF GetTextColor(const bool enable = true);

	/** ****************************************************************************************************
	 * @brief					表示色取得
	 * @param[in] enable		有効か
	 * @retval enable == true	有効色
	 * @retval enable == false	無効色
	 **************************************************************************************************** */
	[[nodiscard]]
	ColorF GetBackColor(const bool enable = true);

	/** ****************************************************************************************************
	 * @brief					表示色取得
	 * @param[in] enable		有効か
	 * @param[in] mouseOver		マウスが重複しているか
	 *
	 * @retval enable == true && mouseOver == true	マウス重複色
	 * @retval enable == true && mouseOver == false	有効色
	 * @retval enable == false						無効色
	 **************************************************************************************************** */
	[[nodiscard]]
	ColorF GetBackColor(const bool enable = true, const bool mouseOver = false);
}

﻿/** ****************************************************************************************************
 * @file
 * @brief GUIラジオボタン機能
 **************************************************************************************************** */
#pragma once

#include <Siv3D.hpp>

namespace MySystem
{
	/** ****************************************************************************************************
	 * @brief GUIラジオボタンクラス
	 **************************************************************************************************** */
	class RadioButtonGUI
	{
	private:
		class Impl;
		std::unique_ptr<Impl> _pImpl;

	public:
		RadioButtonGUI() = delete;
		RadioButtonGUI(const RadioButtonGUI&) = delete;
		RadioButtonGUI& operator=(const RadioButtonGUI&) = delete;

		/** ****************************************************************************************************
		 * @brief				コンストラクタ
		 * @param[in] position	表示位置
		 * @param[in] buttonNum	横の数
		 **************************************************************************************************** */
		RadioButtonGUI(const Vec2& position, const size_t buttonNum);

		/** ****************************************************************************************************
		 * @brief				コンストラクタ
		 * @param[in] position	表示位置
		 * @param[in] buttonNum	横の数
		 * @param[in] enable	有効化するか
		 **************************************************************************************************** */
		RadioButtonGUI(const Vec2& position, const size_t buttonNum, const bool enable);

		/** ****************************************************************************************************
		 * @brief デストラクタ
		 **************************************************************************************************** */
		~RadioButtonGUI();

		/** ****************************************************************************************************
		 * @brief				更新処理
		 * @param[in] parent	親座標
		 * @retval true			マウスが乗っている
		 * @retval false		マウスが乗っていない
		 **************************************************************************************************** */
		bool Update(const Vec2& parent = Vec2{ 0.0, 0.0 }) noexcept;

		/** ****************************************************************************************************
		 * @brief				描画処理
		 * @param[in] parent	親座標
		 **************************************************************************************************** */
		void Draw(const Vec2& parent = Vec2{ 0.0, 0.0 }) const noexcept;

		/** ****************************************************************************************************
		 * @brief				表示位置設定
		 * @param[in] position	表示位置
		 * @return				自身の参照
		 **************************************************************************************************** */
		RadioButtonGUI& SetPosition(const Vec2& position) noexcept;

		/** ****************************************************************************************************
		 * @brief	表示位置取得
		 * @return	表示位置
		 **************************************************************************************************** */
		[[nodiscard]]
		Vec2 GetPosition() const noexcept;

		/** ****************************************************************************************************
		 * @brief				有効化するか設定
		 * @param[in] enable	有効化するか
		 * @return				自身の参照
		 **************************************************************************************************** */
		RadioButtonGUI& SetEnable(const bool enable) noexcept;

		/** ****************************************************************************************************
		 * @brief			有効かどうか取得
		 * @retval true		有効
		 * @retval false	無効
		 **************************************************************************************************** */
		[[nodiscard]]
		bool GetEnable() const noexcept;

		/** ****************************************************************************************************
		 * @brief			現在選択中のボタン位置設定
		 * @param[in] index	現在選択中のボタン位置
		 * @return			自身の参照
		 **************************************************************************************************** */
		RadioButtonGUI& SetCurrentIndex(const size_t& index) noexcept;

		/** ****************************************************************************************************
		 * @brief	現在選択中のボタン位置取得
		 * @return	現在選択中のボタン位置
		 **************************************************************************************************** */
		[[nodiscard]]
		size_t GetCurrentIndex() const noexcept;
	};

	/** ****************************************************************************************************
	 * @brief 文字によるGUIラジオボタンクラス
	 **************************************************************************************************** */
	class TextRadioButtonGUI
	{
	private:
		class Impl;
		std::unique_ptr<Impl> _pImpl;

	public:
		TextRadioButtonGUI() = delete;
		TextRadioButtonGUI(const TextRadioButtonGUI&) = delete;
		TextRadioButtonGUI& operator=(const TextRadioButtonGUI&) = delete;

		/** ****************************************************************************************************
		 * @brief				コンストラクタ
		 * @param[in] position	表示位置
		 * @param[in] width		横幅
		 * @param[in] aLabels	表示文字群
		 **************************************************************************************************** */
		TextRadioButtonGUI(const Vec2& position, const double width, const Array<String>& aLabels);

		/** ****************************************************************************************************
		 * @brief				コンストラクタ
		 * @param[in] position	表示位置
		 * @param[in] width		横幅
		 * @param[in] aLabels	表示文字群
		 * @param[in] enable	有効化するか
		 **************************************************************************************************** */
		TextRadioButtonGUI(const Vec2& position, const double width, const Array<String>& aLabels, const bool enable);

		/** ****************************************************************************************************
		 * @brief デストラクタ
		 **************************************************************************************************** */
		~TextRadioButtonGUI();

		/** ****************************************************************************************************
		 * @brief				更新処理
		 * @param[in] parent	親座標
		 * @retval true			マウスが乗っている
		 * @retval false		マウスが乗っていない
		 **************************************************************************************************** */
		bool Update(const Vec2& parent = Vec2{ 0.0, 0.0 }) noexcept;

		/** ****************************************************************************************************
		 * @brief				描画処理
		 * @param[in] parent	親座標
		 **************************************************************************************************** */
		void Draw(const Vec2& parent = Vec2{ 0.0, 0.0 }) const noexcept;

		/** ****************************************************************************************************
		 * @brief				表示位置設定
		 * @param[in] position	表示位置
		 * @return				自身の参照
		 **************************************************************************************************** */
		TextRadioButtonGUI& SetPosition(const Vec2& position) noexcept;

		/** ****************************************************************************************************
		 * @brief	表示位置取得
		 * @return	表示位置
		 **************************************************************************************************** */
		[[nodiscard]]
		Vec2 GetPosition() const noexcept;

		/** ****************************************************************************************************
		 * @brief			表示サイズ設定
		 * @param[in] width	表示サイズ
		 * @return			自身の参照
		 **************************************************************************************************** */
		TextRadioButtonGUI& SetSize(const double width) noexcept;

		/** ****************************************************************************************************
		 * @brief	表示サイズ取得
		 * @return	表示サイズ
		 **************************************************************************************************** */
		[[nodiscard]]
		Vec2 GetSize() const noexcept;

		/** ****************************************************************************************************
		 * @brief				有効化するか設定
		 * @param[in] enable	有効化するか
		 * @return				自身の参照
		 **************************************************************************************************** */
		TextRadioButtonGUI& SetEnable(const bool enable) noexcept;

		/** ****************************************************************************************************
		 * @brief			有効かどうか取得
		 * @retval true		有効
		 * @retval false	無効
		 **************************************************************************************************** */
		[[nodiscard]]
		bool GetEnable() const noexcept;

		/** ****************************************************************************************************
		 * @brief			現在選択中のボタン位置設定
		 * @param[in] index	現在選択中のボタン位置
		 * @return			自身の参照
		 **************************************************************************************************** */
		TextRadioButtonGUI& SetCurrentIndex(const size_t& index) noexcept;

		/** ****************************************************************************************************
		 * @brief	現在選択中のボタン位置取得
		 * @return	現在選択中のボタン位置
		 **************************************************************************************************** */
		[[nodiscard]]
		size_t GetCurrentIndex() const noexcept;
	};

	/** ****************************************************************************************************
	 * @brief 画像によるGUIラジオボタンクラス
	 **************************************************************************************************** */
	class ImageRadioButtonGUI
	{
	private:
		class Impl;
		std::unique_ptr<Impl> _pImpl;

	public:
		ImageRadioButtonGUI() = delete;
		ImageRadioButtonGUI(const ImageRadioButtonGUI&) = delete;
		ImageRadioButtonGUI& operator=(const ImageRadioButtonGUI&) = delete;

		/** ****************************************************************************************************
		 * @brief				コンストラクタ
		 * @param[in] position	表示位置
		 * @param[in] width		表示幅
		 * @param[in] cellNum	分割数
		 * @param[in] texture	テクスチャ
		 **************************************************************************************************** */
		ImageRadioButtonGUI(const Vec2& position, const double width, const Point& cellNum, const Texture texture);

		/** ****************************************************************************************************
		 * @brief				コンストラクタ
		 * @param[in] position	表示位置
		 * @param[in] width		表示幅
		 * @param[in] cellNum	分割数
		 * @param[in] texture	テクスチャ
		 * @param[in] enable	有効化するか
		 **************************************************************************************************** */
		ImageRadioButtonGUI(const Vec2& position, const double width, const Point& cellNum, const Texture texture, const bool enable);

		/** ****************************************************************************************************
		 * @brief デストラクタ
		 **************************************************************************************************** */
		~ImageRadioButtonGUI();

		/** ****************************************************************************************************
		 * @brief				更新処理
		 * @param[in] parent	親座標
		 * @retval true			マウスが乗っている
		 * @retval false		マウスが乗っていない
		 **************************************************************************************************** */
		bool Update(const Vec2& parent = Vec2{ 0.0, 0.0 }) noexcept;

		/** ****************************************************************************************************
		 * @brief				描画処理
		 * @param[in] parent	親座標
		 **************************************************************************************************** */
		void Draw(const Vec2& parent = Vec2{ 0.0, 0.0 }) const noexcept;

		/** ****************************************************************************************************
		 * @brief				表示位置設定
		 * @param[in] position	表示位置
		 * @return				自身の参照
		 **************************************************************************************************** */
		ImageRadioButtonGUI& SetPosition(const Vec2& position) noexcept;

		/** ****************************************************************************************************
		 * @brief	表示位置取得
		 * @return	表示位置
		 **************************************************************************************************** */
		[[nodiscard]]
		Vec2 GetPosition() const noexcept;

		/** ****************************************************************************************************
		 * @brief			表示サイズ設定
		 * @param[in] width	表示サイズ
		 * @return			自身の参照
		 **************************************************************************************************** */
		ImageRadioButtonGUI& SetSize(const double width) noexcept;

		/** ****************************************************************************************************
		 * @brief	表示サイズ取得
		 * @return	表示サイズ
		 **************************************************************************************************** */
		[[nodiscard]]
		Vec2 GetSize() const noexcept;

		/** ****************************************************************************************************
		 * @brief				有効化するか設定
		 * @param[in] enable	有効化するか
		 * @return				自身の参照
		 **************************************************************************************************** */
		ImageRadioButtonGUI& SetEnable(const bool enable) noexcept;

		/** ****************************************************************************************************
		 * @brief			有効かどうか取得
		 * @retval true		有効
		 * @retval false	無効
		 **************************************************************************************************** */
		[[nodiscard]]
		bool GetEnable() const noexcept;

		/** ****************************************************************************************************
		 * @brief			現在選択中のボタン位置設定
		 * @param[in] index	現在選択中のボタン位置
		 * @return			自身の参照
		 **************************************************************************************************** */
		ImageRadioButtonGUI& SetCurrentIndex(const Point& index) noexcept;

		/** ****************************************************************************************************
		 * @brief	現在選択中のボタン位置取得
		 * @return	現在選択中のボタン位置
		 **************************************************************************************************** */
		[[nodiscard]]
		Point GetCurrentIndex() const noexcept;

		/** ****************************************************************************************************
		 * @brief				分割数設定
		 * @param[in] cellNum	分割数
		 * @return				自身の参照
		 **************************************************************************************************** */
		ImageRadioButtonGUI& SetCellNum(const Point& cellNum) noexcept;

		/** ****************************************************************************************************
		 * @brief	分割数取得
		 * @return	分割数
		 **************************************************************************************************** */
		[[nodiscard]]
		Point GetCellNum() const noexcept;
	};
}

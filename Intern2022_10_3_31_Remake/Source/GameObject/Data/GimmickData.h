/** ****************************************************************************************************
 * @file
 * @brief ギミックのデータ操作関連
 **************************************************************************************************** */
#pragma once

#include <Siv3D.hpp>
#include "Transform.h"

/** ****************************************************************************************************
 * @brief ギミックのデータ操作関連
 **************************************************************************************************** */
namespace MySystem::GimmickData
{
	using TypeIndex = uint8;	//!< タイルの種類用
	using Flag = uint8;

	static constexpr Flag gENABLE = static_cast<Flag>(1 << 0);	//!< 存在しているか
	static constexpr Flag gACTIVE = static_cast<Flag>(1 << 1);	//!< 動けるかどうか

	/** ****************************************************************************************************
	 * @brief ギミックの種類
	 **************************************************************************************************** */
	enum class Type : TypeIndex
	{
		Player,	//!< 矩形タイル
		Goal,	//!< 三角形タイル
		Max
	};

	//!< 各ギミックアイコン世運夫絵文字
	static const std::array<Emoji, static_cast<TypeIndex>(Type::Max) + 1> gEMOJI_SIMPLE_TEXTURE = {
		U"🦕"_emoji,
		U"🚩"_emoji,
		U"✖"_emoji
	};

	//!< 各ギミックの画像
	static const std::array<String, static_cast<TypeIndex>(Type::Max) + 1> gLABEL_TEXTURE = {
		U"PlayerTexture",
		U"GoalTexture",
		U"NullTexture"
	};

	/** ****************************************************************************************************
	 * @brief ギミックのデータ格納用
	 **************************************************************************************************** */
	struct Desc
	{
		Vec2 position;	//!< 表示座標
		double angle;	//!< 表示角度

		TypeIndex gimmickType;	//!< ギミックの種類

		/** ****************************************************************************************************
		 * @brief コンストラクタ
		 **************************************************************************************************** */
		inline Desc() noexcept :
			position{ Vec2{ 0.0, 0.0 } },
			angle{ 0.0 },
			gimmickType{ static_cast<TypeIndex>(0) }
		{

		}
	};

	class Gimmick
	{
	public:
		/** ****************************************************************************************************
		 * @brief コンストラクタ
		 **************************************************************************************************** */
		Gimmick();

		/** ****************************************************************************************************
		 * @brief デストラクタ
		 **************************************************************************************************** */
		virtual ~Gimmick();

		/** ****************************************************************************************************
		 * @brief 更新
		 **************************************************************************************************** */
		virtual void Update();

		/** ****************************************************************************************************
		 * @brief 描画
		 **************************************************************************************************** */
		virtual void Draw() const;

		/** ****************************************************************************************************
		 * @brief	当たり判定の取得
		 * @return	円形の当たり判定
		 **************************************************************************************************** */
		virtual Circle GetCollision() const = 0;

		/** ****************************************************************************************************
		 * @brief				有効化するかどうか設定
		 * @param[in] isActive	有効化するかどうか
		 **************************************************************************************************** */
		virtual void SetIsActive(const bool isActive) = 0;

		/** ****************************************************************************************************
		 * @brief			有効化しているかどうか取得
		 * @retval true		有効
		 * @retval false	無効
		 **************************************************************************************************** */
		virtual bool GetIsActive() = 0;

		/** ****************************************************************************************************
		 * @brief				親設定
		 * @param[in] pParent	親
		 **************************************************************************************************** */
		virtual void SetParent(Transform* pParent) = 0;

		/** ****************************************************************************************************
		 * @brief	ギミックの種類を取得
		 * @return	ギミックの種類
		 **************************************************************************************************** */
		virtual Type GetType() = 0;

		/** ****************************************************************************************************
		 * @brief 二度と有効化出来ないようにする
		 **************************************************************************************************** */
		virtual void SetDestroy() = 0;

		/** ****************************************************************************************************
		 * @brief			存在しているかどうか取得
		 * @retval true		存在している
		 * @retval false	存在していない
		 **************************************************************************************************** */
		virtual bool GetEnable() = 0;
	};
}

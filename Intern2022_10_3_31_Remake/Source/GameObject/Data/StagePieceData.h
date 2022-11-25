/** ****************************************************************************************************
 * @file
 * @brief ステージのカケラのデータ操作関連
 **************************************************************************************************** */
#pragma once

#include "TileData.h"
#include "GimmickData.h"

/** ****************************************************************************************************
 * @brief ステージのカケラのデータ操作関連
 **************************************************************************************************** */
namespace MySystem::StagePieceData
{
	using TypeIndex = uint8;	//!< 背景の種類用

	/** ****************************************************************************************************
	 * @brief 背景の種類
	 **************************************************************************************************** */
	enum class Type : TypeIndex
	{
		Normal,		//!< 通常
		Void,		//!< 半透明
		Gravity,	//!< 重力付加
		Max
	};

	static constexpr uint8	gMAX_PIECE_NUM = 10;	//!< ステージのカケラの最大数
	static constexpr uint16	gMAX_WIDTH = 40;		//!< タイルが横に並ぶ最大数
	static constexpr uint16	gMAX_HEIGHT = 24;		//!< タイルが横に並ぶ最大数

	struct Desc
	{
		Vec2	position;	//!< 座標
		double	scale;		//!< 大きさ
		double	angle;		//!< 角度
		Vec2	backScale;	//!< 背景の大きさ

		bool canMoving;		//!< 移動できるか
		bool canScaling;	//!< 拡大縮小できるか
		bool canRolling;	//!< 回転できるか

		TypeIndex backType;	//!< 背景の種類

		Array<TileData::Desc>		aTileDescs;		//!< タイルのデータ群
		Array<GimmickData::Desc>	aGimmiclDesc;	//!< ギミックのデータ群

		/** ****************************************************************************************************
		 * @brief コンストラクタ
		 **************************************************************************************************** */
		inline Desc() noexcept :
			position{ 0.0, 0.0 },
			scale{ 1.0 },
			angle{ 0.0 },
			backScale{ 0.0, 0.0 },
			canMoving{ false },
			canScaling{ false },
			canRolling{ false },
			backType{ static_cast<TypeIndex>(0) }
		{

		}
	};
};


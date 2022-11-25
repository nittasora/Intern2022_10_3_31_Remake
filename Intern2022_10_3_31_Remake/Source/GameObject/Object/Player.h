/** ****************************************************************************************************
 * @file
 * @brief プレイヤー操作関連
 **************************************************************************************************** */
#pragma once

#include <Siv3D.hpp>
#include "../Data/GimmickData.h"

namespace MySystem
{
	/** ****************************************************************************************************
	 * @brief プレイヤー操作関連
	 **************************************************************************************************** */
	class Player : public GimmickData::Gimmick
	{
	private:
		class Impl;
		std::unique_ptr<Impl> _pImpl;

	public:
		Player() = delete;
		Player(const Player&) = delete;
		Player& operator=(const Player&) = delete;

		/** ****************************************************************************************************
		 * @brief				コンストラクタ
		 * @oaram[in] data		生成用データ
		 * @param[in] pParent	親
		 **************************************************************************************************** */
		Player(const GimmickData::Desc& data, class Transform* pParent);

		/** ****************************************************************************************************
		 * @brief デストラクタ
		 **************************************************************************************************** */
		~Player();

		void Update() override;

		void Draw() const override;

		Circle GetCollision() const override;

		void SetIsActive(const bool isActive) override;

		bool GetIsActive() override;

		void SetParent(Transform* pParent) override;

		GimmickData::Type GetType() override;

		void SetDestroy() override;

		bool GetEnable() override;

		/** ****************************************************************************************************
		 * @brief 移動方向の更新
		 **************************************************************************************************** */
		void UpdateDirection();

		/** ****************************************************************************************************
		 * @brief			移動方向の更新
		 * @oaram[in] angle	角度
		 **************************************************************************************************** */
		void UpdateDirection(const double angle);

		/** ****************************************************************************************************
		 * @brief					当たり判定
		 * @oaram[in] aGroundLines	地面の線分
		 * @param[in] apGimmicks	ギミック群
		 **************************************************************************************************** */
		void HitCheck(const Array<LineString>& aGroundLines, const Array<std::weak_ptr<GimmickData::Gimmick>>& apGimmicks);
	};
}


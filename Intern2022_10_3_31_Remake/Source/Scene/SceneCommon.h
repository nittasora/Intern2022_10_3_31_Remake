#pragma once

#include <Siv3D.hpp>
#include "../GameObject/Data/StageData.h"

namespace MySystem
{
	/** ****************************************************************************************************
	 * @brief シーン間共通データ
	 **************************************************************************************************** */
	struct CommonSceneData
	{
		static constexpr StageData::WorldIndex gMAX_WORLD_NUM = 1;	//!< ワールドの最大数
		static constexpr StageData::StageIndex gMAX_STAGE_NUM = 5;	//!< ステージの最大数

		StageData::WorldIndex worldIndex;	//!< 現在のワールド
		StageData::StageIndex stageIndex;	//!< 現在のステージ

		/** ****************************************************************************************************
		 * @brief コンストラクタ
		 **************************************************************************************************** */
		inline CommonSceneData() :
			worldIndex{ static_cast<StageData::WorldIndex>(0) },
			stageIndex{ static_cast<StageData::StageIndex>(0) }
		{

		}
	};

	using App = SceneManager<String, CommonSceneData>;	//!< シーンマネージャー
}

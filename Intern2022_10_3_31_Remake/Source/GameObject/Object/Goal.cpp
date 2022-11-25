#include "Goal.h"
#include "../Data/Transform.h"
#include "../Data/TileData.h"
#include "../../Utility/BitFlag.h"

namespace MySystem
{
	/** ****************************************************************************************************
	 * @brief Goalメンバ
	 **************************************************************************************************** */
	class Goal::Impl
	{
	public:
		Transform transform;			//!< トランスフォーム情報
		GimmickData::TypeIndex	type;	//!< ギミックの種類
		GimmickData::Flag		flag;	//!< フラグ

		/** ****************************************************************************************************
		 * @brief				コンストラクタ
		 * @oaram[in] data		生成用データ
		 * @param[in] pParent	親
		 **************************************************************************************************** */
		Impl(const GimmickData::Desc& data, Transform* pParent) :
			transform{ data.position, Vec2{ 1.0, 1.0 }, data.angle, pParent },
			type{ data.gimmickType },
			flag{ BitFlag::GetMask(GimmickData::gENABLE, GimmickData::gACTIVE) }
		{
			auto bodyScale = Vec2{ TileData::gTILE_SIZE, TileData::gTILE_SIZE };
			bodyScale /= 2.0;

			transform.SetLocalPosition(transform.GetLocalPosition() + bodyScale);
		}

		/** ****************************************************************************************************
		 * @brief 描画
		 **************************************************************************************************** */
		void Draw() const
		{
			if (BitFlag::CheckAllUnPop(flag, GimmickData::gENABLE))
			{
				return;
			}

			GetBody()(TextureAsset(GimmickData::gLABEL_TEXTURE[type])).draw();
		}

		/** ****************************************************************************************************
		 * @brief	本体サイズの取得
		 * @return	本体サイズ
		 **************************************************************************************************** */
		Transform::Scale GetBodyScale() const
		{
			return transform.GetWorldScale() * TileData::gTILE_SIZE;
		}

		/** ****************************************************************************************************
		 * @brief	本体の取得
		 * @return	本体
		 **************************************************************************************************** */
		Quad GetBody() const
		{
			auto position = transform.GetWorldPosition();
			auto scale = GetBodyScale();
			auto angle = transform.GetWorldAngle();

			return
				RectF{ Arg::center = position, scale }.
				rotatedAt(position, angle);
		}

		/** ****************************************************************************************************
		 * @brief	円形当たり判定の取得
		 * @return	円形当たり判定
		 **************************************************************************************************** */
		Circle GetCollision() const
		{
			auto scale = GetBodyScale() / 2.0;
			return Circle{ GetBody().asPolygon().centroid(), Max(scale.x, scale.y) };
		}
	};

	Goal::Goal(const GimmickData::Desc& data, Transform* pParent) :
		Gimmick{},
		_pImpl{ new Impl{ data, pParent } }
	{

	}

	Goal::~Goal()
	{

	}

	void Goal::Update()
	{

	}

	void Goal::Draw() const
	{
		_pImpl->Draw();
	}

	Circle Goal::GetCollision() const
	{
		return _pImpl->GetCollision();
	}

	void Goal::SetIsActive(const bool isActive)
	{
		BitFlag::PopOrUnPop(_pImpl->flag, GimmickData::gACTIVE, isActive);
	}

	bool Goal::GetIsActive()
	{
		return BitFlag::CheckAnyPop(_pImpl->flag, GimmickData::gACTIVE);
	}

	void Goal::SetParent(Transform* pParent)
	{
		_pImpl->transform.SetParent(pParent);
	}

	GimmickData::Type Goal::GetType()
	{
		return static_cast<GimmickData::Type>(_pImpl->type);
	}

	void Goal::SetDestroy()
	{
		BitFlag::UnPop(_pImpl->flag, GimmickData::gENABLE);
	}

	bool Goal::GetEnable()
	{
		return BitFlag::CheckAnyPop(_pImpl->flag, GimmickData::gENABLE);
	}
}

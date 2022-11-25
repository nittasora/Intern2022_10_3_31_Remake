#include "Player.h"
#include "../Data/Transform.h"
#include "../Data/TileData.h"
#include "../../Utility/BitFlag.h"
#include "../../Utility/CommonKey.h"
#include "../../Utility/MyMath.h"

namespace MySystem
{
	/** ****************************************************************************************************
	 * @brief Playerメンバ
	 **************************************************************************************************** */
	class Player::Impl
	{
	private:
		static constexpr Point _OFFSET_UV_LEFT_TOP{ 4, 8 };		//!< UVのオフセット値
		static constexpr Point _SIZE_UV_WIDTH_HEIGHT{ 13, 19 };	//!< 1コマのサイズ
		static constexpr Point _UV_CELL_NUM{ 6, 4 };			//!< コマの数

		static constexpr GimmickData::Flag _IS_GROUND = BitFlag::Get<GimmickData::Flag>(2);	//!< 着地フラグ

		static constexpr double _MAX_SPEED = 3.0;					//!< 横方向最大速度 
		static constexpr double _INCREMENT_SPEED = _MAX_SPEED;		//!< 横方向速度増加量
		static constexpr double _DECREMENT_SPEED = _MAX_SPEED * 2;	//!< 横方向速度減少量
		static constexpr double _GRAVITY = 9.8;						//!< 重力
		static constexpr double _JUMP_POWER = -3.0;					//!< ジャンプ時の速度

		//! 表示サイズ
		static constexpr Vec2 _BODEY_SIZE{ TileData::gTILE_SIZE, TileData::gTILE_SIZE * (static_cast<double>(_SIZE_UV_WIDTH_HEIGHT.y) / static_cast<double>(_SIZE_UV_WIDTH_HEIGHT.x)) };
		static constexpr double _CIRCLE_RADIUS = Max(_BODEY_SIZE.x, _BODEY_SIZE.y);	//!< 当たり判定のサイズ

	public:
		Transform transform;			//!< トランスフォーム情報
		GimmickData::TypeIndex	type;	//!< ギミックの種類
		GimmickData::Flag		flag;	//!< フラグ

		Vec2	down;	//!< 下向きベクトル
		Vec2	right;	//!< 右向きベクトル
		Vec2	speed;	//!< 速度(横と縦)

		int	animationFrame;	//!< アニメーションのフレーム

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

			bodyScale.y += bodyScale.y - _BODEY_SIZE.y / 2.0;

			transform.SetLocalPosition(transform.GetLocalPosition() + bodyScale);
		}

		/** ****************************************************************************************************
		 * @brief					当たり判定
		 * @oaram[in] aGroundLines	地面の線分
		 * @param[in] apGimmicks	ギミック群
		 **************************************************************************************************** */
		void HitCheck(const Array<LineString>& aGroundLines, const Array<std::weak_ptr<GimmickData::Gimmick>>& apGimmicks)
		{
			if (speed == Vec2::Zero())
			{
				BitFlag::Pop(flag, _IS_GROUND);
				return;
			}

			bool isReverse = false;				// 前回鋭角によって反転したか
			Line* pPrevHitLine = nullptr;		// 前回の線分に比べて反転したかどうか
			Vec2 prevNormal;					// 前回の線分の法線(角度計算用)
			auto collision = GetCollision();	// 自分の当たり判定

			// 現在のスピード
			auto currentSpeed = speed.y * down + speed.x * right;

			BitFlag::UnPop(flag, _IS_GROUND);

			for (auto i : step(10))
			{
				auto speedRight = Vec2{ -currentSpeed.y, currentSpeed.x };	// スピードの右向きベクトル
				double maxDistance = currentSpeed.length();					// 移動できる距離
				Vec2 HitPosition;	// 当たった場所
				Vec2 hitNormal;		// 当たったモノの法線
				Vec2 nextSpeed = currentSpeed;		// 当たり判定後のスピード
				bool hitFlag = false;				// あたったかどうか
				Line* pCurrentHitLine = nullptr;	// 当たった線分

				for (auto lineString : aGroundLines)
				{
					for (size_t j = 0, max = lineString.num_lines(CloseRing::Yes); j < max; ++j)
					{
						auto line = lineString.line(j, CloseRing::Yes);	// 対象の線分

						//法線取得
						auto normal = lineString.normalAtLine(j, CloseRing::Yes);

						// 移動先にないなら無視する
						if (speedRight.cross(normal) < 0.0)
						{
							continue;
						}

						// 線分に寄せたスピードベクトル
						auto speedVec = Line{ (-normal * collision.r) + collision.center, (-normal * collision.r) + collision.center + currentSpeed };

						// 当たってないなら無視する
						if (not HitCheck(line.stretched(collision.r, collision.r), speedVec, HitPosition, maxDistance))
						{
							continue;
						}

						pCurrentHitLine	= &line;
						nextSpeed		= HitPosition;
						hitNormal		= normal;
						hitFlag			= true;
					}
				}

				for (auto pGimmick : apGimmicks)
				{
					auto pRawGimmick = pGimmick.lock();

					// 当たってないなら無視する
					if (not HitCheck(collision, pRawGimmick.get(), currentSpeed, maxDistance))
					{
						continue;
					}

					// タグごとに判定(今回は種類が少ないので直書き)
					switch (pRawGimmick->GetType())
					{
					case GimmickData::Type::Goal:
						pRawGimmick->SetDestroy();
						break;
					}
				}

				// 場所の更新
				transform.SetWorldPosition(transform.GetWorldPosition() + nextSpeed);

				// 何も当たっていないならこれ以上判定しない
				if (not hitFlag)
				{
					break;
				}

				currentSpeed = MyMath::CalcuWallScratchVector(currentSpeed - nextSpeed, -hitNormal);

				{
					// 地面の角度
					auto theta = acos(down.dot(hitNormal));

					if (theta > 140.0_deg)		// 地面判定
					{
						BitFlag::Pop(flag, _IS_GROUND);
						speed.y = 0.0;
					}
					else if (theta > 70.0_deg)	// 壁判定
					{
						speed.x = 0.0;
					}
					else						// 天井判定
					{
						speed.y = 0.0;
					}
				}

				// スピードが0なら移動しなくてよい
				if (currentSpeed == Vec2::Zero())
				{
					break;
				}

				if (pCurrentHitLine)
				{
					if (pPrevHitLine)
					{
						// 前回の線分との角度を取る
						auto theta = acos(prevNormal.dot(hitNormal));

						// 鋭角の場合移動ベクトルを反転
						if (theta > 90.0_deg)
						{
							// ただし連続で反転できないようにする
							if (!isReverse)
							{
								isReverse = true;
								currentSpeed *= -1.0;
							}
						}
						else
						{
							isReverse = false;
						}
					}

					pPrevHitLine	= pCurrentHitLine;
					prevNormal		= hitNormal;
				}
			}
		}

		/** ****************************************************************************************************
		 * @brief					ギミックとの当たり判定
		 * @oaram[in] collision		自分の当たり判定
		 * @param[in] pGimmick		対象ギミック
		 * @param[in] currentSpeed	現在のスピード
		 * @param[out] maxDistance	現在の最大移動距離
		 * @retval true				当たった
		 * @retval false			当たっていない
		 **************************************************************************************************** */
		bool HitCheck(const Circle& collision, GimmickData::Gimmick* pGimmick, const Vec2& currentSpeed, double& maxDistance)
		{
			auto gimmickCollision = pGimmick->GetCollision();	// ギミックの当たり判定
			Vec2 neighborPoint;	// 最近点
			double distance;	// 最短距離

			MyMath::CalcuDistancePointToSegment(collision.center, collision.center + currentSpeed, gimmickCollision.center, distance, neighborPoint);

			if (distance < collision.r + gimmickCollision.r)
			{
				auto nextDistance = (neighborPoint - collision.center).length();

				if (nextDistance <= maxDistance)
				{
					return true;
				}
			}

			return false;
		}

		/** ****************************************************************************************************
		 * @brief					地面との当たり判定
		 * @oaram[in] groundLine	地面の線分
		 * @param[in] speedVec		自信のスピードベクトル
		 * @param[out] hitPosition	当たった場所
		 * @param[out] maxDistance	現在の最大移動距離
		 * @retval true				当たった
		 * @retval false			当たっていない
		 **************************************************************************************************** */
		bool HitCheck(const Line& groundLine, const Line& speedVec, Vec2& hitPosition, double& maxDistance)
		{
			double hitTime;	// 当たった割合
			Vec2 point;		// 当たった位置

			if (MyMath::HitCheckSegmentToSegment(speedVec.begin, speedVec.end, groundLine.begin, groundLine.end, hitTime, point))
			{
				// 当たった位置までの距離
				float nextDistance = (point - speedVec.begin).length();

				// 現在より遠い位置なら無視する
				if (nextDistance >= maxDistance)
					return false;

				hitPosition = point - speedVec.begin;
				maxDistance = nextDistance;

				return true;
			}

			return false;
		}

		/** ****************************************************************************************************
		 * @brief						速度を追加
		 * @oaram[in] incrementSpeed	追加する値
		 **************************************************************************************************** */
		void IncrementSpeed(double incrementSpeed)
		{
			speed.x += incrementSpeed * Scene::DeltaTime();

			speed.x = Clamp(speed.x, -_MAX_SPEED, _MAX_SPEED);
		}

		/** ****************************************************************************************************
		 * @brief 速度を減らす
		 **************************************************************************************************** */
		void DecrementSpeed()
		{
			if (speed.x > 0.0)
			{
				speed.x -= _DECREMENT_SPEED * Scene::DeltaTime();

				speed.x = Max(speed.x, 0.0);
			}
			else if (speed.x < 0.0)
			{
				speed.x += _DECREMENT_SPEED * Scene::DeltaTime();

				speed.x = Min(speed.x, 0.0);
			}
		}

		/** ****************************************************************************************************
		 * @brief 更新
		 **************************************************************************************************** */
		void Update()
		{
			if (BitFlag::CheckAnyUnPop(flag, BitFlag::GetMask(GimmickData::gENABLE, GimmickData::gACTIVE)))
			{
				return;
			}

			if (CommonKeyState::gINPUT_LEFT.pressed())
			{
				IncrementSpeed(-_INCREMENT_SPEED);
			}
			else if (CommonKeyState::gINPUT_RIGHT.pressed())
			{
				IncrementSpeed(_INCREMENT_SPEED);
			}
			else
			{
				DecrementSpeed();
			}

			if (BitFlag::CheckAnyPop(flag, _IS_GROUND))
			{
				if (CommonKeyState::gINPUT_JUMP.pressed())
				{
					speed.y = _JUMP_POWER;
				}
			}
			else
			{
				speed.y += _GRAVITY * Scene::DeltaTime();
			}

			if (speed.x != 0.0f)
			{
				animationFrame = (speed.x < 0 ? 6 : 12) + (int)(Periodic::Triangle0_1(0.5s) * 2.0 + 0.5);
			}
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

			auto texture = TextureAsset{ GimmickData::gLABEL_TEXTURE[type] };

			auto orizinSize = texture.size() / _UV_CELL_NUM;

			const int offsetU = (animationFrame % 6) * orizinSize.x + _OFFSET_UV_LEFT_TOP.x;
			const int offsetV = (animationFrame / 6) * orizinSize.y + _OFFSET_UV_LEFT_TOP.y;

			GetBody()(texture(offsetU, offsetV, _SIZE_UV_WIDTH_HEIGHT)).draw();
		}

		/** ****************************************************************************************************
		 * @brief	本体サイズの取得
		 * @return	本体サイズ
		 **************************************************************************************************** */
		Transform::Scale GetBodyScale() const
		{
			return transform.GetWorldScale() * _BODEY_SIZE;
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
			return Circle{ GetBody().asPolygon().centroid(), Max(scale.x, scale.y)};
		}

		/** ****************************************************************************************************
		 * @brief 移動方向の更新
		 **************************************************************************************************** */
		void UpdateDirection()
		{
			auto angle = transform.GetWorldAngle();

			down = Vec2::Down().rotate(angle);

			right = Vec2{ down.y, -down.x };

			speed.y /= 4.0;
		}

		/** ****************************************************************************************************
		 * @brief			移動方向の更新
		 * @oaram[in] angle	角度
		 **************************************************************************************************** */
		void UpdateDirection(const double angle)
		{
			if (angle == transform.GetWorldAngle())
			{
				return;
			}

			transform.SetWorldAngle(angle);

			UpdateDirection();
		}
	};

	Player::Player(const GimmickData::Desc& data, Transform* pParent) :
		Gimmick{},
		_pImpl{ new Impl{ data, pParent } }
	{
	}

	Player::~Player()
	{
	}

	void Player::Update()
	{
		_pImpl->Update();
	}

	void Player::Draw() const
	{
		_pImpl->Draw();
	}

	Circle Player::GetCollision() const
	{
		return _pImpl->GetCollision();
	}

	void Player::SetIsActive(const bool isActive)
	{
		BitFlag::PopOrUnPop(_pImpl->flag, GimmickData::gACTIVE, isActive);
	}

	bool Player::GetIsActive()
	{
		return BitFlag::CheckAnyPop(_pImpl->flag, GimmickData::gACTIVE);
	}

	void Player::SetParent(Transform* pParent)
	{
		_pImpl->transform.SetParent(pParent);
	}

	GimmickData::Type Player::GetType()
	{
		return static_cast<GimmickData::Type>(_pImpl->type);
	}

	void Player::SetDestroy()
	{
		BitFlag::UnPop(_pImpl->flag, GimmickData::gENABLE);
	}

	bool Player::GetEnable()
	{
		return BitFlag::CheckAnyPop(_pImpl->flag, GimmickData::gENABLE);
	}

	void Player::UpdateDirection()
	{
		_pImpl->UpdateDirection();
	}

	void Player::UpdateDirection(const double angle)
	{
		_pImpl->UpdateDirection(angle);
	}

	void Player::HitCheck(const Array<LineString>& aGroundLines, const Array<std::weak_ptr<GimmickData::Gimmick>>& apGimmicks)
	{
		_pImpl->HitCheck(aGroundLines, apGimmicks);
	}
}

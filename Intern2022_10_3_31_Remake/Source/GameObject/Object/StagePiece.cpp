/** ****************************************************************************************************
 * @file
 * @brief ステージのカケラ操作関連
 **************************************************************************************************** */
#include "StagePiece.h"
#include "Stage.h"
#include "../Data/Transform.h"
#include "../../Utility/BitFlag.h"
#include "../../Utility/CommonKey.h"
#include "Tile.h"

namespace MySystem
{
	class StagePiece::Impl
	{
	private:
		using _Flag = uint8;	//!< フラグ用

		static constexpr _Flag _CAN_MOVING = BitFlag::Get<_Flag>(0);	//!< 移動できるか
		static constexpr _Flag _CAN_SCALING = BitFlag::Get<_Flag>(1);	//!< 拡大縮小できるか
		static constexpr _Flag _CAN_ROLLING = BitFlag::Get<_Flag>(2);	//!< 回転できるか
		static constexpr _Flag _NOW_MOVING = BitFlag::Get<_Flag>(3);	//!< 移動中か
		static constexpr _Flag _NOW_SCALING = BitFlag::Get<_Flag>(4);	//!< 拡大縮小中か
		static constexpr _Flag _NOW_ROLLING = BitFlag::Get<_Flag>(5);	//!< 回転中か
		static constexpr _Flag _NOW_ACTIVE = BitFlag::Get<_Flag>(6);	//!< 選択中か
		static constexpr _Flag _MASK_NOW_ACTION = BitFlag::GetMask<_Flag>(_NOW_MOVING, _NOW_SCALING, _NOW_ROLLING);	//!< 何か作業中か

		//! 各種テクスチャのラベルとパス
		static const std::array<std::pair<String, String>, static_cast<StagePieceData::TypeIndex>(StagePieceData::Type::Max) + 1> _TEXTURE_LABELS;

	public:
		Transform	transform;			//!< 座標
		Vec2		backScale;			//!< 背景サイズ
		Vec2		prevCursorPos;		//!< 前回のカーソル位置
		double		prevScale;			//!< 前回の大きさ
		double		cursorDistance;		//!< カーソルの距離
		StagePieceData::TypeIndex type;	//!< 背景の種類
		_Flag		flag;				//!< フラグ群
		Stage* pStage;					//!< ステージのポインタ
		uint8		pieceNumber;		//!< カケラの格納場所
		Array<std::unique_ptr<Tile>>	apTiles;				//!< タイル群
		Array<Polygon>					aHitBoxes;				//!< タイルの当たり判定
		Array<std::weak_ptr<GimmickData::Gimmick>>	apGimmick;	//!< カケラに所属するギミック

		/** ****************************************************************************************************
		 * @brief					コンストラクタ
		 * @oaram[in] data			生成用データ
		 * @param[in] pieceNumber	生成された順番
		 * @param[in] pStage		所属するステージ
		 * @param[in] pParent		親
		 **************************************************************************************************** */
		Impl(const StagePieceData::Desc& data, const uint8 pieceNumber, Stage* pStage, Transform* pParent) noexcept :
			transform{ data.position, Transform::Scale{ data.scale, data.scale }, data.angle, pParent },
			backScale{ data.backScale },
			prevScale{ 0.0 },
			prevCursorPos{ 0.0, 0.0 },
			cursorDistance{ 0.0 },
			type{ data.backType },
			flag{ static_cast<_Flag>(0) },
			pStage{ pStage },
			pieceNumber{ pieceNumber }
		{
			BitFlag::PopOrUnPop(flag, _CAN_MOVING, data.canMoving);
			BitFlag::PopOrUnPop(flag, _CAN_SCALING, data.canScaling);
			BitFlag::PopOrUnPop(flag, _CAN_ROLLING, data.canRolling);

			for (auto label : _TEXTURE_LABELS)
			{
				TextureAsset::Register(label.first, label.second, TextureDesc::Mipped);
			}

			CreateTile(data.aTileDescs);
			CreateGimmick(data.aGimmiclDesc);
		}

		/** ****************************************************************************************************
		 * @brief					ギミック生成
		 * @oaram[in] aGimmickDatas	生成用データ
		 **************************************************************************************************** */
		void CreateGimmick(const Array<GimmickData::Desc>& aGimmickDatas)
		{
			apGimmick.clear();

			// 最大でデータの数分出来る
			apGimmick.reserve(aGimmickDatas.size());

			for (auto& data : aGimmickDatas)
			{
				switch (static_cast<GimmickData::Type>(data.gimmickType))
				{
				case GimmickData::Type::Player:
					apGimmick.emplace_back(pStage->CreatePlayer(data, &transform));
					break;
				case GimmickData::Type::Goal:
					apGimmick.emplace_back(pStage->CreateGoal(data, &transform));
					break;
				}
			}
		}

		/** ****************************************************************************************************
		 * @brief					タイル生成
		 * @oaram[in] aGimmickDatas	生成用データ
		 **************************************************************************************************** */
		void CreateTile(const Array<TileData::Desc>& aTileDatas)
		{
			apTiles.clear();
			aHitBoxes.clear();

			// 最大でデータの数分出来る
			apTiles.reserve(aTileDatas.size());
			aHitBoxes.reserve(aTileDatas.size());

			for (auto& desc : aTileDatas)
			{
				auto& tile = apTiles.emplace_back(std::make_unique<Tile>(desc, &transform));

				auto collision = tile->GetLocalHitBox();

				// 今までのコリジョンと合成を試みる
				while (true)
				{
					bool isFusion = false;

					for (auto itr = aHitBoxes.begin(); itr != aHitBoxes.end(); ++itr)
					{
						auto collisions = Geometry2D::Or(*itr, collision);

						// 2つが1つになったときのみ合成成功とする
						if (collisions.size() == 1) {
							collision = collisions[0];

							// 合成されたということはオリジナルは消えることになる
							aHitBoxes.erase(itr);

							isFusion = true;
							break;
						}
					}

					// これ以上合成出来ないことが判明した
					if (not isFusion) {
						aHitBoxes.emplace_back(collision);
						break;
					}
				}
			}

			TileData::GetSimplePolygon(aHitBoxes, 2.0, 0.00001);
		}

		/** ****************************************************************************************************
		 * @brief 描画
		 **************************************************************************************************** */
		void Draw() const noexcept
		{
			auto body = GetBody();
			auto bodyScale = GetBodyScale();
			auto texture = TextureAsset(_TEXTURE_LABELS[type].first);
			ColorF color{ static_cast<double>(StagePieceData::gMAX_PIECE_NUM - pieceNumber - 1) * 0.05 + 0.5 };
			color.a = 1.0;

			{
				const ScopedRenderStates2D sampler{ SamplerState::RepeatLinear };
				body(texture.mapped(bodyScale)).draw(color);
			}

			for (auto itr = apGimmick.begin(); itr != apGimmick.end(); ++itr)
			{
				(itr)->lock()->Draw();
			}

			for (auto itr = apTiles.begin(); itr != apTiles.end(); ++itr)
			{
				(*itr)->Draw();
			}

			if (BitFlag::CheckAnyPop(flag, _NOW_ACTIVE))
			{
				body.drawFrame(5.0, Palette::Orange);
			}
		}

		/** ****************************************************************************************************
		 * @brief			カケラの更新処理
		 * @retval true		アクションがあった
		 * @retval false	アクションがなかった
		 **************************************************************************************************** */
		bool UpdatePiece() noexcept
		{
			auto body = GetBody();
			auto isClicked = CommonKeyState::gINPUT_SUBMIT.down() && body.mouseOver();

			if (isClicked)
			{
				BitFlag::Pop(flag, _NOW_ACTIVE);
			}

			if (BitFlag::CheckAllUnPop(flag, _NOW_ACTIVE))
			{
				return false;
			}

			if (BitFlag::CheckAnyPop(flag, _MASK_NOW_ACTION))
			{
				return ProcessAction() || isClicked;
			}
			else
			{
				return CheckAction(body) || isClicked;
			}
		}

		/** ****************************************************************************************************
		 * @brief				有効かどうか設定
		 * @param[in] isActive	有効かどうか
		 **************************************************************************************************** */
		void SetActive(const bool isActive) noexcept
		{
			BitFlag::PopOrUnPop(flag, _NOW_ACTIVE, isActive);
		}

		/** ****************************************************************************************************
		 * @brief	自身の持つ地面の当たり判定取得
		 * @return	地面の当たり判定
		 **************************************************************************************************** */
		Array<Polygon> GetLocalHitBoxes() const
		{
			Array<Polygon> polygons{ aHitBoxes };

			for (auto& polygon : polygons)
			{
				polygon.
					rotate(transform.GetLocalAngle()).
					scale(transform.GetLocalScale()).
					moveBy(transform.GetLocalPosition());
			}

			return polygons;
		}

		/** ****************************************************************************************************
		 * @brief	自身の持つ地面のワールド内の当たり判定取得
		 * @return	地面の当たり判定
		 **************************************************************************************************** */
		Array<Polygon> GetWorldHitBoxes() const
		{
			Array<Polygon> polygons{ aHitBoxes };

			for (auto polygon : polygons)
			{
				polygon.
					rotate(transform.GetWorldAngle()).
					scale(transform.GetWorldScale()).
					moveBy(transform.GetWorldPosition());
			}

			return polygons;
		}

		/** ****************************************************************************************************
		 * @brief	本体サイズの取得
		 * @return	本体サイズ
		 **************************************************************************************************** */
		Transform::Scale GetBodyScale() const
		{
			return transform.GetWorldScale() * backScale;
		}

		/** ****************************************************************************************************
		 * @brief	本体の取得
		 * @return	本体
		 **************************************************************************************************** */
		Quad GetBody() const
		{
			return
				RectF{ Arg::center = transform.GetWorldPosition(), GetBodyScale() }.
				rotated(transform.GetWorldAngle());
		}

	private:
		/** ****************************************************************************************************
		 * @brief			カケラの操作
		 * @retval true		アクションがあった
		 * @retval false	アクションがなかった
		 **************************************************************************************************** */
		bool ProcessAction()
		{
			if (CommonKeyState::gINPUT_PIECE_ACTION.up())
			{
				BitFlag::UnPop(flag, _MASK_NOW_ACTION);
				return false;
			}

			auto nowMoving = BitFlag::CheckAnyPop(flag, _NOW_MOVING);
			auto nowScaling = BitFlag::CheckAnyPop(flag, _NOW_SCALING);
			auto nowRolling = BitFlag::CheckAnyPop(flag, _NOW_ROLLING);

			if (nowMoving)
			{
				auto cursorPosition = Cursor::PosF();
				cursorPosition.x = round(cursorPosition.x);
				cursorPosition.y = round(cursorPosition.y);

				transform.SetWorldPosition(cursorPosition);
			}
			else if (nowScaling)
			{
				auto newScale = (Cursor::PosF() - transform.GetWorldPosition()).length() / (backScale / 2.0).length();

				newScale = round(newScale * 10.0) * 0.1;

				newScale = Clamp(newScale, 0.5, 2.0);

				transform.SetLocalScale(Vec2{ newScale, newScale });
			}
			else if (nowRolling)
			{
				auto position = transform.GetWorldPosition();
				auto prevDirection = prevCursorPos - position;
				auto currentDirection = Cursor::PosF() - position;

				if (currentDirection != Vec2::Zero())
				{
					auto cosTheta = prevDirection.dot(currentDirection) / (prevDirection.length() * currentDirection.length());
					cosTheta = Clamp(cosTheta, -1.0, 1.0);
					auto theta = acos(cosTheta);

					auto prevRigth = Vec2{ -prevDirection.y, prevDirection.x };
					auto rightDot = prevRigth.dot(currentDirection);

					if (rightDot < 0.0)
					{
						theta *= -1.0;
					}

					transform.SetWorldAngle(theta + transform.GetWorldAngle());
				}
			}

			prevCursorPos = Cursor::PosF();

			return true;
		}

		/** ****************************************************************************************************
		 * @brief			カケラの操作決定
		 * @param[in] body	自身の当たり判定
		 * @retval true		アクションがあった
		 * @retval false	アクションがなかった
		 **************************************************************************************************** */
		bool CheckAction(const Quad& body)
		{
			auto bodyScale = GetBodyScale();

			auto size = Min(bodyScale.x, bodyScale.y) / 4.0;
			auto isDown = CommonKeyState::gINPUT_PIECE_ACTION.down();

			if (isDown)
			{
				prevScale = transform.GetLocalScale().x;
				prevCursorPos = Cursor::PosF();
				cursorDistance = (prevCursorPos - transform.GetWorldPosition()).lengthSq() * Max(bodyScale.x, bodyScale.y);
			}

			if (body.stretched(-size).mouseOver())
			{
				if (BitFlag::CheckAnyPop(flag, _CAN_MOVING) && apGimmick.empty())
				{
					Cursor::RequestStyle(CursorStyle::Cross);
					BitFlag::PopOrUnPop(flag, _NOW_MOVING, isDown);
				}
				else
				{
					Cursor::RequestStyle(CursorStyle::NotAllowed);
				}
			}
			else if (body.mouseOver())
			{
				if (BitFlag::CheckAnyPop(flag, _CAN_SCALING))
				{
					Cursor::RequestStyle(CursorStyle::ResizeAll);
					BitFlag::PopOrUnPop(flag, _NOW_SCALING, isDown);
				}
				else
				{
					Cursor::RequestStyle(CursorStyle::NotAllowed);
				}
			}
			else if (body.stretched(size).mouseOver())
			{
				if (BitFlag::CheckAnyPop(flag, _CAN_ROLLING))
				{
					Cursor::RequestStyle(CursorStyle::Hand);
					BitFlag::PopOrUnPop(flag, _NOW_ROLLING, isDown);
				}
				else
				{
					Cursor::RequestStyle(CursorStyle::NotAllowed);
				}
			}

			return isDown;
		}
	};

	const std::array<std::pair<String, String>, static_cast<StagePieceData::TypeIndex>(StagePieceData::Type::Max) + 1> StagePiece::Impl::_TEXTURE_LABELS{
		std::make_pair<String, String>(U"BackNormalTexture",	U"Assets/Image/NormalBack.png"),
		std::make_pair<String, String>(U"BackVoidTexture",		U"Assets/Image/VoidBack.png"),
		std::make_pair<String, String>(U"BackGravityTexture",	U"Assets/Image/GravityBack.png"),
		std::make_pair<String, String>(U"BackNullTexture",		U"Assets/Image/NormalBack.png")
	};

	StagePiece::StagePiece(const StagePieceData::Desc& data, const uint8 pieceNumber, Stage* pStage, Transform* pParent) noexcept :
		_pImpl{ new Impl{ data, pieceNumber, pStage, pParent } }
	{

	}

	StagePiece::~StagePiece() = default;

	void StagePiece::Update() noexcept
	{

	}

	void StagePiece::Draw() const noexcept
	{
		_pImpl->Draw();
	}

	bool StagePiece::UpdatePiece() noexcept
	{
		return _pImpl->UpdatePiece();
	}

	void StagePiece::SetActive(const bool isActive) noexcept
	{
		_pImpl->SetActive(isActive);
	}

	Array<Polygon> StagePiece::GetLocalHitBoxes() const
	{
		return _pImpl->GetLocalHitBoxes();
	}

	Array<Polygon> StagePiece::GetWorldHitBoxes() const
	{
		return _pImpl->GetWorldHitBoxes();
	}

	Quad StagePiece::GetLocalBody() const
	{
		return
			RectF{ Arg::center = _pImpl->transform.GetLocalPosition(), _pImpl->transform.GetLocalScale() * _pImpl->backScale }.
			rotated(_pImpl->transform.GetLocalAngle());
	}

	Quad StagePiece::GetWorldBody() const
	{
		return _pImpl->GetBody();
	}

	StagePieceData::Type StagePiece::GetType() const
	{
		return static_cast<StagePieceData::Type>(_pImpl->type);
	}

	Array<std::weak_ptr<GimmickData::Gimmick>> StagePiece::GetGimmicks() const
	{
		return _pImpl->apGimmick;
	}

	void StagePiece::AddGimmick(std::weak_ptr<GimmickData::Gimmick> pGimmick)
	{
		_pImpl->apGimmick.emplace_back(pGimmick);
		pGimmick.lock()->SetParent(&_pImpl->transform);
	}

	void StagePiece::DeleteActiveGimmicks()
	{
		auto& apGimmicks = _pImpl->apGimmick;

		for (auto itr = apGimmicks.begin(); itr != apGimmicks.end(); )
		{
			auto pGimmick = itr->lock();

			if (not pGimmick->GetIsActive())
			{
				++itr;
				continue;
			}

			pGimmick->SetParent(nullptr);
			itr = apGimmicks.erase(itr);
		}
	}

	double StagePiece::GetAngle()
	{
		return _pImpl->transform.GetWorldAngle();
	}
}

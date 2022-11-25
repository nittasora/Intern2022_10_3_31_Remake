#include "Stage.h"
#include "StagePiece.h"
#include "Player.h"
#include "Goal.h"
#include "../Data/Transform.h"
#include "../../Utility/BitFlag.h"
#include "../../Utility/CommonKey.h"

namespace MySystem
{
	class Stage::Impl
	{
	private:
		using _UpdateFunction = void (Stage::Impl::*)();		//!< 更新関数用
		using _DrawFunction = void (Stage::Impl::*)() const;	//!< 描画関数用
		using _Flag = uint16;

		static constexpr _Flag _ENABLE = BitFlag::Get<_Flag>(0);			//!< 存在フラグ
		static constexpr _Flag _ACTIVE = BitFlag::Get<_Flag>(1);			//!< 有効フラグ
		static constexpr _Flag _PIECE_ACTION = BitFlag::Get<_Flag>(2);		//!< 現在カケラ操作状態か
		static constexpr _Flag _CLEAR = BitFlag::Get<_Flag>(3);				//!< ゲームをクリアした
		static constexpr _Flag _GAME_OVER = BitFlag::Get<_Flag>(4);			//!< ゲームオーバーになった
		static constexpr _Flag _FALL_OUT = BitFlag::Get<_Flag>(5);			//!< ゲームオーバーは画面外への落下が原因だった
		static constexpr _Flag _SCALE_OUT = BitFlag::Get<_Flag>(6);			//!< ゲームオーバーはプレイヤーが小さくなりすぎたのが原因だった
		static constexpr _Flag _LOAD_WAIT_STAGE = BitFlag::Get<_Flag>(7);	//!< ステージ読み込み待ち
		static constexpr _Flag _LOAD_WAIT_SELECT = BitFlag::Get<_Flag>(8);	//!< ステージ選択余も気も待ち

		static constexpr _Flag _MASK_ACTIVE = BitFlag::GetMask<_Flag>(_ENABLE, _ACTIVE);				//!< 有効化されているか判定用
		static constexpr _Flag _MASK_GAME_END = BitFlag::GetMask<_Flag>(_CLEAR, _GAME_OVER);			//!< ゲームが終了したか判定用
		static constexpr _Flag _MASK_CAUSE_GAME_OVER = BitFlag::GetMask<_Flag>(_FALL_OUT, _SCALE_OUT);	//!< ゲームオーバーに原因があるか判定用

	public:
		_Flag			flag;			//!< フラグ
		Stage*			pStage;			//!< ステージの本体
		Transform		transform;		//!< 座標
		_UpdateFunction	pUpdate;		//!< 更新関数
		_DrawFunction	pDraw;			//!< 描画関数
		double			animationTime;	//!< アニメーションに使う時間
		BasicCamera2D	camera;			//!< カメラ
		Array<std::unique_ptr<StagePiece>>	apPiece;	//!< カケラ
		Array<Polygon>						aHitBoxes;	//!< ステージ当たり判定
		Array<std::shared_ptr<Player>>		apPlayers;	//!< プレイヤー
		Array<std::shared_ptr<Goal>>		apGoals;	//!< ゴール
		Array<std::shared_ptr<GimmickData::Gimmick>>	apGimmicks;	//!< カケラに所属しないギミック

		/** ****************************************************************************************************
		 * @brief				コンストラクタ
		 * @oaram[in] position	場所
		 * @param[in] scale		大きさ
		 * @param[in] angle		角度
		 * @param[in] pParent	親
		 **************************************************************************************************** */
		Impl(Stage* pStage, const Vec2& position, const Vec2& scale, const double angle, Transform* pParent) noexcept :
			flag{ static_cast<_Flag>(0) },
			pStage{ pStage },
			transform{ position, scale, angle, pParent },
			pUpdate{ &Stage::Impl::UpdatePieceAction },
			pDraw{ &Stage::Impl::DrawPieceAction },
			animationTime{ 0.0 },
			camera{ }
		{
			camera.setCenter(transform.GetWorldPosition());
			FontAsset::Register(U"Stage", 30);
		}

		/** ****************************************************************************************************
		 * @brief 更新
		 **************************************************************************************************** */
		void Update() noexcept
		{
			// 何らかの要因で有効化されていないなら実行しない
			if (BitFlag::CheckAnyUnPop(flag, _MASK_ACTIVE))
			{
				return;
			}

			auto transformer = camera.createTransformer();

			if (KeyEnter.down())
			{
				SetGameOverPlayerFallOut();
			}

			ChangeMode();

			(this->*pUpdate)();
		}

		/** ****************************************************************************************************
		 * @brief 描画
		 **************************************************************************************************** */
		void Draw() const noexcept
		{
			auto transformer = camera.createTransformer();
			const ScopedRenderStates2D sampler{ SamplerState::ClampNearest };

			(this->*pDraw)();

			//FontAsset(U"Stage")(U"Enterキーでやり直し").draw(0.0, 0.0);
		}

		/** ****************************************************************************************************
		 * @brief ステージ生成
		 **************************************************************************************************** */
		void CreateStage() noexcept
		{
			auto data = StageData::GetCurrentData();

			const auto& pieceData = data->aPieceDescs;

			apGimmicks.clear();
			apPlayers.clear();
			apGoals.clear();

			apPiece.clear();
			apPiece.resize(pieceData.size());

			for (size_t i = 0; i < pieceData.size(); i++)
			{
				apPiece[i].reset();
				apPiece[i] = std::make_unique<StagePiece>(pieceData[i], static_cast<uint8>(i), pStage, &transform);
			}

			flag = static_cast<_Flag>(0);

			BitFlag::Pop(flag, _MASK_ACTIVE);
			BitFlag::Pop(flag, _PIECE_ACTION);
		}

		/** ****************************************************************************************************
		 * @brief ゲームクリアにする
		 **************************************************************************************************** */
		void SetGameClear()
		{
			BitFlag::Pop(flag, _CLEAR);
		}

		/** ****************************************************************************************************
		 * @brief 縮小過多によるゲームオーバーにする
		 **************************************************************************************************** */
		void SetGameOverPlayerScaleOut()
		{
			BitFlag::Pop(flag, _GAME_OVER);
			BitFlag::Pop(flag, _SCALE_OUT);
		}

		/** ****************************************************************************************************
		 * @brief 落下によるゲームオーバーにする
		 **************************************************************************************************** */
		void SetGameOverPlayerFallOut()
		{
			BitFlag::Pop(flag, _GAME_OVER);
			BitFlag::Pop(flag, _FALL_OUT);
		}

		/** ****************************************************************************************************
		 * @brief			ステージ読み込み待ちか確認
		 * @retval true		待っている
		 * @retval false	待っていない
		 **************************************************************************************************** */
		bool LoadStageWait()
		{
			return BitFlag::CheckAnyPop(flag, _LOAD_WAIT_STAGE);
		}

		/** ****************************************************************************************************
		 * @brief			ステージ選択読み込み待ちか確認
		 * @retval true		待っている
		 * @retval false	待っていない
		 **************************************************************************************************** */
		bool LoadSelectWait()
		{
			return BitFlag::CheckAnyPop(flag, _LOAD_WAIT_SELECT);
		}

	private:
		/** ****************************************************************************************************
		 * @brief プレイヤーの行動ターン更新
		 **************************************************************************************************** */
		void UpdatePlayerAction()
		{
			Array<LineString> groundLines;

			for (auto hitBox : aHitBoxes)
			{
				auto worldHitBox = hitBox.
					rotate(transform.GetWorldAngle()).
					scale(transform.GetWorldScale()).
					moveBy(transform.GetWorldPosition());

				groundLines.emplace_back(LineString{ worldHitBox.outer() });

				for (auto inner : worldHitBox.inners())
				{
					groundLines.emplace_back(LineString{ inner });
				}
			}

			Array<std::weak_ptr<GimmickData::Gimmick>> apAllGimmicks;

			apAllGimmicks.insert(apAllGimmicks.end(), apGoals.begin(), apGoals.end());

			for (auto player : apPlayers)
			{
				if (not player->GetIsActive())
				{
					continue;
				}

				player->Update();

				player->HitCheck(groundLines, apAllGimmicks);

				for (auto itr = apPiece.rbegin(); itr != apPiece.rend(); ++itr)
				{
					if ((*itr)->GetType() == StagePieceData::Type::Void) {
						continue;
					}

					if ((*itr)->GetWorldBody().intersects(player->GetCollision()))
					{
						if ((*itr)->GetType() == StagePieceData::Type::Gravity) {
							player->UpdateDirection((*itr)->GetAngle());
						}

						break;
					}
				}
			}

			bool enable = false;
			for (auto pGoal : apGoals)
			{
				enable = enable || pGoal->GetEnable();
			}

			if (not enable)
			{
				SetGameClear();
				return;
			}

			for (auto pPlayer : apPlayers)
			{
				auto collision = pPlayer->GetCollision();

				if (not Scene::Rect().intersects(collision))
				{
					SetGameOverPlayerFallOut();
					return;
				}
			}
		}

		/** ****************************************************************************************************
		 * @brief プレイヤーの行動ターン描画
		 **************************************************************************************************** */
		void DrawPlayerAction() const
		{
			for (auto itr = apGimmicks.begin(); itr != apGimmicks.end(); ++itr)
			{
				(*itr)->Draw();
			}

			for (auto itr = apPiece.begin(); itr != apPiece.end(); ++itr)
			{
				(*itr)->Draw();
			}

			Array<std::weak_ptr<GimmickData::Gimmick>> apAllGimmicks;

			apAllGimmicks.insert(apAllGimmicks.end(), apPlayers.begin(), apPlayers.end());
			apAllGimmicks.insert(apAllGimmicks.end(), apGoals.begin(), apGoals.end());

			for (auto itr = apAllGimmicks.begin(); itr != apAllGimmicks.end(); ++itr)
			{
				auto pGimmick = itr->lock();

				if (not pGimmick->GetIsActive())
				{
					continue;
				}

				pGimmick->Draw();
			}
		}

		/** ****************************************************************************************************
		 * @brief カケラの操作ターン更新
		 **************************************************************************************************** */
		void UpdatePieceAction()
		{
			// 後から描画したものが先にヒットするものとする
			for (auto itr = apPiece.rbegin(); itr != apPiece.rend(); ++itr)
			{
				// 現在操作中でないなら次へ
				if (not (*itr)->UpdatePiece())
				{
					continue;
				}

				// 一度すべて無効化する
				for (auto piece = apPiece.begin(); piece != apPiece.end(); ++piece)
				{
					(*piece)->SetActive(false);
				}

				// 現在操作中のカケラを再び有効化する
				(*itr)->SetActive(true);

				break;
			}
		}

		/** ****************************************************************************************************
		 * @brief カケラの操作ターン描画
		 **************************************************************************************************** */
		void DrawPieceAction() const
		{
			for (auto itr = apPiece.begin(); itr != apPiece.end(); ++itr)
			{
				(*itr)->Draw();
			}
		}

		/** ****************************************************************************************************
		 * @brief ゲームクリア時更新
		 **************************************************************************************************** */
		void UpdateGameClear()
		{
			const double CAMERA_TIME = 0.5;
			const double CLEAR_WAIT = 1.0;
			const double CAMERA_SCALE = 3.0;

			animationTime += Scene::DeltaTime();

			if (animationTime < CAMERA_TIME)
			{
				auto position = apPlayers[0]->GetCollision().center;

				auto rate = animationTime / CAMERA_TIME;
				camera.setScale(Math::Lerp(1.0, CAMERA_SCALE, rate));
				camera.setCenter(transform.GetWorldPosition().lerp(position, rate));
			}
			else if (animationTime < CAMERA_TIME + CLEAR_WAIT)
			{
				auto position = apPlayers[0]->GetCollision().center;

				camera.setScale(CAMERA_SCALE);
				camera.setCenter(position);
			}
			else
			{
				BitFlag::Pop(flag, _LOAD_WAIT_SELECT);
			}
		}

		/** ****************************************************************************************************
		 * @brief ゲームクリア時描画
		 **************************************************************************************************** */
		void DrawGameClear() const
		{
			DrawPlayerAction();
		}

		/** ****************************************************************************************************
		 * @brief ゲームオーバー時更新
		 **************************************************************************************************** */
		void UpdateGameOver()
		{
			const double CAMERA_TIME = 0.5;

			animationTime += Scene::DeltaTime();

			if (animationTime < CAMERA_TIME)
			{
				auto position = transform.GetWorldPosition();
				position.x += Periodic::Triangle0_1(CAMERA_TIME / 5.0) * 5.0;

				camera.setCenter(position);
			}
			else
			{
				BitFlag::Pop(flag, _LOAD_WAIT_STAGE);
			}
		}

		/** ****************************************************************************************************
		 * @brief ゲームオーバー時描画
		 **************************************************************************************************** */
		void DrawGameOver() const
		{
			DrawPlayerAction();
		}

		/** ****************************************************************************************************
		 * @brief 更新・描画の切り替え
		 **************************************************************************************************** */
		void ChangeMode()
		{
			// 全てのフラグがたっていないなら -> ゲーム続行時の処理へ
			if (BitFlag::CheckAllUnPop(flag, _MASK_GAME_END))
			{
				// キーかマウスを入力したときに切り替えるが、キーに比べ余計な行動を生みにくいのでマウス操作の方が優先される。
				if (CommonKeyState::gINPUT_ANY_MOUSE.pressed())
				{
					if (BitFlag::CheckAllUnPop(flag, _PIECE_ACTION))
					{
						AttachmentGimmicks();
					}

					BitFlag::Pop(flag, _PIECE_ACTION);
				}
				else if (CommonKeyState::gINPUT_ANY_KEY.pressed())
				{
					if (BitFlag::CheckAnyPop(flag, _PIECE_ACTION))
					{
						CreateHitBox();
						CheckActiveGimmick();
						for (auto piece = apPiece.begin(); piece != apPiece.end(); ++piece)
						{
							(*piece)->SetActive(false);
						}

						for (auto pPlayer : apPlayers)
						{
							pPlayer->UpdateDirection();
						}
					}

					BitFlag::UnPop(flag, _PIECE_ACTION);
				}

				// フラグが立っている -> カケラ操作モード確定
				if (BitFlag::CheckAnyPop(flag, _PIECE_ACTION))
				{
					pUpdate = &Stage::Impl::UpdatePieceAction;
					pDraw = &Stage::Impl::DrawPieceAction;
				}
				else // プレイヤー操作モード確定 
				{
					pUpdate = &Stage::Impl::UpdatePlayerAction;
					pDraw = &Stage::Impl::DrawPlayerAction;
				}
			}
			else
			{
				// フラグが立っている -> ゲームクリア確定
				if (BitFlag::CheckAnyPop(flag, _CLEAR))
				{
					pUpdate = &Stage::Impl::UpdateGameClear;
					pDraw = &Stage::Impl::DrawGameClear;
				}
				else // ゲームオーバーの可能性あり
				{
					// ゲームオーバーになった原因がない -> 実際はゲームオーバーではなかった
					if (BitFlag::CheckAllUnPop(flag, _MASK_CAUSE_GAME_OVER))
					{
						BitFlag::UnPop(flag, _GAME_OVER);

						// やり直し
						ChangeMode();
						return;
					}

					pUpdate = &Stage::Impl::UpdateGameOver;
					pDraw = &Stage::Impl::DrawGameOver;
				}
			}
		}

		/** ****************************************************************************************************
		 * @brief 当たり判定の生成
		 **************************************************************************************************** */
		void CreateHitBox()
		{
			aHitBoxes.clear();

			for (auto piece = apPiece.begin(); piece != apPiece.end(); ++piece)
			{
				auto pieceHitBoxes = (*piece)->GetLocalHitBoxes();
				Array<Polygon> temp;

				if ((*piece)->GetType() == StagePieceData::Type::Void)
				{
					// 元々当たり判定があったならそのまま
					if (not aHitBoxes.empty())
					{
						temp.insert(temp.end(), aHitBoxes.begin(), aHitBoxes.end());
					}

					// カケラに当たり判定があったならそのまま
					if (not pieceHitBoxes.empty())
					{
						temp.insert(temp.end(), pieceHitBoxes.begin(), pieceHitBoxes.end());
					}
				}
				else
				{
					auto backBox = (*piece)->GetLocalBody().asPolygon();
					for (auto hitBox : aHitBoxes)
					{
						auto results = Geometry2D::Subtract(hitBox, backBox);

						temp.insert(temp.end(), results.begin(), results.end());
					}
				}

				// 今までのコリジョンと合成を試みる
				for (auto collision : pieceHitBoxes)
				{
					while (true)
					{
						bool isFusion = false;

						for (auto itr = temp.begin(); itr != temp.end(); ++itr)
						{
							auto collisions = Geometry2D::Or(*itr, collision);

							// 2つが1つになったときのみ合成成功とする
							if (collisions.size() == 1) {
								collision = collisions[0];

								// 合成されたということはオリジナルは消えることになる
								temp.erase(itr);

								isFusion = true;
								break;
							}
						}

						// これ以上合成出来ないことが判明した
						if (not isFusion) {
							temp << collision;
							break;
						}
					}
				}

				aHitBoxes = temp;
			}

			TileData::GetSimplePolygon(aHitBoxes, 0.0, 0.00001);
		}

		/** ****************************************************************************************************
		 * @brief ギミックを各カケラに取り付ける
		 **************************************************************************************************** */
		void AttachmentGimmicks()
		{
			Array<std::weak_ptr<GimmickData::Gimmick>> apAllGimmicks;

			apAllGimmicks.insert(apAllGimmicks.end(), apPlayers.begin(), apPlayers.end());
			apAllGimmicks.insert(apAllGimmicks.end(), apGoals.begin(), apGoals.end());

			for (auto itr = apAllGimmicks.begin(); itr != apAllGimmicks.end(); ++itr)
			{
				auto pGimmick = itr->lock();

				if (not pGimmick->GetIsActive())
				{
					continue;
				}

				auto collision = pGimmick->GetCollision();

				bool isHit = false;
				for (auto pieceItr = apPiece.rbegin(); pieceItr != apPiece.rend(); ++pieceItr)
				{
					if ((*pieceItr)->GetWorldBody().intersects(collision))
					{
						(*pieceItr)->AddGimmick(pGimmick);
						isHit = true;
						break;
					}
				}

				if (isHit)
				{
					continue;
				}

				apGimmicks.emplace_back(pGimmick);
			}
		}

		/** ****************************************************************************************************
		 * @brief 有効化されるギミックを調べる
		 **************************************************************************************************** */
		void CheckActiveGimmick()
		{
			Array<Polygon> aWorldHitBoxes{ aHitBoxes };

			for (auto& hitBox : aWorldHitBoxes)
			{
				hitBox.
					rotate(transform.GetWorldAngle()).
					scale(transform.GetWorldScale()).
					moveBy(transform.GetWorldPosition());
			}

			for (auto itr = apGimmicks.begin(); itr != apGimmicks.end();)
			{
				(*itr)->SetIsActive(true);
				auto collision = (*itr)->GetCollision().stretched(-1);

				for (auto pieceItr = apPiece.rbegin(); pieceItr != apPiece.rend(); ++pieceItr)
				{
					if ((*pieceItr)->GetWorldBody().intersects(collision))
					{
						(*itr)->SetIsActive(false);
						break;
					}
				}

				if (not (*itr)->GetIsActive())
				{
					++itr;
					continue;
				}

				itr = apGimmicks.erase(itr);
			}

			for (size_t i = 0, max = apPiece.size(); i < max; ++i)
			{
				auto gimmicks = apPiece[i]->GetGimmicks();

				for (auto& gimmick : gimmicks)
				{
					auto pGimmick = gimmick.lock();
					auto collision = pGimmick->GetCollision().stretched(-1);

					pGimmick->SetIsActive(true);

					for (auto hitBox : aWorldHitBoxes)
					{
						if (hitBox.intersects(collision))
						{
							pGimmick->SetIsActive(false);
							break;
						}
					}

					for (size_t j = i + 1; j < max; ++j)
					{
						if (apPiece[j]->GetWorldBody().intersects(collision))
						{
							pGimmick->SetIsActive(false);
						}
					}
				}

				apPiece[i]->DeleteActiveGimmicks();
			}
		}
	};

	Stage::Stage(const Vec2& position, const Vec2& scale, const double angle, Transform* pParent) noexcept :
		_pImpl{ new Impl{ this, position, scale, angle, pParent } }
	{

	}

	Stage::~Stage() = default;

	void Stage::Update() noexcept
	{
		_pImpl->Update();
	}

	void Stage::Draw() const noexcept
	{
		_pImpl->Draw();
	}

	void Stage::CreateStage() noexcept
	{
		_pImpl->CreateStage();
	}

	void Stage::CreateStage(const StageData::WorldIndex worldIndex, const StageData::StageIndex stageIndex) noexcept
	{
		StageData::Load(worldIndex, stageIndex);

		_pImpl->CreateStage();
	}

	void Stage::SetGameClear()
	{
		_pImpl->SetGameClear();
	}

	void Stage::SetGameOverPlayerScaleOut()
	{
		_pImpl->SetGameOverPlayerScaleOut();
	}

	void Stage::SetGameOverPlayerFallOut()
	{
		_pImpl->SetGameOverPlayerFallOut();
	}

	bool Stage::LoadStageWait()
	{
		return _pImpl->LoadStageWait();
	}

	bool Stage::LoadSelectWait()
	{
		return _pImpl->LoadSelectWait();
	}

	std::weak_ptr<GimmickData::Gimmick> Stage::CreatePlayer(const GimmickData::Desc& data, Transform* pParent)
	{
		auto pointer = _pImpl->apPlayers.emplace_back(new Player{ data, pParent });

		return pointer;
	}

	std::weak_ptr<GimmickData::Gimmick> Stage::CreateGoal(const GimmickData::Desc& data, Transform* pParent)
	{
		auto pointer = _pImpl->apGoals.emplace_back(new Goal{ data, pParent });

		return pointer;
	}

	void Stage::SetLocalScale(const Vec2& scale) noexcept
	{
		_pImpl->transform.SetLocalScale(scale);
	}
}

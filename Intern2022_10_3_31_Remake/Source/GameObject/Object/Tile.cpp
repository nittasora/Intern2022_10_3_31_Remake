#include "Tile.h"
#include "../Data/Transform.h"

namespace MySystem
{
	class Tile::Impl
	{
	private:
		//! タイル共通当たり判定
		static const std::array<Polygon, static_cast<size_t>(TileData::Type::Max) + 1U> _STATIC_COLLISIONS;

	public:
		Transform			transform;	//!< 座標
		TileData::TypeIndex	tileType;	//!< タイルの種類
		TextureRegion		texture;	//!< テクスチャ

		/** ****************************************************************************************************
		 * @brief				コンストラクタ
		 * @oaram[in] data		生成用データ
		 * @param[in] pParent	親
		 **************************************************************************************************** */
		Impl(const TileData::Desc& data, Transform* pParent) :
			transform{ data.position, Vec2{ TileData::gTILE_SIZE, TileData::gTILE_SIZE }, data.angle, pParent },
			tileType{ data.tileType },
			texture{  }
		{
			TextureAsset::Register(U"TileTexture", U"Assets/Image/Tile.png");
			Texture newTexture = TextureAsset(U"TileTexture");
			auto textureUVCellSize = newTexture.size() / TileData::gUV_CELL_NUM;
			texture = newTexture(data.uv * textureUVCellSize, textureUVCellSize);
		}

		/** ****************************************************************************************************
		 * @brief 描画
		 **************************************************************************************************** */
		void Draw() const
		{
			auto position = transform.GetWorldPosition();
			auto scale = transform.GetWorldScale();
			auto angle = transform.GetWorldAngle();
			RectF{ position, scale }.
				rotatedAt(position, angle).
				stretched(1)(texture).
				draw();
		}

		/** ****************************************************************************************************
		 * @brief	自身のワールド内の当たり判定取得
		 * @return	当たり判定
		 **************************************************************************************************** */
		Polygon GetWorldHitBox() const
		{
			return _STATIC_COLLISIONS[tileType].
				rotated(transform.GetWorldAngle()).
				scale(transform.GetWorldScale()).
				moveBy(transform.GetWorldPosition());
		}

		/** ****************************************************************************************************
		 * @brief	自身の当たり判定取得
		 * @return	当たり判定
		 **************************************************************************************************** */
		Polygon GetLocalHitBox() const
		{
			return _STATIC_COLLISIONS[tileType].
				rotated(transform.GetLocalAngle()).
				scale(transform.GetLocalScale()).
				moveBy(transform.GetLocalPosition());
		}
	};

	const std::array<Polygon, static_cast<size_t>(TileData::Type::Max) + 1U> Tile::Impl::_STATIC_COLLISIONS = {
		Polygon{ Vec2{ 0.0, 0.0 }, Vec2{ 1.0, 0.0 }, Vec2{ 1.0, 1.0 }, Vec2{ 0.0, 1.0 } },
		Polygon{ Vec2{ 1.0, 0.0 }, Vec2{ 1.0, 1.0 }, Vec2{ 0.0, 1.0 } },
		Polygon{ Vec2{ 0.0, 0.0 }, Vec2{ 1.0, 0.0 }, Vec2{ 1.0, 1.0 }, Vec2{ 0.0, 1.0 } }	// Max
	};

	Tile::Tile(const TileData::Desc& data, Transform* pParent) noexcept :
		_pImpl{ new Impl{ data, pParent } }
	{
	}

	Tile::~Tile()
	{
	}

	void Tile::Draw() const noexcept
	{
		_pImpl->Draw();
	}

	Polygon Tile::GetWorldHitBox() const
	{
		return _pImpl->GetWorldHitBox();
	}

	Polygon Tile::GetLocalHitBox() const
	{
		return _pImpl->GetLocalHitBox();
	}
}

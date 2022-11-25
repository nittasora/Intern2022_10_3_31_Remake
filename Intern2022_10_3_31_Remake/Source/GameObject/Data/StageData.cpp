/** ****************************************************************************************************
 * @file
 * @brief ステージのデータ操作関連
 **************************************************************************************************** */
#include "StageData.h"

namespace MySystem::StageData
{
	/** ****************************************************************************************************
	 * @brief データの読み込みに失敗した場合のデータ
	 **************************************************************************************************** */
	void CreateDefaultData();

	namespace
	{
		static const String gDATA_DIRECTORY = U"Assets/Stage/data_";	//!< データのパス

		std::shared_ptr<Desc> gpDesc{ nullptr };	//!< 所持しているデータ
	}

	bool Load(const WorldIndex worldIndex, const StageIndex stageIndex) noexcept
	{
		String path = gDATA_DIRECTORY;

		path += U"{:2d}"_fmt(worldIndex);
		path += U"_";
		path += U"{:2d}"_fmt(stageIndex);
		path += U".csv";

		CSV reader{ path };
		if (not reader) {
			CreateDefaultData();
			return false;
		}

		gpDesc.reset(new Desc);

		gpDesc->stageName = reader[0][0];

		size_t index = 2;
		for (size_t i = 0, max = Parse<size_t>(reader[1][0]); i < max; ++i)
		{
			auto& pieceData = gpDesc->aPieceDescs.emplace_back();

			pieceData.position.x = Parse<double>(reader[index][0]);
			pieceData.position.y = Parse<double>(reader[index][1]);
			pieceData.scale = Parse<double>(reader[index][2]);
			pieceData.angle = Parse<double>(reader[index][3]);
			pieceData.backScale = Parse<Vec2>(reader[index][4]);

			pieceData.canMoving = Parse<bool>(reader[index + 1][0]);
			pieceData.canScaling = Parse<bool>(reader[index + 1][1]);
			pieceData.canRolling = Parse<bool>(reader[index + 1][2]);
			pieceData.backType = Parse<StagePieceData::TypeIndex>(reader[index + 1][3]);

			index += 3;
			for (size_t t = 0, tMax = Parse<size_t>(reader[index - 1][0]); t < tMax; ++t)
			{
				auto& tileData = pieceData.aTileDescs.emplace_back();

				tileData.position.x = Parse<double>(reader[index][0]);
				tileData.position.y = Parse<double>(reader[index][1]);
				tileData.angle = Parse<double>(reader[index][2]);
				tileData.tileType = Parse<TileData::TypeIndex>(reader[index][3]);
				tileData.uv.x = Parse<double>(reader[index][4]);
				tileData.uv.y = Parse<double>(reader[index][5]);

				index++;
			}

			index += 1;
			for (size_t g = 0, gMax = Parse<size_t>(reader[index - 1][0]); g < gMax; ++g)
			{
				auto& gimmickData = pieceData.aGimmiclDesc.emplace_back();

				gimmickData.position.x = Parse<double>(reader[index][0]);
				gimmickData.position.y = Parse<double>(reader[index][1]);
				gimmickData.angle = Parse<double>(reader[index][2]);
				gimmickData.gimmickType = Parse<GimmickData::TypeIndex>(reader[index][3]);

				index++;
			}
		}

		return true;
	}

	bool Save(const WorldIndex worldIndex, const StageIndex stageIndex) noexcept
	{
		String path = gDATA_DIRECTORY;

		path += U"{:2d}"_fmt(worldIndex);
		path += U"_";
		path += U"{:2d}"_fmt(stageIndex);
		path += U".csv";

		return false;
	}

	std::shared_ptr<Desc> GetCurrentData()
	{
		// オブジェクト生成に使用することが考慮されるためデータがない場合大変危険である
		if (not gpDesc)
		{
			CreateDefaultData();
		}

		return gpDesc;
	}

	void CreateDefaultData()
	{
		gpDesc.reset(new Desc);

		gpDesc->stageName = U"デフォルト";

		gpDesc->aPieceDescs.resize(5);
		auto tileSize = Vec2{ TileData::gTILE_SIZE, TileData::gTILE_SIZE };

		{
			auto& rPieceDesc = gpDesc->aPieceDescs[0];

			rPieceDesc.backType = static_cast<StagePieceData::TypeIndex>(StagePieceData::Type::Normal);
			rPieceDesc.backScale = tileSize * 10.0;

			rPieceDesc.canMoving = false;
			rPieceDesc.canScaling = false;
			rPieceDesc.canRolling = false;

			rPieceDesc.position.y = 5.0 * tileSize.y;

			auto leftTop = -rPieceDesc.backScale / 2.0;

			for (size_t i = 0; i < 10; ++i)
			{
				auto& tile = rPieceDesc.aTileDescs.emplace_back();
				tile.uv.x = 8;
				tile.position.x = leftTop.x + tileSize.x * static_cast<double>(i);
				tile.position.y = leftTop.y + tileSize.y * 9.0;
			}

			for (size_t i = 6; i < 10; i++)
			{
				auto& tile = rPieceDesc.aTileDescs.emplace_back();
				tile.uv.x = 8;
				tile.position.x = leftTop.x + tileSize.x * 5.0;
				tile.position.y = leftTop.y + tileSize.y * static_cast<double>(i);
			}

			{
				auto& tile = rPieceDesc.aTileDescs.emplace_back();
				tile.tileType = static_cast<TileData::TypeIndex>(TileData::Type::Triangle);
				tile.uv.x = 8;
				tile.uv.y = 1;
				tile.position = Vec2{ leftTop.x + tileSize.x * 4.0, leftTop.y + tileSize.y * 8.0 };
			}

			{
				auto& gimmick = rPieceDesc.aGimmiclDesc.emplace_back();
				gimmick.gimmickType = static_cast<GimmickData::TypeIndex>(GimmickData::Type::Player);
				gimmick.position = Vec2{ leftTop.x + tileSize.x * 1, leftTop.y + tileSize.y * 8 };
			}

			{
				auto& gimmick = rPieceDesc.aGimmiclDesc.emplace_back();
				gimmick.gimmickType = static_cast<GimmickData::TypeIndex>(GimmickData::Type::Goal);
				gimmick.position = Vec2{ leftTop.x + tileSize.x * 8, leftTop.y + tileSize.y * 8 };
			}
		}

		{
			auto& rPieceDesc = gpDesc->aPieceDescs[1];

			rPieceDesc.backType = static_cast<StagePieceData::TypeIndex>(StagePieceData::Type::Normal);
			rPieceDesc.backScale.x = tileSize.x * 8.0;
			rPieceDesc.backScale.y = tileSize.y * 6.0;

			rPieceDesc.canMoving = false;
			rPieceDesc.canScaling = false;
			rPieceDesc.canRolling = false;

			rPieceDesc.position.x = -3.5 * tileSize.x;
			rPieceDesc.position.y = -3.0 * tileSize.y;
			rPieceDesc.scale = 1.0;
			rPieceDesc.angle = 0.0;

			auto leftTop = -rPieceDesc.backScale / 2.0;
			for (size_t x = 0; x < 8.0; x++)
			{
				for (size_t y = 0; y < 6; y++)
				{
					auto& tile = rPieceDesc.aTileDescs.emplace_back();
					tile.uv.x = static_cast<int32>(x);
					tile.uv.y = static_cast<int32>(y);
					tile.position = Vec2{ leftTop.x + tileSize.x * static_cast<double>(x), leftTop.y + tileSize.y * static_cast<double>(y) };
				}
			}
		}

		{
			auto& rPieceDesc = gpDesc->aPieceDescs[2];

			rPieceDesc.backType = static_cast<StagePieceData::TypeIndex>(StagePieceData::Type::Normal);
			rPieceDesc.backScale.x = tileSize.x * 7.0;
			rPieceDesc.backScale.y = tileSize.y * 6.0;

			rPieceDesc.canMoving = false;
			rPieceDesc.canScaling = false;
			rPieceDesc.canRolling = false;

			rPieceDesc.position.x = 4.0 * tileSize.x;
			rPieceDesc.position.y = -3.0 * tileSize.y;
			rPieceDesc.scale = 1.0;
			rPieceDesc.angle = 0.0;

			auto leftTop = -rPieceDesc.backScale / 2.0;
			for (size_t x = 0; x < 7; x++)
			{
				for (size_t y = 0; y < 6; y++)
				{
					auto& tile = rPieceDesc.aTileDescs.emplace_back();
					tile.uv.x = static_cast<int32>(x + 9);
					tile.uv.y = static_cast<int32>(y);
					tile.position = Vec2{ leftTop.x + tileSize.x * static_cast<double>(x), leftTop.y + tileSize.y * static_cast<double>(y) };
				}
			}
		}

		{
			auto& rPieceDesc = gpDesc->aPieceDescs[3];

			rPieceDesc.backType = static_cast<StagePieceData::TypeIndex>(StagePieceData::Type::Gravity);
			rPieceDesc.backScale.x = tileSize.x * 6.0;
			rPieceDesc.backScale.y = tileSize.y * 5.0;

			rPieceDesc.canMoving = true;
			rPieceDesc.canScaling = true;
			rPieceDesc.canRolling = true;

			rPieceDesc.position.x = -8.0 * tileSize.x;
			rPieceDesc.position.y = 5.0 * tileSize.y;
			rPieceDesc.scale = 1.0;
			rPieceDesc.angle = 0.0;

			auto leftTop = -rPieceDesc.backScale / 2.0;
			for (size_t x = 0; x < 6; x++)
			{
				auto& tile = rPieceDesc.aTileDescs.emplace_back();
				tile.uv.x = static_cast<int32>(8);
				tile.position = Vec2{ leftTop.x + tileSize.x * static_cast<double>(x), leftTop.y + tileSize.y * 2.0 };
			}
		}

		{
			auto& rPieceDesc = gpDesc->aPieceDescs[4];

			rPieceDesc.backType = static_cast<StagePieceData::TypeIndex>(StagePieceData::Type::Void);
			rPieceDesc.backScale.x = tileSize.x * 6.0;
			rPieceDesc.backScale.y = tileSize.y * 5.0;

			rPieceDesc.canMoving = true;
			rPieceDesc.canScaling = true;
			rPieceDesc.canRolling = true;

			rPieceDesc.position.x = 8.0 * tileSize.x;
			rPieceDesc.position.y = 5.0 * tileSize.y;
			rPieceDesc.scale = 1.0;
			rPieceDesc.angle = 0.0;

			auto leftTop = -rPieceDesc.backScale / 2.0;
			for (size_t x = 0; x < 6; x++)
			{
				auto& tile = rPieceDesc.aTileDescs.emplace_back();
				tile.uv.x = static_cast<int32>(8);
				tile.position = Vec2{ leftTop.x + tileSize.x * static_cast<double>(x), leftTop.y + tileSize.y * 2.0 };
			}
		}
	}
}

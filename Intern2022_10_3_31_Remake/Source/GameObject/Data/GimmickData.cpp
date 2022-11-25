/** ****************************************************************************************************
 * @file
 * @brief ギミックのデータ操作関連
 **************************************************************************************************** */
#include "GimmickData.h"

namespace MySystem::GimmickData
{
	Gimmick::Gimmick()
	{
		TextureAsset::Register(gLABEL_TEXTURE[static_cast<TypeIndex>(Type::Player)], U"Assets/Image/siv3d-kun-16.png");
		TextureAsset::Register(gLABEL_TEXTURE[static_cast<TypeIndex>(Type::Goal)], U"🚩"_emoji);
		TextureAsset::Register(gLABEL_TEXTURE[static_cast<TypeIndex>(Type::Max)], U"✖"_emoji);
	}

	Gimmick::~Gimmick()
	{

	}

	void Gimmick::Update()
	{

	}

	void Gimmick::Draw() const
	{

	}
}

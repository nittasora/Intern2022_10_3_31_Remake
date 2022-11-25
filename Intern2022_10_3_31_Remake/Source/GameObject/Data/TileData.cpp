/** ****************************************************************************************************
 * @file
 * @brief タイルのデータ操作関連
 **************************************************************************************************** */
#include "TileData.h"

namespace MySystem::TileData
{
	void GetSimplePolygon(Array<Polygon>& targets, double nearPointDistance, double nearLineDistance)
	{
		for (auto itr = targets.begin(); itr != targets.end(); ++itr)
		{
			auto& outer = itr->outer();
			auto& inners = itr->inners();
			Array<Vec2> newOuter;
			Array<Array<Vec2>> newInners;

			newOuter = GetSimpleVertex(outer, nearPointDistance, nearLineDistance);

			for (auto& inner : inners)
			{
				newInners << GetSimpleVertex(inner, nearPointDistance, nearLineDistance);
			}

			if (newOuter.size() < 1)
			{
				itr = targets.erase(itr);

				if (itr == targets.end())
				{
					break;
				}

				continue;
			}

			(*itr) = Polygon{newOuter};

			if (newInners.size() < 1)
			{
				continue;
			}

			itr->addHoles(newInners);
		}
	}

	Array<Vec2> GetSimpleVertex(Array<Vec2> targets, double nearPointDistance, double nearLineDistance)
	{
		for (int i = 0; i < targets.size(); ++i)
		{
			int max = static_cast<int>(targets.size());
			auto prev = (i + max - 1) % max;
			auto next = (i + 1) % max;

			double disX = targets[i].x - targets[next].x;
			double disY = targets[i].y - targets[next].y;

			if (disX > -nearPointDistance && nearPointDistance > disX && disY > -nearPointDistance && nearPointDistance > disY) {
				targets.erase(targets.begin() + i);
				i--;
				continue;
			}

			Vec2 ap = targets[i] - targets[prev];
			Vec2 ab = targets[next] - targets[prev];
			Vec2 ba = targets[prev] - targets[next];
			Vec2 bp = targets[i] - targets[next];
			Vec2 neighbor_point;
			double distance;

			if (ap.dot(ab) < 0)
			{
				distance = ap.length();
				neighbor_point = targets[prev];
			}
			else if (bp.dot(ba) < 0)
			{
				distance = bp.length();
				neighbor_point = targets[next];
			}
			else
			{
				auto ai_norm = ap.dot(ab) / ab.length();
				neighbor_point = targets[prev] + ab / ab.length() * ai_norm;
				distance = (targets[i] - neighbor_point).length();
			}

			if (distance < nearLineDistance && -nearLineDistance < distance)
			{
				targets.erase(targets.begin() + i);
				i--;
				continue;
			}
		}

		return targets;
	}
}

/** ****************************************************************************************************
 * @file
 * @brief 数学系
 **************************************************************************************************** */
#pragma once

#include <Siv3D.hpp>

/** ****************************************************************************************************
 * @brief 数学系
 **************************************************************************************************** */
namespace MySystem::MyMath
{
	/** ****************************************************************************************************
	 * @brief			法線計算
	 * @param[in] vec	法線を出したいベクトル
	 * @return			法線
	 **************************************************************************************************** */
	inline Vec2 CalcuNormal(const Vec2& vec)
	{
		return Vec2{ -vec.y, vec.x }.normalized();
	}

	/** ****************************************************************************************************
	 * @brief				壁ずりベクトル計算
	 * @param[in] front		入射ベクトル
	 * @param[in] notmal	壁の法線
	 * @return				壁ずりベクトル
	 **************************************************************************************************** */
	inline Vec2 CalcuWallScratchVector(const Vec2& front, const Vec2& normal) {
		Vec2 normal_n = normal.normalized();

		return (front - front.dot(normal_n) * normal_n);
	}

	/** ****************************************************************************************************
	 * @brief				反射ベクトル計算
	 * @param[in] front		入射ベクトル
	 * @param[in] notmal	壁の法線
	 * @return				反射ベクトル
	 **************************************************************************************************** */
	inline Vec2 CalcuReflectVector(const Vec2& front, const Vec2& normal) {
		Vec2 normal_n = normal.normalized();

		return (front - 2.0f * front.dot(normal_n) * normal_n).normalized();
	}

	/** ****************************************************************************************************
	 * @brief					線分と線分の当たり判定
	 * @param[in] segment1Start	線分1の始点
	 * @param[in] segment1End	線分1の終点
	 * @param[in] segment2Start	線分2の始点
	 * @param[in] segment2End	線分2の終点
	 * @param[out] t			当たった場合の割合
	 * @param[out] fixHitPos	当たった位置に修正された終点
	 * @retval true				当たった
	 * @retval false			当たっていない
	 **************************************************************************************************** */
	inline bool HitCheckSegmentToSegment(
		const Vec2& segment1Start, const Vec2& segment1End,
		const Vec2& segment2Start, const Vec2& segment2End,
		double& t,
		Vec2& fixHitPos
	)
	{
		Vec2 v = segment2Start - segment1Start;
		Vec2 dir1 = segment1End - segment1Start;
		Vec2 dir2 = segment2End - segment2Start;
		double Crs_v1_v2 = dir1.cross(dir2);
		if (Crs_v1_v2 == 0.0f) {
			// 平行状態
			return false;
		}

		double Crs_v_v1 = v.cross(dir1);
		double Crs_v_v2 = v.cross(dir2);

		double t1 = Crs_v_v2 / Crs_v1_v2;
		double t2 = Crs_v_v1 / Crs_v1_v2;

		const double eps = 0.001;
		if (t1 + eps < 0 || t1 - eps > 1 || t2 + eps < 0 || t2 - eps > 1) {
			// 交差していない
			return false;
		}

		t = t1;

		fixHitPos = (segment1End - segment1Start) * t1 + segment1Start;

		return true;
	}

	/** ****************************************************************************************************
	 * @brief						線分と点の最短距離算出
	 * @param[in] segmentStart		線分の始点
	 * @param[in] segmentEnd		線分の終点
	 * @param[in] point				点
	 * @param[out] distance			最短距離
	 * @param[out] neighborPoint	最近点
	 **************************************************************************************************** */
	inline void CalcuDistancePointToSegment(
		const Vec2& segmentStart,
		const Vec2& segmentEnd,
		const Vec2& point,
		double& distance,
		Vec2& neighborPoint
	) noexcept
	{
		Vec2 ap = point - segmentStart;
		Vec2 ab = segmentEnd - segmentStart;
		Vec2 ba = segmentStart - segmentEnd;
		Vec2 bp = point - segmentEnd;

		if (ap.dot(ab) < 0)
		{
			distance = ap.length();
			neighborPoint = segmentStart;
		}
		else if (bp.dot(ba) < 0)
		{
			distance = bp.length();
			neighborPoint = segmentEnd;
		}
		else
		{
			auto ai_norm = ap.dot(ab) / ab.length();
			neighborPoint = segmentStart + ab / ab.length() * ai_norm;
			distance = (point - neighborPoint).length();
		}
	}
}

/** ****************************************************************************************************
 * @file
 * @brief ビットフラグ操作関連
 **************************************************************************************************** */
#pragma once

/** ****************************************************************************************************
 * @brief ビットフラグ操作関連関数
 **************************************************************************************************** */
namespace MySystem::BitFlag
{
	/** ****************************************************************************************************
	 * @brief			ビットフラグを生成
	 * @tparam T		ビットフラグに使う型
	 * @param[in] bit	立てるビット位置
	 * @return			ビットフラグ
	 **************************************************************************************************** */
	template<typename T> [[nodiscard]]
		constexpr inline T Get(const T bit) noexcept
	{
		return 1 << bit;
	}

	/** ****************************************************************************************************
	 * @brief			ビットフラグを合成
	 * @tparam First	ビットフラグに使う型
	 * @param[in] first	ビットフラグ
	 * @return			引数をそのまま返す
	 **************************************************************************************************** */
	template<typename First> [[nodiscard]]
		constexpr inline First GetMask(const First first) noexcept
	{
		return first;
	}

	/** ****************************************************************************************************
	 * @brief				ビットフラグを合成
	 * @tparam First, Rest	ビットフラグに使う型
	 * @param[in] first		ビットフラグ
	 * @param[in] rest		合成するビットフラグ
	 * @return				合成したビットフラグ
	 **************************************************************************************************** */
	template<typename First, typename... Rest> [[nodiscard]]
		constexpr inline First GetMask(const First first, const Rest... rest) noexcept
	{
		return first | GetMask(rest...);
	}

	/** ****************************************************************************************************
	 * @brief			ビット位置からビットフラグを合成
	 * @tparam First	ビットフラグに使う型
	 * @param[in] first	立てるビット位置
	 * @return			引数部を立てたビットを返す
	 **************************************************************************************************** */
	template<typename First> [[nodiscard]]
		constexpr inline First GetMaskByNumber(const First first) noexcept
	{
		return MyDef_BitFlag(first);
	}

	/** ****************************************************************************************************
	 * @brief				ビット位置からビットフラグを合成
	 * @tparam First, Rest	ビットフラグに使う型
	 * @param[in] first		立てるビット位置
	 * @param[in] rest		合成する立てるビット位置
	 * @return				合成したビットフラグ
	 **************************************************************************************************** */
	template<typename First, typename... Rest> [[nodiscard]]
		constexpr inline First GetMaskByNumber(const First first, const Rest... rest) noexcept
	{
		return Get(first) | GetMaskByNumber(rest...);
	}

	/** ****************************************************************************************************
	 * @brief			指定したマスクビットのうちどこか一つ以上が立っているか
	 * @tparam T		ビットフラグに使う型
	 * @param[in] flag	調べる対象フラグ
	 * @param[in] mask	調べる位置を示すマスク
	 * @retval true		一つ以上立っている
	 * @retval false	全て立っていない
	 **************************************************************************************************** */
	template<typename T> [[nodiscard]]
		constexpr inline bool CheckAnyPop(const T& flag, const T& mask) noexcept
	{
		return flag & mask;
	}

	/** ****************************************************************************************************
	 * @brief			指定したマスクビットが全て立っているか
	 * @tparam T		ビットフラグに使う型
	 * @param[in] flag	調べる対象フラグ
	 * @param[in] mask	調べる位置を示すマスク
	 * @retval true		全て立っている
	 * @retval false	どこかが一つ以上立っていない
	 **************************************************************************************************** */
	template<typename T> [[nodiscard]]
		constexpr inline bool CheckAllPop(const T& flag, const T& mask) noexcept
	{
		return (flag & mask) == mask;
	}

	/** ****************************************************************************************************
	 * @brief			指定したマスクビットのうちどこかが一つ以上立っていないか
	 * @tparam T		ビットフラグに使う型
	 * @param[in] flag	調べる対象フラグ
	 * @param[in] mask	調べる位置を示すマスク
	 * @retval true		どこかが一つ以上立っていない
	 * @retval false	全て立っている
	 **************************************************************************************************** */
	template<typename T> [[nodiscard]]
		constexpr inline bool CheckAnyUnPop(const T& flag, const T& mask) noexcept
	{
		return !CheckAllPop(flag, mask);
	}

	/** ****************************************************************************************************
	 * @brief			指定したマスクビットが全て立っていないか
	 * @tparam T		ビットフラグに使う型
	 * @param[in] flag	調べる対象フラグ
	 * @param[in] mask	調べる位置を示すマスク
	 * @retval true		全て立っていない
	 * @retval false	どこかが一つ以上立っている
	 **************************************************************************************************** */
	template<typename T> [[nodiscard]]
		constexpr inline bool CheckAllUnPop(const T& flag, const T& mask) noexcept
	{
		return !CheckAnyPop(flag, mask);
	}

	/** ****************************************************************************************************
	 * @brief				フラグを立てる
	 * @tparam T			ビットフラグに使う型
	 * @param[out] rFlag	対象フラグ
	 * @param[in] mask		立てる位置を示すマスク
	 * @return				mask部を立てたflag
	 **************************************************************************************************** */
	template<typename T>
	constexpr inline T Pop(T& rFlag, const T& mask) noexcept
	{
		rFlag |= mask;

		return rFlag;
	}

	/** ****************************************************************************************************
	 * @brief			フラグを立てたものを取得
	 * @tparam T		ビットフラグに使う型
	 * @param[in] flag	対象フラグ
	 * @param[in] mask	立てる位置を示すマスク
	 * @return			mask部を立てたflag
	 **************************************************************************************************** */
	template<typename T> [[nodiscard]]
		constexpr inline T GetPop(const T& flag, const T& mask) noexcept
	{
		return flag | mask;
	}

	/** ****************************************************************************************************
	 * @brief				フラグを折る
	 * @tparam T			ビットフラグに使う型
	 * @param[out] rFlag	対象フラグ
	 * @param[in] mask		立てる位置を示すマスク
	 * @return				mask部を折ったflag
	 **************************************************************************************************** */
	template<typename T>
	constexpr inline T UnPop(T& rFlag, const T& mask) noexcept
	{
		rFlag &= ~mask;

		return rFlag;
	}

	/** ****************************************************************************************************
	 * @brief			フラグを折ったものを取得
	 * @tparam T		ビットフラグに使う型
	 * @param[in] flag	対象フラグ
	 * @param[in] mask	立てる位置を示すマスク
	 * @return			mask部を折ったflag
	 **************************************************************************************************** */
	template<typename T> [[nodiscard]]
		constexpr inline T GetUnPop(const T& flag, const T& mask) noexcept
	{
		return flag & ~mask;
	}

	/** ****************************************************************************************************
	 * @brief				boolからフラグを立てるか折る
	 * @tparam T			ビットフラグに使う型
	 * @param[out] rFlag	対象フラグ
	 * @param[in] mask		立て折りする位置を示すマスク
	 * @param[in] pop		true 立てる false 折る
	 * @return				mask部を立て折りしたflag
	 **************************************************************************************************** */
	template<typename T>
	constexpr inline T PopOrUnPop(T& rFlag, const T& mask, const bool pop) noexcept
	{
		pop ? rFlag |= mask : rFlag &= ~mask;

		return rFlag;
	}

	/** ****************************************************************************************************
	 * @brief				boolからフラグを立てたか折ったものを取得
	 * @tparam T			ビットフラグに使う型
	 * @param[out] rFlag	対象フラグ
	 * @param[in] mask		立て折りする位置を示すマスク
	 * @param[in] pop		true 立てる false 折る
	 * @return				mask部を立て折りしたflag
	 **************************************************************************************************** */
	template<typename T>
	constexpr inline T GetPopOrUnPop(const T& flag, const T& mask, const bool pop) noexcept
	{
		T result;
		pop ? result = flag | mask : result = flag & ~mask;
		return result;
	}

	/** ****************************************************************************************************
	 * @brief			フラグからマスク部を抜き出したものを取得
	 * @tparam T		ビットフラグに使う型
	 * @param[in] flag	対象フラグ
	 * @param[in] mask	抜き出す位置を示すマスク
	 * @return			flagからmask部を抜き出したもの
	 **************************************************************************************************** */
	template<typename T> [[nodiscard]]
		constexpr inline T Sampling(const T& flag, const T& mask) noexcept
	{
		return flag & mask;
	}
}


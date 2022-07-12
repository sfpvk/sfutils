#pragma once
#include <string>
#include <cstring>
#include <type_traits>
#include <bit>
#include <cassert>
#include <array>
#include <algorithm>
#include <ranges>
#include <concepts>
#include <iconv.h>


namespace sfpvk::utils::unicode {


struct Cvt_endian {
	using cvt_endian_t = unsigned;
	enum enum_e : cvt_endian_t {
		from_native = 0b0,
		from_big    = 0b1,
		from_little = 0b10,
		to_native   = 0b0,
		to_big      = 0b100,
		to_little   = 0b1000
	};
};


namespace details {

	using convdesc_t = iconv_t;

	template <typename T>
	concept CharType = (std::same_as<T, char>
			||  std::same_as<T, char8_t>
			||  std::same_as<T, char16_t>
			||  std::same_as<T, char32_t>
			||  std::same_as<T, wchar_t>);

	template <Cvt_endian::cvt_endian_t endian, CharType From, CharType To>
	convdesc_t create_convdescriptor()
	{
		convdesc_t cd;
		char tocode[8];
		char fromcode[8];
#define SFPVK_SETCODE(dest, T, char_type, encoding)\
		if constexpr (std::is_same_v<T, char_type>) {\
			static_assert(sizeof(dest) >= sizeof(#encoding));\
			std::strcpy(dest, #encoding);\
		}
		SFPVK_SETCODE(fromcode, From, char,    UTF8);
		SFPVK_SETCODE(fromcode, From, char8_t, UTF8);
		SFPVK_SETCODE(fromcode, From, wchar_t, WCHAR_T);
		SFPVK_SETCODE(tocode, To, char,    UTF8);
		SFPVK_SETCODE(tocode, To, char8_t, UTF8);
		SFPVK_SETCODE(tocode, To, wchar_t, WCHAR_T);
		if constexpr (endian & Cvt_endian::from_little) {
			SFPVK_SETCODE(fromcode, From, char16_t, UTF16LE);
			SFPVK_SETCODE(fromcode, From, char32_t, UTF32LE);
		}
		else {
			SFPVK_SETCODE(fromcode, From, char16_t, UTF16BE);
			SFPVK_SETCODE(fromcode, From, char32_t, UTF32BE);
		}
		if constexpr (endian & Cvt_endian::to_little) {
			SFPVK_SETCODE(tocode, To, char16_t, UTF16LE);
			SFPVK_SETCODE(tocode, To, char32_t, UTF32LE);
		}
		else {
			SFPVK_SETCODE(tocode, To, char16_t, UTF16BE);
			SFPVK_SETCODE(tocode, To, char32_t, UTF32BE);
		}
#undef SFPVK_SETCODE
		cd = iconv_open(tocode, fromcode);
		assert(cd != convdesc_t(-1));
		return cd;
	}

	template<Cvt_endian::cvt_endian_t endian, CharType From, CharType To>
	consteval Cvt_endian::cvt_endian_t
	adjust_endian_mode()
	{
		static_assert(std::is_same_v<From, wchar_t>
				?  ((Cvt_endian::from_big|Cvt_endian::from_little) & endian)
					== 0
				: true);
		static_assert(std::is_same_v<To, wchar_t>
				?  ((Cvt_endian::to_big|Cvt_endian::to_little) & endian)
					== 0
				: true);
		static_assert((endian & Cvt_endian::from_big
					?  ! (endian & Cvt_endian::from_little)
					:  true)
				&&  (endian & Cvt_endian::to_big
					?  ! (endian & Cvt_endian::to_little)
					:  true));

		Cvt_endian::cvt_endian_t res = endian;
		if (! std::is_same_v<From, char16_t>
				&&  ! std::is_same_v<From, char32_t>)
			res = (res & ~(Cvt_endian::from_little | Cvt_endian::from_big))
				|  Cvt_endian::from_little;
		if (! std::is_same_v<To, char16_t>  &&  ! std::is_same_v<To, char32_t>)
			res = (res & ~(Cvt_endian::to_little | Cvt_endian::to_big))
				|  Cvt_endian::to_little;

		if (((Cvt_endian::from_big | Cvt_endian::from_little) & res) == 0) {
			if (std::endian::native == std::endian::little)
				res |= Cvt_endian::from_little;
			else
				res |= Cvt_endian::from_big;
		}
		if (((Cvt_endian::to_big | Cvt_endian::to_little) & res) == 0) {
			if (std::endian::native == std::endian::little)
				res |= Cvt_endian::to_little;
			else
				res |= Cvt_endian::to_big;
		}

		return res;
	}

	template <Cvt_endian::cvt_endian_t endian=
			Cvt_endian::from_native|Cvt_endian::to_native,
		CharType From, CharType To>
	ssize_t g_cvt_base(const From *beg, ssize_t sz,
			std::basic_string<To> *o_out, ssize_t max_out_cnt=-1)
	{
		if (max_out_cnt == 0)
			return 0;

		class Convdesc{
		public:
			convdesc_t cd;
			~Convdesc() {
				if (cd != convdesc_t(-1))
					iconv_close(cd);
			}
		} static thread_local const cdesc{
			create_convdescriptor<endian, From, To>()};
		if (cdesc.cd == convdesc_t(-1))
			return -1;

		struct Ibuf {
			std::array<From, 500> buf;
			char *ptr;
			ssize_t pos = 0;
			std::size_t left = 0;
			ssize_t read_cnt;
		} ibuf;
		auto ibuf_ctr = [&]() {
			if (max_out_cnt != -1) {
				int k = sizeof(To) / sizeof(From);
				if (! k)
					k = 1;
				ibuf.read_cnt = max_out_cnt * k;
				if (ibuf.read_cnt < 4)
					ibuf.read_cnt = 4;
				else if (ibuf.read_cnt > std::ssize(ibuf.buf))
					ibuf.read_cnt = std::ssize(ibuf.buf);
			}
			else
				ibuf.read_cnt = std::ssize(ibuf.buf);
		};
		auto ibuf_unget = [&]() {
			ibuf.pos -= ibuf.left / sizeof(From);
		};
		auto ibuf_fill = [&]() -> bool {
			assert(ibuf.left % sizeof(From) == 0);
			if (ibuf.pos == sz)
				return false;
			ibuf_unget();
			ibuf.left = ibuf.read_cnt * sizeof(From);
			std::size_t total_sz = (sz - ibuf.pos) * sizeof(From);
			if (total_sz < ibuf.left)
				ibuf.left = total_sz;
			std::size_t left_in_chars = ibuf.left / sizeof(From);
			std::ranges::copy(beg+ibuf.pos, beg+ibuf.pos+left_in_chars,
					ibuf.buf.data());
			ibuf.pos += left_in_chars;
			// Conversion to char pointer is allowed,
			// it doesn't violate strict aliasing rules.
			ibuf.ptr = reinterpret_cast<char *>(ibuf.buf.data());
			return true;
		};
		ibuf_ctr();
		if (! ibuf_fill())
			return 0;

		struct Obuf {
			std::array<To, 500> buf;
			char *ptr = nullptr;
			std::size_t left = 0;
			ssize_t sz = 0;
		} obuf;
		auto obuf_flush = [&]() -> bool {
			assert(obuf.left % sizeof(To) == 0);
			if (obuf.ptr) {
				o_out->append(obuf.buf.data(),
						obuf.sz-obuf.left/sizeof(To)+obuf.buf.data());
			}
			obuf.sz = std::ssize(obuf.buf);
			if (max_out_cnt != -1) {
				max_out_cnt += obuf.left / sizeof(To);
				if (max_out_cnt < obuf.sz)
					obuf.sz = max_out_cnt;
				max_out_cnt -= obuf.sz;
			}
			bool ret = obuf.sz
				&&  obuf.ptr != reinterpret_cast<char *>(obuf.buf.data());
			obuf.ptr = reinterpret_cast<char *>(obuf.buf.data());
			obuf.left = obuf.sz * sizeof(To);
			return ret;
		};
		obuf_flush();

		// Unicode dose not have shift state, comment it out
		//auto write_shift_sequence = [&]() {
		//	std::size_t ileft = 0;
		//	if (iconv(cdesc.cd, nullptr, &ileft, &obuf.ptr, &obuf.left)
		//			== std::size_t(-1))
		//		return false;
		//	return true;
		//};
		//auto set_initial_state = [&]() {
		//	std::size_t ileft = 0, oleft = 0;
		//	iconv(cdesc.cd, nullptr, &ileft, nullptr, &oleft);
		//};
		//set_initial_state();

		while (true) {
			if (iconv(cdesc.cd, &ibuf.ptr, &ibuf.left, &obuf.ptr, &obuf.left)
					== std::size_t(-1)) {
				switch (errno) {
				case EILSEQ:
					return -1;
				case EINVAL:
					if (! ibuf_fill())
						return -1;
					break;
				case E2BIG:
					if (! obuf_flush())
						goto while_end;
				}
			}
			else
				if (! ibuf_fill())
					break;
		}
while_end:

		// Unicode dose not have shift state, comment it out
		//if (! write_shift_sequence()) {
		//	obuf_flush();
		//	write_shift_sequence();
		//}

		obuf_flush();
		ibuf_unget();
		return ibuf.pos;
	}


} //details
 
/** 
 * Converts a string in one UTF encoding pointed to by "in" to a string
 * in another UTF encoding pointed to by "o_out".
 *
 * @param o_out pointer to the output string. clear() is not called.
 * @param max_out_cnt the maximum number of characters that can be added
 * to the output string, or -1 for no limit.
 *
 * #return returns the number of converted input string characters,
 * or -1 if an error occurs, in which case the contents of the output
 * string is unspecified.
 */
template <Cvt_endian::cvt_endian_t endian=
		Cvt_endian::from_native|Cvt_endian::to_native,
	typename Range, details::CharType To>
requires (! details::CharType<Range>)
ssize_t g_cvt(const Range &in, std::basic_string<To> *o_out,
		ssize_t max_out_cnt=-1)
{
	using from_t = std::ranges::range_value_t<Range>;
	std::basic_string_view<from_t> sv{in};
	return details::g_cvt_base<
			details::adjust_endian_mode<endian, from_t, To>(), from_t, To>(
			sv.data(), sv.size(), o_out, max_out_cnt);
}

/**
 *  Converts the single "in" character to a string pointed to by "o_out".
 */
template <Cvt_endian::cvt_endian_t endian=
		Cvt_endian::from_native|Cvt_endian::to_native,
	details::CharType CharT, details::CharType To>
ssize_t g_cvt(CharT in, std::basic_string<To> *o_out)
{
	static thread_local std::basic_string<CharT> ch;
	ch = in;
	return g_cvt<endian>(ch, o_out, -1);
}


} //sfpvk::utils::unicode

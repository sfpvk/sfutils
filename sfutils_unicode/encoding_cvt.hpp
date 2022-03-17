#pragma once
#include <boost/locale.hpp>
#include <string>
#include <locale>


namespace sfpvk::utils::unicode {


template <typename CharT>
void g_cvt_to_wide(const CharT *in_beg,
		const CharT *in_end,
		std::wstring *o_out,
		const std::locale &)
{
	*o_out = boost::locale::conv::utf_to_utf<wchar_t, CharT>(in_beg, in_end); 
}

template <>
inline void g_cvt_to_wide(const char *in_beg,
		const char *in_end,
		std::wstring *o_out,
		const std::locale &loc)
{
	*o_out = boost::locale::conv::to_utf<wchar_t>(in_beg, in_end, loc); 
}

template <>
inline void g_cvt_to_wide(const wchar_t *in_beg,
		const wchar_t *in_end,
		std::wstring *o_out,
		const std::locale &)
{
	o_out->assign(in_beg, in_end);
}

template <typename CharT>
void g_cvt_to_wide(const std::basic_string<CharT> in,
		std::wstring *o_out,
		const std::locale &loc)
{
	g_cvt_to_wide(&in[0], &in[in.size()], o_out, loc);
}

//----------------------------------------------------------------------------+
//                                                                            |
//----------------------------------------------------------------------------+
template <typename CharT>
void g_cvt_from_wide(const wchar_t *in_begin,
		const wchar_t *in_end,
		std::basic_string<CharT> *o_out,
		const std::locale &)
{
	*o_out = boost::locale::conv::utf_to_utf<CharT, wchar_t>(in_begin,
			in_end); 
}

template <>
inline void g_cvt_from_wide(const wchar_t *in_begin,
		const wchar_t *in_end,
		std::wstring *o_out,
		const std::locale &)
{
	o_out->assign(in_begin, in_end);
}

template <>
inline void g_cvt_from_wide(const wchar_t *in_begin,
		const wchar_t *in_end,
		std::string *o_out,
		const std::locale &loc)
{
	*o_out = boost::locale::conv::from_utf(in_begin, in_end, loc);
}

template <typename CharT>
void g_cvt_from_wide(const std::wstring &in,
		std::basic_string<CharT> *o_out,
		const std::locale &loc)
{
	g_cvt_from_wide(&in[0], &in[in.size()], o_out, loc);
}


} //sfpvk::utils::unicode

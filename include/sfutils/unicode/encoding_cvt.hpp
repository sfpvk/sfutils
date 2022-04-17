#pragma once
#include <string>
#include <iterator>
#include <cpp-unicodelib/unicodelib_encodings.h>
#include "ustring_iterator.hpp"


namespace sfpvk::utils::unicode {


inline void g_cvt_to_utf32(char32_t in, std::u32string *o_out)
{
	o_out->push_back(in);
}

inline void g_cvt_to_utf32(std::basic_string_view<char> in,
		std::u32string *o_out)
{
	::unicode::utf8::decode(in.data(), in.size(), *o_out); 
}

inline void g_cvt_to_utf32(std::basic_string_view<char8_t> in,
		std::u32string *o_out)
{
	// Conversion to char pointer is allowed,
	// it doesn't violate strict aliasing rules.
	::unicode::utf8::decode(reinterpret_cast<const char*>(in.data()),
			in.size(), *o_out); 
}

inline void g_cvt_to_utf32(std::basic_string_view<char16_t> in,
		std::u32string *o_out)
{
	::unicode::utf16::decode(in.data(), in.size(), *o_out); 
}

inline void g_cvt_to_utf32(std::basic_string_view<char32_t> in,
		std::u32string *o_out)
{
	o_out->insert(o_out->end(), in.begin(), in.end());
}

template <typename Void=void>
requires (sizeof(wchar_t) == 4)
void g_cvt_from_wide_helper(std::basic_string_view<wchar_t> in,
		std::u32string *o_out)
{
	o_out->insert(o_out->end(), in.begin(), in.end());
}

template <typename Void=void>
requires (sizeof(wchar_t) == 2)
void g_cvt_from_wide_helper(std::basic_string_view<wchar_t> in,
		std::u32string *o_out)
{
    std::u16string buf(in.begin(), in.end());
	::unicode::utf16::decode(buf.c_str(), buf.size(), *o_out);
}

inline void g_cvt_to_utf32(std::basic_string_view<wchar_t> in,
		std::u32string *o_out)
{
	g_cvt_from_wide_helper(in, o_out);
}

//----------------------------------------------------------------------------+
//                                                                            |
//----------------------------------------------------------------------------+
template<typename T>
void g_cvt_from_utf32(Grapheme_iterator<T> begin,
		Grapheme_iterator<T> end,
		std::string *o_out)
{
	for (;  begin != end;  ++begin)
		for (auto cp_it = begin.begin();  cp_it != begin.end();  ++cp_it)
			::unicode::utf8::encode_codepoint(*cp_it, *o_out);
}

template<typename T>
void g_cvt_from_utf32(Grapheme_iterator<T> begin,
		Grapheme_iterator<T> end,
		std::u8string *o_out)
{
	std::string buf;
	for (;  begin != end;  ++begin)
		for (auto cp_it = begin.begin();  cp_it != begin.end();  ++cp_it)
			::unicode::utf8::encode_codepoint(*cp_it, buf);
	o_out->insert(o_out->end(), buf.begin(), buf.end());
}

template<typename T>
void g_cvt_from_utf32(Grapheme_iterator<T> begin,
		Grapheme_iterator<T> end,
		std::u16string *o_out)
{
	for (;  begin != end;  ++begin)
		for (auto cp_it = begin.begin();  cp_it != begin.end();  ++cp_it)
			::unicode::utf16::encode_codepoint(*cp_it, *o_out);
}

template<typename T>
void g_cvt_from_utf32(Grapheme_iterator<T> begin,
		Grapheme_iterator<T> end,
		std::u32string *o_out)
{
	for (;  begin != end;  ++begin)
		for (auto cp_it = begin.begin();  cp_it != begin.end();  ++cp_it)
			(*o_out) += *cp_it;
}

template<typename T>
requires (sizeof(wchar_t) == 2)
void g_cvt_from_utf32(Grapheme_iterator<T> begin,
		Grapheme_iterator<T> end,
		std::wstring *o_out)
{
    std::u16string buf;
	for (;  begin != end;  ++begin)
		for (auto cp_it = begin.begin();  cp_it != begin.end();  ++cp_it)
			::unicode::utf16::encode_codepoint(*cp_it, buf);
	o_out->insert(o_out->end(), buf.begin(), buf.end());
}

template<typename T>
requires (sizeof(wchar_t) == 4)
void g_cvt_from_utf32(Grapheme_iterator<T> begin,
		Grapheme_iterator<T> end,
		std::wstring *o_out)
{
    std::u16string buf;
	for (;  begin != end;  ++begin)
		for (auto cp_it = begin.begin();  cp_it != begin.end();  ++cp_it)
			(*o_out) += *cp_it;
}


} //sfpvk::utils::unicode

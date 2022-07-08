#pragma once
#include <limits>
#include <utf8proc.h>


namespace sfpvk::utils::unicode {

inline int g_usym_width(char32_t sym)
{
	return utf8proc_charwidth(sym);
}

} //sfpvk::utils::unicode

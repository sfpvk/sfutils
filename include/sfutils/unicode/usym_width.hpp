#pragma once
#include <sfutils_config.h>
#ifdef HAVE_WCWIDTH
#  ifndef _XOPEN_SOURCE
#    define _XOPEN_SOURCE
#    define SFPVK_UNDEF_XOPEN_SOURCE
#  endif
#  include <wchar.h>
#  ifdef SFPVK_UNDEF_XOPEN_SOURCE
#    undef _XOPEN_SOURCE
#    undef SFPVK_UNDEF_XOPEN_SOURCE
#  endif
#else
	extern "C" int wcwidth(wchar_t ucs);
#endif


namespace sfpvk::utils::unicode {

inline int g_usym_width(char32_t sym)
{
	return wcwidth(sym);
}

} //sfpvk::utils::unicode

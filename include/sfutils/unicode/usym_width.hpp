#pragma once
#include <sfutils_config.h>
#ifdef HAVE_WCWIDTH
#  ifndef _XOPEN_SOURCE
#    define _XOPEN_SOURCE
#    define UNDEF_XOPEN_SOURCE
#  endif
#  include <wchar.h>
#  ifdef UNDEF_XOPEN_SOURCE
#    undef _XOPEN_SOURCE
#    undef UNDEF_XOPEN_SOURCE
#  endif
#else
#  include <wcwidth/wcwidth.h>
#endif


namespace sfpvk::utils::unicode {


inline int g_usym_width(char32_t sym)
{
	return wcwidth(sym);
}


} //sfpvk::utils::unicode

#ifndef _XOPEN_SOURCE
#  define _XOPEN_SOURCE
#  define UNDEF_XOPEN_SOURCE
#endif
#include <wchar.h>
#ifdef UNDEF_XOPEN_SOURCE
#  undef _XOPEN_SOURCE
#  undef UNDEF_XOPEN_SOURCE
#endif


namespace sfpvk::utils::unicode {


inline int g_usym_width(const wchar_t *beg,
		[[maybe_unused]] const wchar_t *last)
{
	return wcwidth(*beg);
}


} //sfpvk::utils::unicode

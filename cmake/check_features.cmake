include(CheckSymbolExists)
set(CMAKE_REQUIRED_DEFINITIONS -D_XOPEN_SOURCE)
check_symbol_exists(wcwidth "wchar.h" HAVE_WCWIDTH)

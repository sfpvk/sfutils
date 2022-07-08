#pragma once
#include <cstddef>
#include <type_traits>


namespace sfpvk::utils {


using ssize_t = std::ptrdiff_t;

//----------------------------------------------------------------------------+
//                                                                            |
//----------------------------------------------------------------------------+
template <typename T>
	requires std::is_pointer_v<T>
	using not_null = T;


} //sfpvk::utils

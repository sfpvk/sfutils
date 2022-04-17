#pragma once
#include <cstddef>
#include <concepts>
#include <type_traits>
#include <cstdint>


namespace sfpvk::utils {


using ssize_t = std::ptrdiff_t;

//----------------------------------------------------------------------------+
//                                                                            |
//----------------------------------------------------------------------------+
template <typename T>
	requires std::is_pointer_v<T>
	using not_null = T;

//----------------------------------------------------------------------------+
//                                                                            |
//----------------------------------------------------------------------------+
template <std::integral T>
auto sig(T t) 
{
	using type_t = std::conditional_t<std::is_signed_v<T>, 
		  std::uintmax_t, std::intmax_t>;
	return static_cast<type_t>(t);
}


} //sfpvk::utils

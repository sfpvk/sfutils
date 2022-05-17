#pragma once
#include <functional>
#include <tuple>


namespace sfpvk::utils {


template <typename T>
struct Fun_traits;

template <typename R, typename ...A>
struct Fun_traits<std::function<R(A...)>>
{
	using ret_t = R;
	using args_t = std::tuple<A...>;
};

template <typename R, typename ...A>
struct Fun_traits<R(A...)>
{
	using ret_t = R;
	using args_t = std::tuple<A...>;
};


} //sfpvk::utils

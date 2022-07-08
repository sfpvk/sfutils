#pragma once
#include <type_traits>


namespace sfpvk::utils::unicode {


struct End_iterator_tag {};

template <typename T>
struct Ustring_iterator_traits
{
	using cluster_t = typename T::Grapheme_cluster;
	using code_point_reference_t = char32_t &;
	using grapheme_array_t = typename T::grapheme_array_t;
};

template <typename T>
struct Ustring_iterator_traits<const T>
{
	using cluster_t = const typename T::Grapheme_cluster;
	using code_point_reference_t = const char32_t &;
	using grapheme_array_t = const typename T::grapheme_array_t;
};

//----------------------------------------------------------------------------+
// Code point iterator                                                        |
//----------------------------------------------------------------------------+
template <typename T>
class Codepoint_iterator
{
	static constexpr ssize_t s_grapheme_sz = T::s_grapheme_sz;
	static constexpr char32_t s_grapheme_end_tag = T::s_grapheme_end_tag;
	using cluster_t = typename Ustring_iterator_traits<T>::cluster_t;
	cluster_t *m_cluster;
	ssize_t m_codepoint_pos;
public:
	using iterator_category = std::bidirectional_iterator_tag;
	using difference_type = ssize_t;
	using value_type = char32_t;
	using reference = typename Ustring_iterator_traits<T>::
		code_point_reference_t;
	Codepoint_iterator();
	Codepoint_iterator(cluster_t *r_cluster);
	Codepoint_iterator(cluster_t *r_cluster, ssize_t codepoint_pos);
	Codepoint_iterator &operator++();
	Codepoint_iterator operator++(int);
	Codepoint_iterator &operator--();
	Codepoint_iterator operator--(int);
	reference operator*()const;
	difference_type operator-(const Codepoint_iterator &r)const;
	operator Codepoint_iterator<const std::remove_const_t<T>>()const
		requires (! std::is_const_v<T>);
	template <typename U>
	friend bool operator==(const Codepoint_iterator<U> &l,
			const Codepoint_iterator<U> &r);
	template <typename U>
	friend bool operator==(const Codepoint_iterator<U> &l,
			End_iterator_tag r);
	template <typename U>
	friend bool operator==(End_iterator_tag l,
			const Codepoint_iterator<U> &r);
};

template <typename T>
Codepoint_iterator<T>::Codepoint_iterator() :
	m_cluster{},
	m_codepoint_pos{}
{
}

template <typename T>
Codepoint_iterator<T>::
Codepoint_iterator(cluster_t *r_cluster) :
	m_cluster{r_cluster},
	m_codepoint_pos{0}
{
}

template <typename T>
Codepoint_iterator<T>::
Codepoint_iterator(cluster_t *r_cluster, ssize_t codepoint_pos) :
	m_cluster{r_cluster},
	m_codepoint_pos{codepoint_pos}
{
}

template <typename T>
Codepoint_iterator<T> &Codepoint_iterator<T>::operator++()
{
	++ m_codepoint_pos;
	return *this;
}

template <typename T>
Codepoint_iterator<T> Codepoint_iterator<T>::operator++(int)
{
	Codepoint_iterator ret{*this};
	++ m_codepoint_pos;
	return ret;
}

template <typename T>
Codepoint_iterator<T> &Codepoint_iterator<T>::operator--()
{
	-- m_codepoint_pos;
	return *this;
}

template <typename T>
Codepoint_iterator<T> Codepoint_iterator<T>::operator--(int)
{
	Codepoint_iterator ret{*this};
	-- m_codepoint_pos;
	return ret;
}

template <typename T>
typename Codepoint_iterator<T>::reference
Codepoint_iterator<T>::operator*()const
{
	return m_cluster->cp[m_codepoint_pos];
}

template <typename T>
Codepoint_iterator<T>::difference_type Codepoint_iterator<T>::operator-(
		const Codepoint_iterator &r)const
{
	return m_codepoint_pos - r.m_codepoint_pos;
}

template <typename T>
Codepoint_iterator<T>::
operator Codepoint_iterator<const std::remove_const_t<T>>()const
requires (! std::is_const_v<T>)
{
	return {m_cluster, m_codepoint_pos};
}

template <typename U>
bool operator==(const Codepoint_iterator<U> &l,
		const Codepoint_iterator<U> &r)
{
	return l.m_cluster == r.m_cluster  &&
		l.m_codepoint_pos == r.m_codepoint_pos;
}

template <typename U>
bool operator==(const Codepoint_iterator<U> &l,
		End_iterator_tag)
{
	return l.m_codepoint_pos == l.s_grapheme_sz  ||
		l.m_cluster->cp[l.m_codepoint_pos] == l.s_grapheme_end_tag;
}

template <typename U>
bool operator==(End_iterator_tag,
		const Codepoint_iterator<U> &r)
{
	return r.m_codepoint_pos == r.s_grapheme_sz  ||
		r.m_cluster->cp[r.m_codepoint_pos] == r.s_grapheme_end_tag;
}

//----------------------------------------------------------------------------+
// Grapheme iterator                                                          |
//----------------------------------------------------------------------------+
template <typename T>
class Grapheme_iterator
{
	using grapheme_array_t = typename Ustring_iterator_traits<T>::
		grapheme_array_t;
	grapheme_array_t *m_graphemes;
	ssize_t m_cluster_pos;
public:
	using iterator_category = std::random_access_iterator_tag;
	using difference_type = ssize_t;
	using value_type = Codepoint_iterator<T>;
	Grapheme_iterator();
	Grapheme_iterator(grapheme_array_t *r_graphemes,
			ssize_t cluster_pos);
	Grapheme_iterator &operator++();
	Grapheme_iterator operator++(int);
	Grapheme_iterator &operator--();
	Grapheme_iterator operator--(int);
	Grapheme_iterator &operator+=(difference_type n);
	Grapheme_iterator &operator-=(difference_type n);
	bool operator==(const Grapheme_iterator &)const = default;
	auto operator <=>(const Grapheme_iterator &)const = default;
	difference_type operator-(const Grapheme_iterator &r)const;
	Grapheme_iterator operator-(difference_type n)const;
	value_type operator*()const;
	value_type operator[](difference_type n)const;
	operator Grapheme_iterator<const std::remove_const_t<T>>()const
		requires (! std::is_const_v<T>);
	template <typename U>
		friend Grapheme_iterator<U> operator+(const Grapheme_iterator<U> &l,
				ssize_t r);
	template <typename U>
		friend Grapheme_iterator<U> operator+(ssize_t l,
				const Grapheme_iterator<U> &r);
};

template <typename T>
Grapheme_iterator<T>::Grapheme_iterator() :
	m_graphemes{},
	m_cluster_pos{}
{
}

template <typename T>
Grapheme_iterator<T>::Grapheme_iterator(
		grapheme_array_t *r_graphemes,
		ssize_t cluster_pos) :
	m_graphemes{r_graphemes},
	m_cluster_pos{cluster_pos}
{
}

template <typename T>
Grapheme_iterator<T> &Grapheme_iterator<T>::operator++()
{
	++ m_cluster_pos;
	return *this;
}

template <typename T>
Grapheme_iterator<T> Grapheme_iterator<T>::operator++(int)
{
	Grapheme_iterator ret{*this};
	++ m_cluster_pos;
	return ret;
}

template <typename T>
Grapheme_iterator<T> &Grapheme_iterator<T>::operator--()
{
	-- m_cluster_pos;
	return *this;
}

template <typename T>
Grapheme_iterator<T> Grapheme_iterator<T>::operator--(int)
{
	Grapheme_iterator ret{*this};
	-- m_cluster_pos;
	return ret;
}

template <typename T>
Grapheme_iterator<T> &Grapheme_iterator<T>::operator+=(difference_type n)
{
	m_cluster_pos += n;
	return *this;
}

template <typename T>
Grapheme_iterator<T> &Grapheme_iterator<T>::operator-=(difference_type n)
{
	m_cluster_pos -= n;
	return *this;
}

template <typename T>
typename Grapheme_iterator<T>::difference_type
Grapheme_iterator<T>::operator-(const Grapheme_iterator &r)const
{
	return m_cluster_pos - r.m_cluster_pos;
}

template <typename T>
Grapheme_iterator<T> Grapheme_iterator<T>::operator-(difference_type n)const
{
	return {m_graphemes, m_cluster_pos - n};
}

template <typename T>
typename Grapheme_iterator<T>::value_type Grapheme_iterator<T>::operator*()const
{
	return {&(*m_graphemes)[m_cluster_pos]};
}

template <typename T>
typename Grapheme_iterator<T>::value_type
Grapheme_iterator<T>::operator[](difference_type n)const
{
	ssize_t idx = m_cluster_pos + n;
	return {&(*m_graphemes)[idx]};
}

template <typename T>
Grapheme_iterator<T>::
operator Grapheme_iterator<const std::remove_const_t<T>>()const
requires (! std::is_const_v<T>)
{
	return {m_graphemes, m_cluster_pos};
}

template <typename U>
Grapheme_iterator<U> operator+(const Grapheme_iterator<U> &l,
		ssize_t r)
{
	return Grapheme_iterator<U>{l.m_graphemes, l.m_cluster_pos + r};
}
template <typename U>
Grapheme_iterator<U> operator+(ssize_t l,
		const Grapheme_iterator<U> &r)
{
	return Grapheme_iterator<U>{r.m_graphemes, r.m_cluster_pos + l};
}


} //sfpvk::utils::unicode

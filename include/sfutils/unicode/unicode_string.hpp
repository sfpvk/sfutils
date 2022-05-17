#pragma once
#include <deque>
#include <string>
#include <iterator>
#include <utility>
#include <cstring>
#include <concepts>
#include <type_traits>
#include <cpp-unicodelib/unicodelib.h>
#include "ustring_iterator.hpp"
#include "encoding_cvt.hpp"


namespace sfpvk::utils::unicode {


template <ssize_t Grapheme_size = 5>
class Ustring
{
	static_assert(Grapheme_size > 0);
public:
	struct Grapheme_cluster
	{
		char32_t cp[Grapheme_size];
	};
	using grapheme_array_t = std::deque<Grapheme_cluster>;
	using const_grapheme_iterator_t = Grapheme_iterator<const Ustring>;
	using grapheme_iterator_t = Grapheme_iterator<Ustring>;
	using const_codepoint_iterator_t = Codepoint_iterator<const Ustring>;
	using codepoint_iterator_t = Codepoint_iterator<Ustring>;
	static constexpr char32_t s_grapheme_end_tag = -1;
	static constexpr ssize_t s_grapheme_sz = Grapheme_size;
public:
	template <typename T>
		Ustring &operator=(const T &in);
	template <typename T>
		Ustring &operator+=(const T &in);
	template <typename T>
		Ustring &operator+=(Grapheme_iterator<T> in);
	Ustring &operator+=(const Ustring &in);
	Ustring();
	template <typename T>
		Ustring(const T &in);
	template <typename CharT>
		void to_string(std::basic_string<CharT> *o_dest,
				ssize_t begin_pos=0, ssize_t count=-1)const;
	ssize_t ssize()const;
	std::size_t size()const;
	bool empty()const;
	const_grapheme_iterator_t begin()const;
	const_grapheme_iterator_t end()const;
	grapheme_iterator_t begin();
	grapheme_iterator_t end();
	const_grapheme_iterator_t cbegin()const;
	const_grapheme_iterator_t cend()const;
	const_codepoint_iterator_t operator[](ssize_t i)const;
	codepoint_iterator_t operator[](ssize_t i);
	void erase(ssize_t begin, ssize_t end);
	void clear();
	template <typename T>
		ssize_t push_back(const T &in);
	template <typename T>
		ssize_t push_back(Grapheme_iterator<T> in);
	template <typename T>
		ssize_t insert(ssize_t index, const T &in);
	template <typename T>
		requires std::same_as<std::remove_const_t<T>, Ustring<Grapheme_size>>
		ssize_t insert(ssize_t index, Grapheme_iterator<T> in);
	void close_grapheme();
private:
	grapheme_array_t m_data;
	std::u32string m_cvt_buf;
	std::u32string m_overflow_buf;
	ssize_t m_insert_cluster_pos;
	ssize_t m_insert_codepoint_size;
	ssize_t insert_base(ssize_t pos);
};

template <ssize_t Grapheme_size>
template <typename T>
Ustring<Grapheme_size> &Ustring<Grapheme_size>::operator=(const T &in)
{
	m_data.clear();
	close_grapheme();
	m_cvt_buf.clear();
	g_cvt_to_utf32(in, &m_cvt_buf);
	insert_base(0);
	return *this;
}

template <ssize_t Grapheme_size>
template <typename T>
Ustring<Grapheme_size> &Ustring<Grapheme_size>::operator+=(const T &in)
{
	m_cvt_buf.clear();
	g_cvt_to_utf32(in, &m_cvt_buf);
	insert_base(std::ssize(m_data));
	return *this;
}

template <ssize_t Grapheme_size>
template <typename T>
Ustring<Grapheme_size> &
Ustring<Grapheme_size>::operator+=(Grapheme_iterator<T> in)
{
	insert(std::ssize(m_data), in);
	return *this;
}

template <ssize_t Grapheme_size>
Ustring<Grapheme_size> &
Ustring<Grapheme_size>::operator+=(const Ustring &in)
{
	for (auto b = in.begin();  b != in.end();  ++b)
		this->operator+=(b);
	return *this;
}

template <ssize_t Grapheme_size>
Ustring<Grapheme_size>::Ustring()
{
	close_grapheme();
}

template <ssize_t Grapheme_size>
template <typename T>
Ustring<Grapheme_size>::Ustring(const T &in) : Ustring{}
{
	this->operator=(in);
}

template <ssize_t Grapheme_size>
template <typename CharT>
void Ustring<Grapheme_size>::to_string(std::basic_string<CharT> *o_dest,
		ssize_t begin_pos, ssize_t count)const
{
	const_grapheme_iterator_t beg_it = cbegin() + begin_pos;
	const_grapheme_iterator_t end_it;
	if (count != -1)
		end_it = beg_it + count;
	else
		end_it = cbegin() + std::ssize(m_data);

	g_cvt_from_utf32(beg_it, end_it, o_dest);
}

template <ssize_t Grapheme_size>
ssize_t Ustring<Grapheme_size>::ssize()const
{
	return std::ssize(m_data);
}

template <ssize_t Grapheme_size>
std::size_t Ustring<Grapheme_size>::size()const
{
	return m_data.size();
}

template <ssize_t Grapheme_size>
bool Ustring<Grapheme_size>::empty()const
{
	return m_data.empty();
}

template <ssize_t Grapheme_size>
typename Ustring<Grapheme_size>::const_grapheme_iterator_t
Ustring<Grapheme_size>::begin()const
{
	return {&m_data, 0};
}

template <ssize_t Grapheme_size>
typename Ustring<Grapheme_size>::const_grapheme_iterator_t
Ustring<Grapheme_size>::end()const
{
	return {&m_data, std::ssize(m_data)};
}

template <ssize_t Grapheme_size>
typename Ustring<Grapheme_size>::grapheme_iterator_t Ustring<Grapheme_size>::
begin()
{
	return {&m_data, 0};
}

template <ssize_t Grapheme_size>
typename Ustring<Grapheme_size>::grapheme_iterator_t Ustring<Grapheme_size>::
end()
{
	return {&m_data, std::ssize(m_data)};
}

template <ssize_t Grapheme_size>
typename Ustring<Grapheme_size>::const_grapheme_iterator_t
Ustring<Grapheme_size>::cbegin()const
{
	return {&m_data, 0};
}

template <ssize_t Grapheme_size>
typename Ustring<Grapheme_size>::const_grapheme_iterator_t
Ustring<Grapheme_size>::cend()const
{
	return {&m_data, std::ssize(m_data)};
}

template <ssize_t Grapheme_size>
typename Ustring<Grapheme_size>::const_codepoint_iterator_t
Ustring<Grapheme_size>::operator[](ssize_t i)const
{
	return {&m_data[i]};
}

template <ssize_t Grapheme_size>
typename Ustring<Grapheme_size>::codepoint_iterator_t
Ustring<Grapheme_size>::operator[](ssize_t i)
{
	return {&m_data[i]};
}

template <ssize_t Grapheme_size>
void Ustring<Grapheme_size>::erase(ssize_t begin, ssize_t end)
{
	close_grapheme();
	m_data.erase(m_data.begin()+begin, m_data.begin()+end);
}

template <ssize_t Grapheme_size>
void Ustring<Grapheme_size>::clear()
{
	close_grapheme();
	m_data.clear();
}

template <ssize_t Grapheme_size>
template <typename T>
ssize_t Ustring<Grapheme_size>::push_back(const T &in)
{
	m_cvt_buf.clear();
	g_cvt_to_utf32(in, &m_cvt_buf);
	return insert_base(std::ssize(m_data));
}

template <ssize_t Grapheme_size>
template <typename T>
ssize_t Ustring<Grapheme_size>::push_back(Grapheme_iterator<T> in)
{
	return insert(std::ssize(m_data), in);
}

template <ssize_t Grapheme_size>
template <typename T>
ssize_t Ustring<Grapheme_size>::insert(ssize_t index, const T &in)
{
	m_cvt_buf.clear();
	g_cvt_to_utf32(in, &m_cvt_buf);
	return insert_base(index);
}

template <ssize_t Grapheme_size>
template <typename T>
requires std::same_as<std::remove_const_t<T>, Ustring<Grapheme_size>>
ssize_t Ustring<Grapheme_size>::insert(ssize_t index,
		Grapheme_iterator<T> in)
{
	Grapheme_cluster cluster;
	memcpy(&cluster, &**in, sizeof(Grapheme_cluster));
	m_data.insert(m_data.begin()+index, cluster);
	close_grapheme();
	return 1;
}

template <ssize_t Grapheme_size>
void Ustring<Grapheme_size>::close_grapheme()
{
	m_insert_cluster_pos = -2;
	m_insert_codepoint_size = 0;
}

template <ssize_t Grapheme_size>
ssize_t Ustring<Grapheme_size>::insert_base(ssize_t pos)
{
	ssize_t inscluster_cnt = 0;
	auto insert_cluster = [&]() {
		auto get_end = [&](ssize_t) {return s_grapheme_end_tag;};
		[&]<ssize_t ...I>(std::integer_sequence<ssize_t, I...>) {
			m_data.insert(m_data.begin()+m_insert_cluster_pos,
					Grapheme_cluster{get_end(I)...});
		}(std::make_integer_sequence<ssize_t, Grapheme_size>());
		++ inscluster_cnt;
	};
	auto push_sym = [&](char32_t sym) {
		if (m_insert_codepoint_size < Grapheme_size) {
			m_data[m_insert_cluster_pos].cp[m_insert_codepoint_size] = sym;
		}
		else {
			if (m_insert_codepoint_size == Grapheme_size) {
				m_overflow_buf.clear();
				m_overflow_buf.insert(m_overflow_buf.end(),
						m_data[m_insert_cluster_pos].cp,
						m_data[m_insert_cluster_pos].cp + Grapheme_size);
			}
			m_overflow_buf.push_back(sym);
		}
		++ m_insert_codepoint_size;
	};
	auto check_for_next_grapheme = [&]()->bool {
		if (m_insert_codepoint_size == 1)
			return false;
		if (m_insert_codepoint_size > Grapheme_size)
			return ::unicode::is_grapheme_boundary(m_overflow_buf.c_str(),
					m_overflow_buf.size(),
					m_insert_codepoint_size-1);
		return ::unicode::is_grapheme_boundary(m_data[m_insert_cluster_pos].cp,
				m_insert_codepoint_size,
				m_insert_codepoint_size-1);
	};

	if (pos - m_insert_cluster_pos != 1) {
		m_insert_cluster_pos = pos;
		m_insert_codepoint_size = 0;
		insert_cluster();
	}
	for (char32_t sym : m_cvt_buf) {
		push_sym(sym);
		if (check_for_next_grapheme()) {
			if (m_insert_codepoint_size <= Grapheme_size)
				m_data[m_insert_cluster_pos].cp[m_insert_codepoint_size-1] =
					s_grapheme_end_tag;
			++ m_insert_cluster_pos;
			m_insert_codepoint_size = 0;
			insert_cluster();
			push_sym(sym);
		}
	}
	return inscluster_cnt;
}


} //sfpvk::utils::unicode

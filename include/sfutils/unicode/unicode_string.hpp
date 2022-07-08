#pragma once
#include <deque>
#include <utility>
#include <utf8proc.h>
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
	static constexpr char32_t s_grapheme_end_tag = char32_t(-1);
	static constexpr ssize_t s_grapheme_sz = Grapheme_size;
public:
	template <Cvt_endian::cvt_endian_t endian_mode=Cvt_endian::from_native,
			 typename T>
		Ustring &operator=(const T &in);
	template <Cvt_endian::cvt_endian_t endian_mode=Cvt_endian::from_native,
			 typename T>
		Ustring &operator+=(const T &in);
	template <typename T>
		Ustring &operator+=(Grapheme_iterator<T> in);
	Ustring &operator+=(const Ustring &in);
	Ustring();
	template <typename T,
			 typename Endian=std::integral_constant<Cvt_endian::cvt_endian_t, 
				 Cvt_endian::from_native>>
		Ustring(const T &in, Endian endian_mode={});

	template <Cvt_endian::cvt_endian_t endian_mode=Cvt_endian::to_native,
			 typename CharT>
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
	template <Cvt_endian::cvt_endian_t endian_mode=Cvt_endian::from_native,
			 typename T>
		ssize_t push_back(const T &in);
	template <typename T>
		ssize_t push_back(Grapheme_iterator<T> in);
	template <Cvt_endian::cvt_endian_t endian_mode=Cvt_endian::from_native,
			 typename T>
		ssize_t insert(ssize_t index, const T &in);
	template <typename T>
		ssize_t insert(ssize_t index, Grapheme_iterator<T> in);
	void close_grapheme();
private:
	grapheme_array_t m_data;
	std::u32string m_cvt_buf;
	std::u32string m_overflow_buf;
	ssize_t m_insert_cluster_pos;
	ssize_t m_insert_codepoint_size;
	utf8proc_int32_t m_grapheme_break_state;
	ssize_t insert_base(ssize_t pos);
};

template <ssize_t Grapheme_size>
template <Cvt_endian::cvt_endian_t endian_mode, typename T>
Ustring<Grapheme_size> &Ustring<Grapheme_size>::operator=(const T &in)
{
	static_assert(((Cvt_endian::to_little|Cvt_endian::to_big)
			& endian_mode) == 0);
	m_data.clear();
	close_grapheme();
	m_cvt_buf.clear();
	g_cvt<endian_mode>(in, &m_cvt_buf);
	insert_base(0);
	return *this;
}

template <ssize_t Grapheme_size>
template <Cvt_endian::cvt_endian_t endian_mode, typename T>
Ustring<Grapheme_size> &Ustring<Grapheme_size>::operator+=(const T &in)
{
	static_assert(((Cvt_endian::to_little|Cvt_endian::to_big)
			& endian_mode) == 0);
	m_cvt_buf.clear();
	g_cvt<endian_mode>(in, &m_cvt_buf);
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
template <typename T, typename Endian>
Ustring<Grapheme_size>::Ustring(const T &in, Endian endian_mode) : Ustring{}
{
	this->operator=<endian_mode>(in);
}

template <ssize_t Grapheme_size>
template <Cvt_endian::cvt_endian_t endian_mode,
		 typename CharT>
void Ustring<Grapheme_size>::to_string(std::basic_string<CharT> *o_dest,
		ssize_t begin_pos, ssize_t count)const
{
	static_assert(((Cvt_endian::from_little|Cvt_endian::from_big)
			& endian_mode) == 0);

	struct Buf {
		std::array<char32_t, Grapheme_size*100> buf;
		ssize_t sz = 0;
	} buf;
	auto push_to_buf = [&](const char32_t *beg, const char32_t *end) -> bool {
		if (std::ssize(buf.buf)-buf.sz < end-beg)
			return false;
		std::ranges::copy(beg, end, buf.buf.data()+buf.sz);
		buf.sz += end - beg;
		return true;
	};
	auto flush_buf = [&]() {
		g_cvt<endian_mode>(std::u32string_view(buf.buf.data(), buf.sz),
				o_dest);
		buf.sz = 0;
	};

	const_grapheme_iterator_t beg_it = cbegin() + begin_pos;
	const_grapheme_iterator_t end_it;
	if (count != -1)
		end_it = beg_it + count;
	else
		end_it = cbegin() + std::ssize(m_data);

	for (;  beg_it != end_it;  ++beg_it) {
		auto cp_beg = *beg_it;
		auto cp_end = *beg_it;
		for (;  cp_end != End_iterator_tag{};  ++cp_end);
		while (true)
			if (! push_to_buf(&*cp_beg, &*cp_beg+(cp_end-cp_beg)))
				flush_buf();
			else
				break;
	}
	flush_buf();
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
template <Cvt_endian::cvt_endian_t endian_mode, typename T>
ssize_t Ustring<Grapheme_size>::push_back(const T &in)
{
	static_assert(((Cvt_endian::to_little|Cvt_endian::to_big)
			& endian_mode) == 0);
	m_cvt_buf.clear();
	g_cvt<endian_mode>(in, &m_cvt_buf);
	return insert_base(std::ssize(m_data));
}

template <ssize_t Grapheme_size>
template <typename T>
ssize_t Ustring<Grapheme_size>::push_back(Grapheme_iterator<T> in)
{
	return insert(std::ssize(m_data), in);
}

template <ssize_t Grapheme_size>
template <Cvt_endian::cvt_endian_t endian_mode, typename T>
ssize_t Ustring<Grapheme_size>::insert(ssize_t index, const T &in)
{
	static_assert(((Cvt_endian::to_little|Cvt_endian::to_big)
			& endian_mode) == 0);
	m_cvt_buf.clear();
	g_cvt<endian_mode>(in, &m_cvt_buf);
	return insert_base(index);
}

template <ssize_t Grapheme_size>
template <typename T>
ssize_t Ustring<Grapheme_size>::insert(ssize_t index,
		Grapheme_iterator<T> in)
{
	Grapheme_cluster cluster;
	ssize_t cl_pos = 0;
	for (auto i = *in;  i != End_iterator_tag{}
			&&  cl_pos != Grapheme_size;
			++i, ++cl_pos)
		cluster.cp[cl_pos] = *i;
	for (;  cl_pos != Grapheme_size;  ++cl_pos)
		cluster.cp[cl_pos] = s_grapheme_end_tag;
	m_data.insert(m_data.begin()+index, cluster);
	close_grapheme();
	return 1;
}

template <ssize_t Grapheme_size>
void Ustring<Grapheme_size>::close_grapheme()
{
	m_insert_cluster_pos = -2;
	m_insert_codepoint_size = 0;
	m_grapheme_break_state = 0;
}

template <ssize_t Grapheme_size>
ssize_t Ustring<Grapheme_size>::insert_base(ssize_t pos)
{
	ssize_t inscluster_cnt = 0;
	if (m_cvt_buf.empty())
		return inscluster_cnt;

	auto insert_cluster = [&]() {
		auto get_end = [&](ssize_t) {return s_grapheme_end_tag;};
		[&]<ssize_t ...I>(std::integer_sequence<ssize_t, I...>) {
			m_data.insert(m_data.begin()+m_insert_cluster_pos,
					Grapheme_cluster{get_end(I)...});
		}(std::make_integer_sequence<ssize_t, Grapheme_size>());
		++ inscluster_cnt;
		m_insert_codepoint_size = 0;
	};
	auto push_sym = [&](char32_t sym) {
		if (m_insert_codepoint_size < Grapheme_size)
			m_data[m_insert_cluster_pos].cp[m_insert_codepoint_size] = sym;
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
			return utf8proc_grapheme_break_stateful(
					m_overflow_buf[m_insert_codepoint_size-2],
					m_overflow_buf[m_insert_codepoint_size-1],
					&m_grapheme_break_state);
		return utf8proc_grapheme_break_stateful(
				m_data[m_insert_cluster_pos].cp[m_insert_codepoint_size-2],
				m_data[m_insert_cluster_pos].cp[m_insert_codepoint_size-1],
				&m_grapheme_break_state);
	};

	if (pos - m_insert_cluster_pos != 1) {
		m_insert_cluster_pos = pos;
		insert_cluster();
	}
	for (char32_t sym : m_cvt_buf) {
		push_sym(sym);
		if (check_for_next_grapheme()) {
			if (m_insert_codepoint_size <= Grapheme_size)
				m_data[m_insert_cluster_pos].cp[m_insert_codepoint_size-1] =
					s_grapheme_end_tag;
			++ m_insert_cluster_pos;
			insert_cluster();
			push_sym(sym);
		}
	}
	return inscluster_cnt;
}


} //sfpvk::utils::unicode

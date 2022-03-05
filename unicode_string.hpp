#include <string>
#include <utility>
#include <locale>
#include <tuple>
#include <vector>
#include <utility>
#include <cassert>
#include <type_traits>
#include <concepts>
#include "encoding_cvt.hpp"
#include "base.hpp"


namespace sfpvk::utils {


class Ustring
{
	friend void swap(Ustring &l, Ustring &r);
	enum markup_e : unsigned char
	{
		e_unmarked   = 0b00,
		e_characters = 0b01,
		e_words      = 0b10
	};
	void markup(markup_e m);
	std::wstring m_str;
	std::vector<std::pair<std::wstring::const_iterator,
				std::wstring::const_iterator>> m_chedges;
	boost::locale::boundary::wssegment_index m_characters;
	boost::locale::boundary::wssegment_index m_words;
	const std::locale *m_loc;
	ssize_t m_insert_pos;
	unsigned char m_markup_state;
public:
	static bool s_is_newline(std::wstring::const_iterator begin,
			std::wstring::const_iterator end);
	// r_loc - locale generated by boost::locale::generator
	Ustring(const std::locale *r_loc);
	Ustring(const Ustring &other);
	template <typename T>
		Ustring(T &&str,
				const std::locale *r_loc);
	Ustring(Ustring &&other)noexcept;
	Ustring &operator=(const Ustring &other);
	Ustring &operator=(Ustring &&other)noexcept;
	template <typename CharT>
		requires (! std::same_as<CharT, wchar_t>)
		Ustring &operator=(const std::basic_string<CharT> &str);
	template <typename T>
		requires (std::same_as<std::remove_cvref_t<T>,
					 std::basic_string<wchar_t>>  ||
				std::same_as<std::decay_t<T>, const wchar_t *>)
		Ustring &operator=(T &&str);
	template <typename T>
		Ustring &operator+=(const T &str);
	template <typename CharT>
		requires (! std::same_as<CharT, wchar_t>)
		Ustring &operator+=(const std::basic_string<CharT> &str);
	Ustring &operator+=(const Ustring &str);
	template <typename CharT>
		void cvt(std::basic_string<CharT> *o_dest);
	ssize_t ssize();
	void reserve(ssize_t sz);
	void insert(ssize_t index, wchar_t ch);
	void resume_insert(wchar_t ch);
	void insert(ssize_t index, const std::wstring &ch);
	void push_back(wchar_t c);
	void erase(ssize_t first, ssize_t end);
	void clear();
	// i == size is forbidden
	const std::pair<std::wstring::const_iterator,
			  std::wstring::const_iterator> &operator[](ssize_t i);
	const std::wstring &base_wstring()const &;
	std::wstring base_wstring()&&;
	ssize_t prev_word(ssize_t pos);
	ssize_t next_word(ssize_t pos);
	ssize_t prev_line(ssize_t pos, int line_width);
	ssize_t next_line(ssize_t pos, int line_width);
};

inline Ustring::Ustring(const Ustring &other) :
	Ustring{other.m_str, other.m_loc}
{
}

inline Ustring::Ustring(Ustring &&other)noexcept :
	Ustring{other.m_loc}
{
	swap(*this, other);
}

inline Ustring::Ustring(const std::locale *r_loc) :
	m_loc{r_loc},
	m_insert_pos{-1},
	m_markup_state{e_unmarked} {}

template <typename T>
Ustring::Ustring(T &&str,
		const std::locale *r_loc) : Ustring{r_loc}
{
	operator=(std::forward<T>(str));
}

template <typename T>
requires (std::same_as<std::remove_cvref_t<T>,
			 std::basic_string<wchar_t>>  ||
		std::same_as<std::decay_t<T>, const wchar_t *>)
Ustring &Ustring::operator=(T &&str)
{
	m_str = std::forward<T>(str);
	m_insert_pos = -1;
	m_markup_state = e_unmarked;
	return *this;
}

template <typename CharT>
requires (! std::same_as<CharT, wchar_t>)
Ustring &Ustring::operator=(const std::basic_string<CharT> &str)
{
	g_cvt_to_wide(str, &m_str, *m_loc);
	return operator=(m_str);
}

inline Ustring &Ustring::operator=(const Ustring &other)
{
	Ustring other_new(other);
	swap(*this, other_new);
	return *this;
}

inline Ustring &Ustring::operator=(Ustring &&other)noexcept
{
	swap(*this, other);
	return *this;
}

template <typename CharT>
requires (! std::same_as<CharT, wchar_t>)
Ustring &Ustring::operator+=(const std::basic_string<CharT> &str)
{
	std::wstring buf;
	g_cvt_to_wide(str, &buf, *m_loc);
	return operator+=(buf);
}

template <typename T>
inline Ustring &Ustring::operator+=(const T &str)
{
	m_str += str;
	m_markup_state = e_unmarked;
	return *this;
}

inline Ustring &Ustring::operator+=(const Ustring &str)
{
	return operator+=(str.m_str);
}

inline void swap(Ustring &l, Ustring &r)
{
	using std::ranges::swap;
	swap(l.m_str, r.m_str);
	swap(l.m_chedges, r.m_chedges);
	swap(l.m_characters, r.m_characters);
	swap(l.m_words, r.m_words);
	swap(l.m_loc, r.m_loc);
	swap(l.m_insert_pos, r.m_insert_pos);
	swap(l.m_markup_state, r.m_markup_state);
}

inline std::wstring Ustring::base_wstring()&&
{
	std::wstring ret(std::move(m_str));
	m_insert_pos = -1;
	m_markup_state = e_unmarked;
	return ret;
}

inline void Ustring::insert(ssize_t index, wchar_t ch)
{
	markup(e_characters);
	auto pos = index>=std::ssize(m_chedges) ? m_str.cend() :
		(m_chedges.begin()+index)->first;
	m_insert_pos = pos - m_str.begin();
	m_str.insert(m_insert_pos++, 1, ch);
	m_markup_state = e_unmarked;
}

inline void Ustring::resume_insert(wchar_t ch)
{
	assert(m_insert_pos != -1);
	m_str.insert(m_insert_pos++, 1, ch);
	m_markup_state = e_unmarked;
}

inline void Ustring::insert(ssize_t index, const std::wstring &ch)
{
	if (ch.empty())
		return;
	insert(index, ch.front());
	for (auto pos = ch.begin()+1;  pos != ch.end();  ++pos)
		resume_insert(*pos);
}

inline void Ustring::push_back(wchar_t c)
{
	m_str += c;
	m_markup_state = e_unmarked;
}

inline void Ustring::clear()
{
	m_str.clear();
	m_insert_pos = -1;
	m_markup_state = e_unmarked;
}

inline void Ustring::reserve(ssize_t sz)
{
	m_str.reserve(sig(sz));
	m_markup_state = e_unmarked;
}

inline const std::wstring &Ustring::base_wstring()const &
{
	return m_str;
}

inline ssize_t Ustring::next_line(ssize_t pos, int line_width)
{
	markup(e_characters);

	while (line_width  &&  pos < std::ssize(m_chedges)) {
		if (s_is_newline((m_chedges.begin()+pos)->first,
					(m_chedges.begin()+pos)->second))
			line_width = 1;
		--line_width, ++pos;
	}
	return pos;
}

inline ssize_t Ustring::prev_line(ssize_t pos, int line_width)
{
	markup(e_characters);

	if (line_width <= 0  ||  m_chedges.empty())
		return pos;
	if (pos >= std::ssize(m_chedges)) {
		pos = std::ssize(m_chedges) - 1;
		-- line_width;
	}
	while (line_width  &&  pos) {
		if (s_is_newline((m_chedges.begin()+pos)->first,
					(m_chedges.begin()+pos)->second))
			line_width = 1;
		--line_width, --pos;
	}
	return pos;
}

inline bool Ustring::s_is_newline(std::wstring::const_iterator begin,
		std::wstring::const_iterator end)
{
	switch (end - begin) {
	case 1:
		switch (*begin) {
		case L'\u000A':
		case L'\u000B':
		case L'\u000C':
		case L'\u000D':
		case L'\u0085':
		case L'\u2028':
		case L'\u2029':
			return true;
		default:
			return false;
		}
	case 2:
		if (*begin == L'\u000D'  &&  *(begin+1) == L'\u000A')
			return true;
		return false;
	default:
		return false;
	}
}

inline ssize_t Ustring::prev_word(ssize_t pos)
{
	markup(e_characters);
	markup(e_words);

	auto word_it_end = m_words.end();
	decltype(m_words)::iterator word_it;
	ssize_t ret = 0;

	if (pos >= std::ssize(m_chedges))
		word_it = word_it_end;
	else
		word_it = m_words.find((m_chedges.begin()+pos)->first);
	if (word_it == m_words.begin())
		return ret;
	if (word_it == word_it_end  ||
			word_it->begin() >= (m_chedges.begin()+pos)->first)
		-- word_it;

	for (;  ret < std::ssize(m_chedges);  ++ret)
		if ((m_chedges.begin()+ret)->first == word_it->begin())
			break;
	return ret;
}

inline ssize_t Ustring::next_word(ssize_t pos)
{
	markup(e_characters);
	markup(e_words);

	ssize_t ret = std::ssize(m_chedges);
	if (pos >= ret)
		return ret;
	auto word_it = m_words.find((m_chedges.begin()+pos)->first);
	if (word_it == m_words.end())
		return ret;
	for (ret = 0;  ret < std::ssize(m_chedges);  ++ret)
		if ((m_chedges.begin()+ret)->first == word_it->end())
			break;
	return ret;
}

inline const std::pair<std::wstring::const_iterator,
		std::wstring::const_iterator> &Ustring::operator[](ssize_t i)
{
	markup(e_characters);
	assert(i < std::ssize(m_chedges));
	return *(m_chedges.cbegin()+i);
}

inline void Ustring::erase(ssize_t first, ssize_t end)
{
	markup(e_characters);
	auto beg = first>=std::ssize(m_chedges) ? m_str.cend() :
		(m_chedges.begin()+first)->first;
	auto end_it = end>=std::ssize(m_chedges) ? m_str.cend() :
		(m_chedges.begin()+end)->first;
	m_str.erase(beg, end_it);
	m_insert_pos = -1;
	m_markup_state = e_unmarked;
}

inline ssize_t Ustring::ssize()
{
	markup(e_characters);
	return std::ssize(m_chedges);
}

template <typename CharT>
void Ustring::cvt(std::basic_string<CharT> *o_dest)
{
	o_dest->clear();
	if (! m_str.empty())
		g_cvt_from_wide(m_str, o_dest, *m_loc);
}

inline void Ustring::markup(markup_e m)
{
	using namespace boost::locale;
	if (m_markup_state & m)
		return;
	if (m == e_characters) {
		m_characters.map(boundary::character, m_str.begin(), m_str.end(),
				*m_loc);
		m_chedges.clear();
		m_chedges.reserve(m_str.size());
		auto char_it_end = m_characters.end();
		for (auto char_it = m_characters.begin();  char_it != char_it_end;
				++char_it)
			m_chedges.emplace_back(char_it->begin(), char_it->end());
	}
	else {
		m_words.map(boundary::word, m_str.begin(), m_str.end(), *m_loc);
		m_words.rule(boundary::word_any);
	}
	m_markup_state |= m;
}


} //sfpvk::utils

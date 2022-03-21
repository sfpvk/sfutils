#include <string>
#include <utility>
#include <locale>
#include <tuple>
#include <vector>
#include <utility>
#include <cassert>
#include <type_traits>
#include <concepts>
#include <algorithm>
#include <string_view>
#include <iostream>
#include "encoding_cvt.hpp"
#include "../sfutils_base.hpp"


namespace sfpvk::utils::unicode {


enum class segment_type_e
{
	character,
	word,
	sentence,
	line
};

enum normalization_mode_e
{
	// Canonical decomposition.
	e_nfd,
	// Canonical decomposition followed by canonical composition.
	e_nfc,
	// Compatibility decomposition.
	e_nfkd,
	// Compatibility decomposition followed by canonical composition.
	e_nfkc
};

enum comparison_mode_e
{
	// Ignore accents and character case, comparing base letters only.
	// For example "facade" and "Façade" are the same.
	e_primary,
	// Ignore character case but consider accents. "facade" and
	// "façade" are different but "Façade" and "façade" are the same.
	e_secondary,
	// Consider both case and accents: "Façade" and "façade"
	// are different. Ignore punctuation.
	e_tertiary,
	// Consider all case, accents, and punctuation. The words 
	// must be identical in terms of Unicode representation.
	e_quaternary,
	// As quaternary, but compare code points as well.
	e_identical
};
//----------------------------------------------------------------------------+
//                                                                            |
//----------------------------------------------------------------------------+
#define SFPVK_VAL(val, reference_value)\
	([](){static_assert(boost::locale::boundary::val==reference_value);}(),\
		boost::locale::boundary::val)
using segment_rule_t = unsigned long;
enum word_rules_e : segment_rule_t
{
	// Not a word, like white space or punctuation mark.
	e_word_none      = SFPVK_VAL(word_none,      0x0000F),
	// Word that appear to be a number.
	e_word_number    = SFPVK_VAL(word_number,    0x000F0),
	// Word that contains letters, excluding kana and ideographic characters.
	e_word_letter    = SFPVK_VAL(word_letter,    0x00F00),
	// Word that contains kana characters.
	e_word_kana      = SFPVK_VAL(word_kana,      0x0F000),
	// Word that contains ideographic characters.
	e_word_ideo      = SFPVK_VAL(word_ideo,      0xF0000),
	// Any word including numbers, 0 is special flag, equivalent to 15.
	e_word_any       = SFPVK_VAL(word_any,       0xFFFF0),
	// Any word, excluding numbers but including letters, kana and ideograms.
	e_word_letters   = SFPVK_VAL(word_letters,   0xFFF00),
	// Word that includes kana or ideographic characters.
	e_word_kana_ideo = SFPVK_VAL(word_kana_ideo, 0xFF000),
	e_word_mask      = SFPVK_VAL(word_mask,      0xFFFFF)
};

enum line_break_rules_e : segment_rule_t
{
	// Soft line break: optional but not required.
	e_line_soft = SFPVK_VAL(line_soft, 0x0F),
	// Hard line break: like break is required (as per CR/LF)
	e_line_hard = SFPVK_VAL(line_hard, 0xF0),
	e_line_mask = SFPVK_VAL(line_mask, 0xFF),
};

enum sentence_rules_e : segment_rule_t
{
	// The sentence was terminated with a sentence terminator like
	// ".", "!" possible followed by hard separator like CR, LF, PS.
	e_sentence_term = SFPVK_VAL(sentence_term, 0x0F),
	// The sentence does not contain terminator like ".", "!" but
	// ended with hard separator like CR, LF, PS or end of input.
	e_sentence_sep  = SFPVK_VAL(sentence_sep,  0xF0),
	// Select all sentence breaking points.
	e_sentence_mask = SFPVK_VAL(sentence_mask, 0xFF)
};
#undef SFPVK_VAL

//----------------------------------------------------------------------------+
//                                                                            |
//----------------------------------------------------------------------------+
template <typename T>
concept Wchar_string = (std::same_as<std::decay_t<
			std::remove_pointer_t<std::decay_t<T>>>, wchar_t>  ||
		std::same_as<typename std::remove_cvref_t<T>::value_type, wchar_t>);

template <typename T>
concept Wchar_std_string = Wchar_string<T>  &&
		!std::same_as<typename std::remove_cvref_t<T>, class Ustring>;

template <typename T>
concept Non_wchar_std_string = !Wchar_string<T>  &&
		!std::same_as<typename std::remove_cvref_t<T>, class Ustring>;

struct Segment
{
	std::wstring::const_iterator begin;
	std::wstring::const_iterator end;
	segment_rule_t type;
};

class Ustring
{
	struct Segment_internal
	{
		ssize_t begin;
		ssize_t end;
		segment_rule_t type;
	};
public:
	// Position measured in code points.
	using cpnt_pos = ssize_t;
	// Position measured in segments.
	// A segment type is one of segment_type_e enum, it is set
	// by set_segment_type() call. Default type is character.
	using sgmnt_pos = ssize_t;
	template <Wchar_std_string T>
		Ustring &operator=(T &&str);
	template <Non_wchar_std_string T>
		Ustring &operator=(T &&str);
	template <Wchar_std_string T>
		Ustring &operator+=(const T &str);
	template <Non_wchar_std_string T>
		Ustring &operator+=(const T &str);
	Ustring &operator+=(const Ustring &str);
	// internally uses std::wstring::operator<=>
	auto operator<=>(const Ustring &other)const;
	// internally uses std::wstring::operator==
	bool operator==(const Ustring &other)const;
	// r_loc - locale generated by boost::locale::generator,
	// will be stored inside. 
	Ustring(const std::locale *r_loc);
	template <typename T>
		Ustring(T &&str, const std::locale *r_loc);
	// Converts the internal string to o_dest.
	template <typename CharT>
		void cvt(std::basic_string<CharT> *o_dest);
	cpnt_pos ssize_in_codepoints()const;
	sgmnt_pos ssize_in_segments();
	// 0 <= pos <= ssize_in_segments().
	// Returns a first code point index of a segment with index pos
	// or ssize_in_codepoints().
	cpnt_pos sgmnt_to_cpntpos(sgmnt_pos pos);
	// 0 <= pos <= ssize_in_codepoints().
	// Find a first segment following a position pos.
	// If pos is inside a segment, then a index of
	// this segment is returned.
	sgmnt_pos cpnt_to_sgmntpos(cpnt_pos pos);
	// 0 <= i <= ssize_in_segments(). If i == ssize_in_segments(), then
	// returns {{wstring::end(), wstring::end()}, 0}.
	Segment operator[](sgmnt_pos i);
	// Sets a type of segments that are traversed with operator[].
	// For segment_type_e::character the rule_mask is ignored.
	// full_select - see the reference information for
	// boost::locale::boundary::segment_index::full_select.
	// Default is (segment_type_e::character, 0, false).
	void set_segment_type(segment_type_e type,
			segment_rule_t rule_mask,
			bool full_select);
	void reserve(ssize_t sz);
	void insert(cpnt_pos index, wchar_t ch);
	// Insert from any C or std string.
	template <typename T>
		requires requires {typename T::value_type;}
		void insert(cpnt_pos index, const T &str);
	template <typename T>
		void insert(cpnt_pos index, const T &str);
	void push_back(wchar_t c);
	void erase(cpnt_pos first, cpnt_pos end);
	void clear();
	const std::wstring &base_wstring()const &;
	std::wstring base_wstring()&&;
	void normalize(normalization_mode_e m=e_nfc);
	void fold_case();
	void to_title();
	void to_lower();
	void to_upper();
	// Returns negative value if *this appears before,
	// zero if both are equivalent,
	// positive value if *this appears after.
	bool compare(comparison_mode_e m, const std::wstring &str)const;
private:
	ssize_t find_pos(cpnt_pos pos);
	void markup();
	std::wstring m_str;
	std::vector<Segment_internal> m_segment_edges;
	const std::locale *m_loc;
	bool m_markup_state;
	segment_type_e m_markup_mode;
	segment_rule_t m_rule_mask;
	bool m_full_select;
};

template <Wchar_std_string T>
Ustring &Ustring::operator=(T &&str)
{
	m_str = std::forward<T>(str);
	m_markup_state = false;
	return *this;
}

template <Non_wchar_std_string T>
Ustring &Ustring::operator=(T &&str)
{
	g_cvt_to_wide(str, &m_str, *m_loc);
	return operator=(m_str);
}

template <Wchar_std_string T>
inline Ustring &Ustring::operator+=(const T &str)
{
	m_str += str;
	m_markup_state = false;
	return *this;
}

template <Non_wchar_std_string T>
Ustring &Ustring::operator+=(const T &str)
{
	std::wstring buf;
	g_cvt_to_wide(str, &buf, *m_loc);
	return operator+=(buf);
}

inline Ustring &Ustring::operator+=(const Ustring &str)
{
	return operator+=(str.m_str);
}

inline auto Ustring::operator<=>(const Ustring &other)const
{
	return m_str <=> other.m_str;
}

inline bool Ustring::operator==(const Ustring &other)const
{
	return m_str == other.m_str;
}

inline Ustring::Ustring(const std::locale *r_loc) :
	m_loc{r_loc},
	m_markup_state{false},
	m_markup_mode{segment_type_e::character},
	m_rule_mask{boost::locale::boundary::character_mask},
	m_full_select{true}
{
}

template <typename T>
Ustring::Ustring(T &&str, const std::locale *r_loc) : Ustring{r_loc}
{
	operator=(std::forward<T>(str));
}

template <typename CharT>
void Ustring::cvt(std::basic_string<CharT> *o_dest)
{
	o_dest->clear();
	if (! m_str.empty())
		g_cvt_from_wide(m_str, o_dest, *m_loc);
}

inline Ustring::cpnt_pos Ustring::ssize_in_codepoints()const
{
	return std::ssize(m_str);
}

inline Ustring::sgmnt_pos Ustring::ssize_in_segments()
{
	markup();
	return std::ssize(m_segment_edges);
}

inline Ustring::cpnt_pos Ustring::sgmnt_to_cpntpos(sgmnt_pos pos)
{
	markup();
	assert(pos <= std::ssize(m_segment_edges));
	if (pos == std::ssize(m_segment_edges))
		return std::ssize(m_str);
	return m_segment_edges[sig(pos)].begin;
}

inline Ustring::sgmnt_pos Ustring::cpnt_to_sgmntpos(cpnt_pos pos)
{
	assert(pos <= std::ssize(m_str));
	markup();
	return find_pos(pos);
}

inline Segment Ustring::operator[](sgmnt_pos i)
{
	markup();
	assert(i <= std::ssize(m_segment_edges));
	if (i == std::ssize(m_segment_edges))
		return {m_str.end(), m_str.end(), 0};
	const auto &[begin, end, rule_type] = m_segment_edges[sig(i)];
	return {m_str.begin()+begin, m_str.begin()+end, rule_type};
}

inline void Ustring::set_segment_type(segment_type_e type,
		segment_rule_t rule_mask,
		bool full_select)
{
	m_markup_state = false;
	m_markup_mode = type;
	if (type == segment_type_e::character)
		m_rule_mask = boost::locale::boundary::character_mask;
	else
		m_rule_mask = rule_mask;
	m_full_select = full_select;
}

inline void Ustring::reserve(ssize_t sz)
{
	m_str.reserve(sig(sz));
}

inline void Ustring::insert(cpnt_pos index, wchar_t ch)
{
	assert(index <= std::ssize(m_str));
	m_str.insert(index, 1, ch);
	m_markup_state = false;
}

template <typename T>
requires requires {typename T::value_type;}
void Ustring::insert(cpnt_pos index, const T &str)
{
	assert(index <= std::ssize(m_str));
	if (str.empty())
		return;
	if constexpr (std::is_same_v<typename T::value_type, wchar_t>)
		m_str.insert(m_str.begin()+index, str.begin(), str.end());
	else {
		std::wstring buf;
		g_cvt_to_wide(str, &buf, *m_loc);
		m_str.insert(m_str.begin()+index, buf.begin(), buf.end());
	}
	m_markup_state = false;
}

template <typename T>
inline void Ustring::insert(cpnt_pos index, const T &str)
{
	static_assert(std::is_pointer_v<std::decay_t<T>>, "T is not pointer");
	using pureT_t = std::decay_t<std::remove_pointer_t<std::decay_t<T>>>;
	if constexpr (std::is_same_v<pureT_t, wchar_t>)
		insert(index, std::wstring_view(str));
	else {
		std::wstring buf;
		g_cvt_to_wide(str, &buf, *m_loc);
		insert(index, buf);
	}
}

inline void Ustring::push_back(wchar_t c)
{
	m_str += c;
	m_markup_state = false;
}

inline void Ustring::erase(cpnt_pos first, cpnt_pos end)
{
	assert(first <= std::ssize(m_str)  &&
			end <= std::ssize(m_str));
	m_str.erase(m_str.begin()+first, m_str.begin()+end);
	m_markup_state = false;
}

inline void Ustring::clear()
{
	m_str.clear();
	m_markup_state = false;
}

inline const std::wstring &Ustring::base_wstring()const &
{
	return m_str;
}

inline std::wstring Ustring::base_wstring()&&
{
	m_markup_state = false;
	return std::move(m_str);
}

inline void Ustring::normalize(normalization_mode_e m)
{
	boost::locale::norm_type bm = boost::locale::norm_nfc;
	switch (m) {
	case e_nfd:
		bm = boost::locale::norm_nfd; break;
	case e_nfc:
		bm = boost::locale::norm_nfc; break;
	case e_nfkd:
		bm = boost::locale::norm_nfkd; break;
	case e_nfkc:
		bm = boost::locale::norm_nfkc;
	}
	m_str = boost::locale::normalize(m_str, bm, *m_loc);
	m_markup_state = false;
}

inline void Ustring::fold_case()
{
	m_str = boost::locale::fold_case(m_str, *m_loc);
	m_markup_state = false;
}

inline void Ustring::to_title()
{
	m_str = boost::locale::to_title(m_str, *m_loc);
	m_markup_state = false;
}

inline void Ustring::to_lower()
{
	m_str = boost::locale::to_lower(m_str, *m_loc);
	m_markup_state = false;
}

inline void Ustring::to_upper()
{
	m_str = boost::locale::to_upper(m_str, *m_loc);
	m_markup_state = false;
}

inline bool Ustring::compare(comparison_mode_e m,
		const std::wstring &str)const
{
	boost::locale::collator_base::level_type lev =
		boost::locale::collator_base::primary;
	switch (m) {
	case e_primary:
		lev = boost::locale::collator_base::primary; break;
	case e_secondary:
		lev = boost::locale::collator_base::secondary; break;
	case e_tertiary:
		lev = boost::locale::collator_base::tertiary; break;
	case e_quaternary:
		lev = boost::locale::collator_base::quaternary; break;
	case e_identical:
		lev = boost::locale::collator_base::identical;
	}
	return use_facet<boost::locale::collator<wchar_t>>(*m_loc).compare(
			lev, m_str, str);
}

inline ssize_t Ustring::find_pos(cpnt_pos pos)
{
	auto res = std::ranges::upper_bound(m_segment_edges, pos, {},
			[](const auto &r) {
				return r.begin;});
	if (res != m_segment_edges.begin()  &&
			pos < (res - 1)->end)
		-- res;
	return res - m_segment_edges.begin();
}

inline void Ustring::markup()
{
	using namespace boost::locale;
	if (m_markup_state)
		return;

	auto mode = boundary::word;
	switch (m_markup_mode) {
	case segment_type_e::character:
		mode = boundary::character;
		break;
	case segment_type_e::word:
		mode = boundary::word;
		break;
	case segment_type_e::sentence:
		mode = boundary::sentence;
		break;
	case segment_type_e::line:
		mode = boundary::line;
	}

	static thread_local boundary::wssegment_index segments;
	segments.map(mode, m_str.begin(), m_str.end(), *m_loc);
	segments.rule(m_rule_mask);
	segments.full_select(m_full_select);
	m_segment_edges.clear();
	auto it_end = segments.end();
	for (auto it = segments.begin();  it != it_end;  ++it)
		m_segment_edges.push_back({it->begin() - m_str.begin(),
				it->end() - m_str.begin(),
				it->rule()});

	m_markup_state = true;
}

std::wostream& operator<<(std::wostream &stream, const Ustring &str)
{
	stream << str.base_wstring();
    return stream;
}


} //sfpvk::utils::unicode

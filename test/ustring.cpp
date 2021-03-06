#include <sfutils/unicode/unicode_string.hpp>
#include "base.hpp"

using namespace sfpvk::utils::unicode;


template <ssize_t Sz, typename T>
bool compare_helper(const Ustring<Sz> &us, const T *r)
{
	std::basic_string<T> buf;
	us.to_string(&buf);
	return buf == r;
}

bool subtest_1()
{
// 16 code pints, 14 graphemes.
#define TEST_STR "hello\U0001F595\U0001F3FFworld\u0418\u0306!!"
#define MAKE_TEST_STR_(prefix, str) prefix ## str
#define MAKE_TEST_STR(prefix, str) MAKE_TEST_STR_(prefix, str)
	Ustring us{MAKE_TEST_STR(u8, TEST_STR)};
	TEST(us.ssize() == 14);
	TEST(compare_helper(us, MAKE_TEST_STR(u8, TEST_STR)));
	TEST(compare_helper(us, MAKE_TEST_STR(u, TEST_STR)));
	TEST(compare_helper(us, MAKE_TEST_STR(U, TEST_STR)));
	TEST(compare_helper(us, MAKE_TEST_STR(L, TEST_STR)));

	std::u32string buf;
	us.to_string(&buf);
	TEST(buf.size() == 16);

	Ustring us2;
	TEST(us2.empty());
	TEST(! us2.ssize());
	us2 = "";
	TEST(us2.empty());
	TEST(! us2.ssize());
	us2 = "hello";
	us2 += " world";
	TEST(compare_helper(us2, "hello world")  &&
			us2.size() == 11);

	return true;
#undef TEST_STR
#undef MAKE_TEST_STR_
#undef MAKE_TEST_STR
}

bool subtest_2()
{
	Ustring us{L"qw\u0418\u0306er"};
	TEST(us.size() == 5);
	for (auto gr_it = us.begin();  gr_it != us.end();  ++gr_it)
		for (auto cp = *gr_it;  cp != End_iterator_tag{};  ++cp)
			*cp = '0' + gr_it - us.begin();
	TEST(compare_helper(us, "012234"));

	for (auto cp_it = us[2];  cp_it != End_iterator_tag{};  ++cp_it)
		*cp_it = '0' + 7;
	TEST(compare_helper(us, "017734"));

	auto gr_it = us.end();
	-- gr_it;
	TEST(**(gr_it -= 4) == '0');
	TEST(**(gr_it += 1) == '1');
	++ gr_it;
	**gr_it-- = '6';
	**gr_it++ = '3';
	**(gr_it-2) = '8';
	gr_it += 1;
	*gr_it[1] = '0';
	gr_it -= 1;
	*gr_it[1] = '2';
	TEST(compare_helper(us, "836720"));

	auto cp_it = us[2];
	*(cp_it++) = 'z';
	*(cp_it--) = 'q';
	TEST(compare_helper(us, "83zq20"));
	*(++cp_it) = 'v';
	*(--cp_it) = 'x';
	TEST(compare_helper(us, "83xv20"));

	return true;
}

bool subtest_3()
{
	Ustring us{"hello world"};
	TEST(us.insert(1, U"\u0418\u0306") == 1);
	TEST(compare_helper(us, L"h\u0418\u0306ello world"));
	us.erase(1, 2);
	TEST(compare_helper(us, L"hello world"));
	TEST(us.insert(1, U"\u0418") == 1);
	TEST(us.insert(2, U"\u0306") == 0);
	TEST(compare_helper(us, L"h\u0418\u0306ello world")  &&
			us.size() == 12);
	TEST(! us.empty());
	us.erase(1, 2);
	TEST(us.insert(1, U"\u0418") == 1);
	us.close_grapheme();
	TEST(us.insert(2, U"\u0306") == 1);
	TEST(compare_helper(us, L"h\u0418\u0306ello world")  &&
			us.size() == 13);
	us.clear();
	TEST(! us.size());
	TEST(us.empty());
	TEST(us.push_back(U"\u0418") == 1);
	TEST(us.push_back(U"\u0306") == 0);
	TEST(us.push_back(U"\U0001F595") == 1);
	TEST(us.push_back(U"\U0001F3FF") == 0);
	TEST(compare_helper(us, L"\u0418\u0306\U0001F595\U0001F3FF")  &&
			us.size() == 2);

	Ustring us2{"pre||suf"};
	TEST(us2.insert(4, us.begin()) == 1);
	us2 += us.begin() + 1;
	TEST(compare_helper(us2, L"pre|\u0418\u0306|suf\U0001F595\U0001F3FF")  &&
			us2.size() == 10);

	us = L'\u0306';
	us2 = L'\u0306';
	us2.push_back(us.begin());
	TEST(us2.size() == 2);
	us2 = L'\u0306';
	us2 += L'\u0306';
	TEST(us2.size() == 1);

	return true;
}

bool subtest_4()
{
	Ustring us{U"\U0001F610\U0001F3FC"};
	us.push_back(U'\U0001F3FC');
	us += U'\U0001F3FC';
	us.insert(1, U'\U0001F3FC');
	us.push_back(U'\U0001F3FC');
	us += U'\U0001F3FC';
	us.insert(1, U'\U0001F3FC');
	TEST(us.size() == 1);
	us.push_back('=');
	TEST(us.size() == 2);
	TEST(compare_helper(us, U"\U0001F610\U0001F3FC"
				U"\U0001F3FC\U0001F3FC\U0001F3FC="));

	return true;
}

bool subtest_5()
{
	Ustring<1> us1{U"\U0001F610\U0001F3FC\U0001F3FC\U0001F3FC\U0001F3FC="};
	Ustring<2> us2;
	Ustring<4> us4{U"\U0001F610\U0001F3FC\U0001F3FC\U0001F3FC\U0001F3FC="};
	TEST(us1.size() == 2  &&  us4.size() == 2);
	us2.insert(0, us1.begin());
	us2.push_back(us1.begin() + 1);
	us1.clear();
	us1.push_back(us4.begin());
	us1.push_back(us4.begin() + 1);
	TEST(us1.size() == 2  &&  us2.size() == 2  &&  us4.size() == 2);
	TEST(compare_helper(us1, U"\U0001F610="));
	TEST(compare_helper(us2, U"\U0001F610="));
	TEST(compare_helper(us4, U"\U0001F610\U0001F3FC\U0001F3FC\U0001F3FC="));

	return true;
}

int main()
{
	if (! subtest_1())
		return 1;
	if (! subtest_2())
		return 1;
	if (! subtest_3())
		return 1;
	if (! subtest_4())
		return 1;
	if (! subtest_5())
		return 1;
}

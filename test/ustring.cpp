#include <iostream>
#include <sfutils_unicode/unicode_string.hpp>


#define STR_(a) #a
#define STR(a) STR_(a)
#define TEST(e)\
	if (!(e)) {\
		std::cerr << "Test at line " STR(__LINE__) " is failed" << std::endl;\
		return false;\
	}

bool subtest_1()
{
	using namespace sfpvk::utils::unicode;
	boost::locale::generator gen;
	std::locale loc(gen("en_US.utf8"));

	Ustring us_1(&loc);
	Ustring us_2(&loc);
	
	us_1 = "Hellow world";

	us_2 = L"Hellow";
	us_2 += std::string(" ");
	us_2 += u8"wo";
	us_2.push_back('r');
	us_2.insert(us_2.ssize_in_codepoints(), L'l');
	us_2.insert(us_2.ssize_in_codepoints(), std::wstring(L"d"));
	// Add Emoji - 1 character, 2 - code points.
	us_2.insert(us_2.ssize_in_codepoints(),
			std::wstring(L"\U0001F595\U0001F3FF"));

	TEST(us_2.ssize_in_codepoints() == us_2.ssize_in_segments()+1);

	auto us2_begin = us_2.base_wstring().begin();
	us_2.erase(us_2[us_2.ssize_in_segments()-1].begin - us2_begin,
			us_2[us_2.ssize_in_segments()-1].end - us2_begin);

	TEST(us_1 == us_2);

	us_1.clear();
	TEST(us_1.ssize_in_codepoints() == 0  &&
			us_1.ssize_in_segments() == 0);
	us_1 = us_2;
	TEST(us_1 == us_2);

	return true;
}

bool subtest_2()
{
	using namespace sfpvk::utils::unicode;
	boost::locale::generator gen;
	std::locale loc(gen("en_US.utf8"));

	Ustring us(&loc);
	for (int i = 0;  i < 3;  ++i)
		us.insert(us.ssize_in_codepoints(),
				std::wstring(L"\U0001F595\U0001F3FF"));

	TEST(us.ssize_in_codepoints() == 6  &&  us.ssize_in_segments() == 3);

	// Add Emoji - 1 character, 2 - code points.
	TEST(std::wstring(us[0].begin, us[0].end) == L"\U0001F595\U0001F3FF");
	
	for (int i = 0;  i <= 3;  ++i)
		TEST(us.sgmnt_to_cpntpos(i) == i * 2);

	for (int i = 0;  i <= 6;  ++i)
		TEST(us.cpnt_to_sgmntpos(i) == i / 2);

	return true;
}

bool subtest_3()
{
	using namespace sfpvk::utils::unicode;
	boost::locale::generator gen;
	std::locale loc(gen("en_US.utf8"));

	Ustring us(&loc);
	us = "World, hello! 23";
	us.set_segment_type(segment_type_e::word, e_word_mask, false);
	TEST(us.ssize_in_segments() == 7);

	TEST(us[0].type == e_word_letter  &&
			std::wstring(us[0].begin, us[0].end) == L"World");
	TEST(us[1].type == e_word_none  &&
			std::wstring(us[1].begin, us[1].end) == L",");
	TEST(us[2].type == e_word_none  &&
			std::wstring(us[2].begin, us[2].end) == L" ");
	TEST(us[3].type == e_word_letter  &&
			std::wstring(us[3].begin, us[3].end) == L"hello");
	TEST(us[4].type == e_word_none  &&
			std::wstring(us[4].begin, us[4].end) == L"!");
	TEST(us[5].type == e_word_none  &&
			std::wstring(us[5].begin, us[5].end) == L" ");
	TEST(us[6].type == e_word_number  &&
			std::wstring(us[6].begin, us[6].end) == L"23");

	return true;
}

bool subtest_4()
{
	using namespace sfpvk::utils::unicode;
	boost::locale::generator gen;
	std::locale loc(gen("en_US.utf8"));

	Ustring us(&loc);
	us = "Hello world!";
	std::u8string u8str;
	us.cvt(&u8str);
	TEST(u8str == u8"Hello world!");

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
}

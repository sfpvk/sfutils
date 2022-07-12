#include <sfutils/unicode/encoding_cvt.hpp>
#include "base.hpp"


using namespace sfpvk::utils::unicode;

bool subtest_1()
{
	std::u32string u32n = U"hello";
	for (int i = 0;  i < 30;  ++i) {
		u32n.append(550, U'\U00030000');
		u32n += 'q';
	}

	std::u32string u32l;
	std::u32string u32b;
	TEST(g_cvt<Cvt_endian::from_native|Cvt_endian::to_little>("", &u32l)
			== 0);
	TEST(g_cvt<Cvt_endian::from_native|Cvt_endian::to_little>(u32n, &u32l)
			== std::ssize(u32n));
	TEST(g_cvt<Cvt_endian::from_native|Cvt_endian::to_big>(u32n, &u32b)
			== std::ssize(u32n));
	TEST(u32l != u32b);

	std::u16string u16l;
	std::u16string u16b;
	TEST(g_cvt<Cvt_endian::from_little|Cvt_endian::to_little>(u32l, &u16l)
			== std::ssize(u32l));
	TEST(g_cvt<Cvt_endian::from_big|Cvt_endian::to_big>(u32b, &u16b)
			== std::ssize(u32b));
	TEST(u16l.find(u"hello") != std::u16string::npos
			||  u16b.find(u"hello") != std::u16string::npos);
	TEST(u16l != u16b);

	std::u8string u8l;
	std::u8string u8b;
	TEST(g_cvt<Cvt_endian::from_little|Cvt_endian::to_little>(u16l, &u8l)
			== std::ssize(u16l));
	TEST(g_cvt<Cvt_endian::from_big|Cvt_endian::to_big>(u16b, &u8b)
			== std::ssize(u16b));
	TEST(u8l.find(u8"hello") != std::u8string::npos
			||  u8b.find(u8"hello") != std::u16string::npos);
	TEST(u8l == u8b);

	std::string ul;
	std::string ub;
	TEST(g_cvt<Cvt_endian::from_little|Cvt_endian::to_little>(u8l, &ul)
			== std::ssize(u8l));
	TEST(g_cvt<Cvt_endian::from_big|Cvt_endian::to_big>(u8b, &ub)
			== std::ssize(u8b));
	TEST(ul.find("hello") != std::string::npos
			||  ub.find("hello") != std::u16string::npos);
	TEST(ul == ub);

	std::wstring uwl;
	std::wstring uwb;
	TEST(g_cvt<Cvt_endian::from_little|Cvt_endian::to_native>(ul, &uwl)
			== std::ssize(ul));
	TEST(g_cvt<Cvt_endian::from_big|Cvt_endian::to_native>(ub, &uwb)
			== std::ssize(ub));
	TEST(uwl.find(L"hello") != std::wstring::npos
			||  uwb.find(L"hello") != std::u16string::npos);
	TEST(uwl == uwb);

	std::u32string u32res_l;
	std::u32string u32res_b;
	TEST(g_cvt<Cvt_endian::from_native|Cvt_endian::to_little>(uwl, &u32res_l)
			== std::ssize(uwl));
	TEST(g_cvt<Cvt_endian::from_native|Cvt_endian::to_big>(uwb, &u32res_b)
			== std::ssize(uwb));

	TEST(u32l == u32res_l);
	TEST(u32b == u32res_b);
	return true;
}

bool subtest_2()
{
	std::u32string u32;
	std::u8string u8ref;
	for (int i = 700;  i >= 0;  --i) {
		u32 += U'\U00030000';
		if (i != 0)
			u8ref += u8"\U00030000";
	}

	std::u8string u8;
	TEST(g_cvt(u32, &u8, (u32.size()-1)*4) == std::ssize(u32)-1);
	TEST(u8ref == u8);
	u8.clear();
	TEST(g_cvt(u32, &u8, 0) == 0);
	TEST(g_cvt(u32, &u8, 3) == 0);
	TEST(g_cvt(u32, &u8, 6) == 1);
	TEST(u8 == u8"\U00030000");

	u8.clear();
	g_cvt(u32, &u8);
	std::u32string u32cp(u32);
	u32.clear();
	TEST(g_cvt(u8, &u32, 0) == 0);
	TEST(u32.empty());
	TEST(g_cvt(u8, &u32, u32cp.size()) == std::ssize(u32cp)*4);
	TEST(u32 == u32cp);
	u32.clear();
	TEST(g_cvt(u8, &u32, u32cp.size()-1) == (std::ssize(u32cp)-1)*4);
	u32cp.pop_back();
	TEST(u32 == u32cp);
	u32.clear();
	TEST(g_cvt(u8, &u32, 1) == 4);
	TEST(u32 == U"\U00030000");

	return true;
}

int main()
{
	if (! subtest_1())
		return 1;
	if (! subtest_2())
		return 1;
}

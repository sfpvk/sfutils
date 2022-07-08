#include <sfutils/unicode/encoding_cvt.hpp>
#include "base.hpp"


using namespace sfpvk::utils::unicode;

bool subtest_1()
{
	std::u32string u32n = U"hello";
	for (int i = 0;  i < 30;  ++i) {
		u32n.append(U'\U00030000', 550);
		u32n += 'q';
	}

	std::u32string u32l;
	std::u32string u32b;
	TEST(g_cvt<Cvt_endian::from_native|Cvt_endian::to_little>(u32n, &u32l));
	TEST(g_cvt<Cvt_endian::from_native|Cvt_endian::to_big>(u32n, &u32b));
	TEST(u32l != u32b);

	std::u16string u16l;
	std::u16string u16b;
	TEST(g_cvt<Cvt_endian::from_little|Cvt_endian::to_little>(u32l, &u16l));
	TEST(g_cvt<Cvt_endian::from_big|Cvt_endian::to_big>(u32b, &u16b));
	TEST(u16l.find(u"hello") != std::u16string::npos
			||  u16b.find(u"hello") != std::u16string::npos);
	TEST(u16l != u16b);

	std::u8string u8l;
	std::u8string u8b;
	TEST(g_cvt<Cvt_endian::from_little|Cvt_endian::to_little>(u16l, &u8l));
	TEST(g_cvt<Cvt_endian::from_big|Cvt_endian::to_big>(u16b, &u8b));
	TEST(u8l.find(u8"hello") != std::u8string::npos
			||  u8b.find(u8"hello") != std::u16string::npos);
	TEST(u8l == u8b);

	std::string ul;
	std::string ub;
	TEST(g_cvt<Cvt_endian::from_little|Cvt_endian::to_little>(u8l, &ul));
	TEST(g_cvt<Cvt_endian::from_big|Cvt_endian::to_big>(u8b, &ub));
	TEST(ul.find("hello") != std::string::npos
			||  ub.find("hello") != std::u16string::npos);
	TEST(ul == ub);

	std::wstring uwl;
	std::wstring uwb;
	TEST(g_cvt<Cvt_endian::from_little|Cvt_endian::to_native>(ul, &uwl));
	TEST(g_cvt<Cvt_endian::from_big|Cvt_endian::to_native>(ub, &uwb));
	TEST(uwl.find(L"hello") != std::wstring::npos
			||  uwb.find(L"hello") != std::u16string::npos);
	TEST(uwl == uwb);

	std::u32string u32res_l;
	std::u32string u32res_b;
	TEST(g_cvt<Cvt_endian::from_native|Cvt_endian::to_little>(uwl, &u32res_l));
	TEST(g_cvt<Cvt_endian::from_native|Cvt_endian::to_big>(uwb, &u32res_b));

	TEST(u32l == u32res_l);
	TEST(u32b == u32res_b);
	return true;
}

int main()
{
	if (! subtest_1())
		return 1;
}

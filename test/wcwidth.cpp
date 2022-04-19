#include <iostream>
#include <clocale>
#include <sfutils/unicode/usym_width.hpp>

using namespace sfpvk::utils::unicode;


#define STR_(a) #a
#define STR(a) STR_(a)
#define TEST(e)\
	if (!(e)) {\
		std::cerr << "Test at line " STR(__LINE__) " is failed" << std::endl;\
		return false;\
	}

bool subtest_1()
{
	TEST(g_usym_width(U'q') == 1);
	
	return true;
}

int main()
{
	setlocale(LC_ALL, "en_US.UTF-8");
	if (! subtest_1())
		return 1;
}

#include <sfutils/unicode/usym_width.hpp>
#include "base.hpp"

using namespace sfpvk::utils::unicode;


bool subtest_1()
{
	TEST(g_usym_width(U'q') == 1);
	
	return true;
}

int main()
{
	if (! subtest_1())
		return 1;
}

#pragma once
#include <iostream>

#define STR_(a) #a
#define STR(a) STR_(a)
#define TEST(e)\
	if (!(e)) {\
		std::cerr << "Test at line " STR(__LINE__) " is failed" << std::endl;\
		return false;\
	}

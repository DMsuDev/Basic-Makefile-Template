#include <iostream>
#include "math_test.h"

int main()
{
	int a = 5;
	int b = 7;

	std::cout << "Testing math library:\n";
	std::cout << "add(" << a << ", " << b << ") = " << math::add(a, b) << "\n";
	std::cout << "multiply(" << a << ", " << b << ") = " << math::multiply(a, b) << "\n";

	return 0;
}
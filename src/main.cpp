// Tiny main that delegates to the donut module (keeps makefile demo visible)
#include <iostream>
#include <algorithm>
#include <donut.h>

int main(int argc, char** argv)
{
	donut::Config cfg;
	if (argc >= 3)
	{
		cfg.width = std::max(20, std::atoi(argv[1]));
		cfg.height = std::max(10, std::atoi(argv[2]));
	}
	if (argc >= 5)
	{
		cfg.frames = std::atoi(argv[3]);
		cfg.fps = std::max(1, std::atoi(argv[4]));
	}

	donut::run(cfg);
	return 0;
}
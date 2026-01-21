// Donut ASCII demo (rotating torus) - improved: constants, z-buffer, single-write output, clamp, FPS control
#include <iostream>
#include <cmath>
#include <chrono>
#include <thread>
#include <vector>
#include <cstring>
#include <algorithm>
#include <limits>
#include <string>

struct Config
{
	int width = 80;
	int height = 24;
	float R2 = 2.0f;          // circle radius
	float K2 = 5.0f;          // depth offset
	float theta_step = 0.07f;
	float phi_step = 0.02f;
	float scale_x = 30.0f;
	float scale_y = 15.0f;
	int frames = 0;           // 0 = infinite
	int fps = 30;
};

int main(int argc, char** argv)
{
	Config cfg;
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

	constexpr float PI = 3.14159265f;
	const char* luminance = ".,-~:;=!*#$@";
	const int lum_len = static_cast<int>(std::strlen(luminance));

	const int width = cfg.width;
	const int height = cfg.height;
	const int screen_size = width * height;

	std::vector<char> output(screen_size);
	std::vector<float> zbuffer(screen_size);

	float A = 0.0f, B = 0.0f;

	using clock = std::chrono::steady_clock;
	const std::chrono::duration<double> target_frame_time(1.0 / cfg.fps);

	int frame = 0;
	while (cfg.frames == 0 || frame < cfg.frames)
	{
		auto frame_start = clock::now();

		std::fill(output.begin(), output.end(), ' ');
		std::fill(zbuffer.begin(), zbuffer.end(), -std::numeric_limits<float>::infinity());

		for (float theta = 0.0f; theta < 2.0f * PI; theta += cfg.theta_step)
		{
			for (float phi = 0.0f; phi < 2.0f * PI; phi += cfg.phi_step)
			{
				float sinTheta = std::sin(theta), cosTheta = std::cos(theta);
				float sinPhi = std::sin(phi), cosPhi = std::cos(phi);
				float sinA = std::sin(A), cosA = std::cos(A);
				float sinB = std::sin(B), cosB = std::cos(B);

				float h = cosPhi + cfg.R2; // distance from center of torus
				float denom = (sinTheta * h * sinA + sinPhi * cosA + cfg.K2);
				if (denom == 0.0f) continue;
				float D = 1.0f / denom;

				float t = sinTheta * h * cosA - sinPhi * sinA;

				int x = static_cast<int>(width/2 + cfg.scale_x * D * (cosTheta * h * cosB - t * sinB));
				int y = static_cast<int>(height/2 + cfg.scale_y * D * (cosTheta * h * sinB + t * cosB));

				if (y >= 0 && y < height && x >= 0 && x < width)
				{
					int o = x + width * y;
					float L = cosPhi * cosTheta * sinB - D * (sinPhi * cosB + sinTheta * cosA * sinB);
					int lum_index = static_cast<int>(std::lround(std::clamp(8.0f * L, 0.0f, static_cast<float>(lum_len - 1))));
					if (D > zbuffer[o])
					{
						zbuffer[o] = D;
						output[o] = luminance[lum_index];
					}
				}
			}
		}

		// build single string per frame to reduce I/O calls
		std::string frame_out;
		frame_out.reserve(screen_size + height);
		for (int i = 0; i < screen_size; ++i)
		{
			frame_out.push_back(output[i]);
			if ((i + 1) % width == 0) frame_out.push_back('\n');
		}

		// print once
		std::cout << "\x1b[H" << frame_out << std::flush;

		A += 0.08f;
		B += 0.03f;

		// frame timing
		auto elapsed = clock::now() - frame_start;
		if (elapsed < target_frame_time)
		{
			std::this_thread::sleep_for(target_frame_time - elapsed);
		}

		++frame;
	}

	return 0;
}
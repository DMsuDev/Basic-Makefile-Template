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

	// Precompute trig tables for theta/phi to reduce cost in inner loop
	const int theta_count = static_cast<int>(std::ceil(2.0f * PI / cfg.theta_step));
	const int phi_count = static_cast<int>(std::ceil(2.0f * PI / cfg.phi_step));
	std::vector<float> sinTheta(theta_count), cosTheta(theta_count);
	std::vector<float> sinPhi(phi_count), cosPhi(phi_count);
	for (int i = 0; i < theta_count; ++i)
	{
		float t = i * cfg.theta_step;
		sinTheta[i] = std::sin(t);
		cosTheta[i] = std::cos(t);
	}
	for (int j = 0; j < phi_count; ++j)
	{
		float p = j * cfg.phi_step;
		sinPhi[j] = std::sin(p);
		cosPhi[j] = std::cos(p);
	}

	// previous frame buffer for double-buffer diff
	std::vector<char> prev_output(screen_size, ' ');

	// clear screen and hide cursor
	std::cout << "\x1b[2J\x1b[?25l";

	int frame = 0;
	while (cfg.frames == 0 || frame < cfg.frames)
	{
		auto frame_start = clock::now();

		std::fill(output.begin(), output.end(), ' ');
		std::fill(zbuffer.begin(), zbuffer.end(), -std::numeric_limits<float>::infinity());

		float sinA = std::sin(A), cosA = std::cos(A);
		float sinB = std::sin(B), cosB = std::cos(B);

		for (int ti = 0; ti < theta_count; ++ti)
		{
			float sTheta = sinTheta[ti];
			float cTheta = cosTheta[ti];
			for (int pj = 0; pj < phi_count; ++pj)
			{
				float sPhi = sinPhi[pj];
				float cPhi = cosPhi[pj];

				float h = cPhi + cfg.R2; // distance from center of torus
				float denom = (sTheta * h * sinA + sPhi * cosA + cfg.K2);
				if (denom == 0.0f) continue;
				float D = 1.0f / denom;

				float t = sTheta * h * cosA - sPhi * sinA;

				int x = static_cast<int>(width/2 + cfg.scale_x * D * (cTheta * h * cosB - t * sinB));
				int y = static_cast<int>(height/2 + cfg.scale_y * D * (cTheta * h * sinB + t * cosB));

				if (y >= 0 && y < height && x >= 0 && x < width)
				{
					int o = x + width * y;
					float L = cPhi * cTheta * sinB - D * (sPhi * cosB + sTheta * cosA * sinB);
					int lum_index = static_cast<int>(std::lround(std::clamp(8.0f * L, 0.0f, static_cast<float>(lum_len - 1))));
					if (D > zbuffer[o])
					{
						zbuffer[o] = D;
						output[o] = luminance[lum_index];
					}
				}
			}
		}

		// compute diff against prev_output and build minimal update string
		std::string diff_out;
		diff_out.reserve(512);
		for (int row = 0; row < height; ++row)
		{
			int col = 0;
			while (col < width)
			{
				int idx = row * width + col;
				if (output[idx] == prev_output[idx])
				{
					++col;
					continue;
				}
				// start run
				int start = col;
				while (col < width && output[row*width + col] != prev_output[row*width + col]) ++col;
				int run_len = col - start;
				// move cursor (1-based)
				diff_out += "\x1b[" + std::to_string(row + 1) + ";" + std::to_string(start + 1) + "H";
				diff_out.append(&output[row*width + start], run_len);
				// update prev_output for that run
				for (int k = 0; k < run_len; ++k) prev_output[row*width + start + k] = output[row*width + start + k];
			}
		}

		// write diffs in one I/O call
		if (!diff_out.empty()) std::cout << diff_out << std::flush;

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

	// show cursor again
	std::cout << "\x1b[?25h";

	return 0;
}
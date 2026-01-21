#include <donut.h>
#include <iostream>
#include <cmath>
#include <chrono>
#include <thread>
#include <vector>
#include <cstring>
#include <algorithm>
#include <limits>
#include <string>

namespace donut
{
    namespace {
        constexpr float PI = 3.14159265f;
        const char* luminance = ".,-~:;=!*#$@";
    }
    // Helper: precompute sine/cosine tables for theta and phi
    // This reduces repeated trig calls in the inner loop.
    static void precomputeTrig(float theta_step, float phi_step,
                               std::vector<float>& sinTheta, std::vector<float>& cosTheta,
                               std::vector<float>& sinPhi, std::vector<float>& cosPhi)
    {
        int theta_count = static_cast<int>(std::ceil(2.0f * PI / theta_step));
        int phi_count = static_cast<int>(std::ceil(2.0f * PI / phi_step));
        sinTheta.resize(theta_count); cosTheta.resize(theta_count);
        sinPhi.resize(phi_count);   cosPhi.resize(phi_count);
        for (int i = 0; i < theta_count; ++i)
        {
            float t = i * theta_step;
            sinTheta[i] = std::sin(t);
            cosTheta[i] = std::cos(t);
        }
        for (int j = 0; j < phi_count; ++j)
        {
            float p = j * phi_step;
            sinPhi[j] = std::sin(p);
            cosPhi[j] = std::cos(p);
        }
    }

    // Helper: render a single frame into `output` and `zbuffer`.
    // Responsibilities: compute torus geometry, project to screen, compute luminance, update z-buffer.
    static void renderFrame(const Config& cfg,
                            const std::vector<float>& sinTheta, const std::vector<float>& cosTheta,
                            const std::vector<float>& sinPhi, const std::vector<float>& cosPhi,
                            float A, float B,
                            std::vector<char>& output, std::vector<float>& zbuffer)
    {
        const int width = cfg.width;
        const int height = cfg.height;
        const int lum_len = static_cast<int>(std::strlen(luminance));

        const int theta_count = static_cast<int>(sinTheta.size());
        const int phi_count = static_cast<int>(sinPhi.size());

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

                // h is the distance from the torus center to the tube surface along the circle
                float h = cPhi + cfg.R2;
                float denom = (sTheta * h * sinA + sPhi * cosA + cfg.K2);
                if (denom == 0.0f) continue; // skip degenerate projection
                float D = 1.0f / denom; // approximate 1/z for depth comparison

                // parameter t used to compute screen-space coordinates
                float t = sTheta * h * cosA - sPhi * sinA;

                int x = static_cast<int>(width/2 + cfg.scale_x * D * (cTheta * h * cosB - t * sinB));
                int y = static_cast<int>(height/2 + cfg.scale_y * D * (cTheta * h * sinB + t * cosB));

                if (y >= 0 && y < height && x >= 0 && x < width)
                {
                    int o = x + width * y;
                    
                    // Old luminance calculation
                    // float L = cPhi * cTheta * sinB - D * (sPhi * cosB + sTheta * cosA * sinB);
                    // int lum_index = static_cast<int>(std::lround(std::clamp(8.0f * L, 0.0f, static_cast<float>(lum_len - 1))));

                    // approximate luminance using the same expression as the C example
                    // (variables: j=phi, i=theta => sPhi,cPhi,sTheta,cTheta)
                    
                    float L = (sPhi * sinA - sTheta * cPhi * cosA) * cosB
                              - sTheta * cPhi * sinA
                              - sPhi * cosA
                              - cTheta * cPhi * sinB;
                    int lum_index = static_cast<int>(8.0f * L);
                    lum_index = std::clamp(lum_index, 0, lum_len - 1);
                    if (D > zbuffer[o])
                    {
                        zbuffer[o] = D;
                        output[o] = luminance[lum_index];
                    }
                }
            }
        }
    }

    // Helper: build minimal diff between current `output` and `prev_output`.
    // It returns an ANSI-encoded string containing only the runs that changed and
    // updates `prev_output` in-place to reflect the written characters.
    static std::string buildDiffAndUpdatePrev(const std::vector<char>& output, std::vector<char>& prev_output, int width, int height)
    {
        std::string diff_out;
        diff_out.reserve(512);
        for (int row = 0; row < height; ++row)
        {
            int col = 0;
            while (col < width)
            {
                int idx = row * width + col;
                if (output[idx] == prev_output[idx]) { ++col; continue; }
                int start = col;
                while (col < width && output[row*width + col] != prev_output[row*width + col]) ++col;
                int run_len = col - start;
                // move cursor to (row+1,start+1)
                diff_out += "\x1b[" + std::to_string(row + 1) + ";" + std::to_string(start + 1) + "H";
                diff_out.append(&output[row*width + start], run_len);
                for (int k = 0; k < run_len; ++k) prev_output[row*width + start + k] = output[row*width + start + k];
            }
        }
        return diff_out;
    }

    void run(const Config& cfg)
    {
        const int width = cfg.width;
        const int height = cfg.height;
        const int screen_size = width * height;

        std::vector<char> output(screen_size);
        std::vector<char> prev_output(screen_size, ' ');
        std::vector<float> zbuffer(screen_size);

        // precompute trig tables once
        std::vector<float> sinTheta, cosTheta, sinPhi, cosPhi;
        precomputeTrig(cfg.theta_step, cfg.phi_step, sinTheta, cosTheta, sinPhi, cosPhi);

        // clear screen and hide cursor (ANSI)
        std::cout << "\x1b[2J\x1b[?25l";

        float A = 0.0f, B = 0.0f;
        using clock = std::chrono::steady_clock;
        const std::chrono::duration<double> target_frame_time(1.0 / cfg.fps);

        int frame = 0;
        while (cfg.frames == 0 || frame < cfg.frames)
        {
            auto frame_start = clock::now();

            // reset output and depth buffer
            std::fill(output.begin(), output.end(), ' ');
            std::fill(zbuffer.begin(), zbuffer.end(), -std::numeric_limits<float>::infinity());

            // compute single frame into output/zbuffer
            renderFrame(cfg, sinTheta, cosTheta, sinPhi, cosPhi, A, B, output, zbuffer);

            // compute small diff and update previous buffer
            std::string diff_out = buildDiffAndUpdatePrev(output, prev_output, width, height);

            // write diffs in one I/O call
            if (!diff_out.empty()) std::cout << diff_out << std::flush;

            // advance rotation
            A += 0.08f;
            B += 0.03f;

            // frame timing (stable FPS)
            auto elapsed = clock::now() - frame_start;
            if (elapsed < target_frame_time)
            {
                std::this_thread::sleep_for(target_frame_time - elapsed);
            }

            ++frame;
        }

        // show cursor again
        std::cout << "\x1b[?25h";
    }
}
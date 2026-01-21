#pragma once

#include <string>

namespace donut
{
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

    // Run the donut demo (blocking). argc/argv parsing kept in main; pass config.
    void run(const Config& cfg);
}

#include "ktimer.h"

namespace kemena
{
    kTimer::kTimer()
    {
        then = std::chrono::system_clock::now();
    }

    void kTimer::tick()
    {
        // Calculate deltaTime
        const auto now = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_seconds = now - then;
        then = now;
        deltaTime = (float)elapsed_seconds.count();
    }

    float kTimer::getDeltaTime()
    {
        return deltaTime;
    }
}

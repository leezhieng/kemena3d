#ifndef KTIMER_H
#define KTIMER_H

#include "kexport.h"

#include <chrono>

namespace kemena
{
    class KEMENA3D_API kTimer
    {
        public:
            kTimer();

            void tick();
            float getDeltaTime();

        protected:

        private:
            float deltaTime;
            std::chrono::system_clock::time_point then;
    };
}

#endif // KTIMER_H

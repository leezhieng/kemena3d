#ifndef KWINDOW_H
#define KWINDOW_H

#include "kexport.h"

#include "kdatatype.h"
#include "ktimer.h"

#include <iostream>
#include <string>

// Tell SDL not to mess with main()
#define SDL_MAIN_HANDLED
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

namespace kemena
{
    class KEMENA3D_API kWindow
    {
    public:
        kWindow();
        virtual ~kWindow();

        bool init(int width, int height, std::string title, bool maximized = false, kWindowType type = kWindowType::WINDOW_DEFAULT, void *nativeHandle = nullptr);
        void destroy();
        void swap();
        int getWindowWidth();
        int getWindowHeight();
        std::string getWindowTitle();

        SDL_Window *getSdlWindow();

        bool getRunning();
        void setRunning(bool newRunning);

        kTimer *getTimer();

    protected:
    private:
        SDL_Window *sdlWindow = NULL;

        std::string windowTitle;
        int windowWidth;
        int windowHeight;

        bool running = true;
        kTimer *timer;
    };
}

#endif // KWINDOW_H

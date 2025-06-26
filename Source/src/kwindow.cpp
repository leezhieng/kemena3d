#include "kwindow.h"

namespace kemena
{
    kWindow::kWindow()
    {
        timer = new kTimer();
    }

    kWindow::~kWindow()
    {
    }

    bool kWindow::init(int width, int height, std::string title)
    {
        windowWidth = width;
        windowHeight = height;
        windowTitle = title;

        if(!SDL_Init(SDL_INIT_VIDEO))
        {
            std::cout << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
            return false;
        }

        sdlWindow = SDL_CreateWindow(title.c_str(), width, height, SDL_WINDOW_OPENGL);
        if(sdlWindow == NULL)
        {
            std::cout << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
            return false;
        }

        return true;
    }

    void kWindow::destroy()
    {
        SDL_DestroyWindow(sdlWindow);
        SDL_Quit();
    }

    void kWindow::swap()
    {
        SDL_GL_SwapWindow(getSdlWindow());
    }

    int kWindow::getWindowWidth()
    {
        return windowWidth;
    }

    int kWindow::getWindowHeight()
    {
        return windowHeight;
    }

    std::string kWindow::getWindowTitle()
    {
        return windowTitle;
    }

    bool kWindow::getRunning()
    {
        timer->tick();

        return running;
    }

    void kWindow::setRunning(bool newRunning)
    {
        running = newRunning;
    }

    kTimer* kWindow::getTimer()
    {
        return timer;
    }

    SDL_Window* kWindow::getSdlWindow()
    {
        return sdlWindow;
    }
}

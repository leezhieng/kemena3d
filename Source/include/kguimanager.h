#ifndef KGUIMANAGER_H
#define KGUIMANAGER_H

#include "kexport.h"

#include "kdatatype.h"
#include "krenderer.h"
#include "kwindow.h"

#include <SDL3/SDL.h>

#include "imgui.h"
#include "backends/imgui_impl_sdl3.h"
#include "backends/imgui_impl_opengl3.h"

namespace kemena
{
    class KEMENA3D_API kGuiManager
    {
        public:
            kGuiManager();
            virtual ~kGuiManager();

            void init(kRenderer* newRenderer);
			void processEvent(kSystemEvent event);
			
			void canvasStart();
			void canvasEnd();
			
			void windowStart(std::string title, bool *open = nullptr);
			void windowEnd();
			
			void text(std::string text);
			
			void destroy();

        protected:

        private:
            float mainScale;
			kRenderer* renderer;
    };
}

#endif // KGUIMANAGER_H

#include "kguimanager.h"

namespace kemena
{
    kGuiManager::kGuiManager()
    {
        //ctor
    }

    kGuiManager::~kGuiManager()
    {
        //dtor
    }

    void kGuiManager::init(kRenderer* newRenderer)
    {
        renderer = newRenderer;
		
		float mainScale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());

		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsLight();

		// Setup scaling
		ImGuiStyle& style = ImGui::GetStyle();
		style.ScaleAllSizes(mainScale);        // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
		style.FontScaleDpi = mainScale;        // Set initial font scale. (using io.ConfigDpiScaleFonts=true makes this unnecessary. We leave both here for documentation purpose)

		// Setup Platform/Renderer backends
		ImGui_ImplSDL3_InitForOpenGL(renderer->getWindow()->getSdlWindow(), renderer->getOpenGlContext());
		const char* glsl_version = "#version 150";
		ImGui_ImplOpenGL3_Init(glsl_version);
    }
	
	void kGuiManager::processEvent(kSystemEvent event)
	{
		ImGui_ImplSDL3_ProcessEvent(event.getSdlEvent());
	}
	
	void kGuiManager::canvasStart()
	{
		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();
	}
	
	void kGuiManager::canvasEnd()
	{
		ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}
	
	void kGuiManager::windowStart(std::string title, bool *open)
	{
		ImGui::Begin(title.c_str(), open);
	}
	
	void kGuiManager::windowEnd()
	{
		ImGui::End();
	}
	
	void kGuiManager::text(std::string text)
	{
		ImGui::Text(text.c_str());
	}
	
	void kGuiManager::destroy()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplSDL3_Shutdown();
		ImGui::DestroyContext();
	}
}

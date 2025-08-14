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
		
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

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
		glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
		
		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();
	}
	
	void kGuiManager::canvasEnd()
	{
		ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		
		// IMPORTANT: multi-viewport handling
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}
	}
	
	void kGuiManager::windowStart(std::string title, bool *open)
	{
		ImGui::Begin(title.c_str(), open);
	}
	
	void kGuiManager::windowEnd()
	{
		ImGui::End();
	}
	
	void kGuiManager::dockSpaceStart(std::string name)
	{
		bool show_main_dockspace = true;
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

        // Get main viewport
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);

        // Remove window decorations and make it fixed
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

        // Remove padding
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

        ImGui::Begin(name.c_str(), &show_main_dockspace, window_flags);
        ImGui::PopStyleVar();

        ImGuiID dockspace_id = ImGui::GetID(name.c_str());
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);
	}
	
	void kGuiManager::dockSpaceEnd()
	{
		ImGui::End();
	}
	
	bool kGuiManager::menuBar()
	{
		return ImGui::BeginMainMenuBar();
	}
	
	void kGuiManager::menuBarEnd()
	{
		ImGui::EndMainMenuBar();
	}
	
	bool kGuiManager::menu(std::string text)
	{
		return ImGui::BeginMenu(text.c_str());
	}
	
	void kGuiManager::menuEnd()
	{
		ImGui::EndMenu();
	}
	
	bool kGuiManager::menuItem(std::string text, std::string shortcut, bool selected, bool enabled)
	{
		return ImGui::MenuItem(text.c_str(), shortcut.c_str(), selected, enabled);
	}
	
	void kGuiManager::groupStart()
	{
		ImGui::BeginGroup();
	}
	
	void kGuiManager::groupEnd()
	{
		ImGui::EndGroup();
	}
			
	void kGuiManager::sameLine()
	{
		ImGui::SameLine();
	}
	
	void kGuiManager::spacing()
	{
		ImGui::Spacing();
	}
	
	void kGuiManager::separator()
	{
		ImGui::Separator();
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
